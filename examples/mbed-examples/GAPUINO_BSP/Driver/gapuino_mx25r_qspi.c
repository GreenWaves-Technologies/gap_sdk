/**
******************************************************************************
* @file    gapuino_mx25r_qspi.c
* @author  GreenWave Technologies Application Team
* @brief   This file includes a standard driver for the MX25R6435F QSPI
*          memory mounted on GAPUINO board.
@verbatim
==============================================================================
##### How to use this driver #####
==============================================================================
[..]
(#) This driver is used to drive the MX25R6435F QSPI external
memory mounted on GAPUINO board.

(#) This driver need a specific component driver (MX25R6435F) to be included with.

(#) Initialization steps:
(++) Initialize the QPSI external memory using the MX25R_QSPI_Init() function. This
function includes the MSP layer hardware resources initialization and the
QSPI interface with the external memory. The MX25R_QSPI_DeInit() can be used
to deactivate the QSPI interface.

(#) QSPI memory operations
(++) QSPI memory can be accessed with read/write operations once it is
initialized.
Read/write operation can be performed with AHB access using the functions
MX25R_QSPI_Read()/MX25R_QSPI_Write().
(++) The function MX25R_QSPI_GetInfo() returns the configuration of the QSPI memory.
(see the QSPI memory data sheet)
(++) Perform erase block operation using the function MX25R_QSPI_Erase_Block() and by
specifying the block address. You can perform an erase operation of the whole
chip by calling the function MX25R_QSPI_Erase_Chip().
(++) The function MX25R_QSPI_GetStatus() returns the current status of the QSPI memory.
(see the QSPI memory data sheet)
(++) Perform erase sector operation using the function MX25R_QSPI_Erase_Sector()
which is not blocking. So the function MX25R_QSPI_GetStatus() should be used
to check if the memory is busy, and the functions MX25R_QSPI_SuspendErase()/
MX25R_QSPI_ResumeErase() can be used to perform other operations during the
sector erase.
(++) Deep power down of the QSPI memory is managed with the call of the functions
MX25R_QSPI_EnterDeepPowerDown()/MX25R_QSPI_LeaveDeepPowerDown()
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
#include "gapuino_mx25r_qspi.h"

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
static uint8_t MX25R_QSPI_ResetMemory        ();
static uint8_t MX25R_QSPI_WriteEnable        ();
static uint8_t MX25R_QSPI_WriteCMD           (uint32_t CMD, uint8_t Bytes);
static uint8_t MX25R_QSPI_AutoPollingMemReady(uint32_t Timeout);
static uint8_t MX25R_QSPI_QuadMode           (uint8_t Operation);
static uint8_t MX25R_QSPI_HighPerfMode       (uint8_t Operation);

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
uint8_t MX25R_QSPI_Init(void)
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
    if (MX25R_QSPI_ResetMemory() != QSPI_OK)
    {
        return QSPI_NOT_SUPPORTED;
    }

    /* QSPI quad enable */
    if (MX25R_QSPI_QuadMode(QSPI_QUAD_ENABLE) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* High performance mode enable */
    if (MX25R_QSPI_HighPerfMode(QSPI_HIGH_PERF_ENABLE) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  De-Initializes the QSPI interface.
 * @retval QSPI status
 */
uint8_t MX25R_QSPI_DeInit(void)
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
uint8_t MX25R_QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
    spi_command_sequence_t s_command;

    /* Initialize the config command */
    s_command.csn       = uSPI_csn1;
    s_command.cmd       = QUAD_INOUT_READ_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_bits = 24;
    s_command.addr      = ReadAddr;
    s_command.addr_mode = uSPI_Quad;
    s_command.tx_bits   = 0;
    s_command.tx_data   = 0;
    s_command.tx_buffer = 0;
    s_command.rx_bits   = Size * 8;
    s_command.alter_data_bits = 8;
    s_command.alter_data      = MX25R6435F_ALT_BYTES_NO_PE_MODE;
    s_command.alter_data_mode = uSPI_Quad;
    s_command.dummy     = MX25R6435F_DUMMY_CYCLES_READ_QUAD;
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
uint8_t MX25R_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
    spi_command_sequence_t s_command;

    uint32_t end_addr, current_size, current_addr;

    /* Calculation of the size between the write address and the end of the page */
    current_size = MX25R6435F_PAGE_SIZE - (WriteAddr % MX25R6435F_PAGE_SIZE);

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > Size)
    {
        current_size = Size;
    }

    /* Initialize the adress variables */
    current_addr = WriteAddr;
    end_addr = WriteAddr + Size;

    /* Initialize the write command */
    s_command.csn       = uSPI_csn1;
    s_command.cmd       = QUAD_PAGE_PROG_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_bits = 24;
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
        if (MX25R_QSPI_WriteEnable() != QSPI_OK)
        {
            return QSPI_ERROR;
        }

        /* Transmission of the data */
        SPI_MasterTransferCommandSequence(SPIM0, &s_command);

        /* Configure automatic polling mode to wait for end of program */
        if (MX25R_QSPI_AutoPollingMemReady(0xF000) != QSPI_OK)
        {
            return QSPI_ERROR;
        }

        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        pData += current_size;
        current_size = ((current_addr + MX25R6435F_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : MX25R6435F_PAGE_SIZE;
    } while (current_addr < end_addr);

    return QSPI_OK;
}

/**
 * @brief  Erases the specified block of the QSPI memory.
 * @param  BlockAddress : Block address to erase
 * @retval QSPI memory status
 */
uint8_t MX25R_QSPI_Erase_Block(uint32_t BlockAddress)
{
    spi_command_sequence_t s_command;

    /* Initialize the erase block command */
    s_command.csn       = uSPI_csn1;
    s_command.cmd       = BLOCK_ERASE_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_bits = 24;
    s_command.addr      = BlockAddress;
    s_command.addr_mode = uSPI_Single;
    s_command.tx_data   = 0;
    s_command.tx_buffer = 0;
    s_command.tx_bits   = 0;
    s_command.rx_bits   = 0;
    s_command.alter_data_bits = 0;
    s_command.dummy     = 0;
    s_command.rx_buffer = 0;
    s_command.data_mode = uSPI_Single;

    /* Enable write operations */
    if (MX25R_QSPI_WriteEnable() != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    /* Configure automatic polling mode to wait for end of erase */
    if (MX25R_QSPI_AutoPollingMemReady(0xF000) != QSPI_OK)
    {
      return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  Erases the specified sector of the QSPI memory.
 * @param  Sector : Sector address to erase (0 to 255)
 * @retval QSPI memory status
 * @note This function is non blocking meaning that sector erase
 *       operation is started but not completed when the function
 *       returns. Application has to call MX25R_QSPI_GetStatus()
 *       to know when the device is available again (i.e. erase operation
 *       completed).
 */
uint8_t MX25R_QSPI_Erase_Sector(uint32_t Sector)
{
    spi_command_sequence_t s_command;

    if (Sector >= (uint32_t)(MX25R6435F_FLASH_SIZE/MX25R6435F_SECTOR_SIZE))
    {
        return QSPI_ERROR;
    }

    /* Initialize the erase sector command */
    s_command.csn       = uSPI_csn1;
    s_command.cmd       = SECTOR_ERASE_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_bits = 24;
    s_command.addr      = (Sector * MX25R6435F_SECTOR_SIZE);
    s_command.addr_mode = uSPI_Single;
    s_command.tx_data   = 0;
    s_command.tx_buffer = 0;
    s_command.tx_bits   = 0;
    s_command.rx_bits   = 0;
    s_command.alter_data_bits = 0;
    s_command.dummy     = 0;
    s_command.rx_buffer = 0;
    s_command.data_mode = uSPI_Single;

    /* Enable write operations */
    if (MX25R_QSPI_WriteEnable() != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    /* Configure automatic polling mode to wait for end of erase */
    if (MX25R_QSPI_AutoPollingMemReady(0xF000) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  Erases the entire QSPI memory.
 * @retval QSPI memory status
 */
uint8_t MX25R_QSPI_Erase_Chip(void)
{
    /* Enable write operations */
    if (MX25R_QSPI_WriteEnable() != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Send the erase suspend command */
    MX25R_QSPI_WriteCMD(CHIP_ERASE_CMD, 1);

    /* Configure automatic polling mode to wait the memory is ready */
    if (MX25R_QSPI_AutoPollingMemReady(0xF000) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  Reads current status of the QSPI memory.
 * @retval QSPI memory status
 */
uint8_t MX25R_QSPI_GetStatus(void)
{
    spi_command_sequence_t s_command;

    /* Initialize the status command */
    s_command.csn       = uSPI_csn1;
    s_command.cmd       = READ_SEC_REG_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_bits = 0;
    s_command.addr      = 0;
    s_command.addr_mode = uSPI_Single;
    s_command.tx_data   = 0;
    s_command.tx_buffer = 0;
    s_command.tx_bits   = 0;
    s_command.rx_bits   = 8;
    s_command.alter_data_bits = 0;
    s_command.dummy     = 0;
    s_command.rx_buffer = (uint8_t *)&reg;
    s_command.data_mode = uSPI_Single;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    /* Check the value of the register */
    if ((reg & (MX25R6435F_SECR_P_FAIL | MX25R6435F_SECR_E_FAIL)) != 0)
    {
        return QSPI_ERROR;
    }
    else if ((reg & (MX25R6435F_SECR_PSB | MX25R6435F_SECR_ESB)) != 0)
    {
        return QSPI_SUSPENDED;
    }

    /* Initialize the read status register command */
    s_command.cmd       = READ_STATUS_REG_CMD;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    /* Check the value of the register */
    if ((reg & MX25R6435F_SR_WIP) != 0)
    {
        return QSPI_BUSY;
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
uint8_t MX25R_QSPI_GetInfo(MX25R_QSPI_Info* pInfo)
{
    /* Configure the structure with the memory configuration */
    pInfo->FlashSize          = MX25R6435F_FLASH_SIZE;
    pInfo->EraseSectorSize    = MX25R6435F_SECTOR_SIZE;
    pInfo->EraseSectorsNumber = (MX25R6435F_FLASH_SIZE/MX25R6435F_SECTOR_SIZE);
    pInfo->ProgPageSize       = MX25R6435F_PAGE_SIZE;
    pInfo->ProgPagesNumber    = (MX25R6435F_FLASH_SIZE/MX25R6435F_PAGE_SIZE);

    return QSPI_OK;
}

/**
 * @brief  This function suspends an ongoing erase command.
 * @retval QSPI memory status
 */
uint8_t MX25R_QSPI_SuspendErase(void)
{
    /* Check whether the device is busy (erase operation is
       in progress).
    */
    if (MX25R_QSPI_GetStatus() == QSPI_BUSY)
    {
        /* Send the erase suspend command */
        MX25R_QSPI_WriteCMD(PROG_ERASE_SUSPEND_CMD, 1);

        if (MX25R_QSPI_GetStatus() == QSPI_SUSPENDED)
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
uint8_t MX25R_QSPI_ResumeErase(void)
{
    /* Check whether the device is in suspended state */
    if (MX25R_QSPI_GetStatus() == QSPI_SUSPENDED)
    {
        /* Send the erase resume command */
        MX25R_QSPI_WriteCMD(PROG_ERASE_RESUME_CMD, 1);

        /*
          When this command is executed, the status register write in progress bit is set to 1, and
          the flag status register program erase controller bit is set to 0. This command is ignored
          if the device is not in a suspended state.
        */

        if (MX25R_QSPI_GetStatus() == QSPI_BUSY)
        {
            return QSPI_OK;
        }

        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  This function enter the QSPI memory in deep power down mode.
 * @retval QSPI memory status
 */
uint8_t MX25R_QSPI_EnterDeepPowerDown(void)
{
    /* Send the deep power down command */
    MX25R_QSPI_WriteCMD(DEEP_POWER_DOWN_CMD, 1);

    /* ---          Memory takes 10us max to enter deep power down          --- */
    /* --- At least 30us should be respected before leaving deep power down --- */

    return QSPI_OK;
}

/**
 * @brief  This function leave the QSPI memory from deep power down mode.
 * @retval QSPI memory status
 */
uint8_t MX25R_QSPI_LeaveDeepPowerDown(void)
{
    /* Send the leave deep power down command */
    MX25R_QSPI_WriteCMD(NO_OPERATION_CMD, 1);

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
 * @brief  This function reset the QSPI memory.
 * @retval None
 */
static uint8_t MX25R_QSPI_ResetMemory()
{
    /* Send the reset memory enable command */
    MX25R_QSPI_WriteCMD(RESET_ENABLE_CMD, 1);

    /* Send the reset memory command */
    MX25R_QSPI_WriteCMD(RESET_MEMORY_CMD, 1);

    /* Configure automatic polling mode to wait the memory is ready */
    if (MX25R_QSPI_AutoPollingMemReady(0xF000) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  This function Write a CMD.
 * @param  CMD : The command need to send
 * @param  Bytes : The bytes need to send up to 4
 * @retval None
 */
static uint8_t MX25R_QSPI_WriteCMD(uint32_t value, uint8_t Bytes)
{
    int index = 0;

    s_command_sequence[index++] = SPIM_CMD_SOT(uSPI_csn1);
    if (Bytes == 4 ) {
        s_command_sequence[index++] = SPIM_CMD_SEND_ADDR(Bytes * 8, uSPI_Single);
        s_command_sequence[index++] = value;
    } else {
        s_command_sequence[index++] = SPIM_CMD_SEND_CMD(value, Bytes * 8, uSPI_Single);
    }
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
 * @retval None
 */
static uint8_t MX25R_QSPI_WriteEnable()
{
    MX25R_QSPI_WriteCMD(WRITE_ENABLE_CMD, 1);

    /* Configure automatic polling mode to wait for WEL bit */
    spi_polling_config_t conf;
    conf.csn       = uSPI_csn1;
    conf.cmd       = READ_STATUS_REG_CMD;
    conf.cmd_mode  = uSPI_Single;
    conf.match_type  = uSPI_Match_Bits_One;
    conf.match_bits  = 8;
    conf.match_value = (MX25R6435F_SR_WEL);
    conf.data_mode = uSPI_Single;
    conf.time_out  = 0x500;

    /* Auto polling */
    SPI_Master_AutoPolling(SPIM0, &conf);

    return QSPI_OK;
}

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  Timeout : Timeout for auto-polling
  * @retval None
  */
static uint8_t MX25R_QSPI_AutoPollingMemReady(uint32_t Timeout)
{
    /* Configure automatic polling mode to wait for end of program */
    spi_polling_config_t conf;
    conf.csn       = uSPI_csn1;
    conf.cmd       = READ_STATUS_REG_CMD;
    conf.cmd_mode  = uSPI_Single;
    conf.match_type  = uSPI_Match_Bits_Zero;
    conf.match_bits  = 8;
    conf.match_value = 0XFFFF & ~(MX25R6435F_SR_WIP);
    conf.data_mode = uSPI_Single;
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
static uint8_t MX25R_QSPI_QuadMode(uint8_t Operation)
{
    spi_command_sequence_t s_command;

    /* Initialize the status command */
    s_command.csn       = uSPI_csn1;
    s_command.cmd       = READ_STATUS_REG_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_bits = 0;
    s_command.addr      = 0;
    s_command.addr_mode = uSPI_Single;
    s_command.tx_data   = 0;
    s_command.tx_buffer = 0;
    s_command.tx_bits   = 0;
    s_command.rx_bits   = 8;
    s_command.alter_data_bits = 0;
    s_command.dummy     = 0;
    s_command.rx_buffer = (uint8_t *)&reg;
    s_command.data_mode = uSPI_Single;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    /* Enable write operations */
    if (MX25R_QSPI_WriteEnable() != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Activate/deactivate the Quad mode */
    if (Operation == QSPI_QUAD_ENABLE)
    {
        reg |= MX25R6435F_SR_QE;
    }
    else
    {
        reg &= ~(MX25R6435F_SR_QE);
    }

    /* Write status register */
    uint32_t data = WRITE_STATUS_CFG_REG_CMD;
    data |= reg << 8;
    MX25R_QSPI_WriteCMD(data, 2);

    /* Wait that memory is ready */
    if (MX25R_QSPI_AutoPollingMemReady(0xF000) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Check the configuration has been correctly done */
    s_command.cmd = READ_STATUS_REG_CMD;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    if ((((reg & MX25R6435F_SR_QE) == 0) && (Operation == QSPI_QUAD_ENABLE)) ||
        (((reg & MX25R6435F_SR_QE) != 0) && (Operation == QSPI_QUAD_DISABLE)))
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
 * @brief  This function enables/disables the high performance mode of the memory.
 * @param  Operation : QSPI_HIGH_PERF_ENABLE or QSPI_HIGH_PERF_DISABLE high performance mode
 * @retval None
 */
static uint8_t MX25R_QSPI_HighPerfMode(uint8_t Operation)
{
    spi_command_sequence_t s_command;
    uint8_t regs[3];

    /* Initialize the status command */
    s_command.csn       = uSPI_csn1;
    s_command.cmd       = READ_STATUS_REG_CMD;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Single;
    s_command.addr_bits = 0;
    s_command.addr      = 0;
    s_command.addr_mode = uSPI_Single;
    s_command.tx_data   = 0;
    s_command.tx_buffer = 0;
    s_command.tx_bits   = 0;
    s_command.rx_bits   = 8;
    s_command.alter_data_bits = 0;
    s_command.dummy     = 0;
    s_command.rx_buffer = (uint8_t *)&reg;
    s_command.data_mode = uSPI_Single;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    regs[0] = reg & 0xFF;

    /* Read configuration registers */
    s_command.cmd = READ_CFG_REG_CMD;
    s_command.rx_bits    = 8*2;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    regs[1] = reg & 0xFF;
    regs[2] = (reg >> 8) & 0xFF;

    /* Enable write operations */
    if (MX25R_QSPI_WriteEnable() != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Activate/deactivate the Quad mode */
    if (Operation == QSPI_HIGH_PERF_ENABLE)
    {
        regs[2] |= MX25R6435F_CR2_LH_SWITCH;
    }
    else
    {
        regs[2] &= ~(MX25R6435F_CR2_LH_SWITCH);
    }

    /* Write status register */
    uint32_t data = WRITE_STATUS_CFG_REG_CMD;
    data |= regs[0] << 8;
    data |= regs[1] << 16;
    data |= regs[2] << 24;
    MX25R_QSPI_WriteCMD(data, 4);

    /* Wait that memory is ready */
    if (MX25R_QSPI_AutoPollingMemReady(0xF000) != QSPI_OK)
    {
        return QSPI_ERROR;
    }

    /* Check the configuration has been correctly done */
    s_command.cmd = READ_CFG_REG_CMD;
    s_command.rx_bits     = 2 * 8;

    /* Send the command */
    SPI_MasterTransferCommandSequence(SPIM0, &s_command);

    regs[1] = reg & 0xFF;

    if ((((regs[1] & MX25R6435F_CR2_LH_SWITCH) == 0) && (Operation == QSPI_HIGH_PERF_ENABLE)) ||
        (((regs[1] & MX25R6435F_CR2_LH_SWITCH) != 0) && (Operation == QSPI_HIGH_PERF_DISABLE)))
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
