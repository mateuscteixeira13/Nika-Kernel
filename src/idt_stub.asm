global idt_load
idt_load:
    mov eax, [esp + 4]
    lidt [eax] ; Load IDT
    ret