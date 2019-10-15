#include<string.h>
// MBED OS
#include "cmsis_os2.h"
#include "gapuino_mx25r_qspi.h"

#define BUFFER_SIZE 16

GAP_L2_DATA uint8_t SPI_TX_BUFFER[BUFFER_SIZE] ;
GAP_L2_DATA uint8_t SPI_RX_BUFFER[BUFFER_SIZE] ;

int main()
{
    printf("Fabric controller code execution for mbed_os BSP!\n");
    MX25R_QSPI_Info pInfo;

    MX25R_QSPI_GetInfo               (&pInfo);

    printf("FlashSize          = %x\n", pInfo.FlashSize);
    printf("EraseSectorSize    = %x\n", pInfo.EraseSectorSize);
    printf("EraseSectorsNumber = %x\n", pInfo.EraseSectorsNumber);
    printf("ProgPageSize       = %x\n", pInfo.ProgPageSize);
    printf("ProgPagesNumber    = %x\n", pInfo.ProgPagesNumber);

    for (int i = 0 ; i < 16; i++)
    {
        SPI_TX_BUFFER[i] = i;
    }

    MX25R_QSPI_Init();

    MX25R_QSPI_Erase_Block(0);

    MX25R_QSPI_Write(SPI_TX_BUFFER, 0, BUFFER_SIZE);

    printf("Status = %x \n", MX25R_QSPI_GetStatus());

    MX25R_QSPI_Read(SPI_RX_BUFFER, 0, BUFFER_SIZE);

    MX25R_QSPI_EnterDeepPowerDown();
    MX25R_QSPI_LeaveDeepPowerDown();

    MX25R_QSPI_DeInit();

    // Check read values
    int error = 0;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf(" %x ", SPI_RX_BUFFER[i]);
        if(SPI_RX_BUFFER[i] != (char)(i)) error++;
    }

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;

    return 0;
}
