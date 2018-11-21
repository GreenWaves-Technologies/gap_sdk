#include <stdio.h>
#include "cmsis.h"
#include "gap_efuse.h"
#include "mbed_wait_api.h"


/* #define GAP_EFUSE_PLT_OTHER   0 */
/* #define GAP_EFUSE_PLT_FPGA    1 */
/* #define GAP_EFUSE_PLT_RTL     2 */
/* #define GAP_EFUSE_PLT_VP      3 */
/* #define GAP_EFUSE_PLT_CHIP    4 */


/* #define GAP_EFUSE_BOOT_OTHER    0 */
/* #define GAP_EFUSE_BOOT_SPI      1 */
/* #define GAP_EFUSE_BOOT_JTAG     2 */
/* #define GAP_EFUSE_BOOT_ROM      3 */
/* #define GAP_EFUSE_BOOT_PRELOAD  4 */
/* #define GAP_EFUSE_BOOT_HYPERBUS 5 */
/* #define GAP_EFUSE_BOOT_SPIM     6 */
/* #define GAP_EFUSE_BOOT_SPIM_QPI 7 */

/* #define GAP_EFUSE_MAX_USER_BYTE_NUM  88 */

int main()
{
    printf("Fabric controller code execution for mbed_os EFUSE \n");

    /* bits[29:20] sets the long interval   default 200 cycles */
    EFUSE_CTRL->CFG = EFUSE_CTRL_LONG(200) | EFUSE_CTRL_MEDIUM(50) | EFUSE_CTRL_SHORT(5);

    EFUSE_StartRead();
    printf("Info     = 0x%x\n", EFUSE_GetInfo());
    printf("Info2    = 0x%x\n", EFUSE_GetInfo2());
    printf("UserByte = 0x%x\n", EFUSE_GetUserByte(87));
    EFUSE_Sleep();

    /* Program efuse last USER byte, USER_REG[0 - 87]
     *
     * You can start by burn last byte
     * So the bits will become '0' to '1'
     *
     */
    printf("Write User byte = 0x%x\n", 0x1);
    EFUSE_StartProgram();
    EFUSE_SetUserByte(87, 0x1);
    wait(1);
    EFUSE_Sleep();

    /* Program efuse to boot from HYPERFLASH.
     *
     * When you read the two information register in GAPUINO before, they will always be zero.
     * Because the efuse has not been burned at all. And now we will start to burn the efuse.
     * So the bits will become '0' to '1'
     *
     */

    /* /\* 0x2C *\/ */
    /* uint8_t efuse_data = EFUSE_INFO_BOOT(GAP_EFUSE_BOOT_HYPERBUS) | */
    /*                      EFUSE_INFO_PLT(GAP_EFUSE_PLT_CHIP)       | */
    /*                      EFUSE_INFO_ENCRYPTED(0)                  | */
    /*                      EFUSE_INFO_WAIT_XTAL(0); */

    /* printf("Write Info = 0x%x\n", efuse_data); */

    /* EFUSE_StartProgram(); */
    /* EFUSE_SetInfo(efuse_data); */
    /* wait(1); */
    /* EFUSE_Sleep(); */

    EFUSE_StartRead();
    printf("Info     = 0x%x\n", EFUSE_GetInfo());
    printf("Info2    = 0x%x\n", EFUSE_GetInfo2());
    printf("UserByte = 0x%x\n", EFUSE_GetUserByte(87));
    EFUSE_Sleep();

    /* Check result */
    EFUSE_StartRead();
    for ( int i = 0x1a109200; i < 0x1a109400; i+=4) {
        printf("0x%x\n", *((unsigned int*)i));
    }
    EFUSE_Sleep();

    return 0;
}
