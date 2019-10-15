/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://github.com/armmbed/mbed-os-example-fat-filesystem
 */
#include "mbed.h"
#include "LittleFileSystem.h"
#include <stdio.h>
#include <errno.h>

#include "HYPERBUSBlockDevice.h"
// Create flash device on HYPERBUS with PTE5 as chip select
HYPERBUSBlockDevice bd(HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
                       HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7,
                       HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS, HYPERBUS_CSN0, HYPERBUS_CSN1);
LittleFileSystem fs("fs");

void return_error(int ret_val){
    if (ret_val)
        printf("Failure. %d\r\n", ret_val);
    else
        printf("done.\r\n");
}

void errno_error(void* ret_val){
    if (ret_val == NULL)
        printf(" Failure. %d \r\n", errno);
    else
        printf(" done.\r\n");
}

int main() {
    int error = 0;
    printf("Welcome to the filesystem example.\r\n"
           "Formatting a Little filesystem, HYPERBUS_FLASH-backed filesystem. ");

    printf("Mounting the filesystem on \"/fs\". ");
    error = fs.mount(&bd);
    return_error(error);

    printf("Opening file read-only.");
    FILE* fd = fopen("/fs/hi.txt", "r");
    errno_error(fd);

    printf("Dumping file to screen.\r\n");
    char buff[16] = {0};
    while (!feof(fd)){
        int size = fread(&buff[0], 1, 15, fd);
        fwrite(&buff[0], 1, size, stdout);
    }
    printf("EOF.\r\n");

    printf("Closing file.");
    fclose(fd);
    printf(" done.\r\n");

    printf("Opening root directory.");
    DIR* dir = opendir("/fs/");
    errno_error(fd);

    struct dirent* de;
    printf("Printing all filenames:\r\n");
    while((de = readdir(dir)) != NULL){
        printf("  %s\r\n", &(de->d_name)[0]);
    }

    printf("Closeing root directory. ");
    error = closedir(dir);
    return_error(error);
    printf("Filesystem Demo complete.\r\n");

    if (error)
    {
        printf("Test failed\n");
        #ifdef JENKINS_TEST_FLAG
        exit(-1);
        #else
        return -1;
        #endif
    }
    else
    {
        printf("Test success\n");
        #ifdef JENKINS_TEST_FLAG
        exit(0);
        #else
        return 0;
        #endif
    }
}
