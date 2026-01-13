#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct ramfs_file{
    char name[64];
    uint8_t *data;
    size_t size;
} ramfs_file_t;

typedef enum {
    SUCCESS,
    FAULT
} FS_STATUS;


typedef struct fs_ops{
    FS_STATUS(*create)(const char *name);
    FS_STATUS(*write)(const char *name, const uint8_t *buf, size_t len);
    FS_STATUS(*read)(const char *name, uint8_t *buf, size_t max_len);

    void(*list)(void);
} fs_ops_t;

typedef struct mount {
    const char *path;
    fs_ops_t *ops;
} mount_t;

void ramfs_init();
FS_STATUS ramfs_create(const char* name);
FS_STATUS ramfs_write(const char *name, const uint8_t* buf, size_t len);
FS_STATUS ramfs_read(const char *name, uint8_t* buf, size_t max_len);
FS_STATUS fs_mount(const char *path, fs_ops_t *ops);
fs_ops_t* fs_get_ops(const char *path);
void ramfs_mount(const char *path);
void ramfs_list();
void __initramfs();
