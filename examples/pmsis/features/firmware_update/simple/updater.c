// This examples shows how to update the firmware in the flash.
//
// The build process is producing the flash image which is read by this example
// and written to the flash.


#include <pmsis.h>
#include <bsp/bsp.h>
#include <bsp/fs/hostfs.h>


#define BUFF_SIZE (1<<16)


static PI_L2 char buffer[BUFF_SIZE];

int entry(void)
{
    printf("Entering updater, updating flash with image %s\n", FLASH_IMAGE);

    printf("Opening host FS to get flash image\n");

    // The host FS gives access to the workstation file-system. We use it
    // here to get the flash image containing the new firmware.
    // This can be replaced by any mean like Wifi, BLE and so on to get the new
    // flash image
    struct pi_hostfs_conf conf;
    static struct pi_device fs;
    pi_hostfs_conf_init(&conf);

    pi_open_from_conf(&fs, &conf);

    if (pi_fs_mount(&fs))
        return -1;

    pi_fs_file_t *file = pi_fs_open(&fs, FLASH_IMAGE, 0);
    if (file == NULL) return -2;


    printf("Opening flash to write flash image\n");

    // Now we open the flash where we'll write the new flash image.
    struct pi_hyperflash_conf flash_conf;
    struct pi_device flash;

    pi_hyperflash_conf_init(&flash_conf);
 
     pi_open_from_conf(&flash, &flash_conf);

    if (pi_flash_open(&flash))
      return -3;

    uint32_t flash_addr = 0;


    printf("Erasing flash\n");

    // Erase the full chip
    pi_flash_erase_chip(&flash);

    // Now we iterate to transfer the full image.
    // We get one block from host FS and write it to flash.
    printf("Copying from host FS to flash\n");
    while (1)
    {
        int size = pi_fs_read(file, buffer, BUFF_SIZE);

        pi_flash_program(&flash, flash_addr, buffer, size);

        printf("Copied %d bytes\n", size);

        if (size < BUFF_SIZE)
            break;

        flash_addr += size;
    }

    printf("Finished updating firmware\n");

    // The flash operation is done, now the device has been restarted.

    return 0;
}


void pmsis_wrapper()
{
    int retval = entry();
    pmsis_exit(retval);
}


int main(void)
{
    return pmsis_kickoff((void *) pmsis_wrapper);
}

