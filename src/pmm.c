#include <stdint.h>
#include <stddef.h>
#include <vga.h>
#include <multiboot.h>
#include <mem.h>
#include <heap.h>

#define MAX_MEM_BLOCKS 256


static uint8_t *pmm_bitmap;
static uint32_t pmm_bitmap_size;

#define MAGIC_INVALID -1
#define MEMMAP_INVALID -2

/**
 * Call extern declarations
 */
extern uint32_t kernel_start;
extern uint32_t kernel_end;

enum MemType{
    USABLE = 1,
    RESERVED = 2,
    ACPI_RECLAIMABLE = 3,
    ACPI_NVS = 4,
    BAD_RAM = 5
};

typedef struct {
    uint32_t Begin;
    uint32_t End;
    uint32_t Type;
    uint32_t ACPI;
} MemoryBlock;

MemoryBlock memMap[MAX_MEM_BLOCKS];
size_t memBlockCount = 0;

static uint32_t total_pages = 0;

/**
 * Get the memory map of GRUB (OSDev)
 */
int get_memmap(multiboot_info_t *mbd, uint32_t magic){
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC){
        panic("Invalid magic number!\n");
        return MAGIC_INVALID;
    }

    if (!(mbd->flags & (1 << 6))){
        panic("No memory map from GRUB\n");
        return MEMMAP_INVALID;
    }

    for (uint32_t i = 0; i < mbd->mmap_length;){
        multiboot_memory_map_t *mmmt =
            (multiboot_memory_map_t *)(mbd->mmap_addr + i);

        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE &&
            memBlockCount < MAX_MEM_BLOCKS){

            uint64_t base = mmmt->addr;
            uint64_t length = mmmt->len;
            uint64_t end = base + length;

            if (base < 0x100000000ULL){
                if (end > 0x100000000ULL)
                    end = 0x100000000ULL;

                memMap[memBlockCount].Begin = (uint32_t)base;
                memMap[memBlockCount].End = (uint32_t)end;
                memMap[memBlockCount].Type = USABLE;
                memMap[memBlockCount].ACPI = 0;
                memBlockCount++;
            }
        }

        i += mmmt->size + sizeof(mmmt->size);
    }

    return 0;
}

/**
 * Initialize the Physical Memory Manager
 */
int pmm_init(multiboot_info_t *mbd, uint32_t magic){
    get_memmap(mbd, magic);
     
    for(size_t i = 0; i < memBlockCount; i++){
        uint32_t size = memMap[i].End - memMap[i].Begin;
        total_pages += size / PAGE_SIZE;
    }

    pmm_bitmap_size = (total_pages + 7) / 8;
    pmm_bitmap = (uint8_t*)0x100000;
    memset(pmm_bitmap, 0xFF, pmm_bitmap_size);

    for(size_t i = 0; i < memBlockCount; i++){
        for(uint32_t addr = memMap[i].Begin; addr < memMap[i].End; addr += PAGE_SIZE){
            pmm_mark_free(addr);
        }
    }

    /**
     * Reserve kernel
     */
    pmm_reserve_kernel();

    uint32_t bitmap_start = (uint32_t)pmm_bitmap;
    uint32_t bitmap_end = bitmap_start + pmm_bitmap_size;

    /**
     * Reserve bitmap
     */
    for(uint32_t addr  = bitmap_start; addr < bitmap_end; addr += PAGE_SIZE ){
        uint32_t page = addr/ PAGE_SIZE;
        pmm_bitmap[page / 8 ] |= (1 << (page % 8));
    }

    /**
     * Reserve VGA addr
     */
    uint32_t vga_page = 0xB8000 / PAGE_SIZE;
    pmm_bitmap[vga_page / 8 ] |= (1 << (vga_page % 8));

    return 0;
}

void pmm_reserve_kernel(){
    for(uint32_t addr  = (uint32_t)&kernel_start; addr < (uint32_t)&kernel_end; addr += PAGE_SIZE){
        uint32_t page = addr / PAGE_SIZE;
        pmm_bitmap[page / 8] |= (1 << (page % 8));
    }
}

void pmm_mark_free(uint32_t addr){
    uint32_t page = addr / PAGE_SIZE;
    pmm_bitmap[page / 8] &= ~(1 << (page % 8));
}

void pmm_mark_used(uint32_t addr){
    uint32_t page = addr / PAGE_SIZE;
    pmm_bitmap[page / 8] |= (1 << (page % 8));
}

void* pmm_alloc_page(){
    for(uint32_t i = 0; i < total_pages; i++){
        if(!(pmm_bitmap[i / 8 ] & (1 << (i % 8)))){
            pmm_bitmap[i / 8] |= (1 << (i % 8));
            return (void*)(i *  PAGE_SIZE);
        }
    }

    return NULL;
}