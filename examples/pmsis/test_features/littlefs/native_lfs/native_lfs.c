#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "stdbool.h"

#include "pmsis.h"
#include "bsp/fs/pi_lfs.h"
#include "bsp/flash/hyperflash.h"

void open_flash(pi_device_t *flash, struct pi_hyperflash_conf *flash_conf)
{
    pi_hyperflash_conf_init(flash_conf);
    pi_open_from_conf(flash, flash_conf);
    if (pi_flash_open(flash))
    {
        printf("Error flash open !\n");
        pmsis_exit(-1);
    }
}

int list_dir(lfs_t *lfs, const char *dir_name, const uint8_t level, const bool recursive)
{
    int rc;
    uint8_t nbr_of_files = 0;
    lfs_dir_t dir;
    struct lfs_info info;

    rc = lfs_dir_open(lfs, &dir, dir_name);
    if (rc < 0)
    {
        printf("Little FS open dir %s error %d\r\n", dir_name, rc);
        pmsis_exit(-1);
    }

    if (level == 0)
        printf("%s\n", dir_name);

    while ((rc = lfs_dir_read(lfs, &dir, &info)) > 0)
    {
        for (int i = 0; i <= level; i++)printf("\t");
        printf("%s", info.name);
        nbr_of_files++;
        if (info.type == LFS_TYPE_DIR
            && strcmp(".", info.name)
            && strcmp("..", info.name))
        {
            printf("/\r\n");
            if (recursive)
                nbr_of_files += list_dir(lfs, info.name, level + 1, recursive);
        } else
            printf("\r\n");
    }
    if (rc < 0)
    {
        printf("Little FS read dir error %d\r\n", rc);
        pmsis_exit(-1);
    }

    rc = lfs_dir_close(lfs, &dir);
    if (rc < 0)
    {
        printf("Little FS close dir error %d\r\n", rc);
        pmsis_exit(-1);
    }
    return nbr_of_files;
}

void read_and_check_file(lfs_t *lfs, const char *path, const char *pattern)
{
    int rc;
    lfs_file_t file;
#define TEXT_SIZE 1024
    char *text;

    printf("Open %s\n", path);
    
    rc = lfs_file_open(lfs, &file, path, LFS_O_RDONLY);
    if (rc < 0)
    {
        printf("Little FS open error %d\n", rc);
        pmsis_exit(-1);
    }

    text = pi_l2_malloc(TEXT_SIZE);
    text[TEXT_SIZE - 1] = 0;
    rc = lfs_file_read(lfs, &file, text, 1023);
    if (rc <= 0)
    {
        printf("Little FS read file error %d\r\n", rc);
        pmsis_exit(-1);
    }

    text[rc - 1] = 0;
    printf("Read %d characters from %s:\r\n'%s'\r\n", rc, path, text);
    rc = strcmp(text, pattern);
    if (rc != 0)
    {
        printf("Little FS read file error, `)' not found\r\n", pattern);
        pmsis_exit(-1);
    }

    pi_l2_free(text, TEXT_SIZE);
    rc = lfs_file_close(lfs, &file);
    if (rc < 0)
    {
        printf("Little FS close file error %d\r\n", rc);
        pmsis_exit(-2);
    }

}

void test_write_file(lfs_t *lfs)
{
    int rc;
    char *text = "Write from GAP8";
    lfs_file_t file;

    rc = lfs_file_open(lfs, &file, "/w.txt",
                          LFS_O_RDWR | LFS_O_CREAT);
    if (rc < 0)
    {
        printf("Little FS open error %d\n", rc);
        pmsis_exit(-1);
    }

    printf("Write `%s'\n", text);
    rc = lfs_file_write(lfs, &file, text, strlen(text));
    if (rc < (int) strlen(text))
    {
        printf("Litle FS write file error %d\r\n", rc);
        pmsis_exit(-1);
    }

    rc = lfs_file_close(lfs, &file);
    if (rc < 0)
    {
        printf("Little FS close file error %d\r\n", rc);
        pmsis_exit(-2);
    }
}

void test_lfs(void)
{
    int rc;
    pi_device_t flash;
    struct pi_hyperflash_conf flash_conf;
    pi_device_t lfs_dev;
    struct pi_lfs_conf lfs_conf;
    lfs_t *lfs;

    printf("Entering main controller\n");

    puts("Open flash");
    open_flash(&flash, &flash_conf);

    puts("Mount Little FS");
    pi_lfs_conf_init(&lfs_conf);
    lfs_conf.fs.flash = &flash;
    /*
     * Auto format
     * - true: if the LFS file system is not found inside of the LFS partition,
     * the partition will be formated and littlefs mounted.
     * - false (by default): an error will be returned if the file system is not found.
     */
    lfs_conf.fs.auto_format = false;
    
    lfs_dev.config = &lfs_conf;
    rc = pi_fs_mount(&lfs_dev);
    if (rc < 0)
    {
        printf("LFS mount eror: rc = %d\n", rc);
        pmsis_exit(-1);
    }
    puts("Mount Little FS done");

    lfs = pi_lfs_get_native_lfs(&lfs_dev);

    puts("\nTest list files");
    uint8_t nbr_of_files;
    nbr_of_files = list_dir(lfs, "/", 0, true);
    printf("Number of files : %u\n", nbr_of_files);
    if (nbr_of_files != 8)
    {
        puts("List files error: expect 8 files");
        pmsis_exit(-1);
    }
    puts("Test list files done\n");

    puts("Little FS read file test /a.txt");
    read_and_check_file(lfs, "/a.txt", "hello word");
    puts("Little FS read file test done\n");

    puts("Little FS write file test /w.txt");
    test_write_file(lfs);
    puts("list files:");
    nbr_of_files = list_dir(lfs, "/", 0, false);
    printf("Number of files in / : %u\n", nbr_of_files);
    puts("Check /w.txt");
    read_and_check_file(lfs, "/w.txt", "Write from GAP");
    puts("Remove /w.txt");
    lfs_remove(lfs, "/w.txt");
    puts("List files:");
    nbr_of_files = list_dir(lfs, "/", 0, false);
    printf("Number of files in / : %u\n", nbr_of_files);
    puts("Little FS write file test done\n");

    puts("Unmount Little FS");
    pi_fs_unmount(&lfs_dev);
    puts("Unmount Little FS done");

    pi_flash_close(&flash);

    printf("\nTest native Little FS success!\n");
    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS native Little FS test ***\n\n");
    return pmsis_kickoff((void *) test_lfs);
}
