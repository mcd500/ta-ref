#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

/**
 * profiler_write() - Performs the file operations like open, write and close.
 * 
 * This function performs the three actions - opens the log file, writes into  
 * file and closes the file. It returns 0 when the file performance is done. 
 * Upon the failure of file it returns -1.
 * 
 * @param ptr		This is the pointer to the array of elements to be written.
 * @param sz		This is the size in bytes of each element to be written.
 *
 * @return 0 		If success, else error occured.
 */
int profiler_write(void *ptr, uint64_t sz) {
        int fd = open(LOG_FILE, O_RDWR | O_CREAT, (mode_t)0600);
        if(fd == 0) return -1;
        if(write(fd, ptr, sz) <= 0) {
            return -1;
        }
        if(close(fd) == -1) return -1;
    return 0;
}
