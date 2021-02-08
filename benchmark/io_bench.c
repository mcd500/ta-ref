#include "bench.h"
#include "config_bench.h"

#define SPLITS 32
// from secure_stoage.c

/**
 * io_write_benchmark() - About input and output write benchmark.
 *
 * This function creates a persistent object with initial attributes and an initial
 * data stream content, and optionally returns either a handle on the created object
 * or TEE_HANDLE_NULL upon failure. Using the for_loop based on the SPLITS value
 * it will write the object data. TEE_WriteObjectData function writes "size/SPLITS" 
 * bytes from the "b" pointed to by buffer to the data stream associated with the 
 * open object handle object. Finally it will close the object.  
 *
 * @param buf		A pointer to a buffer which will be written to the file.
 * @param fname		The pointer type argument for filename
 * @param size		The length of the buffer.
 */
void NO_PERF io_write_benchmark(char *buf, char *fname, int size) {
    TEE_Result rv;
    int i;
    char *b;
    /* write */
    TEE_ObjectHandle object;
    rv = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
				    fname, strlen(fname),
				    (TEE_DATA_FLAG_ACCESS_WRITE
				     | TEE_DATA_FLAG_OVERWRITE),
				    TEE_HANDLE_NULL,
				    NULL, 0,
				    &object);
    for(i = 0; i < SPLITS; i++) {
        b = &buf[i*size/SPLITS];
        rv = TEE_WriteObjectData(object, b, size/SPLITS);
    }
    TEE_CloseObject(object);
    return;
}

/**
 * io_read_benchmark() - About input and output read benchmark.
 *
 * This function creates a persistent object with initial attributes and an initial
 * data stream content, and optionally returns either a handle on the created object
 * or TEE_HANDLE_NULL upon failure. Using the for_loop based on the SPLITS value
 * it will read the object data. TEE_ReadObjectData function reads "size/SPLITS" 
 * bytes from the "b" pointed to by buffer to the data stream associated with the 
 * open object handle object. Finally it will close the object.  
 *
 * @param buf		A pointer to a buffer which will be written to the file.
 * @param fname		The pointer type argument for filename
 * @param size		The length of the buffer.
 */
void NO_PERF io_read_benchmark(char *buf, char *fname, int size) {
    TEE_Result rv;
    int i;
    char *b;
    /* write */
    TEE_ObjectHandle object;
    rv = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
				  fname, strlen(fname),
				  TEE_DATA_FLAG_ACCESS_READ,
				  &object);
    uint32_t count;
    for(i = 0; i < SPLITS; i++) {
        b = &buf[i*size/SPLITS];
        rv = TEE_ReadObjectData(object, b, size/SPLITS, &count);
    }
    TEE_CloseObject(object);
    return;
}
