    .globl	_start
_start:
    /* Clear the link register, so debuggers know to terminate their
     * backtrace. */
    /* Setup the global pointer, which the ABI assumes points to the
     * __global_pointer$ symbol. */
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    /* Setup the stack pointer.  This symbol will only be looked at
     * here, so you can pick any name you want. */
    la sp, _sstack

    /* Call main, leaving the arguments undefined. */
    call main

    /* Spin after main() returns. */
    1:
    j 1b