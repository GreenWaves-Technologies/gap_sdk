/* PMSIS includes */
#include "pmsis.h"

/* App includes. */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Variables used. */
int8_t buffer[8192];

void test_bridge(void)
{
    printf("Entering main controller\n");

    uint32_t errors = 0;

    BRIDGE_Init();
    printf("Connecting to bridge\n");
    BRIDGE_Connect(0, NULL);
    printf("Connection done\n");

    int32_t file = BRIDGE_Open("../../../test_bridge.c", 0, 0, NULL);
    int32_t file2 = BRIDGE_Open("../../../out_bridge.c", O_RDWR | O_CREAT, S_IRWXU, NULL);
    if ((file == -1) || (file2 == -1))
    {
        printf("Error open file !\n");
        pmsis_exit(-1);
    }

    uint32_t size_read = 0, size_write = 0;
    while (1)
    {
        int32_t res = BRIDGE_Read(file, buffer, 8192, NULL);
        if (res == 0)
        {
            break;
        }
        size_write += res;
        BRIDGE_Write(file2, buffer, res, NULL);

        //puts(buffer);
    }
    BRIDGE_Close(file, NULL);
    BRIDGE_Close(file2, NULL);

    int32_t file3 = BRIDGE_Open("../../../out_bridge.c", 0, 0, NULL);
    if (file3 == -1)
    {
        printf("Error open file !\n");
        pmsis_exit(-2);
    }

    while (1)
    {
        int32_t res = BRIDGE_Read(file3, buffer, 8192, NULL);
        if (res == 0)
        {
            break;
        }
        size_read += res;
    }

    BRIDGE_Close(file3, NULL);
    BRIDGE_Disconnect(NULL);

    errors = (size_read != size_write);

    printf("Bridge copy file done !\n");

    printf("\nTest %s with %d error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Bridge Test ***\n\n");
    return pmsis_kickoff((void *) test_bridge);
}
