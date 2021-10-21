#if defined(__FREERTOS__)
#include "pmsis.h"
#include "pmsis_types.h"
#else
# include "Gap.h"
#endif

#ifdef __EMUL__
# include "extra_emul_stubs.h"
#endif

#include "dnn_utils.h"

#define MEMORY_POOL_SIZE 340000
struct pi_device HyperRam;


int loadLayerFromFsToL2(struct pi_device *fs, const char* file_name, void* buffer, int size)
{
    printf("Loading layer \"%s\" from FS to L2\n", file_name);
    pi_fs_file_t * file = pi_fs_open(fs, file_name, 0);
    if (file == NULL)
    {
        printf("file open failed\n");
        return 0;
    }

    if((int)file->size > size)
    {
        printf("Provided buffer size %d is smaller than file size %d\n", size, file->size);
        return -1;
    }

    pi_task_t task;
    int size_read = pi_fs_read_async(file, buffer, file->size, pi_task_block(&task));
    pi_task_wait_on(&task);
    printf("Read %d bytes from %s\n", size_read, file_name);

    pi_fs_close(file);

    return size_read;
}

void* loadLayerFromFsToL3(struct pi_device *fs, const char* file_name, struct pi_device *hyper, int* layer_size)
{
    //signed char* buff = (signed char*)memory_pool;
    signed char* buff = (signed char*) pmsis_l2_malloc( MEMORY_POOL_SIZE);

    printf("Loading layer \"%s\" from FS to L3\n", file_name);

    pi_fs_file_t * file = pi_fs_open(fs, file_name, 0);
    if (file == NULL)
    {
        printf("file open failed\n");
        return NULL;
    }
    printf("File System Opened\n");

    uint32_t hyper_buff;
    pi_ram_alloc(hyper, &hyper_buff, file->size);
    //hyper_buff = rt_hyperpi_ram_alloc(hyper, file->size);
    if(hyper_buff == NULL)
    {
        printf("HyperRAM allocation failed\n");
        return NULL;
    }

    unsigned int size_total = 0;
    unsigned int size = 0;
    pi_task_t task;
    do
    {
        //printf("Readning data to local bufer\n");
        size = pi_fs_read_async(file, buff, IO_BUFF_SIZE, pi_task_block(&task));
        pi_task_wait_on(&task);
        //printf("Read %d bytes from %s\n", size, file_name);
        size = ((size + 3) & ~3);
        if(size)
        {
            //printf("Writing data to L3\n");
            pi_ram_write(hyper, (uint32_t)(hyper_buff+size_total), buff, size);
            // printf("Writing data to L3 done\n");
            //rt_hyperpi_ram_write(hyper, buff, (uint32_t)(hyper_buff+size_total) , size, NULL);
        }
        size_total += size;
    } while(size_total < file->size);

    pi_fs_close(file);

    *layer_size = size_total;

    pmsis_l2_malloc_free(buff, MEMORY_POOL_SIZE);

    return hyper_buff;
}

void loadLayerFromL3ToL2(struct pi_device *hyper, void* hyper_buff, void* base_addr, int layer_size)
{
    pi_cl_hyper_req_t req;
    //printf("hyper_buff address: %p\n", hyper_buff);
    //printf("base_addr: %p, size %d\n", base_addr, layer_size);
    cl_ram_read(hyper, (uint32_t)hyper_buff, base_addr, layer_size, &req);
    //printf("after pi_cl_hyper_read\n");
    pi_cl_hyper_read_wait(&req);
    //printf("after pi_cl_hyper_read_wait\n");
}


