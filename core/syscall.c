#include <stdint.h>
#include <idt.h>
#include <irq.h>
#include <vbe.h>
#include <paging.h>

extern void isr80_stub();
extern void set_kernel_stack(uint32_t stack);
extern uint32_t* kernel_page_dir;

typedef int (*syscall_t)(int,int,int,int);

static uint8_t kernel_stack[8192] __attribute__((aligned(16)));

int kwrite(int fd, int buf_ptr, int len, int c){
    (void)c;
    char* buf = (char*)buf_ptr;
    if(fd == 1) {
        for(int i=0; i<len; i++) {
            gdl_putchar(buf[i]); 
        }
        return len;
    }
    return -1;
}

int kexit(int code, int a, int b, int c) {
    (void)a; (void)b; (void)c;
    (void)code;
    while(1) asm("hlt");
    return 0;
}

syscall_t syscall_table[] = {
    [0] = kexit,
    [1] = kwrite
};

void syscall_handler(registers_t* regs) {
    uint32_t syscall_num = regs->eax; 
    uint32_t arg1 = regs->ebx;        
    uint32_t arg2 = regs->ecx;        
    uint32_t arg3 = regs->edx;
    uint32_t arg4 = regs->esi;
    
    int result = -1;
    
    if(syscall_num < sizeof(syscall_table)/sizeof(syscall_t) && syscall_table[syscall_num]) {
        result = syscall_table[syscall_num](arg1, arg2, arg3, arg4);
    }
    
    regs->eax = result;
}

void enter_usermode(uint32_t code_addr, uint32_t code_size, uint32_t stack_top) {
    map_user_space(kernel_page_dir, code_addr, code_size, stack_top, 0x4000);
    
    asm volatile(
        "mov %%cr3, %%eax\n"
        "mov %%eax, %%cr3\n"
        ::: "eax"
    );
    
    set_kernel_stack((uint32_t)kernel_stack + sizeof(kernel_stack));
    
    asm volatile(
        "cli\n"
        "mov $0x23, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "pushl $0x23\n"
        "pushl %0\n"
        "pushf\n"
        "popl %%eax\n"
        "orl $0x200, %%eax\n"
        "pushl %%eax\n"
        "pushl $0x1B\n"
        "pushl %1\n"
        "iret\n"
        :: "r"(stack_top), "r"(code_addr)
        : "eax"
    );
}

void __hiuser(){
    idt_set_gate(0x80, (uint32_t)isr80_stub, 0x08, 0xEE);
}