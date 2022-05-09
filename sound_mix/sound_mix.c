#include <stdint.h>
#include "rvv_helpers.h"

#ifdef DEBUG
#include <stdio.h>
#endif

/*****************
To build (Using my own paths):
/Volumes/Workspace/riscvv08/gnu/bin/riscv64-unknown-elf-gcc -DSIZE=10000 -O3 -march=rv64gcv -lm init.S rvv_helpers_v08.S sound_mix.c -T link.ld -nostartfiles -lm
To test on Spike:
../../../../../TestRig/riscv-implementations/riscv-isa-sim/build/bin/spike -l --isa=RV64gcV ./a.out

To run with kernel proxy for 1.0 (Using my own paths):
/Volumes/Workspace/riscv64-unknown-elf-toolchain-10.2.0-2020.12.8-x86_64-apple-darwin/bin/riscv64-unknown-elf-gcc -DCOMPARE -DSIZE=10000 -march=rv64gcv rvv_helpers_v10_128.S sound_mix.c
spike --isa=RV64gcV pk a.out
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

void sat_add_seq(int n, volatile uint16_t *x, volatile  uint16_t *y, volatile  uint16_t* z){
	for(int i=0; i<n; i++){
		int temp = x[i]+y[i];
		if (temp>0xffff){
			temp = 0xffff;
		}
		z[i] = temp;
	}
}

int main(int argc, char *argv[])
{
	uint16_t h = 0;
	uint16_t* wyhash16_x = &h; 
	int n = SIZE;

	volatile uint16_t a[n];
	volatile uint16_t b[n];
	volatile uint16_t c[n];
	volatile uint16_t d[n];

	for(uint16_t i=0; i<n; i++){
		a[i] = wyhash_16(wyhash16_x);
		b[i] = wyhash_16(wyhash16_x);
	}

	#ifdef VECTOR
		sat_add_vectorised(n, a, b, c);
	#endif
	#ifdef SEQ
		sat_add_seq(n, a, b, c);
	#endif


	#ifdef COMPARE 
		sat_add_vectorised(n, a, b, c);
		sat_add_seq(n, a, b, d);
			
		int mismatches = 0;
		for(uint16_t i=0; i<n; i++){
			if (c[i]!=d[i]){
				mismatches += 1;
				#ifdef DEBUG 
				printf("iteration:%d\n",i);
				printf("a:%d vs b:%d\n",a[i],b[i]);
				printf("vec:%d vs seq:%d\n",c[i],d[i]);
				#endif
			}
		}

		if (mismatches>0){
			#ifdef DEBUG 
			printf("Failure");
			#else
			failure(); 
			#endif
		} else {
			#ifdef DEBUG 
			printf("Success");
			#else
			success();
			#endif
		}
	#endif
	
	//Try to optimise this Clang
	
	return 0;
}

