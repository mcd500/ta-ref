#include "bench.h"
#include "config_bench.h"

#define SPLITS 32
// from secure_stoage.c
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
