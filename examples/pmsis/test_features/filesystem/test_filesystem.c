/* PMSIS includes */
#include "pmsis.h"
#include "bsp/fs.h"
#include "bsp/fs/readfs.h"
#include "bsp/flash/hyperflash.h"

/* Variables used. */
#define BUFFER_SIZE      ( 1024 )
#define COUNT            ( 5 )
#define NB_FILE          ( 1 )

static char buff[BUFFER_SIZE];
static uint32_t errors = 0;

static void check_file(pi_fs_file_t *file)
{
    printf("Check file on cluster %ld\n", pi_cluster_id());
    int32_t size = 0;
    uint32_t size_total = 0;
    uint32_t count_done = 0;
    #if defined(USE_CLUSTER)
    pi_cl_fs_req_t req = {0};
    #else
    pi_task_t task = {0};
    pi_task_block(&task);
    #endif  /* USE_CLUSTER */
    for (count_done = 0; count_done < COUNT; count_done++)
    {
        do
        {
            /* Read from filesystem(on flash) to a buffer in L2 memory. */
            #if defined(USE_CLUSTER)
            pi_cl_fs_read(file, buff, BUFFER_SIZE, &req);
            size = pi_cl_fs_wait(&req);
            #else
            size = pi_fs_read_async(file, buff, BUFFER_SIZE, &task);
            pi_task_wait_on(&task);
            #endif  /* USE_CLUSTER */
            printf("%ld) Read %ld bytes : \n%s\n", count_done, size, buff);
            size_total += size;
        } while (size_total < file->size);
        #if defined(USE_CLUSTER)
        /* Reinit offset. */
        pi_cl_fs_seek(file, 0, &req);
        pi_cl_fs_wait(&req);
        #else
        /* Reinit offset. */
        pi_fs_seek(file, 0);
        #endif  /* USE_CLUSTER */
        errors += (size_total != file->size);
        size_total = 0;
    }
}

#if defined(USE_CLUSTER)
void open_cluster(struct pi_device *cluster)
{
    /* Init & open cluster if used. */
    struct pi_cluster_conf cluster_conf;
    pi_cluster_conf_init(&cluster_conf);
    cluster_conf.id = 0;
    pi_open_from_conf(cluster, &cluster_conf);
    if (pi_cluster_open(cluster))
    {
        printf("Error cluster open !\n");
        pmsis_exit(-3);
    }
}
#endif  /* USE_CLUSTER */

void open_filesystem(struct pi_device *flash, struct pi_device *fs)
{
    struct pi_readfs_conf conf;
    struct pi_hyperflash_conf flash_conf;

    /* Init & open flash. */
    pi_hyperflash_conf_init(&flash_conf);
    pi_open_from_conf(flash, &flash_conf);
    if (pi_flash_open(flash))
    {
        printf("Error flash open !\n");
        pmsis_exit(-1);
    }

    /* Open filesystem on flash. */
    pi_readfs_conf_init(&conf);
    conf.fs.flash = flash;
    pi_open_from_conf(fs, &conf);
    if (pi_fs_mount(fs))
    {
        printf("Error FS mounting !\n");
        pmsis_exit(-2);
    }
}

void test_fs(void)
{
    printf("Entering main controller\n");

    pi_fs_file_t *file[NB_FILE] = {0};
    char *name[NB_FILE] = {"hello.txt"};//, "Makefile"};

    struct pi_device fs;
    struct pi_device flash;
    open_filesystem(&flash, &fs);

    #if defined(USE_CLUSTER)
    printf("Use cluster\n");
    struct pi_device cluster = {0};
    struct pi_cluster_task cluster_task = {0};
    open_cluster(&cluster);
    cluster_task.stack_size = 1024;
    cluster_task.entry = (void *) check_file;
    cluster_task.arg = NULL;
    #endif  /* USE_CLUSTER */

    for (uint32_t i = 0; i < (uint32_t) NB_FILE; i++)
    {
        /* Open file from filesystem. */
        file[i] = pi_fs_open(&fs, name[i], 0);
        if (file[i] == NULL)
        {
            printf("File %s open failed !\n", name[i]);
            pmsis_exit(-4 - i);
        }
        printf("File %s open success.\n", name[i]);
    }

    for (uint32_t i = 0; i < (uint32_t) NB_FILE; i++)
    {
        printf("\nFile %s: %d bytes\n", name[i], file[i]->size);
        #if defined(USE_CLUSTER)
        cluster_task.arg = file[i];
        pi_cluster_send_task_to_cl(&cluster, &cluster_task);
        #else
        check_file(file[i]);
        #endif  /* USE_CLUSTER */
    }

    for (uint32_t i = 0; i < (uint32_t) NB_FILE; i++)
    {
        pi_fs_close(file[i]);
        printf("File %s closed.\n", name[i]);
    }

    #if defined(USE_CLUSTER)
    pi_cluster_close(&cluster);
    #endif  /* USE_CLUSTER */
    pi_fs_unmount(&fs);
    printf("FS unmounted.\n");
    pi_flash_close(&flash);

    printf("Test %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Filesystem Test ***\n\n");
    return pmsis_kickoff((void *) test_fs);
}
