.global sdot_vec
.global end_program
.global success
.global failure
sdot_vec:                               # @sdot_vec
        addi a3, zero, 11
        slli a3, a3, 23
        csrrs zero, mstatus, a3
        addi    sp, sp, -32
        li    a3, 16
        sub     sp, sp, a3
        li a4, 8
        vsetvli        zero, a4, e16
        sd      zero, 24(sp)
        addi    a3, sp, 24
        vsub.vv v8, v8, v8
        addi    a3, sp, 32
        vse.v  v8, (a3)
        blez    a0, .LBB0_7
        li      a5, 0
        li      a3, 0
        j       .LBB0_4
.LBB0_2:                                #   in Loop: Header=BB0_4 Depth=1
        lh      t0, 0(t1)
        add     a4, a2, a7
        lh      a4, 0(a4)
        mul     a4, a4, t0
        add     a3, a3, a4
        li      a4, 1
.LBB0_3:                                #   in Loop: Header=BB0_4 Depth=1
        addw    a5, a5, a4
        bge     a5, a0, .LBB0_8
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
        slli    a4, a5, 32
        srli    a7, a4, 32
        srli    a4, a4, 31
        add     t1, a1, a4
        andi    a4, t1, 127
        slli    a7, a7, 1
        bnez    a4, .LBB0_2
        add     t0, a2, a7
        andi    a4, t0, 127
        seqz    t2, a4
        addiw   a4, a5, 7
        slt     a4, a4, a0
        and     a4, t2, a4
        beqz    a4, .LBB0_2
        vle.v v8, (t1)
        vle.v v9, (t0)
        addi    a4, sp, 32
        vle.v       v10, (a4)
        vmul.vv v8, v8, v9
        vadd.vv v8, v10, v8
        addi    a4, sp, 32
        vse.v  v8, (a4)
        li      a4, 8
        j       .LBB0_3
.LBB0_7:
        li      a3, 0
.LBB0_8:
        li      a0, 0
        addi    a1, sp, 32
        li      a2, 8
.LBB0_9:                                # =>This Inner Loop Header: Depth=1
        add     a4, a1, a0
        lh      a4, 0(a4)
        addi    a0, a0, 2
        add     a3, a3, a4
        bne     a0, a2, .LBB0_9
        slli    a0, a3, 48
        srai    a0, a0, 48
        li    a1, 16
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