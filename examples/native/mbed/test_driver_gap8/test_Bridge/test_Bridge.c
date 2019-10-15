#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gap_common.h"

char buffer[8192];

int main() {

    printf("FC mbed_os Bridge test\n");

    BRIDGE_Init();

    printf("Connecting to bridge\n");

    BRIDGE_Connect(0, NULL);

    printf("Connection done\n");

    int file  = BRIDGE_Open("../../../test_Bridge.c", 0, 0, NULL);
    int file2 = BRIDGE_Open("../../../out_test.c", O_RDWR | O_CREAT, S_IRWXU, NULL);
    if (file == -1 || file2 == -1) return -1;

    while(1)
    {
        int res = BRIDGE_Read(file, buffer, 8192, NULL);
        if (res == 0) break;
        BRIDGE_Write(file2, buffer, res, NULL);

        buffer[res] = 0;

        puts(buffer);
    }

    BRIDGE_Disconnect(NULL);
    printf("Test success\n");

    return 0;
}
