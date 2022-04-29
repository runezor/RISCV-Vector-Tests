.global sdot_vec
.global end_program
.global success
.global failure
sdot_vec:                               # @sdot_vec
        addi a3, zero, 11
        slli a3, a3, 23
        csrrs zero, mstatus, a3
        addi    sp, sp, -32
        #csrr    a3, vlenb
        li a3, 8
        slli    a3, a3, 2
        sub     sp, sp, a3
        sd      zero, 24(sp)
        li a4, 4
        vsetvli        zero, a4, e16
        addi    a3, sp, 24
        vle.v v28, (a3)
        andi    a6, a0, -4
        addi    a3, sp, 32
        vse.v  v28, (a3) #Could probably be simplified...
        blez    a6, .LBB0_6
        addi    a3, sp, 32
        vle.v       v28, (a3)
        mv      a4, zero
        mv      a5, a1
        mv      a3, a2
.LBB0_2:                                # =>This Inner Loop Header: Depth=1
        vle.v v8, (a5)
        vle.v v12, (a3)
        vmul.vv v8, v8, v12
        vadd.vv v28, v28, v8
        addi    a4, a4, 4
        addi    a3, a3, 8
        addi    a5, a5, 8
        blt     a4, a6, .LBB0_2
        addi    a3, sp, 32
        vse.v  v28, (a3)
        sext.w  a3, a4
        bge     a3, a0, .LBB0_7
.LBB0_4:
        mv      a3, zero
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
        mv      a4, zero
        sext.w  a3, a4
        blt     a3, a0, .LBB0_4
.LBB0_7:
        mv      a3, zero
.LBB0_8:
        mv      a0, zero
        addi    a1, sp, 32
        addi    a2, zero, 8
.LBB0_9:                                # =>This Inner Loop Header: Depth=1
        add     a4, a1, a0
        lh      a4, 0(a4)
        addi    a0, a0, 2
        add     a3, a3, a4
        bne     a0, a2, .LBB0_9
        slli    a0, a3, 48
        srai    a0, a0, 48
        #csrr    a1, vlenb
        li      a1, 8
        slli    a1, a1, 2
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