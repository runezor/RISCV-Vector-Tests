#ifdef DEBUG
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#endif
extern "C" {
#include "sdot_vec.h"
}


/*****************
THIS CODE WAS BASED ON THE FOLLOWING GITHUB REPO
https://github.com/attractivechaos/matmul
IT HAS BEEN HEAVILY EDITED
Credit goes to attractivechaos

To build (Using my own paths):
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-g++ -DALGO=-1 -DSIZE=96 -O3 -march=rv64gc -nodefaultlibs mat_mul_aligned.cpp -c -o mat_mul.o
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-gcc -march=rv64gv -lm init.S sdot_vec08.S mat_mul.o -T link.ld -nostartfiles -lm

To test on Spike:
../../../../../TestRig/riscv-implementations/riscv-isa-sim/build/bin/spike -l --isa=RV64gcV ./a.out

To run with kernel proxy for 1.0 (Using my own paths):
/Volumes/Workspace/riscv64-unknown-elf-toolchain-10.2.0-2020.12.8-x86_64-apple-darwin/bin/riscv64-unknown-elf-g++ -DALGO=5 -DSIZE=10 -DDEBUG -march=rv64gcv sdot_vec10.S mat_mul.cpp
spike --isa=RV64gcV pk a.out

To build with alignment:

*****************/

/************************************************************************************
 * WYHASH-based Pseudo-random number generator									    *
 Based on: https://lemire.me/blog/2019/07/03/a-fast-16-bit-random-number-generator/ *
 ************************************************************************************/
uint32_t hash16(uint32_t input, uint32_t key) {
  uint32_t hash = input * key;
  return ((hash >> 16) ^ hash) & 0xFFFF;
}

uint16_t wyhash_16(uint16_t* wyhash16_x) {
  *wyhash16_x = *wyhash16_x+0xfc15;
  return hash16(*wyhash16_x, 0x2ab);
}

/*******************************************
 * Helper routines for matrix manipulation *
 *******************************************/
volatile int16_t sdot_1(int n, const volatile int16_t *x, const volatile int16_t *y)
{
	int i;
	volatile int16_t s = 0;
	for (i = 0; i < n; ++i) s += x[i] * y[i];
	return s;
}

volatile int16_t sdot_8(int n, const volatile int16_t *x, const volatile int16_t *y)
{
	int i, n8 = n>>3<<3;
	volatile int16_t s, t[8];
	t[0] = t[1] = t[2] = t[3] = t[4] = t[5] = t[6] = t[7] = 0;
	for (i = 0; i < n8; i += 8) {
		t[0] += x[i+0] * y[i+0];
		t[1] += x[i+1] * y[i+1];
		t[2] += x[i+2] * y[i+2];
		t[3] += x[i+3] * y[i+3];
		t[4] += x[i+4] * y[i+4];
		t[5] += x[i+5] * y[i+5];
		t[6] += x[i+6] * y[i+6];
		t[7] += x[i+7] * y[i+7];
	}
	for (s = 0.0f; i < n; ++i) s += x[i] * y[i];
	s += t[0] + t[1] + t[2] + t[3] + t[4] + t[5] + t[6] + t[7];
	return s;
}

void mat_transpose(volatile int16_t m[][SIZE])
{
	for (int i = 0; i < SIZE; i++)
		for (int j = i; j < SIZE; j++){
			volatile int16_t t = m[j][i];
			m[j][i] = m[i][j];
			m[i][j] = t;
		}
}


/*****************
 * For debugging *
 *****************/

#ifdef DEBUG
void print_arrs(int n, volatile int16_t *x, volatile int16_t *y)
{
    int i;
	volatile int16_t s;
    
	for (s, i = 0; i < ((n>>2)<<2); i += 4) {
        int64_t v = *((int64_t*)(&x[i*4]));
		s=s+x[i] * y[i];
	printf("%d",s);
	}
	printf("\n\n%d\n\n",s);
}

void **mat_print(volatile int16_t m[][SIZE])
{
	for (int i = 0; i < SIZE; ++i){
		for (int j = 0; j < SIZE; ++j)
			printf("%d	",m[i][j]);
		printf("\n");
	}
}

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}
#endif

/*************************
 * Matrix multiplication *
 *************************/

void mat_mul0(volatile int16_t a[][SIZE], volatile int16_t b[][SIZE], volatile int16_t out[][SIZE])
{
	int i, j, k;
	for (i = 0; i < SIZE; ++i) {
		for (j = 0; j < SIZE; ++j) {
			uint16_t t = 0;
			for (k = 0; k < SIZE; ++k)
				t += a[i][k] * b[k][j];
			out[i][j] = t;
		}
	}
}

void mat_mul1(volatile int16_t a[][SIZE], volatile int16_t b[][SIZE], volatile int16_t out[][SIZE])
{
	int i, j, k = SIZE;
	mat_transpose(b);
	for (i = 0; i < SIZE; ++i) {
		const volatile int16_t *ai = a[i];
		volatile int16_t *oi = out[i];
		for (j = 0; j < SIZE; ++j) {
			volatile int16_t t = 0, *bj = b[j];
			for (k = 0; k < SIZE; ++k)
				t += ai[k] * bj[k];
			oi[j] = t;
		}
	}
	mat_transpose(b);
}

void mat_mul2(volatile int16_t a[][SIZE], volatile int16_t b[][SIZE], volatile int16_t out[][SIZE])
{
	int i, j = SIZE;
	mat_transpose(b);
	for (i = 0; i < SIZE; ++i)
		for (j = 0; j < SIZE; ++j)
			out[i][j] = sdot_1(SIZE, a[i], b[j]);
	mat_transpose(b);
}

void mat_mul3(volatile int16_t a[][SIZE], volatile int16_t b[][SIZE], volatile int16_t out[][SIZE])
{
	mat_transpose(b);
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			out[i][j] = sdot_8(SIZE, a[i], b[j]);
	mat_transpose(b);
}

void mat_mul_vec(volatile int16_t a[][SIZE], volatile int16_t b[][SIZE], volatile int16_t out[][SIZE])
{
	mat_transpose(b);
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++){
			out[i][j] = sdot_vec(SIZE, (int16_t*) a[i], (int16_t*) b[j]);
		}
	//Restore b
	mat_transpose(b);
}

/*****************
 * Main function *
 *****************/
int main(int argc, char *argv[])
{
	int n = SIZE;
	uint16_t h = 0;
	uint16_t* wyhash16_x = &h; 
	alignas(128) volatile int16_t a[SIZE][SIZE];
	alignas(128) volatile int16_t b[SIZE][SIZE];
	alignas(128) volatile int16_t m[SIZE][SIZE];

	/*volatile int16_t (*a)[SIZE] = (volatile int16_t (*)[SIZE])a_v;
	volatile int16_t (*b)[SIZE] = (volatile int16_t (*)[SIZE])b_v;
	volatile int16_t (*m)[SIZE] = (volatile int16_t (*)[SIZE])m_v;	*/

	//Initialises matrices
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			a[i][j] = wyhash_16(wyhash16_x)%256-128;
			b[i][j] = wyhash_16(wyhash16_x)%256-128;
			m[i][j] = 0;
		}
	}

	if (ALGO == 0) {
	    mat_mul0(a, b, m);
	} else if (ALGO == 1) {
		mat_mul1(a, b, m);
	} else if (ALGO == 2) {
		mat_mul2(a, b, m);
	} else if (ALGO == 3) {
		mat_mul3(a, b, m);
	} else if (ALGO == 4) {
		mat_mul_vec(a, b, m);
	} else if (ALGO == 5) {

		volatile int16_t m1[SIZE][SIZE];
		volatile int16_t m2[SIZE][SIZE];
		volatile int16_t m3[SIZE][SIZE];
		volatile int16_t m4[SIZE][SIZE];
		volatile int16_t m5[SIZE][SIZE];

		for(int i=0; i<n; i++){
			for(int j=0; j<n; j++){
				m1[i][j] = 0;
				m2[i][j] = 0;
				m3[i][j] = 0;
				m4[i][j] = 0;
				m5[i][j] = 0;
			}
		}

		mat_mul0(a, b, m1);
		mat_mul1(a, b, m2);
		mat_mul2(a, b, m3);
		mat_mul3(a, b, m4);
		mat_mul_vec(a, b, m5);
		
		#ifdef DEBUG
		printf("MATRIX A:\n");
		mat_print(a);
		printf("MATRIX B:\n");
		mat_print(b);
		printf("MATRIX RESULT:\n");
		mat_print(m1);
		#endif

		int matches = 1;
		for(int i=0; i<n; i++){
			for(int j=0; j<n; j++){
				if (m1[i][j]!=m2[i][j] || m1[i][j]!=m3[i][j] || m1[i][j]!=m4[i][j] || m1[i][j]!=m5[i][j]){
					matches = 0;
					#ifdef DEBUG
						printf("MISMATCH AT x: %d, y:%d\n", j, i);
						printf("MAT1: %d\n", m1[i][j]);
						printf("MAT2: %d\n", m2[i][j]);
						printf("MAT3: %d\n", m3[i][j]);
						printf("MAT4: %d\n", m4[i][j]);
						printf("MAT5: %d\n", m5[i][j]);
					#endif
				}
			}
		}
		if (matches==1){
			success();
			#ifdef DEBUG
			printf("Success!");
			#endif
		}
		else {
			failure();
			#ifdef DEBUG
			printf("Failure!");
			#endif
		}
	}

	#ifdef DEBUG
	fprintf(stderr, "Central cell: %d\n", m[n/2][n/2]);
	#endif

	return 0;
}

