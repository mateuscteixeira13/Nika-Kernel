#include <kernel.h>

typedef enum{
    KERNEL_SUCCESS,
    KERNEL_FAULT
} KernelStatus;

const char kernel_version[] = "nikakrnl-generic:052c3";

/**
 * A kernel initialization function
 */
KernelStatus kernel_init(multiboot_info_t* mbd, uint32_t magic){
    x86Cli();

    if(!gd_init(mbd, magic)){
        WARN("GD just failed\n");
    }

    InitStdio();

    gdt_init();
    idt_init();
  
    pic_init();
    pit_init();

    if(pmm_init(mbd, magic) != 0){
       kDebug("Error to initialize the PMM\n");
       return KERNEL_FAULT;
    }
    
    init_pages();   
    
    if(!gd_init(mbd, magic)){
        WARN("GD just failed\n");
    }

    INFO("Maping VGA memory to virtual context\n");
    map_framebuffer((uintptr_t)vga, VGA_WIDTH * VGA_HEIGHT * 2);

    INFO("Success!\n");

    INFO("Initializing heap\n");
    heap_init();

    INFO("Success!\n");

    INFO("Initializing keyboard device\n");
    kybrd_init();

    INFO("Success!\n");

    INFO("Initializing mouse device\n");
    mouse_init();

    INFO("Success!\n");

    x86Sti();

    INFO("Initializing video driver(BIOS Mode)\n");
    
    INFO("Success!\n");

    INFO("Initializing PCI Subsystem\n");
    pcisub_init();

    INFO("Success!\n");

    /**
     * Now init a basic fork
     */ 
    INFO("Initializing fork\n");
    __fork();

    INFO("Success!\n");

    /**
     * Initialize the RAM FileSystem
     */
    INFO("Initializing RAM FileSystem\n");
    __initramfs();

    INFO("Success!\n");

    return KERNEL_SUCCESS;
}

/**
 * Main of kernel
 */
void kmain(multiboot_info_t* mbd, uint32_t magic){ 
    if(kernel_init(mbd, magic) == KERNEL_FAULT){
        kDebug("[SYSBOOT] Fault to init kernel\n");
        return;
    }

    INFO("Kernel just booted successfully!\n");
    for(;;){
        schedule();
        Halt();
    }
}