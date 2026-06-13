#include <assert.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../src/wal.h"

void test_wal_open(){
    char wal_file_path[128] = "file.wal";
    write_ahead_log *wal = NULL;
    int ret = wal_open(&wal, wal_file_path, WAL_SYNC_NONE);
    assert(ret == 0);
    assert(wal != NULL);
    assert(wal->is_open == 1);
    assert(strcmp(wal->file_path, wal_file_path) == 0);

    free(wal);
    remove(wal_file_path);
}

void test_wal_close(){
    char wal_file_path[128] = "file.wal";
    write_ahead_log *wal = NULL;
    wal_open(&wal, wal_file_path, WAL_SYNC_NONE);
    int ret = wal_close(wal);
    assert(ret == 0);

    remove(wal_file_path);
}

void test_wal_write(){
    char wal_file_path[128] = "file.wal";
    unsigned char data[128] = "Hello world";
    write_ahead_log *wal = NULL;

    if(wal_open(&wal, wal_file_path, WAL_SYNC_NONE) != 0) return;

    // Happy case: successfully writes data to file if WAL is valid
    int ret = wal_write(wal, data, sizeof(data));
    assert(ret == 0);

    // Sad case: fails to write data to WAL file is wal is not open
    atomic_store(&wal->is_open, 0);
    ret = wal_write(wal, data, sizeof(data));
    assert(ret == -1);

    // Sad case: fails to write data to WAL file if file descriptor is invalid
    wal->fd = -1;
    ret = wal_write(wal, data, sizeof(data));
    assert(ret == -1);

    // Sad case: fails to write data to WAL file if WAL is null
    wal->fd = -1;
    ret = wal_write(NULL, data, sizeof(data));
    assert(ret == -1);

    // Sad case: fails to write data to WAL file if data is nul
    wal->fd = -1;
    ret = wal_write(wal, NULL, sizeof(data));
    assert(ret == -1);

    // Sad case: fails to write data to WAL file if buffer_len is invalid
    wal->fd = -1;
    ret = wal_write(wal, data, -1);
    assert(ret == -1);

    wal_close(wal);
    remove(wal_file_path);
}

int main(void){
    test_wal_open();
    test_wal_close();
    test_wal_write();
    printf("All tests passed.\n");
    return 0;
}
