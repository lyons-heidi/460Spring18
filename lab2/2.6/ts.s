/*********** ts.s file of C2.6 *********/
.global reset_start

reset_start:
    LDR sp, =stack_top // set SVC stack pointer
    BL main
    B .
    