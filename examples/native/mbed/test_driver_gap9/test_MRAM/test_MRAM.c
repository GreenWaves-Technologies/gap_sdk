// MBED OS
#include "cmsis_os2.h"
#include "gap_common.h"
#include "mbed_wait_api.h"

#define BUFFER_SIZE       512

GAP_L2_DATA uint8_t RX_BUFFER[BUFFER_SIZE];
GAP_L2_DATA uint8_t TX_BUFFER[BUFFER_SIZE];

#define TRIM_BUFFER_SIZE  133

GAP_L2_DATA uint32_t TRIM_BUFFER[TRIM_BUFFER_SIZE];

/*
 * Note for configuration registers bits:
 * section erase_en = cr_lat[3960]
 * prog_pulse_cfg   = cr_lat[3955:3953]
 *                    cr_lat[3955:3953]= 000 => number of program pulse = 8 (the default)
 *                    cr_lat[3955:3953]= 001 => number of program pulse = 1
 *                    cr_lat[3955:3953]= 010 => number of program pulse = 2
 *                    cr_lat[3955:3953]= 011 => number of program pulse = 3
 *                    cr_lat[3955:3953]= 100 => number of program pulse = 4
 *                    cr_lat[3955:3953]= 101 => number of program pulse = 5
 *                    cr_lat[3955:3953]= 110 => number of program pulse = 6
 *                    cr_lat[3955:3953]= 111 => number of program pulse = 7
 */
void trim_config(int sector_erase_en, int program_pulse) {
    // Init the CFG zone in L2 to scan the TRIM CGF in the MRAM
    for(int i = 0; i < TRIM_BUFFER_SIZE; i ++) {
        TRIM_BUFFER[i] = 0;
    }

    /* Write the info to num Pulses and Sector enable */
    int section_option;
    /*
     * cr_lat[3960]      = SECTOR_ERASE_EN
     * cr_lat[3955:3953] = PROGRAM_PULSE
     */
    section_option = (sector_erase_en << 24) | ( program_pulse << 17 );

    TRIM_BUFFER[123] = section_option;
}

int test_mram()
{
    /* Initialize the buffer */
    for (int i = 0; i < BUFFER_SIZE; i++)
        TX_BUFFER[i] = i;

    /* Start MRAM Operation */
    mram_config_t config;

    MRAM_GetDefaultConfig(&config);

    /* 1. Clock Enable */
    MRAM_Init(MRAM, &config, SystemCoreClock);

    /* 2. MRAM power up */
    MRAM_PowerUp(MRAM);

    /* 3. Reference line configuration */
    /* Reference line 0 */
    MRAM_ReferenceLineConfig(MRAM, 0, 1, 1);
    /* Reference line 1 */
    MRAM_ReferenceLineConfig(MRAM, 1, 1, 1);
    /* Setting back */
    MRAM_ReferenceLineConfig(MRAM, 0, 0, 0);

    /* 4. TRIM configuration - SECTOR ERASE EN */
    trim_config(0b1, 0b001);
    MRAM_TRIMConfig(MRAM, 0x1D000000, (char *)TRIM_BUFFER, (TRIM_BUFFER_SIZE << 2));
    /* Wait 8 us */
    wait_us(8);

    /* 5. Sector Erase, must after 4 */
    MRAM_SectorErase(MRAM, 0x1D000000, 1);

    /* /\* 6. Chip Erase, must after 4 *\/ */
    /* MRAM_ChipErase(MRAM); */

    /* 7. TRIM configuration - SECTOR ERASE DISABLE */
    trim_config(0b0, 0b001);
    MRAM_TRIMConfig(MRAM, 0x1D000000, (char *)TRIM_BUFFER, (TRIM_BUFFER_SIZE << 2));
    /* Wait 8 us */
    wait_us(8);

    /* 8. Word Erase, must after 7 */
    MRAM_WordErase(MRAM, 0x1D000000, BUFFER_SIZE);
    MRAM_WordErase(MRAM, 0x1D001000, BUFFER_SIZE);

    /* 9. Program */
    MRAM_Program(MRAM, 0x1D000000, (char *)TX_BUFFER, BUFFER_SIZE, 0);
    MRAM_Program(MRAM, 0x1D001000, (char *)TX_BUFFER, BUFFER_SIZE, 0);

    /* 10. Read */
    MRAM_Read(MRAM, 0x1D000000, (char *)RX_BUFFER, BUFFER_SIZE, 0);
    MRAM_Read(MRAM, 0x1D001000, (char *)RX_BUFFER, BUFFER_SIZE, 0);

    /* 11. MRAM clock disable */
    MRAM_Deinit(MRAM);

    int error = 0;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
#if DEBUG == 1
        printf(" %x \n", RX_BUFFER[i]);
#endif
        if(RX_BUFFER[i] != (char)i) error++;
    }

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os MRAM driver test\n");

    int error = 0;

    for (int i = 0; i < 3; i++)
    {
        error = test_mram();

        if (error) {
            printf("Test failed with %d errors\n", error);
            return error;
        }
    }

    printf("Test success\n");

    return error;
}
