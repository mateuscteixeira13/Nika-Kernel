#include <stdint.h>
#include <stddef.h>
#include <lib.h>
#include <heap.h>
#include <vga.h>
#include <stdbool.h>
#include <ramfs.h>
#include <vbe.h>

#define Stdout(str) gdl_print(str)


/**
 * A simple rootfs in RAM :)
 */

#define RAMFS_MAX_FILES 64
#define RAMFS_MAX_FILE_SIZE 1024
#define MAX_MOUNTS 16

bool fs_true = false;
fs_ops_t ramfs_ops;

ramfs_file_t ramfs_files[RAMFS_MAX_FILES];
static size_t ramfs_file_count = 0;

static mount_t mounts[MAX_MOUNTS];
static size_t mount_count = 0;

#define MAX_MOUNTS 16


void ramfs_init(){
    memset(ramfs_files, 0, sizeof(ramfs_files));
    ramfs_file_count = 0;

    fs_true = true;
}

FS_STATUS ramfs_create(const char* name){
    if(fs_true != true){
        Stdout("Ocorred a error: ROOT is not avaliable! :(\n");
        return FAULT;
    }

    if(ramfs_file_count >= RAMFS_MAX_FILES){
        Stdout("Ocorred a error the file excedds the ROOT max files\n");
        return -1;
    }

    ramfs_file_t* f = &ramfs_files[ramfs_file_count++];
    strncpy(f->name, name, 63);
    f->data = (uint8_t*) malloc(RAMFS_MAX_FILE_SIZE);
    if(f->data){
        return -1;
    }

    f->size = 0;
    memset(f->data, 0, RAMFS_MAX_FILE_SIZE);
    return SUCCESS;
}

FS_STATUS ramfs_write(const char *name, const uint8_t* buf, size_t len){
    if(fs_true != true){
        Stdout("Ocorred a error: ROOT is not avaliable! :(\n");
        return FAULT;
    }

    for(size_t i = 0; i < ramfs_file_count; i++){
        if(strcmp(ramfs_files[i]. name, name) == 0){
            if(len > RAMFS_MAX_FILE_SIZE){
                len = RAMFS_MAX_FILE_SIZE;
                memcpy(ramfs_files[i].data, buf, len);
                ramfs_files[i].size = len;

                return (int)len;
            }
        }
    }

    return FAULT;
}

FS_STATUS ramfs_read(const char *name, uint8_t* buf, size_t max_len){
    if(fs_true != true){
        Stdout("Ocorred a error: ROOT is not avaliable! :(\n");
        return FAULT;
    }

    for(size_t i = 0; i < ramfs_file_count; i++){
        if(strcmp(ramfs_files[i]. name, name) == 0){
            size_t len = ramfs_files[i].size;
            if(len > max_len){
                len = max_len;
                memcpy(buf, ramfs_files[i].data, len);
    
                return (int)len;
            }
        }
    }

    return FAULT;
}


FS_STATUS fs_mount(const char *path, fs_ops_t *ops){
    if(mount_count >= MAX_MOUNTS){
        kprintf("fs_mount: máximo de mounts atingido!\n");
        return FAULT;
    }
    
    if(fs_true != true){
        Stdout("Ocorred a error: ROOT is not avaliable! :(\n");
        return FAULT;
    }

    mounts[mount_count].path = path;
    mounts[mount_count].ops  = ops;
    mount_count++;

    return SUCCESS;  
}

fs_ops_t* fs_get_ops(const char *path){
    for(size_t i = 0; i < mount_count; i++){
        if(strcmp(mounts[i].path, path) == 0){
            return mounts[i].ops;
        }
    }
    return NULL;
}

void ramfs_mount(const char *path){
    ramfs_ops.create = ramfs_create;
    ramfs_ops.write  = ramfs_write;
    ramfs_ops.read   = ramfs_read;
    ramfs_ops.list   = ramfs_list;

    fs_mount(path, &ramfs_ops);
}

void ramfs_list(){
    if(fs_true != true){
        Stdout("Ocorred a error: ROOT is not avaliable! :(\n");
        return;
    }
    
    kprintf("r:/? %(u):\n", (unsigned)ramfs_file_count);
    for(size_t i = 0; i < ramfs_file_count; i++){
        kprintf(" - %s (%u bytes)\n", ramfs_files[i].name, (unsigned)ramfs_files[i].size);
    }
}

void __initramfs(){
    ramfs_init();
    Stdout("ROOT: RAMFS initialized!\n");
    Stdout("Mounting r:\n");
    ramfs_mount("r:/");
    Stdout("Mounted r: at RAM (Root dir)\n");
}