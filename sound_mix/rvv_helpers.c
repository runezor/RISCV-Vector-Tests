#include <riscv_vector.h>

void sat_add_vectorised(int n, uint16_t *x, uint16_t *y, uint16_t* z)
{

    size_t vl = vsetvl_e16m1(n);
    int64_t z = 0; 
    vint16m4_t v3 = vle16_v_i16m1((void*)(&z), vl); //Initialised v3 to 0
    
	for (int i = 0; i < ((n>>2)<<2); i += 4) {
        vint16m4_t v1 = vle16_v_i16m1((int64_t*)(&x[i]), vl);
        vint16m4_t v2 = vle16_v_i16m1((int64_t*)(&y[i]), vl);
        vse16_v_i16m1((int64_t*)(&z[i]), vadd_vv_i16m1(v1, v2, vl), vl);
	}

	for (s = 0; i < n; ++i){
        int temp = x[i] + y[i];
        if (temp>0xffff){
            temp = 0xffff;
        }
        z[i] = temp; //Calculates remaining vals
    }
}