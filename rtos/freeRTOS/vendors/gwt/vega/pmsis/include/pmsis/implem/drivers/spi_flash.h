#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

#include "hal.h"

#define DUMMY 8

static inline void spi_flash_read_register(int spim_id, uint8_t cmd,
        int size, uint32_t *reg)
{
    volatile spi_command_sequence_t s_command;
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = cmd;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = 0;
    s_command.rx_size   = size;
    s_command.rx_buffer = (uint8_t *)reg;
    s_command.data_mode = 0;

    spi_master_transfer_command_sequence(spim_id, &s_command);
}

static inline void spi_flash_sync_polling(uint32_t spim_id)
{
    uint32_t ret;
    do{
        spi_flash_read_register(spim_id, 5, 4, &ret);
    }while(ret & 0x1);
}

static inline void spi_flash_write_enable(int spim_id, uint8_t qpi)
{
    volatile spi_command_sequence_t s_command;
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x06;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = qpi;
    spi_master_transfer_command_sequence(spim_id, &s_command);
}

static inline spi_flash_read(int spim_id, uint32_t flash_addr,
        void *read_buffer, uint32_t size, int qspi)
{
    volatile spi_command_sequence_t s_command;
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    // setup cmd
    s_command.cmd       = 0x03;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = qspi;
    // setup flash addr
    s_command.addr_bits = 24;
    s_command.addr      = flash_addr;
    s_command.addr_mode = qspi;
    // setup rx
    s_command.rx_size   = size;
    s_command.rx_buffer = (uint8_t *)read_buffer;
    s_command.data_mode = qspi;
    
    spi_master_transfer_command_sequence(spim_id, &s_command);
}

static inline spi_flash_write(int spim_id, uint32_t flash_addr,
        void *write_buffer, uint32_t size, int qspi)
{
    volatile spi_command_sequence_t s_command;
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    // setup cmd
    s_command.cmd       = 0x02;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = qspi;
    // setup flash addr
    s_command.addr_bits = 24;
    s_command.addr      = flash_addr;
    s_command.addr_mode = qspi;
    // setup tx
    s_command.tx_size   = size;
    s_command.tx_buffer = (uint8_t *)write_buffer;
    s_command.data_mode = qspi;
    
    spi_master_transfer_command_sequence(spim_id, &s_command);
    spi_flash_sync_polling(spim_id);
}


static inline void spi_flash_erase(int spim_id, uint32_t flash_addr,
        uint32_t size, int qspi)
{
    spi_flash_write_enable(spim_id, qspi);
    volatile spi_command_sequence_t s_command;
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    // setup cmd
    s_command.cmd       = 0x20;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = qspi;
    // setup flash addr
    s_command.addr_bits = 24;
    s_command.addr      = flash_addr;
    s_command.addr_mode = qspi;
    
    spi_master_transfer_command_sequence(spim_id, &s_command);
}

static inline void spi_flash_conf(int spim_id, int qspi)
{
    spi_flash_write_enable(spim_id, qspi);
    volatile spi_command_sequence_t s_command;
    
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = 0x71;
    s_command.cmd_bits  = 8;
    s_command.addr_bits = 32;
    s_command.addr      =  (0x80000300 | DUMMY);
    s_command.cmd_mode  =  qspi;
    s_command.addr_mode =  qspi;
    spi_master_transfer_command_sequence(spim_id, &s_command);
}

#endif
