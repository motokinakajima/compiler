.global _main
_main:
        sub sp, sp, #16
        str x29, [sp]
        mov x29, sp
        sub sp, sp, 32

        //generate lval address start

        mov x0, x29
        sub x0, x0, 16
        sub sp, sp, #16
        str x0, [sp]

        //generate lval address end

        mov x0, 0
        sub sp, sp, #16
        str x0, [sp]

        //assign start

        ldr x1, [sp]
        add sp, sp, #16
        ldr x0, [sp]
        add sp, sp, #16
        str x1, [x0]
        sub sp, sp, #16
        str x1, [sp]

        //assign end

        ldr x0, [sp]
        add sp, sp, #16

        //for clause start

        .Lbegin_0:

        //load value start


        //generate lval address start

        mov x0, x29
        sub x0, x0, 16
        sub sp, sp, #16
        str x0, [sp]

        //generate lval address end

        ldr x0, [sp]
        add sp, sp, #16
        ldr x0, [x0]
        sub sp, sp, #16
        str x0, [sp]

        //load value end

        mov x0, 5
        sub sp, sp, #16
        str x0, [sp]
        ldr x1, [sp]
        add sp, sp, #16
        ldr x0, [sp]
        add sp, sp, #16
        cmp x0, x1
        cset x0, lt
        sub sp, sp, #16
        str x0, [sp]
        ldr x0, [sp]
        add sp, sp, #16
        cmp x0, 0
        b.eq .Lend_1

        //block start


        //generate lval address start

        mov x0, x29
        sub x0, x0, 16
        sub sp, sp, #16
        str x0, [sp]

        //generate lval address end


        //load value start


        //generate lval address start

        mov x0, x29
        sub x0, x0, 16
        sub sp, sp, #16
        str x0, [sp]

        //generate lval address end

        ldr x0, [sp]
        add sp, sp, #16
        ldr x0, [x0]
        sub sp, sp, #16
        str x0, [sp]

        //load value end

        mov x0, 1
        sub sp, sp, #16
        str x0, [sp]
        ldr x1, [sp]
        add sp, sp, #16
        ldr x0, [sp]
        add sp, sp, #16
        add x0, x0, x1
        sub sp, sp, #16
        str x0, [sp]

        //assign start

        ldr x1, [sp]
        add sp, sp, #16
        ldr x0, [sp]
        add sp, sp, #16
        str x1, [x0]
        sub sp, sp, #16
        str x1, [sp]

        //assign end


        //generate lval address start

        mov x0, x29
        sub x0, x0, 32
        sub sp, sp, #16
        str x0, [sp]

        //generate lval address end


        //load value start


        //generate lval address start

        mov x0, x29
        sub x0, x0, 16
        sub sp, sp, #16
        str x0, [sp]

        //generate lval address end

        ldr x0, [sp]
        add sp, sp, #16
        ldr x0, [x0]
        sub sp, sp, #16
        str x0, [sp]

        //load value end


        //assign start

        ldr x1, [sp]
        add sp, sp, #16
        ldr x0, [sp]
        add sp, sp, #16
        str x1, [x0]
        sub sp, sp, #16
        str x1, [sp]

        //assign end


        //block end

        b .Lbegin_0
        .Lend_1:

        //for clause end

        ldr x0, [sp]
        add sp, sp, #16
        bl _foo

        //if clause start

        ldr x0, [sp]
        add sp, sp, #16
        cmp x0, 0
        b.eq .Lend_2
        .Lend_2:

        //if clause end

        ldr x0, [sp]
        add sp, sp, #16

        //return start


        //load value start


        //generate lval address start

        mov x0, x29
        sub x0, x0, 32
        sub sp, sp, #16
        str x0, [sp]

        //generate lval address end

        ldr x0, [sp]
        add sp, sp, #16
        ldr x0, [x0]
        sub sp, sp, #16
        str x0, [sp]

        //load value end

        ldr x0, [sp]
        add sp, sp, #16
        mov sp, x29
        ldr x29, [sp]
        add sp, sp, #16
        mov x0, #0
        mov x16, #1
        svc #0
        ret

        //return end

        ldr x0, [sp]
        add sp, sp, #16
.extern _foo
