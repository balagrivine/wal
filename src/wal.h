#ifndef __WAL_H__
#define __WAL_H__

#include <stdatomic.h>
#include <unistd.h>

#define WAL_FILE_MODE 0644
#define FILE_PATH_MAX 256

typedef struct{
    int fd;
    char file_path[FILE_PATH_MAX];
    _Atomic(uint32_t) is_open;
} write_ahead_log;

/*
 * wal_open
 * opens the WAL file for IO operations
 * @param wal the WAL to be opened
 * @wal_file_path the file backing the WAL
 * @return 0 if successful, -1 if unsuccessful
 */
int
wal_open(write_ahead_log **wal, char *wal_file_path);

/*
 * wal_close
 * closes a WAL and makes it unusable for IO operations
 * @param wal the WAL to be closed
 * @return 0 on success, -1 if unsuccessful
 */
int
wal_close(write_ahead_log *wal);

/*
 * wal_write
 * writes a buffer of a specified length to an open WAL file descriptor
 * @param wal the WAL structure
 * @param data the buffer we will write from
 * @param buffer_len the length of the buffer to be written to the WAL file
 * @return 0 if successful, -1 if unsuccessful
 */
int
wal_write(write_ahead_log *wal, const void *data, uint32_t buffer_len);

#endif /* __WAL_H__ */
