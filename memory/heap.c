#include <stdint.h>
#include <stddef.h>
#include <mem.h>
#include <heap.h>

#define HEAP_PAGE_SIZE PAGE_SIZE

typedef struct HeapBlock {
    uint32_t size;
    uint8_t flags;
    struct HeapBlock* next;
} HeapBlock;

static HeapBlock* heap_start = NULL;
static HeapBlock* heap_end = NULL;
static uint8_t* heap_limit = NULL;

void heap_init() {
    /** Try to use PMM */
    uint8_t* page = (uint8_t*) pmm_alloc_page();
    heap_start = (HeapBlock*) page;
    heap_start->size = HEAP_PAGE_SIZE - sizeof(HeapBlock);
    heap_start->flags = 0;
    heap_start->next = NULL;
    heap_end = heap_start;
    heap_limit = page + HEAP_PAGE_SIZE;
}

void* malloc(uint32_t size) {
    HeapBlock* block = heap_start;
    while(block) {
        if(block->flags == 0 && block->size >= size) {
            if(block->size > size + sizeof(HeapBlock)) {
                HeapBlock* new_block = (HeapBlock*) ((uint8_t*)block + sizeof(HeapBlock) + size);
                new_block->size = block->size - size - sizeof(HeapBlock);
                new_block->flags = 0;
                new_block->next = block->next;
                block->next = new_block;
                block->size = size;
            }
            block->flags = 1;
            return (uint8_t*)block + sizeof(HeapBlock);
        }
        block = block->next;
    }

    uint8_t* page = (uint8_t*) pmm_alloc_page();
    if(!page) return NULL;
    HeapBlock* new_block = (HeapBlock*) page;
    new_block->size = HEAP_PAGE_SIZE - sizeof(HeapBlock);
    new_block->flags = 0;
    new_block->next = NULL;
    heap_end->next = new_block;
    heap_end = new_block;
    heap_limit += HEAP_PAGE_SIZE;
    return malloc(size);
}

void free(void* ptr) {
    if(!ptr) return;
    HeapBlock* block = (HeapBlock*) ((uint8_t*)ptr - sizeof(HeapBlock));
    block->flags = 0;
    HeapBlock* current = heap_start;
    while(current) {
        if(current->flags == 0 && current->next && current->next->flags == 0) {
            current->size += sizeof(HeapBlock) + current->next->size;
            current->next = current->next->next;
            continue;
        }
        current = current->next;
    }
}
