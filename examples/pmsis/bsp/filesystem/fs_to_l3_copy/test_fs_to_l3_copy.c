/* PMSIS includes */
#include "pmsis.h"
#include "bsp/fs.h"
#include "bsp/ram.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/ram/hyperram.h"
#include "bsp/fs/readfs.h"

/* Variables used. */
#define  BUFF_SIZE       ( 1024 )

static uint8_t *buff;

static struct pi_device flash;
static struct pi_device fs;
static struct pi_device ram;
static struct pi_hyperflash_conf flash_conf;
static struct pi_readfs_conf fs_conf;
static struct pi_hyperram_conf ram_conf;

int32_t copy_file_from_flash_to_l3(char *file_name)
{
    uint32_t hyper_addr = 0;
    pi_fs_file_t *file;

    printf("Loading layer \"%s\" from FS to L3\n", file_name);

    /* Open file from filesystem. */
    file = pi_fs_open(&fs, file_name, 0);
    if (file == NULL)
    {
        printf("File open failed !\n");
        pmsis_exit(-11);
    }
    //printf("File opened: %s -> %d bytes\n", file_name, file->size);

    /* Allocate memory in ram memory to copy file. */
    if (pi_ram_alloc(&ram, &hyper_addr, file->size))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-12);
    }
    printf("Writing file \"%s\" (%d bytes) to %lx\n", file_name, file->size, hyper_addr);

    int32_t size = 0;
    uint32_t size_total = 0;
    pi_task_t task;
    do
    {
        /* First read from filesystem(on flash) to a buffer in L2 memory. */
        size = pi_fs_read_async(file, buff, (uint32_t) BUFF_SIZE, pi_task_block(&task));
        pi_task_wait_on(&task);
        size = ((size + 3) & ~3);
        if (size)
        {
            /* Then copy the L2 buffer into ram memory. */
            pi_ram_write(&ram, (hyper_addr+size_total), buff, size);
            size_total += size;
        }
    } while (size_total < file->size);
    pi_fs_seek(file, 0);

    //ram_free(&ram, hyper_addr, file->size);

    if (size_total != file->size)
    {
        hyper_addr = 1;
    }
    else
    {
        hyper_addr = 0;
    }

    pi_fs_close(file);

    return hyper_addr;
}

void test_fs_to_l3_copy()
{
    printf("Entering main controller\n");

    uint32_t errors = 0;

    buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFF_SIZE);
    if (buff == NULL)
    {
        printf("L2 buffer malloc failed !\n");
        pmsis_exit(-1);
    }

    /* Init & open ram. */
    pi_hyperram_conf_init(&ram_conf);
    pi_open_from_conf(&ram, &ram_conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_l2_malloc_free(buff, (uint32_t) BUFF_SIZE);
        pmsis_exit(-2);
    }

    /* Init & open flash. */
    pi_hyperflash_conf_init(&flash_conf);
    pi_open_from_conf(&flash, &flash_conf);
    if (pi_flash_open(&flash))
    {
        printf("Error flash open !\n");
        pmsis_l2_malloc_free(buff, (uint32_t) BUFF_SIZE);
        pi_ram_close(&ram);
        pmsis_exit(-3);
    }

    /* Open filesystem on flash. */
    pi_readfs_conf_init(&fs_conf);
    fs_conf.fs.flash = &flash;
    pi_open_from_conf(&fs, &fs_conf);
    if (pi_fs_mount(&fs))
    {
        printf("Error FS mounting !\n");
        pmsis_l2_malloc_free(buff, (uint32_t) BUFF_SIZE);
        pi_flash_close(&ram);
        pi_ram_close(&ram);
        pmsis_exit(-4);
    }
    printf("FS mounted.\n");

    char *name[6] = {"Cifar10_Filter0.dat", "Cifar10_Bias0.dat",
                     "Cifar10_Filter1.dat", "Cifar10_Bias1.dat",
                     "Cifar10_Filter2.dat", "Cifar10_Bias2.dat"};

    for (uint32_t i=0; i<6; i++)
    {
        errors += copy_file_from_flash_to_l3(name[i]);
    }

    pi_fs_unmount(&fs);
    printf("FS unmounted.\n");
    pi_flash_close(&flash);
    pi_ram_close(&ram);
    pmsis_l2_malloc_free(buff, (uint32_t) BUFF_SIZE);

    printf("Test %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

int main(void)
{
    printf("\n\n\t *** PMSIS Hyperflash to Hyperram Copy Test ***\n\n");
    return pmsis_kickoff((void *) test_fs_to_l3_copy);
}

