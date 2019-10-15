/**
******************************************************************************
* @file    gapuino_s25fs_qspi.c
* @author  GreenWave Technologies Application Team
* @brief   This file includes a standard driver for the S25FS256S QSPI
*          memory mounted on GAPUINO board.
@verbatim
==============================================================================
##### How to use this driver #####
==============================================================================
[..]
(#) This driver is used to drive the S25FS256S QSPI external
memory mounted on GAPUINO board.

(#) This driver need a specific component driver (S25FS256S) to be included with.

(#) Initialization steps:
(++) Initialize the QPSI external memory using the S25FS_QSPI_Init() function. This
function includes the MSP layer hardware resources initialization and the
QSPI interface with the external memory. The S25FS_QSPI_DeInit() can be used
to deactivate the QSPI interface.

(#) QSPI memory operations
(++) QSPI memory can be accessed with read/write operations once it is
initialized.
Read/write operation can be performed with AHB access using the functions
S25FS_QSPI_Read()/S25FS_QSPI_Write().
(++) The function S25FS_QSPI_GetInfo() returns the configuration of the QSPI memory.
(see the QSPI memory data sheet)
(++) You can perform an erase operation of the whole chip by calling the function S25FS_QSPI_Erase_Chip().
(++) The function S25FS_QSPI_GetStatus() returns the current status of the QSPI memory.
(see the QSPI memory data sheet)
(++) Perform erase sector operation using the function S25FS_QSPI_Erase_Sector()
which is not blocking. So the function S25FS_QSPI_GetStatus() should be used
to check if the memory is busy, and the functions S25FS_QSPI_SuspendErase()/
S25FS_QSPI_ResumeErase() can be used to perform other operations during the
sector erase.
(++) Deep power down of the QSPI memory is managed with the call of the functions
S25FS_QSPI_EnterDeepPowerDown()/S25FS_QSPI_LeaveDeepPowerDown()
@endverbatim
**/
/*
* Copyright (c) 2018, GreenWaves Technologies, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of GreenWaves Technologies, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Includes ------------------------------------------------------------------*/
#include "gapuino_s25fs_qspi.h"

/** @addtogroup BSP
 * @{
 */

/** @addtogroup GAPUINO
 * @{
 */

/** @defgroup GAPUINO_QSPI QSPI
 * @{
 */

/* Private constants --------------------------------------------------------*/
/** @defgroup GAPUINO_QSPI_Private_Constants QSPI Private Constants
 * @{
 */
#define QSPI_QUAD_DISABLE       0x0
#define QSPI_QUAD_ENABLE        0x1

#define QSPI_HIGH_PERF_DISABLE  0x0
#define QSPI_HIGH_PERF_ENABLE   0x1
/**
 * @}
 */
/* Private variables ---------------------------------------------------------*/

/** @defgroup GAPUINO_QSPI_Private_Variables QSPI Private Variables
 * @{
 */
/* SPI transfer command sequence array */
static uint32_t s_command_sequence[6];
static uint32_t reg;

/**
 * @}
 */


/* Private functions ---------------------------------------------------------*/

/** @defgroup GAPUINO_QSPI_Private_Functions QSPI Private Functions
 * @{
 */
static uint8_t S25FS_QSPI_ResetMemory        ();
static uint8_t S25FS_QSPI_PageSize           (uint32_t Size);
static uint8_t S25FS_QSPI_WriteEnable        (spi_qpi_t QPI);
static uint8_t S25FS_QSPI_WriteCMD           (uint32_t CMD, spi_qpi_t QPI);
static uint8_t S25FS_QSPI_AutoPollingMemReady(uint32_t Timeout, spi_qpi_t QPI);
static uint8_t S25FS_QSPI_QuadMode           (uint8_t Operation);


/**
 * @}
 */

/* Exported functions ---------------------------------------------------------*/

/** @addtogroup GAPUINO_QSPI_Exported_Functions
 * @{
 */


/**
 * @brief  Initializes the QSPI interface.
 * @retval QSPI status
 */
uint8_t S25FS_QSPI_Init(void)
{
    /* SPI 0 Pin connected */
    PORT_SetPinMux(PORTA, SPI0_SDIO0 - GAP_PIN_OFFSET, uPORT_MuxAlt0);
    PORT_SetPinMux(PORTA, SPI0_SDIO1 - GAP_PIN_OFFSET, uPORT_MuxAlt0);
    PORT_SetPinMux(PORTA, SPI0_SDIO2 - GAP_PIN_OFFSET, uPORT_MuxAlt0);
    PORT_SetPinMux(PORTA, SPI0_SDIO3 - GAP_PIN_OFFSET, uPORT_MuxAlt0);
    PORT_SetPinMux(PORTA, SPI0_CSN1  - GAP_PIN_OFFSET, uPORT_MuxAlt0);
    PORT_SetPinMux(PORTA, SPI0_SCLK  - GAP_PIN_OFFSET, uPORT_MuxAlt0);

    spi_master_config_t master_config;
    int mode = 0;

    /* Master config */
    SPI_MasterGetDefaultConfig(&master_config);

    master_config.clkDiv = 2;
    master_config.cpol = (mode & 0x2) ? uSPI_ClockPolarityActiveLow : uSPI_ClockPolarityActiveHigh;
    master_config.cpha = (mode & 0x1) ? uSPI_ClockPhaseSecondEdge : uSPI_ClockPhaseFirstEdge;

    SPI_MasterInit(SPIM0, &master_config, FLL_GetFrequency(uFLL_SOC));

    /* QSPI memory reset */
    if (S25FS_QSPI_ResetMemory() != QSPI_OK)
    {
        return QSPI_NOT_SUPPORTED;
    }

    /* Set page Size */
    if (S25FS_QSPI_PageSize(S25FS256S_256_PAGE_SIZE) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* QSPI quad enable */
    if (S25FS_QSPI_QuadMode(QSPI_QUAD_ENABLE) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  De-Initializes the QSPI interface.
 * @retval QSPI status
 */
uint8_t S25FS_QSPI_DeInit(void)
{
    SPI_MasterDeInit(SPIM0);

    return QSPI_OK;
}

/**
 * @brief  Reads an amount of data from the QSPI memory.
 * @param  pData    : Pointer to data to be read
 * @param  ReadAddr : Read start address
 * @param  Size     : Size of data to read
 * @retval QSPI memory status
 */
uint8_t S25FS_QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
    spi_command_sequence_t s_command;

    /* Initialize the config command */
    s_command.csn       = uSPI_csn0;
    s_command.cmd       = QUAD_IO_READ4_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.addr_bits = 32;
    s_command.addr      = ReadAddr;
    s_command.addr_mode = uSPI_Quad;
    s_command.tx_bits   = 0;
    s_command.tx_data   = 0;
    s_command.tx_buffer = 0;
    s_command.rx_bits   = Size * 8;
    s_command.alter_data_bits = 8;
    s_command.alter_data      = S25FS256S_ALT_BYTES_NO_PE_MODE;
    s_command.alter_data_mode = uSPI_Quad;
    s_command.dummy     = S25FS256S_DUMMY_CYCLES_READ;
    s_command.rx_buffer = (uint8_t *)pData;
    s_command.data_mode = uSPI_Quad;

    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    return QSPI_OK;
}

/**
 * @brief  Writes an amount of data to the QSPI memory.
 * @param  pData     : Pointer to data to be written
 * @param  WriteAddr : Write start address
 * @param  Size      : Size of data to write
 * @retval QSPI memory status
 */
uint8_t S25FS_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
    spi_command_sequence_t s_command;

    uint32_t end_addr, current_size, current_addr;

    /* Calculation of the size between the write address and the end of the page */
    current_size = S25FS256S_256_PAGE_SIZE - (WriteAddr % S25FS256S_256_PAGE_SIZE);

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > Size)
    {
        current_size = Size;
    }

    /* Initialize the adress variables */
    current_addr = WriteAddr;
    end_addr = WriteAddr + Size;

    /* Initialize the write command */
    s_command.csn       = uSPI_csn0;
    s_command.cmd_bits  = 8;
    s_command.cmd       = PAGE_PROG4_CMD;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.addr_bits = 32;
    s_command.addr_mode = uSPI_Quad;
    s_command.tx_data   = 0;
    s_command.rx_bits   = 0;
    s_command.alter_data_bits = 0;
    s_command.dummy     = 0;
    s_command.rx_buffer = 0;
    s_command.data_mode = uSPI_Quad;

    /* Perform the write page by page */
    do
    {
        s_command.tx_buffer = (uint8_t*)pData;
        s_command.addr      = current_addr;
        s_command.tx_bits   = current_size * 8;

        /* Enable write operations */
        if (S25FS_QSPI_WriteEnable(uSPI_Quad) != QSPI_OK)
        {
            return QSPI_ERROR;
        }

        /* Transmission of the data */
        SPI_MasterTransferCommandSequence(SPIM0, &s_command);

        /* Configure automatic polling mode to wait for end of program */
        if (S25FS_QSPI_AutoPollingMemReady(0xF000, uSPI_Quad) != QSPI_OK)
        {
            return QSPI_ERROR;
        }

        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        pData += current_size;
        current_size = ((current_addr + S25FS256S_256_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : S25FS256S_256_PAGE_SIZE;
    } while (current_addr < end_addr);

    return QSPI_OK;
}

/**
 * @brief  Erases the specified sector of the QSPI memory.
 * @param  Sector : Sector address to erase (0 to 255)
 * @retval QSPI memory status
 * @note This function is non blocking meaning that sector erase
 *       operation is started but not completed when the function
 *       returns. Application has to call S25FS_QSPI_GetStatus()
 *       to know when the device is available again (i.e. erase operation
 *       completed).
 */
uint8_t S25FS_QSPI_Erase_Sector(uint32_t Sector)
{
    spi_command_sequence_t s_command;

    if (Sector >= (uint32_t)(S25FS256S_FLASH_SIZE/S25FS256S_64K_SECTOR_SIZE))
    {
        return QSPI_ERROR;
    }

    /* Initialize the erase sector command */
    s_command.csn       = uSPI_csn0;
    s_command.cmd       = SECTOR_ERASE_64K_256K4_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.addr_bits = 32;
    s_command.addr      = (Sector * S25FS256S_64K_SECTOR_SIZE);
    s_command.addr_mode = uSPI_Quad;
    s_command.tx_data   = 0;
    s_command.tx_buffer = 0;
    s_command.tx_bits   = 0;
    s_command.rx_bits   = 0;
    s_command.alter_data_bits = 0;
    s_command.dummy     = 0;
    s_command.rx_buffer = 0;
    s_command.data_mode = uSPI_Quad;

    /* Enable write operations */
    if (S25FS_QSPI_WriteEnable(uSPI_Quad) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    /* Configure automatic polling mode to wait for end of erase */
    if (S25FS_QSPI_AutoPollingMemReady(0xF000, uSPI_Quad) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  Erases the entire QSPI memory.
 * @retval QSPI memory status
 */
uint8_t S25FS_QSPI_Erase_Chip(void)
{
    /* Enable write operations */
    if (S25FS_QSPI_WriteEnable(uSPI_Quad) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Send the erase suspend command */
    S25FS_QSPI_WriteCMD(BULK_ERASE_CMD, uSPI_Quad);

    /* Configure automatic polling mode to wait the memory is ready */
    if (S25FS_QSPI_AutoPollingMemReady(0xFFFF, uSPI_Quad) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  Reads current status of the QSPI memory.
 * @retval QSPI memory status
 */
uint8_t S25FS_QSPI_GetStatus()
{
    spi_command_sequence_t s_command;

    /* Initialize the status command */
    s_command.csn       = uSPI_csn0;
    s_command.cmd       = READ_STATUS_REG1_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.addr_bits = 0;
    s_command.addr      = 0;
    s_command.addr_mode = uSPI_Quad;
    s_command.tx_data   = 0;
    s_command.tx_buffer = 0;
    s_command.tx_bits   = 0;
    s_command.rx_bits   = 8;
    s_command.alter_data_bits = 0;
    s_command.dummy     = 0;
    s_command.rx_buffer = (uint8_t *)&reg;
    s_command.data_mode = uSPI_Quad;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    /* Check the value of the register */
    if ((reg & S25FS256S_SR1_WIP) != 0)
    {
        return QSPI_BUSY;
    }
    else
    {
        return QSPI_OK;
    }
}

/**
 * @brief  Reads current status of the QSPI memory.
 * @retval QSPI memory status
 */
uint8_t S25FS_QSPI_GetStatus2()
{
    spi_command_sequence_t s_command;

    /* Initialize the status command */
    s_command.csn       = uSPI_csn0;
    s_command.cmd       = READ_STATUS_REG2_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.addr_bits = 0;
    s_command.addr      = 0;
    s_command.addr_mode = uSPI_Quad;
    s_command.tx_data   = 0;
    s_command.tx_buffer = 0;
    s_command.tx_bits   = 0;
    s_command.rx_bits   = 8;
    s_command.alter_data_bits = 0;
    s_command.dummy     = 0;
    s_command.rx_buffer = (uint8_t *)&reg;
    s_command.data_mode = uSPI_Quad;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    /* Check the value of the register */
    if ((reg & S25FS256S_SR2_ES) != 0 || (reg & S25FS256S_SR2_PS) != 0)
    {
        return QSPI_SUSPENDED;
    }
    else
    {
        return QSPI_OK;
    }
}

/**
 * @brief  Return the configuration of the QSPI memory.
 * @param  pInfo : pointer on the configuration structure
 * @retval QSPI memory status
 */
uint8_t S25FS_QSPI_GetInfo(S25FS_QSPI_Info* pInfo)
{
    /* Configure the structure with the memory configuration */
    pInfo->FlashSize          = S25FS256S_FLASH_SIZE;
    pInfo->EraseSectorSize    = S25FS256S_64K_SECTOR_SIZE;
    pInfo->EraseSectorsNumber = (S25FS256S_FLASH_SIZE/S25FS256S_64K_SECTOR_SIZE);
    pInfo->ProgPageSize       = S25FS256S_256_PAGE_SIZE;
    pInfo->ProgPagesNumber    = (S25FS256S_FLASH_SIZE/S25FS256S_256_PAGE_SIZE);

    return QSPI_OK;
}

/**
 * @brief  This function suspends an ongoing erase command.
 * @retval QSPI memory status
 */
uint8_t S25FS_QSPI_SuspendErase(void)
{
    /* Check whether the device is busy (erase operation is
       in progress).
    */
    if (S25FS_QSPI_GetStatus() == QSPI_BUSY)
    {
        /* Send the erase suspend command */
        S25FS_QSPI_WriteCMD(PROG_ERASE_SUSPEND_CMD, uSPI_Quad);

        /* Wait that memory is ready */
        if (S25FS_QSPI_AutoPollingMemReady(0x100, uSPI_Quad) != QSPI_OK)
        {
            return QSPI_ERROR;
        }

        if (S25FS_QSPI_GetStatus2() == QSPI_SUSPENDED)
        {
            return QSPI_OK;
        }

        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  This function resumes a paused erase command.
 * @retval QSPI memory status
 */
uint8_t S25FS_QSPI_ResumeErase(void)
{
    /* Check whether the device is in suspended state */
    if (S25FS_QSPI_GetStatus2() == QSPI_SUSPENDED)
    {
        /* Send the erase resume command */
        S25FS_QSPI_WriteCMD(PROG_ERASE_RESUME_CMD, uSPI_Quad);

        /*
          When this command is executed, the status register write in progress bit is set to 1, and
          the flag status register program erase controller bit is set to 0. This command is ignored
          if the device is not in a suspended state.
        */

        if (S25FS_QSPI_GetStatus() == QSPI_BUSY)
        {
            if (S25FS_QSPI_GetStatus2() != QSPI_SUSPENDED)
            {
                return QSPI_OK;
            }
        }

        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  This function enter the QSPI memory in deep power down mode.
 * @retval QSPI memory status
 */
uint8_t S25FS_QSPI_EnterDeepPowerDown(void)
{
    /* Send the deep power down command */
    S25FS_QSPI_WriteCMD(DEEP_POWER_DOWN_CMD, uSPI_Quad);

    /* ---          Memory takes 10us max to enter deep power down          --- */
    /* --- At least 30us should be respected before leaving deep power down --- */

    return QSPI_OK;
}

/**
 * @brief  This function leave the QSPI memory from deep power down mode.
 * @retval QSPI memory status
 */
uint8_t S25FS_QSPI_LeaveDeepPowerDown(void)
{
    /* Send the leave deep power down command */
    S25FS_QSPI_WriteCMD(RELEASE_DEEP_POWER_DOWN_CMD, uSPI_Quad);

    /* --- A NOP command is sent to the memory, as the nCS should be low for at least 20 ns --- */
    /* ---                  Memory takes 35us min to leave deep power down                  --- */

    return QSPI_OK;
}

/**
 * @}
 */

/** @addtogroup GAPUINO_QSPI_Private_Functions
 * @{
 */

/**
 * @brief  This function set flash page size in QSPI memory.
 * @param  Size : The page size
 * @retval None
 */
static uint8_t S25FS_QSPI_PageSize(uint32_t Size)
{
    spi_command_sequence_t s_command;
    uint8_t cr3;

    S25FS_QSPI_WriteEnable(uSPI_Single);

    if(Size == S25FS256S_256_PAGE_SIZE)
        cr3 = 0x00;
    else
        cr3 = 0x10;

    /* Initialize the write command */
    s_command.csn       = uSPI_csn0;
    s_command.cmd       = WRITE_ANY_REG_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_bits = 24;
    s_command.addr      = CR3V << 8;
    s_command.addr_mode = uSPI_Single;
    s_command.tx_data   = cr3;
    s_command.tx_buffer = 0;
    s_command.tx_bits   = 8;
    s_command.rx_bits   = 0;
    s_command.alter_data_bits = 0;
    s_command.dummy     = 0;
    s_command.rx_buffer = 0;
    s_command.data_mode = uSPI_Single;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    return QSPI_OK;
}

/**
 * @brief  This function reset the QSPI memory.
 * @retval None
 */
static uint8_t S25FS_QSPI_ResetMemory()
{
    /* Send the reset memory enable command */
    S25FS_QSPI_WriteCMD(RESET_ENABLE_CMD, uSPI_Single);

    /* Send the reset memory command */
    S25FS_QSPI_WriteCMD(RESET_MEMORY_CMD, uSPI_Single);

    /* Configure automatic polling mode to wait the memory is ready */
    if (S25FS_QSPI_AutoPollingMemReady(0xF000, uSPI_Single) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  This function Write a CMD.
 * @param  CMD : The command need to send
 * @param  QPI : Quad Mode or not
 * @retval None
 */
static uint8_t S25FS_QSPI_WriteCMD(uint32_t CMD, spi_qpi_t QPI)
{
    int index = 0;

    s_command_sequence[index++] = SPIM_CMD_SOT(uSPI_csn0);
    s_command_sequence[index++] = SPIM_CMD_SEND_CMD(CMD, 8, QPI);
    s_command_sequence[index++] = SPIM_CMD_EOT(0);

    /* Blocking transfer */
    SPI_MasterTransferBlocking(SPIM0,
                               (uint32_t* )s_command_sequence,
                               (index *sizeof(uint32_t)),
                               0, 0, 32);
    return QSPI_OK;
}

/**
 * @brief  This function send a Write Enable and wait it is effective.
 * @param  QPI : Quad Mode or not
 * @retval None
 */
static uint8_t S25FS_QSPI_WriteEnable(spi_qpi_t QPI)
{
    S25FS_QSPI_WriteCMD(WRITE_ENABLE_CMD, QPI);

    /* Configure automatic polling mode to wait for WEL bit */
    spi_polling_config_t conf;
    conf.csn       = uSPI_csn0;
    conf.cmd       = READ_STATUS_REG1_CMD;
    conf.cmd_mode  = QPI;
    conf.match_type  = uSPI_Match_Bits_One;
    conf.match_bits  = 8;
    conf.match_value = (S25FS256S_SR1_WEL);
    conf.data_mode = QPI;
    conf.time_out  = 0x500;

    /* Auto polling */
    SPI_Master_AutoPolling(SPIM0, &conf);

    return QSPI_OK;
}

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  Timeout : Timeout for auto-polling
  * @param  QPI : Quad Mode or not
  * @retval None
  */
static uint8_t S25FS_QSPI_AutoPollingMemReady(uint32_t Timeout, spi_qpi_t QPI)
{
    /* Configure automatic polling mode to wait for end of program */
    spi_polling_config_t conf;
    conf.csn       = uSPI_csn0;
    conf.cmd       = READ_STATUS_REG1_CMD;
    conf.cmd_mode  = QPI;
    conf.match_type  = uSPI_Match_Bits_Zero;
    conf.match_bits  = 8;
    conf.match_value = 0XFFFF & ~(S25FS256S_SR1_WIP);
    conf.data_mode = QPI;
    conf.time_out  = Timeout;

    /* Auto polling */
    SPI_Master_AutoPolling(SPIM0, &conf);

    return QSPI_OK;
}

/**
 * @brief  This function enables/disables the Quad mode of the memory.
 * @param  Operation : QSPI_QUAD_ENABLE or QSPI_QUAD_DISABLE mode
 * @retval None
 */
static uint8_t S25FS_QSPI_QuadMode(uint8_t Operation)
{
    spi_command_sequence_t s_command;
    reg = 0;

    S25FS_QSPI_WriteEnable(uSPI_Single);
    /* Activate/deactivate the Quad mode */
    if (Operation == QSPI_QUAD_ENABLE)
    {
        reg |= S25FS256S_CR1_QUAD;
    }
    else
    {
        reg &= ~(S25FS256S_CR1_QUAD);
    }

    /* Initialize the write command */
    s_command.csn       = uSPI_csn0;
    s_command.cmd       = WRITE_ANY_REG_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_bits = 24;
    s_command.addr      = CR1V << 8;
    s_command.addr_mode = uSPI_Single;
    s_command.tx_data   = reg;
    s_command.tx_buffer = 0;
    s_command.tx_bits   = 8;
    s_command.rx_bits   = 0;
    s_command.alter_data_bits = 0;
    s_command.dummy     = 0;
    s_command.rx_buffer = 0;
    s_command.data_mode = uSPI_Single;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    S25FS_QSPI_WriteEnable(uSPI_Single);
    reg = S25FS256S_DUMMY_CYCLES_READ;

    /* Activate/deactivate the Quad mode */
    if (Operation == QSPI_QUAD_ENABLE)
    {
        reg |= S25FS256S_CR2_QA_NV;
    }
    else
    {
        reg &= ~(S25FS256S_CR2_QA_NV);
    }

    /* Initialize the write command */
    s_command.addr      = CR2V << 8;
    s_command.tx_data   = reg;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    /* Wait that memory is ready */
    if (S25FS_QSPI_AutoPollingMemReady(0xF000, uSPI_Quad) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Check the configuration has been correctly done */
    s_command.csn       = uSPI_csn0;
    s_command.cmd       = READ_ANY_REG_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Quad;
    s_command.addr_bits = 24;
    s_command.addr      = CR2V << 8;
    s_command.addr_mode = uSPI_Quad;
    s_command.tx_data   = 0;
    s_command.tx_buffer = 0;
    s_command.tx_bits   = 0;
    s_command.rx_bits   = 8;
    s_command.alter_data_bits = 0;
    s_command.dummy     = S25FS256S_DUMMY_CYCLES_READ;
    s_command.rx_buffer = (uint8_t *)&reg;
    s_command.data_mode = uSPI_Quad;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    if ((((reg & S25FS256S_CR2_QA_NV) == 0) && (Operation == QSPI_QUAD_ENABLE)) ||
        (((reg & S25FS256S_CR2_QA_NV) != 0) && (Operation == QSPI_QUAD_DISABLE)))
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
