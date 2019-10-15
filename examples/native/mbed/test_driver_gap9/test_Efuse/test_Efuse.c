#include <stdio.h>
#include "cmsis.h"
#include "gap_efuse.h"
#include "mbed_wait_api.h"

/* Efuse Size = 128 * 32 bits*/
#define  EFUSE_SIZE 128

volatile int efuse_content[EFUSE_SIZE];

int main()
{
    printf("Fabric controller code execution for mbed_os EFUSE \n");

    int error = 0;

    /* Configure the timing counters for 50MHz
       short --> 30ns    = 2   cycles
       mid   --> 1020ns  = 52  cycles
       long  --> 10000ns = 510 cycles
    */
    EFUSE_CTRL->CFG = EFUSE_CTRL_MARGIN(1)  | EFUSE_CTRL_LONG(510) |
                      EFUSE_CTRL_MEDIUM(52) | EFUSE_CTRL_SHORT(2);

    /*
     * Read efuse
     */
    EFUSE_StartRead();
    for(int i = 0; i < EFUSE_SIZE; i++) {
        efuse_content[i] = (int)EFUSE_Read(i);
    }
    EFUSE_Sleep();

    /* Initialize */
    for(int i=0; i < EFUSE_SIZE; i++) {
        /* Filter already burned bits for boot */
        if (efuse_content[i] == 0)
            efuse_content[i] =  ((i*2+1) << 16) | (i*2) ;
    }

    /* Program efuse
     *
     * You can start by burnning efuse
     * So the bits will become '0' to '1'
     *
     */
    EFUSE_StartProgram();
    for(int i = 0; i < EFUSE_SIZE; i++) {

        EFUSE_Program(i, efuse_content[i]);
    }
    EFUSE_Sleep();

    /*
     * Read efuse
     */
    EFUSE_StartRead();
    for(int i = 0; i < EFUSE_SIZE; i++) {

        if ((int)EFUSE_Read(i) != efuse_content[i])
            error++;
    }
    EFUSE_Sleep();

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;
}
