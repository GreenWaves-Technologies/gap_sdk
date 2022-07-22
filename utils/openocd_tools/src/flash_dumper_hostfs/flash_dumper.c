/* PMSIS includes */
#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/fs/hostfs.h"

/* Variables used. */
#define BUFFER_SIZE      ( 2*527 )
#define PRINT            ( 0 )

static uint8_t *rcv_buff;
static uint32_t flash_buff = 0;
static struct pi_device flash;
static struct pi_default_flash_conf conf;

void dump_flash(void)
{
    printf("Entering main controller\n\n");

    uint32_t errors = 0;
    struct pi_flash_info flash_info;

    rcv_buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
    if (rcv_buff == NULL)
    {
        printf("rcv_buff alloc failed !\n");
        pmsis_exit(-2);
    }

    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        rcv_buff[i] = 0;
    }

    /* Init & open flash. */
    pi_default_flash_conf_init(&conf);
    pi_open_from_conf(&flash, &conf);
    if (pi_flash_open(&flash))
    {
        printf("Error flash open !\n");
        pmsis_exit(-3);
    }
    printf("flash open done\n");

    /* Init & open hostfs */
    struct pi_hostfs_conf conf;
    pi_hostfs_conf_init(&conf);
    struct pi_device fs;

    pi_open_from_conf(&fs, &conf);

    if (pi_fs_mount(&fs))
     return;
    printf("fs mounted\n");

    void *file = pi_fs_open(&fs, "../../../flash_dump_output.dat", PI_FS_FLAGS_WRITE);
    if (file == 0)
    {
        printf("Failed to open file\n");
        pmsis_exit(-1);
    }
    printf("host file opened\n");

    int total_size = TOTAL_SIZE;
    int buff_rcvd = 0;
    while(total_size > 0)
    {
        if (total_size > BUFFER_SIZE)
        {
            pi_flash_read(&flash, flash_buff, rcv_buff, (uint32_t) BUFFER_SIZE);
            buff_rcvd = BUFFER_SIZE;
            flash_buff += BUFFER_SIZE;
            total_size -= BUFFER_SIZE;
        }
        else
        {
            pi_flash_read(&flash, flash_buff, rcv_buff, total_size);
            buff_rcvd = total_size;
            flash_buff += total_size;
            total_size -= total_size;
        }
        printf("read from flash done, write to fs\n");
        if( PRINT )
        {
            for(int i=0; i<buff_rcvd; i++)
            {
                printf("%x, ", rcv_buff[i]);
                if ((i%128) == 0 && i)
                    printf("\n");
            }
        }
        else
        {
            pi_fs_write(file, rcv_buff, buff_rcvd);
        }
    }
    pi_fs_close(file);
    pi_fs_unmount(&fs);

    pmsis_l2_malloc_free(rcv_buff, (uint32_t) BUFFER_SIZE);
    pi_flash_close(&flash);

    printf("flash dump done\n");
    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** Flash Dumper ***\n\n");
    return pmsis_kickoff((void *) dump_flash);
}
