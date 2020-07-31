;
; terminaldemo.s
;
; This is a simple demo program that prints a bunch of characters to the screen
; by writing to video memory. The first character in the upper left corner of
; the screen is stored at address 0xB8000, and each character occupies two
; bytes. A diagram of the screen with associated locations in video memory is
; shown below.
;
; Use `make` to compile this program and `make run` to run it in qemu.
; `make debug` will step thru in gdb.
;
;
;  /-- 0xB8000
;  | /-- 0xB8002
;  | | /-- 0xB8004
;  | | |
; |-----------------------------------------------------------|
; |A B C                                                      |
; |                                                           |
; |                                                           |
; |                                                           |
; |                                                           |
; |                                                           |
; |                                                           |
; |                                                           |
; |                                                           |
; |                                                           |
; |                                                           |
; |                                                           |
; |                                                           |
; |                                                           |
; |                                                           |
; |                                                           |
; |-----------------------------------------------------------|
;
    section .text

global _start
extern main      ; declare main
_start:
    call main        ; Call main
    hlt
    lea esi,[0xb8000] ; Pointer to terminal buffer in AX
    mov bl, 'A'      ; Character to print in BL
    mov cx,0         ; Character count in CX
.loop:
    inc cx           ; Increment character count
    cmp cx,500       ; Check if we've written 500 characters yet...
    jg .done         ; If we have written more than 100 characters, stop

    mov [esi],bl     ; Write character to terminal buffer
    add esi,2        ; Increment terminal buffer pointer to next character position
    add bl,1         ; Increment ASCII
    cmp bl, 'Z'      ; Check if end of alphabet
    jle .loop        ; If not, keep writing
    mov bl,'A'       ; If end of alphabet, start back over at A
    jmp .loop

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
