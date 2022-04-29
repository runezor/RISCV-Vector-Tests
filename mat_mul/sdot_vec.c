#include <riscv_vector.h>

// *IMPORTANT*
// THIS WAS WRITTEN AS A HELP FOR WRITING THE ASSEMBLY
// IT WAS COMPILED USING CLANG VECTOR INTRINSICS, AND THEN MODIFIED AFTER
// DO NOT EXPECT THIS TO YIELD AN IDENTICAL ASSEMBLY TO EITHER OF
// THE TWO FILES IN THIS FOLDER

int16_t sdot_vec(int n, int16_t *x, int16_t *y)
{
    int i;
	int16_t s;


    size_t vl = vsetvl_e16m4(n);
    int64_t z = 0; 
    vint16m4_t v3 = vle16_v_i16m4((void*)(&z), vl); //Initialised v3 to 0
    
	for (i = 0; i < ((n>>2)<<2); i += 4) {
        int64_t v = *((int64_t*)(&x[i*4]));
        vint16m4_t v1 = vle16_v_i16m4((int64_t*)(&x[i]), vl);
        vint16m4_t v2 = vle16_v_i16m4((int64_t*)(&y[i]), vl);
        v3 = vadd_vv_i16m4(v3, vmul_vv_i16m4(v1, v2, vl), vl);
	}

    int16_t* a = (int16_t*) &v3;
	for (s = 0; i < n; ++i) s += x[i] * y[i]; //Collects remainder
    for (int i = 0; i<4; i++) s += a[i]; //Reduces and adds vector
	return s;
}

int64_t simple_add(int n, int16_t *x, int16_t *y)
{
    int i;
	int16_t s;

    size_t vl = vsetvl_e16m4(n);
    int64_t z = 0; 
    vint16m4_t v3 = vle16_v_i16m4((void*)(&z), vl); //Initialised v3 to 0
    
	for (i = 0; i < ((n>>2)<<2); i += 4) {
        vint16m4_t v1 = vle16_v_i16m4((int64_t*)(&x[i*4]), vl);
        vint16m4_t v2 = vle16_v_i16m4((int64_t*)(&y[i*4]), vl);
        v3 = vadd_vv_i16m4(v3, v1, vl);
        v3 = vadd_vv_i16m4(v3, v2, vl);
	}

    return *((int64_t*)&v3);
}

size_t get_vl(){
    return vsetvl_e16m4(4);
}