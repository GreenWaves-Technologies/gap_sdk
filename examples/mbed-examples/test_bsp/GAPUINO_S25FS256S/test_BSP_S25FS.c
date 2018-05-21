#include<string.h>
// MBED OS
#include "cmsis_os2.h"
#include "gapuino_s25fs_qspi.h"

#define BUFFER_SIZE 16

GAP_L2_DATA uint8_t SPI_TX_BUFFER[BUFFER_SIZE] ;
GAP_L2_DATA uint8_t SPI_RX_BUFFER[BUFFER_SIZE] ;

int main()
{
    printf("Fabric controller code execution for mbed_os BSP!\n");
    S25FS_QSPI_Info pInfo;

    S25FS_QSPI_GetInfo               (&pInfo);

    printf("FlashSize          = %x\n", pInfo.FlashSize);
    printf("EraseSectorSize    = %x\n", pInfo.EraseSectorSize);
    printf("EraseSectorsNumber = %x\n", pInfo.EraseSectorsNumber);
    printf("ProgPageSize       = %x\n", pInfo.ProgPageSize);
    printf("ProgPagesNumber    = %x\n", pInfo.ProgPagesNumber);

    for (int i = 0 ; i < 16; i++)
    {
        SPI_TX_BUFFER[i] = i;
    }

    S25FS_QSPI_Init();

    S25FS_QSPI_Erase_Sector(1);

    S25FS_QSPI_Write(SPI_TX_BUFFER, pInfo.EraseSectorSize, BUFFER_SIZE);

    printf("Status = %x \n", S25FS_QSPI_GetStatus());

    S25FS_QSPI_Read(SPI_RX_BUFFER, pInfo.EraseSectorSize, BUFFER_SIZE);

    S25FS_QSPI_EnterDeepPowerDown();
    S25FS_QSPI_LeaveDeepPowerDown();

    S25FS_QSPI_DeInit();

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
