#include <riscv_vector.h>

// *IMPORTANT*
// THIS WAS WRITTEN AS A HELP FOR WRITING THE ASSEMBLY
// IT WAS COMPILED USING CLANG VECTOR INTRINSICS, AND THEN MODIFIED AFTER
// DO NOT EXPECT THIS TO YIELD AN IDENTICAL ASSEMBLY TO EITHER OF
// THE TWO FILES IN THIS FOLDER

int16_t sdot_vec(int n, int16_t *x, int16_t *y)
{
    int i=0;
	int16_t s = 0;

    size_t vl = vsetvl_e16m1(n);
    int64_t z = 0; 
    vint16m1_t v3 = vle16_v_i16m1((void*)(&z), vl); //Initialised v3 to 0
    

    /*while(((long)&x[i])%128!=0){
        s += x[i] * y[i];
        i = i+1;
    }*/

	for (; i < ((n>>3)<<3); i += 8) {
        vint16m1_t v1 = vle16_v_i16m1(&x[i], vl);
        vint16m1_t v2 = vle16_v_i16m1(&y[i], vl);
        v3 = vadd_vv_i16m1(v3, vmul_vv_i16m1(v1, v2, vl), vl);
	}

    int16_t* a = (int16_t*) &v3;
	for (; i < n; ++i) s += x[i] * y[i]; //Collects remainder
    for (int i = 0; i<8; i++) s += a[i]; //Reduces and adds vector
	return s;
}