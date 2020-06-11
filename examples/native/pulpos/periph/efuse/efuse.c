#include "rt/rt_api.h"

/*
Uncomment the following line to burn the an efuse. Be carefully! Using plp_efuse_writeByte is not reversible.
*/
/*
#define BURN
*/

int main() {
    printf("efuse start\n");

#ifdef BURN
#warning "Burning E-Fuses Enabled!"
    printf("Wait for 10 seconds until burning the e-fuses. This is not reversible!.\r\n");
    rt_time_wait_us(10 * 1000 * 1000);

    plp_efuse_startProgram();

    /*
    According to the docs there are 128 bytes available, but only
    the first 55 (2020-05-20) bytes are used so far. Bits can only
    be burned to 1.
    */
    plp_efuse_writeByte(127, 0x01);

    plp_efuse_sleep();
#endif

    plp_efuse_startRead();

    printf("efuse fuses:\n");
    const uint32_t block_size = 8;
    const uint32_t efuse_size = 128;
    for (uint32_t i = 0; i < efuse_size; i += block_size) {
        printf("%3i", i);
        for (uint32_t j = 0; j < block_size; j += 1) {
            uint32_t address = i + j;
            if (address < efuse_size) {
                printf(" %.2x", plp_efuse_readByte(address));
            }
        }
        printf("\n");
    }

    printf("efuse end\n");
}
