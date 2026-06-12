#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "wal.h"

int
wal_open(write_ahead_log **wal, char *wal_file_path){
    if (!wal || !wal_file_path){
        return -1;
    }

    size_t path_len = strlen(wal_file_path);
    if (path_len == 0 || path_len > FILE_PATH_MAX){
        return -1;
    }

    int fd = open(wal_file_path, O_RDWR | O_CREAT | O_APPEND, WAL_FILE_MODE);
    if(fd < 0){
        return -1;
    }

    write_ahead_log *w = calloc(1, sizeof(write_ahead_log));
    if (!w){
        close(fd);
        return -1;
    }

    w->fd = fd;

    int result = snprintf(w->file_path, sizeof(w->file_path), "%s", wal_file_path);
    if (result == -1 || result >= (int)sizeof(w->file_path)){
        close(fd);
        free(w);
        return -1;
    }

    atomic_store(&w->is_open, 1);

    *wal = w;
    return 0;
}

int
wal_close(write_ahead_log *wal){
    if (!wal){
        return -1;
    }

    if(atomic_load(&wal->is_open) == 0){
        return 0;
    }

    if (wal->fd >= 0) {
        fsync(wal->fd);
        if (close(wal->fd) != 0) {
            return -1;
        }
    }

    atomic_store(&wal->is_open, 0);
    free(wal);

    wal = NULL;

    return 0;
}

int
wal_write(write_ahead_log *wal, const void *data, uint32_t buffer_len){
    if(!wal || !data || buffer_len <= 0){
        return -1;
    }

    int is_open = atomic_load(&wal->is_open);
    if (wal->fd < 0 || is_open == 0){
        return -1;
    }

    ssize_t written = write(wal->fd, data, buffer_len);
    if (written != buffer_len){
        return -1;
    }

    return 0;
}
