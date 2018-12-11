#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gap_common.h"

char buffer[8192];

int main() {

    printf("FC mbed_os Bridge test\n");

    BRIDGE_EventInit();

    printf("Connecting to bridge\n");

    BRIDGE_EventConnect(0, NULL);

    printf("Connection done\n");

    int file  = BRIDGE_EventOpen("../../../test_Bridge.c", 0, 0, NULL);
    int file2 = BRIDGE_EventOpen("../../../out_test.c", O_RDWR | O_CREAT, S_IRWXU, NULL);
    if (file == -1 || file2 == -1) return -1;

    while(1)
    {
        int res = BRIDGE_EventRead(file, buffer, 8192, NULL);
        if (res == 0) break;
        BRIDGE_EventWrite(file2, buffer, res, NULL);

        buffer[res] = 0;

        puts(buffer);
    }

    BRIDGE_EventDisconnect(NULL);
    printf("Test success\n");

    return 0;
}
