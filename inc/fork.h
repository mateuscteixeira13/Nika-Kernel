#ifndef FORK_H
#define FORK_H

#include <stdint.h>
#include <stddef.h>

typedef enum{
    READY,
    RUNNING
} proc_status_t;

typedef struct process process_t;

struct process{
    uint32_t pid;
    uint32_t eip, esp, ebp;
    uint32_t *page_dir;
    uint32_t page_dir_phys;
    proc_status_t status;
    uint32_t eflags;
    process_t* next;
};

extern process_t* current_process;
extern process_t* process_list;
extern uint32_t next_pid;


process_t* create_process(uint32_t eip,  __attribute__((unused))uint32_t esp,  __attribute__((unused))uint32_t ebp);
void schedule();
int __fork();

#endif