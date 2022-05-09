.global sdot_vec
.global end_program
.global success
.global failure
sdot_vec:                               # @sdot_vec
        addi    sp, sp, -32
        csrr    a3, vlenb
        sub     sp, sp, a3
        li a4, 8
        vsetvli        zero, a4, e16
        sd      zero, 24(sp)
        addi    a3, sp, 24
        vle16.v v8, (a3)
        andi    a7, a0, -8
        addi    a3, sp, 32
        vs1r.v  v8, (a3)
        blez    a7, .LBB0_6
        addi    a3, sp, 32
        vl1re16.v       v8, (a3)
        li      a4, 0
        mv      a3, a1
        mv      a5, a2
.LBB0_2:                                # =>This Inner Loop Header: Depth=1
        vle16.v v9, (a3)
        vle16.v v10, (a5)
        vmul.vv v9, v9, v10
        vadd.vv v8, v8, v9
        addi    a4, a4, 8
        addi    a5, a5, 16
        addi    a3, a3, 16
        blt     a4, a7, .LBB0_2
        addi    a3, sp, 32
        vs1r.v  v8, (a3)
        sext.w  a3, a4
        bge     a3, a0, .LBB0_7
.LBB0_4:
        li      a3, 0
        slli    a4, a4, 32
        srli    a5, a4, 31
        add     a2, a2, a5
        add     a1, a1, a5
        srli    a4, a4, 32
        slli    a0, a0, 32
        srli    a0, a0, 32
        sub     a0, a0, a4
.LBB0_5:                                # =>This Inner Loop Header: Depth=1
        lh      a4, 0(a1)
        lh      a5, 0(a2)
        mul     a4, a5, a4
        add     a3, a3, a4
        addi    a2, a2, 2
        addi    a0, a0, -1
        addi    a1, a1, 2
        bnez    a0, .LBB0_5
        j       .LBB0_8
.LBB0_6:
        li      a4, 0
        sext.w  a3, a4
        blt     a3, a0, .LBB0_4
.LBB0_7:
        li      a3, 0
.LBB0_8:
        li      a0, 0
        addi    a1, sp, 32
        li      a2, 16
.LBB0_9:                                # =>This Inner Loop Header: Depth=1
        add     a4, a1, a0
        lh      a4, 0(a4)
        addi    a0, a0, 2
        add     a3, a3, a4
        bne     a0, a2, .LBB0_9
        slli    a0, a3, 48
        srai    a0, a0, 48
        csrr    a1, vlenb
        add     sp, sp, a1
        addi    sp, sp, 32
        ret
success:                               # @end_program
        addi a1, a1, 0
        ret
failure:                               # @end_program
        addi a2, a2, 0
        ret

end_program:                               # @end_program
        fence
        unimp
        unimp
        unimp
        unimp
        unimp
        unimp
        unimp
        unimp