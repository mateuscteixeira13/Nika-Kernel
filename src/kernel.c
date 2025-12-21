#include <vga.h>
#include <hal.h>
#include <mem.h>
#include <multiboot.h>
#include <fork.h>
#include <ramfs.h>

typedef enum{
    KERNEL_SUCCESS,
    KERNEL_FAULT
} KernelStatus;

#define RELEASE "v0.52"

/**
 * A kernel initialization function
 */
KernelStatus kernel_init(multiboot_info_t* mbd, uint32_t magic){
    x86Cli();

    InitStdio();

    if(Hal_init(mbd, magic) == HAL_ERROR){
        Stdout("Error to initializing the HAL\n");
        return KERNEL_FAULT;
    }

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
        Stdout("[SYSBOOT] Fault to init kernel\n");
        return;
    }

    Stdout_color("\nNika Kernel booted!\n", 0x0B);
    Stdout("Nika Kernel info:\n");
    kprintf("Version: %s\n", RELEASE);
   
    for(;;){
        schedule();
        CPU_halt();
    }
}