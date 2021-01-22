#include "bench.h"
#include "config_bench.h"

#define SPLITS 32
// from secure_stoage.c

/**
 * io_write_benchmark() - It is about using the TEE objects like handling 
 * the object write memory benchmark and splits the size of the storage buffer 
 * and closing the created object.
 * 
 * This function firstly declared required variables using the TEE_objectHandle 
 * handle on an object. These handles are returned by the functions like
 * TEE_CreatePersistentObject function, It creates a persistent object with 
 * initial attributes of fname, data flags access write and an initial data 
 * stream content optionally returns either a handle on the created object or 
 * handle failure. Using the loop condtition based on the splits for each 
 * iteration some manipulation of buffer size with splits that result will store 
 * in the variable b, TEE_WriteObjectData function that function writes size 
 * bytes from the b pointed to by buffer to the data stream associated with the open 
 * object handle object. that values stored in rv variable. Finally closed the 
 * created object using closeobject function. 
 * 
 * @param buf      A pointer to a buffer which will be written to the file.
 * @param fname    The pointer type argument for filename
 * @param size     The length of the buffer.
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
 * io_read_benchmark() - It is about using the TEE objects like handling 
 * the object read memory benchmark and splits the size of the storage buffer 
 * and closing the created object.
 * 
 * This function firstly declared required variables using the TEE_objectHandle 
 * handle on an object. These handles are returned by the functions like
 * TEE_CreatePersistentObject function, It creates a persistent object with 
 * initial attributes of fname, data flags access read and an initial data 
 * stream content optionally returns either a handle on the created object or 
 * handle failure. Using the loop condtition based on the splits for each 
 * iteration some manipulation of buffer size with splits that result will store 
 * in the variable b, TEE_WriteObjectData function that function writes size 
 * bytes from the b pointed to by buffer to the data stream associated with the open 
 * object handle object. that values stored in rv variable. Finally closed the 
 * created object using closeobject function. 
 * 
 * @param buf      A pointer to a buffer which will be written to the file.
 * @param fname    The pointer type argument for filename
 * @param size     The length of the buffer.
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
