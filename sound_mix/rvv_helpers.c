#include <riscv_vector.h>

// *IMPORTANT*
// THIS WAS WRITTEN AS A HELP FOR WRITING THE ASSEMBLY
// IT WAS COMPILED USING CLANG VECTOR INTRINSICS, AND THEN MODIFIED AFTER
// DO NOT EXPECT THIS TO YIELD AN IDENTICAL ASSEMBLY TO EITHER OF
// THE TWO FILES IN THIS FOLDER

void sat_add_vectorised(int n, uint16_t *x, uint16_t *y, uint16_t* z)
{
    size_t vl = vsetvl_e16m1(n);
    vint16m1_t v3 = vle16_v_i16m1((void*)(&z), vl); //Initialised v3 to 0
    int i;
	for (i = 0; i < ((n>>3)<<3); i += 8) {
        vint16m1_t v1 = vle16_v_i16m1((int64_t*)(&x[i]), vl);
        vint16m1_t v2 = vle16_v_i16m1((int64_t*)(&y[i]), vl);
        vse16_v_i16m1((int64_t*)(&z[i]), vadd_vv_i16m1(v1, v2, vl), vl);
	}

	for (int temp; i < n; ++i){
        temp = x[i] + y[i];
        if (temp>0xffff){
            temp = 0xffff;
        }
        z[i] = temp; //Calculates remaining vals
    }
}