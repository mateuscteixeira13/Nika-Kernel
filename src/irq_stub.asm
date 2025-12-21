extern irq_handler ; declareted in C

%macro IRQ 2
global irq%1
irq%1:
    cli
    push byte 0
    push byte %2
    jmp irq_common_stub
%endmacro

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47


irq_common_stub:
    pusha

    mov ax, ds
    push eax
    
    ; Here load the segments of the kernel
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call the handler of C
    push esp
    call irq_handler
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa

    add esp, 8

    iret ; return

global page_fault_stub
extern page_fault_handler

page_fault_stub:
    cli
    pushad

    mov eax, cr2
    push eax        ; addr
    push dword [esp + 36] ; error code

    call page_fault_handler

    add esp, 8
    popad
    sti
    iretd
