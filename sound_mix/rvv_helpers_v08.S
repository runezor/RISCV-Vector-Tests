.global sat_add_vectorised
.global success
.global failure
sat_add_vectorised:                     # @sat_add_vectorised
        addi t0, zero, 11
        slli t0, t0, 23
        csrrs zero, mstatus, t0
        andi    a7, a0, -4
        li      t0, 4
        vsetvli a6, t0, e16
        li      t0, 0
        blez    a7, .LBB0_3
        mv      t1, a1
        mv      a4, a2
        mv      a5, a3
.LBB0_2:                                # =>This Inner Loop Header: Depth=1
        vle.v v8, (t1)
        vle.v v9, (a4)
        addi    t0, t0, 4
        addi    a4, a4, 8
        addi    t1, t1, 8
        vsaddu.vv v8, v8, v9
        vse.v v8, (a5)
        addi    a5, a5, 8
        blt     t0, a7, .LBB0_2
.LBB0_3:
        sext.w  a4, t0
        bge     a4, a0, .LBB0_8
        slli    a4, t0, 32
        srli    a6, a4, 32
        slli    a0, a0, 32
        srli    a5, a0, 32
        srli    a4, a4, 31
        add     a0, a3, a4
        add     a2, a2, a4
        add     a1, a1, a4
        sub     a3, a5, a6
        lui     a4, 16
        addiw   a6, a4, -1
        j       .LBB0_6
.LBB0_5:                                #   in Loop: Header=BB0_6 Depth=1
        sh      a5, 0(a0)
        addi    a0, a0, 2
        addi    a2, a2, 2
        addi    a3, a3, -1
        addi    a1, a1, 2
        beqz    a3, .LBB0_8
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
        lhu     a5, 0(a1)
        lhu     a4, 0(a2)
        add     a5, a5, a4
        bltu    a5, a6, .LBB0_5
        mv      a5, a6
        j       .LBB0_5
.LBB0_8:
        ret
success:
        addi a1, a1, 0
        ret
failure:
        addi a2, a2, 0
        ret