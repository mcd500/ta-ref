#include "bench.h"
#include "config_bench.h"

#define SPLITS 32
static const char filename[] = "benchmark";


// from secure_stoage.c
void NO_PERF io_benchmark(char *buf, int size) {
    TEE_Result rv;
    int i;
    char *b;

    /* write */
    TEE_ObjectHandle object;
    rv = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
				    filename, strlen(filename),
				    (TEE_DATA_FLAG_ACCESS_WRITE
				     | TEE_DATA_FLAG_OVERWRITE),
				    TEE_HANDLE_NULL,
				    NULL, 0,
				    &object);
    for(i = 0; i < SPLITS; i++) {
        b = &buf[i*size/SPLITS];
        rv = TEE_WriteObjectData(object, (const char *)b, size/SPLITS);
    }
    TEE_CloseObject(object);

    /* read */
    rv = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
				  filename, strlen(filename),
				  TEE_DATA_FLAG_ACCESS_READ,
				  &object);
    uint32_t count;
    for(i = 0; i < SPLITS; i++) {
        b = &buf[i*size/SPLITS];
        rv = TEE_ReadObjectData(object, (const char *)b, size/SPLITS, &count);
    }
    TEE_CloseObject(object);
    return;
}
