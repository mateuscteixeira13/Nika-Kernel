global isr80_stub
extern syscall_handler

isr80_stub:
    cli
    push byte 0
    push byte 0x80
    
    pusha
    
    push ds
    push es
    push fs
    push gs
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov eax, esp
    push eax
    call syscall_handler
    add esp, 4
    
    pop gs
    pop fs
    pop es
    pop ds
    
    popa
    
    add esp, 8
    
    sti
    iretd