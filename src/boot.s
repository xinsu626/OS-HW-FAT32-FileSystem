
    section .text

    global _start
    extern main
_start:
    call main

.done:
    jmp .done       ; Loop forever once we've filled the screen

    section .multiboot

    ; Multiboot 2 Header
    ; Needed to be loadable by GRUB
multiboot_header:
    dd  0xe85250d6
    dd  0
    dd  16
    dd  -(16+0xe85250d6)

    dw  0
    dw  0
    dd  12
