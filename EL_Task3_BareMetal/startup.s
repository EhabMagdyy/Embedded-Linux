.section ".text.boot"       // Place this at the very start of .text
.global _start

_start:
    // Only core 0 runs; park other cores immediately
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbnz    x1, hang

    // Set up stack pointer, grows downward from 0x80000
    ldr     x1, =0x80000
    mov     sp, x1

    // Zero out BSS section
    ldr     x1, =__bss_start
    ldr     x2, =__bss_end
bss_loop:
    cmp     x1, x2
    b.ge    bss_done
    str     xzr, [x1], #8
    b       bss_loop
bss_done:

    // Jump to C main()
    bl      main

    // If main() ever returns, hang forever
hang:
    wfe
    b       hang