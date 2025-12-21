#include <stdint.h>
#include <stddef.h>
#include <mem.h>
#include <heap.h>

#define HEAP_PAGE_SIZE PAGE_SIZE

static uint8_t* heap_base = NULL;
static uint32_t heap_offset = 0;
static uint32_t heap_size = 0;

void heap_init(){
    heap_base = (uint8_t*) pmm_alloc_page();
    heap_offset = 0;
    heap_size = HEAP_PAGE_SIZE;
}

void* malloc(uint32_t size){
    if(heap_offset + size > heap_size){
        /**
         * Try to call memory of PMM
         */
        uint8_t* new_page = (uint8_t*) pmm_alloc_page();
        if(!new_page){
            return NULL;
        }

        heap_base = new_page;
        heap_offset = 0;
        heap_size = HEAP_PAGE_SIZE;
        
    }

    void* ptr = heap_base + heap_offset;
    heap_offset += size;
    return ptr;
}

void free(__attribute__((unused)) void* ptr){
    // TODO: This will be implemented
}

void *memset(void *dst, int val, size_t size){
    uint8_t *d = (uint8_t *)dst;
    while (size--){
        *d++ = (uint8_t)val;
    }
    
    return dst;
}

void *memcpy(void *dest, const void *src, size_t n){
    uint8_t *d = dest;
    const uint8_t *s = src;
    for(size_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}
