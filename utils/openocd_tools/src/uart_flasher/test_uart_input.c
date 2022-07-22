/* PMSIS includes */
#include "pmsis.h"
#include "bsp/bsp.h"
#ifdef HOST_FS
#include "bsp/fs/hostfs.h"
#else
#include "bsp/flash.h"
#include "bsp/flash/spiflash.h"
#endif

/* Variables used. */
#ifdef HOST_FS
#define SECTOR_SIZE      ( 256*1024 )
#else
#define SECTOR_SIZE      ( 4096 )
#endif

char l2_in[SECTOR_SIZE];
struct pi_device uart;
volatile uint8_t done = 0;
char *sentence = "\nPlease provide your flash image\n";
char *sentence_rx = NULL;

void test_uart_input(void)
{
    struct pi_uart_conf conf;

    /* Init & open uart. */
    pi_uart_conf_init(&conf);
    conf.enable_tx = 1;
    conf.enable_rx = 1;
    conf.baudrate_bps = 921600;
    conf.uart_id = 0;
    conf.use_ctrl_flow = 1;
    //conf.use_fast_clk = 1;
    pi_open_from_conf(&uart, &conf);
    if (pi_uart_open(&uart))
    {
        printf("Uart open failed !\n");
        pmsis_exit(-1);
    }

#ifdef HOST_FS
    struct pi_hostfs_conf fs_conf;
    pi_hostfs_conf_init(&fs_conf);
    struct pi_device fs;


    pi_open_from_conf(&fs, &fs_conf);
    if (pi_fs_mount(&fs))
        pmsis_exit(-2);

    void *file = pi_fs_open(&fs, "../../../uart_rx", PI_FS_FLAGS_WRITE);
    if (file == 0)
    {
        printf("Failed to open file\n");
        pmsis_exit(-1);
    }
#else
#ifdef USE_MRAM
    struct pi_mram_conf flash_conf;
    pi_mram_conf_init(&flash_conf);
#else
    struct pi_default_flash_conf flash_conf;
    pi_default_flash_conf_init(&flash_conf);
#endif
    struct pi_device flash;
    pi_open_from_conf(&flash, &flash_conf);
    if (pi_flash_open(&flash))
    {
        printf("pi_flash_open failed\n");
        pmsis_exit(-3);
    }
#endif

    int flash_img_size = IMG_SIZE;
    int flash_addr = 0;
    int buff_rcvd = 0;
    /* Write on uart then wait for data from uart. */
    pi_uart_write(&uart, sentence, strlen(sentence));
    sprintf(sentence, "Size = %d\n", flash_img_size); 
    pi_uart_write(&uart, sentence, strlen(sentence));

    while(flash_img_size)
    {
        if (flash_img_size > SECTOR_SIZE)
        {
            pi_uart_read(&uart, l2_in, SECTOR_SIZE);
            buff_rcvd = SECTOR_SIZE;
            flash_img_size -= SECTOR_SIZE;
        }
        else
        {
            pi_uart_read(&uart, l2_in, flash_img_size);
            buff_rcvd = flash_img_size;
            flash_img_size -= flash_img_size;
        }
#ifdef HOST_FS
        pi_fs_write(file, l2_in, buff_rcvd);
#else
        pi_flash_erase_sector(&flash, flash_addr);
        pi_flash_program(&flash, flash_addr, (void*) l2_in,
                buff_rcvd);
        flash_addr += buff_rcvd;
        sprintf(sentence, "%d Bytes remain\n", flash_img_size); 
        pi_uart_write(&uart, sentence, strlen(sentence));
#endif
    }
    
#ifdef HOST_FS
    pi_fs_close(file);
    pi_fs_unmount(&fs);
#else
    pi_flash_close(&flash);
#endif
    pi_uart_close(&uart);

    printf("Flasher end \n");

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Uart Flasher ***\n\n");
    return pmsis_kickoff((void *) test_uart_input);
}

