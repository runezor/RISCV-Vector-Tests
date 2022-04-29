#ifdef DEBUG
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#endif
#include "sdot_vec.h"

/*****************
THIS CODE WAS BASED ON THE FOLLOWING GITHUB REPO
https://github.com/attractivechaos/matmul
IT HAS BEEN HEAVILY EDITED
Credit goes to attractivechaos

To build (Using my own paths):
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-gcc -DALGO=0 -DSIZE=100 -march=rv64gcv -lm init.S sdot_vec08.S matmul.c -T link.ld -nostartfiles -lm
To test on Spike:
../../../../TestRig/riscv-implementations/riscv-isa-sim/build/bin/spike -l --isa=RV64gcV ./a.out

To run with kernel proxy for 1.0 (Using my own paths):
/Volumes/Workspace/riscv64-unknown-elf-toolchain-10.2.0-2020.12.8-x86_64-apple-darwin/bin/riscv64-unknown-elf-gcc -DALGO=5 -DSIZE=1000 -march=rv64gcv sdot_vec10.S matmul.c
spike --isa=RV64gcV pk a.out
*****************/

/***********************************************
 * WYHASH-based Pseudo-random number generator *
 ***********************************************/


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
int16_t sdot_1(int n, const int16_t *x, const int16_t *y)
{
	int i;
	int16_t s = 0;
	for (i = 0; i < n; ++i) s += x[i] * y[i];
	return s;
}

int16_t sdot_8(int n, const int16_t *x, const int16_t *y)
{
	int i, n8 = n>>3<<3;
	int16_t s, t[8];
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

void mat_transpose(int16_t m[SIZE][SIZE])
{
	for (int i = 0; i < SIZE; i++)
		for (int j = i; j < SIZE; j++){
			int16_t t = m[j][i];
			m[j][i] = m[i][j];
			m[i][j] = t;
		}
}


/*****************
 * For debugging *
 *****************/

#ifdef DEBUG
void print_arrs(int n, int16_t *x, int16_t *y)
{
    int i;
	int16_t s;
    
	for (s, i = 0; i < ((n>>2)<<2); i += 4) {
        int64_t v = *((int64_t*)(&x[i*4]));
		s=s+x[i] * y[i];
	printf("%d",s);
	}
	printf("\n\n%d\n\n",s);
}

void **mat_print(int16_t m[SIZE][SIZE])
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

void **mat_mul0(int16_t a[SIZE][SIZE], int16_t b[SIZE][SIZE], int16_t out[SIZE][SIZE])
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

void **mat_mul1(int16_t a[SIZE][SIZE], int16_t b[SIZE][SIZE], int16_t out[SIZE][SIZE])
{
	int i, j, k = SIZE;
	mat_transpose(b);
	for (i = 0; i < SIZE; ++i) {
		const int16_t *ai = a[i];
		int16_t *oi = out[i];
		for (j = 0; j < SIZE; ++j) {
			int16_t t = 0, *bj = b[j];
			for (k = 0; k < SIZE; ++k)
				t += ai[k] * bj[k];
			oi[j] = t;
		}
	}
	mat_transpose(b);
}

void **mat_mul2(int16_t a[SIZE][SIZE], int16_t b[SIZE][SIZE], int16_t out[SIZE][SIZE])
{
	int i, j = SIZE;
	mat_transpose(b);
	for (i = 0; i < SIZE; ++i)
		for (j = 0; j < SIZE; ++j)
			out[i][j] = sdot_1(SIZE, a[i], b[j]);
	mat_transpose(b);
}

void **mat_mul3(int16_t a[SIZE][SIZE], int16_t b[SIZE][SIZE], int16_t out[SIZE][SIZE])
{
	mat_transpose(b);
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			out[i][j] = sdot_8(SIZE, a[i], b[j]);
	mat_transpose(b);
}

void **mat_mul_vec(int16_t a[SIZE][SIZE], int16_t b[SIZE][SIZE], int16_t out[SIZE][SIZE])
{
	mat_transpose(b);
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++){
			//print_arrs(SIZE,a[i],b[j]);
			out[i][j] = sdot_vec(SIZE, a[i], b[j]);
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
	int16_t a[n][n];
	int16_t b[n][n];
	int16_t m[n][n];
	
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

		int16_t m1[n][n];
		int16_t m2[n][n];
		int16_t m3[n][n];
		int16_t m4[n][n];
		int16_t m5[n][n];

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
	} else {
		#ifdef DEBUG
		fprintf(stderr, "ERROR: unknown algorithm %d\n", ALGO);
		#endif
		return 0;
	}

	#ifdef DEBUG
	fprintf(stderr, "Central cell: %d\n", m[n/2][n/2]);
	#endif

	return 0;
}

