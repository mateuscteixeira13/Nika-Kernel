#include <kernel.h>

typedef enum{
    KERNEL_SUCCESS,
    KERNEL_FAULT
} KernelStatus;

const char kernel_version[] = "nikakrnl-generic:052c3";

extern void user_main();

/**
 * A kernel initialization function
 */
KernelStatus kernel_init(multiboot_info_t* mbd, uint32_t magic){
    x86Cli();

    InitStdio();

    __serial();

    if(Hal_init() == HAL_ERROR){
        gdl_print("Error to initializing the HAL\n");
        return KERNEL_FAULT;
    }

    gdt_init();
    idt_init();
    pic_init();

    pit_init();

    if(pmm_init(mbd, magic) != 0){
       gdl_print("Error to initialize the PMM\n");
       return KERNEL_FAULT;
    }

    init_pages();

    gd_init(mbd, magic);

    heap_init();

    kybrd_init();

    mouse_init();

    x86Sti();

    /**
     * Now init a basic fork
     */ 
    __fork();

    /**
     * Initialize the RAM FileSystem
     */
    __initramfs();

    return KERNEL_SUCCESS;
}

/**
 * Main of kernel
 */
void kmain(multiboot_info_t* mbd, uint32_t magic){ 
    if(kernel_init(mbd, magic) == KERNEL_FAULT){
        gdl_print("[SYSBOOT] Fault to init kernel\n");
        return;
    }

    gdl_print("Kernel Booted!\n");
    gdl_print_color("Happy birthday Jesus Christ!\n", GREEN);
    gdl_print_color("Happy Christmas!\n", GREEN);
   


    /**
     * TODO: Will be used, now the code have a #PF and Reboot, this will be correct
     * 
     * __hiuser();
     * enter_usermode(0x10000, 0x1000, 0x800000);
     * user_main();
    */
    
    for(;;){
        schedule();
        Halt();
    }
}