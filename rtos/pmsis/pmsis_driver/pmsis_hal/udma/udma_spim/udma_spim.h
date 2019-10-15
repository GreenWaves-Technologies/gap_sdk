/**
 * Defines/types and small low level driver for udma spim main functionnalities
 */

#ifndef __UDMA_SPIM_H__
#define __UDMA_SPIM_H__

#include "targets/pmsis_targets.h"

#ifdef __GAP9__

#define SPIM(id) ((spim_t *) UDMA_SPIM(id))

/*! @brief SPI master request create sequence structure.*/
typedef struct _spi_command_sequence {
    uint8_t  csn;           /*!< The chip select to device */
    uint16_t cmd;           /*!< The command sent to device */
    uint8_t  cmd_bits;      /*!< The bits of cmd sent to device */
    uint8_t  cmd_mode;      /*!< Specifies the Instruction Mode single or quad */
    uint8_t  addr_bits;     /*!< The bits of address sent to device */
    uint32_t addr;          /*!< If addr_bits is not 0, the address sent to device*/
    uint8_t  addr_mode;     /*!< Specifies the Address Mode 1 or 4 */
    uint32_t tx_size;       /*!< The size (in bytes) of buffer sent to device*/
    uint8_t *tx_buffer;     /*!< If tx_bits is more than a word, the data buffer pointer written to device*/
    uint32_t rx_size;       /*!< The length (in bytes) of data readen from device*/
    uint8_t  alter_data;       /*!< Specifies the alternative data Mode (used for dummy cycles and data phases)*/
    uint8_t  alter_data_bits;  /*!< Specifies the alternative data bits */
    uint8_t  alter_data_mode;  /*!< Specifies the alternative data Mode single or quad */
    uint8_t  dummy;         /*!< Specifies the Number of Dummy Cycles */
    uint8_t  data_mode;     /*!< Specifies the Data Mode single or quad */
    uint8_t *rx_buffer;    /*!< If rx_size is not 0, pointer of receive buffer*/
} spi_command_sequence_t;
#include "udma_spim_gap9.h"
#endif

#ifdef __GAP8__
#define SPIM(id) ((spi_t *) UDMA_SPIM(id))
#include "pmsis_hal/udma/udma_spim/udma_spim_gap8.h"
#endif

#endif
