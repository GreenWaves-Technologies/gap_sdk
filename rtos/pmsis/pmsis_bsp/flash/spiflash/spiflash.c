/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Authors: Antoine Faravelon, GreenWaves Technologies 
 * (antoine.faravelon@greenwaves-technologies.com)
 */


/*
 * TODO 1: Mutexes on all functions using shared vars
 * TODO 2: re uniformize api
 * TODO 3: Port to new spi API (depend on implem new api for spi)
 */
#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/flash/spiflash.h"
#include "pmsis/drivers/spi.h"

#define SECTOR_SIZE (1<<12)
#define QSPI_FLASH_CS 0

#define QSPIF_DUMMY_CYCLES(x)   ((uint8_t)((x&0xF)<<3))
#define QSPIF_BURST_LEN(x)      ((uint8_t)((x&0x3)<<0))
#define QSPIF_BURST_ENA(x)      ((uint8_t)((x&0x1)<<1))

#define QSPIF_WR_READ_REG_CMD   ((uint8_t)0xC0)
#define QSPIF_WR_EN_CMD         ((uint8_t)0x06)
#define QSPIF_QPI_EN_CMD        ((uint8_t)0x35)
#define QSPIF_QPI_FAST_READ_CMD ((uint8_t)0x0b)
#define QSPIF_QIO_FAST_READ_CMD ((uint8_t)0xEB)
#define QSPIF_QO_FAST_READ_CMD  ((uint8_t)0x6B)
#define QSPIF_PAGE_PROG_CMD     ((uint8_t)0x02)
#define QSPIF_QIO_PAGE_PROG_CMD ((uint8_t)0x32)
#define QSPIF_ERASE_SECTOR_CMD  ((uint8_t)0x20)
#define QSPIF_READ_STATUS_CMD   ((uint8_t)0x05)

#define QSPIF_WR_PROLOGUE_SIZE 0x4
#define QSPIF_RCV_UCODE_SIZE   (36)
#define QSPIF_PPQ_UCODE_SIZE   (5*sizeof(uint32_t))
#define QSPIF_ERASE_SIZE       0x4
#define QSPIF_PAGE_SIZE        0x100

// TODO: replace by conf
#define QE_STATUS_ID        0
#define QE_BIT_POS          6
#define READ_QE_STATUS_CMD  0x5
#define WRITE_STATUS_QE_CMD 0x1
#define STATUS_REG_SIZE     1
#define BUSY_BIT_ID         0
#define BUSY_BIT_POS        0

#define IS_BUSY(status_reg) ((status_reg[BUSY_BIT_ID] >> BUSY_BIT_POS) & 0x1)

// DUMMY Cycles defined for typical 50MHz settings
#define DUMMY_CYCLES 3
// Avoid QPI problems
#define SPI_FLASH_USE_QUAD_IO

#ifndef SPI_FLASH_USE_QUAD_IO 
#define SPI_LINES_FLAG PI_SPI_LINES_QUAD
#else
#define SPI_LINES_FLAG PI_SPI_LINES_SINGLE
#endif

#define STALL_TASK_PROGRAM      0
#define STALL_TASK_ERASE_CHIP   1
#define STALL_TASK_ERASE_SECTOR 2
#define STALL_TASK_REG_SET      3
#define STALL_TASK_REG_GET      4
#define STALL_TASK_READ         5
#define STALL_TASK_READ_2D      6

typedef struct {
  struct pi_device qspi_dev;
  struct pi_spi_conf qspi_conf;
  size_t sector_size;
  // Used for communications with spiflash through udma
  uint32_t ucode_buffer[9];
  uint32_t cmd_buf[2];
  uint32_t status_reg[(STATUS_REG_SIZE/4)+1];
  
#if defined(PMSIS_DRIVERS)
  pi_mutex_t flash_op_mutex;
#else
  // Waiting que
  // Task used for internal FSM scheduling for common operations
  pi_task_t task;

  // Description of on-going task for common operations. The FSM will keep executing
  // until this operation is done
  uint32_t pending_flash_addr;
  uint32_t pending_data;
  uint32_t pending_size;

  // Waiting queue for common operations (only 1 is handled at the same time)
  pi_task_t *waiting_first;
  pi_task_t *waiting_last;

  // Task to be enqueued when the on-going operation is done
  pi_task_t *pending_task;
#endif

} spi_flash_t;


// ------ globals for some regular operations
PI_L2 static const uint8_t g_set_qspif_dummy[] = {QSPIF_WR_READ_REG_CMD,
    QSPIF_DUMMY_CYCLES(DUMMY_CYCLES)};
PI_L2 static const uint8_t g_enter_qpi_mode[] = {0x35,0};
PI_L2 static const uint8_t g_exit_qpi_mode[]  = {0xf5,0};
PI_L2 static const uint8_t g_chip_erase[]     = {0x60,0};
PI_L2 static const uint8_t g_write_enable[]   = {QSPIF_WR_EN_CMD,0};


static void wait_wip(spi_flash_t *flash_dev);

#if !defined(PMSIS_DRIVERS)
static void spiflash_program_async(struct pi_device *device, uint32_t addr,
        const void *data, uint32_t size, pi_task_t *task);

static void spiflash_check_program(void *arg);

static void spiflash_erase_chip_async(struct pi_device *device, pi_task_t *task);

static void spiflash_erase_sector_async(struct pi_device *device, uint32_t addr, pi_task_t *task);

static void spiflash_erase_async(struct pi_device *device, uint32_t addr,
    int size, pi_task_t *task);
    
static void spiflash_reg_set_async(struct pi_device *device, uint32_t addr,
        uint8_t *value, pi_task_t *task);

static void spiflash_reg_get_async(struct pi_device *device, uint32_t addr,
        uint8_t *value, pi_task_t *task);

static void spiflash_read_async(struct pi_device *device, uint32_t addr, void *data,
        uint32_t size, pi_task_t *task);

static int spiflash_copy_2d_async(struct pi_device *device, uint32_t flash_addr,
        void *buffer, uint32_t size, uint32_t stride, uint32_t length,
        int ext2loc, pi_task_t *task);

static void spiflash_erase_resume(void *arg);

static int spiflash_copy_async(struct pi_device *device, uint32_t flash_addr,
        void *buffer, uint32_t size, int ext2loc, pi_task_t *task);
#endif


static inline void qpi_flash_set_quad_enable(spi_flash_t *flash_dev)
{
    volatile uint8_t* cmd_buf = (uint8_t*)flash_dev->cmd_buf;
    uint8_t* status_reg = (uint8_t*)flash_dev->status_reg;
    pi_device_t *qspi_dev = &flash_dev->qspi_dev;
    // read status register, and then write it back with QE bit = 1
    cmd_buf[0] = READ_QE_STATUS_CMD; // read status reg
    pi_spi_send(qspi_dev, (void*)&cmd_buf[0], 1*8,
            PI_SPI_LINES_SINGLE | PI_SPI_CS_KEEP);
    pi_spi_receive(qspi_dev, (void*)&status_reg[QE_STATUS_ID], 1*8,
            PI_SPI_LINES_SINGLE | PI_SPI_CS_AUTO);
    if(!((status_reg[QE_STATUS_ID] >> QE_BIT_POS)&1))
    {
        status_reg[QE_STATUS_ID] |= (1 << QE_BIT_POS);
        // WREN before WRSR
        cmd_buf[0] = QSPIF_WR_EN_CMD;
        pi_spi_send(qspi_dev, (void*)cmd_buf, 1*8,
                PI_SPI_LINES_SINGLE | PI_SPI_CS_AUTO);
        cmd_buf[0] = WRITE_STATUS_QE_CMD; // write status reg (holds QE bit)
        cmd_buf[1] = status_reg[QE_STATUS_ID];
        pi_spi_send(qspi_dev, (void*)cmd_buf, 2*8,
                PI_SPI_LINES_SINGLE | PI_SPI_CS_AUTO);
        // flash takes some time to recover from QE bit/status reg write
        pi_time_wait_us(100000);
    }
}

static inline void qpi_flash_pre_config(spi_flash_t *flash_dev)
{
    pi_device_t *qspi_dev = &flash_dev->qspi_dev;
#ifndef SPI_FLASH_USE_QUAD_IO
    // Enter QPI mode
#warn "QSPI enter is compiled -- boot from spi flash might be compromised"
    pi_spi_send(qspi_dev, (void*)g_enter_qpi_mode, 1*8,
            PI_SPI_LINES_SINGLE | PI_SPI_CS_AUTO);
#endif
#ifdef FLASH_CHIP_ERASE
    // flash chip erase (optional!)
    pi_spi_send(qspi_dev, (void*)g_write_enable, 8,
            SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    pi_spi_send(qspi_dev, (void*)g_chip_erase, 8,
            SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    wait_wip(flash_dev);
    printf("erase done\n");
#endif

    qpi_flash_set_quad_enable(flash_dev);

    // Set read parameters (dummy cycles)
    pi_spi_send(qspi_dev, (void*)g_write_enable, 8,
            SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    pi_spi_send(qspi_dev, (void*)g_set_qspif_dummy, 2*8,
            SPI_LINES_FLAG | PI_SPI_CS_AUTO);
}

static int get_wip(spi_flash_t *flash_dev)
{
    volatile uint8_t wip = 0;
    uint8_t *status_reg = (uint8_t*)flash_dev->status_reg;
    uint8_t *cmd_buf = (uint8_t*)flash_dev->cmd_buf;
    pi_device_t *qspi_dev = &flash_dev->qspi_dev;
    cmd_buf[0] = QSPIF_READ_STATUS_CMD; // read status reg
    pi_spi_send(qspi_dev, (void*)&cmd_buf[0], 1*8,
            SPI_LINES_FLAG | PI_SPI_CS_KEEP);
    pi_spi_receive(qspi_dev, (void*)&status_reg[BUSY_BIT_ID], 1*8,
            SPI_LINES_FLAG | PI_SPI_CS_AUTO);
            
    return IS_BUSY(status_reg);
}

static void wait_wip(spi_flash_t *flash_dev)
{
    do
    {
        pi_time_wait_us(5000);
    } while(get_wip(flash_dev));
}

static inline void pi_qpi_flash_conf_spi(struct pi_spi_conf *conf,
        struct pi_spiflash_conf *flash_conf)
{
    memset(conf, 0, sizeof(struct pi_spi_conf));
    pi_spi_conf_init(conf);
    // we remove 1500 to make sure divisor will be okay even if freq is not 
    // precisely at target. Value is empiric
    // we then mult by two to workaround divisor bogus spec
    conf->max_baudrate = flash_conf->baudrate;
    conf->wordsize = PI_SPI_WORDSIZE_8;
    conf->polarity = 0;
    conf->phase = conf->polarity;
    conf->cs = flash_conf->spi_cs;
    conf->itf = flash_conf->spi_itf;
    conf->big_endian = 1;
}

static int spiflash_open(struct pi_device *bsp_flash_dev)
{
    spi_flash_t *flash_dev = pi_l2_malloc(sizeof(spi_flash_t));
    if(flash_dev == NULL)
    {
        return -1;
    }
    memset(flash_dev, 0, sizeof(spi_flash_t));
    struct pi_spiflash_conf *flash_conf = bsp_flash_dev->config;
    pi_qpi_flash_conf_spi(&flash_dev->qspi_conf, flash_conf);
    pi_open_from_conf(&flash_dev->qspi_dev,&flash_dev->qspi_conf);
    int ret = pi_spi_open(&flash_dev->qspi_dev);
    if(ret)
    {
        goto error0;
    }

    flash_dev->sector_size = flash_conf->sector_size;
    bsp_flash_dev->data = (void*)flash_dev;

#if !defined(PMSIS_DRIVERS)
    uint32_t ucode[4];

    ucode[0] = SPI_UCODE_CMD_SEND_CMD(QSPIF_QIO_PAGE_PROG_CMD, 8, 0);
    ucode[1] = SPI_UCODE_CMD_SEND_ADDR(24, 0);

    uint8_t *send_ucode = pi_spi_send_ucode_set(&flash_dev->qspi_dev, (uint8_t *)ucode, 3*4);
    if (send_ucode == NULL)
    {
        goto error1;
    }

    pi_spi_send_ucode_set_addr_info(&flash_dev->qspi_dev, send_ucode + 2*4 + 1, 3);

    ucode[0] = SPI_UCODE_CMD_SEND_CMD(QSPIF_QO_FAST_READ_CMD, 8, 0);
    ucode[1] = SPI_UCODE_CMD_SEND_ADDR(24, 1);
    ucode[3] = SPI_CMD_DUMMY(6);

    uint8_t *receive_ucode = pi_spi_receive_ucode_set(&flash_dev->qspi_dev, (uint8_t *)ucode, 4*4);
    if (receive_ucode == NULL)
    {
        goto error1;
    }

    pi_spi_receive_ucode_set_addr_info(&flash_dev->qspi_dev, receive_ucode + 2*4 + 1, 3);
#endif

    qpi_flash_pre_config(flash_dev);

    return 0;

#if !defined(PMSIS_DRIVERS)
error1:
    pi_spi_close(&flash_dev->qspi_dev);
#endif
error0:
    pi_l2_free(flash_dev, sizeof(spi_flash_t));

    return -1;
}

void spiflash_close(struct pi_device *bsp_flash_dev)
{
    spi_flash_t *flash_dev = (spi_flash_t*)bsp_flash_dev->data;
    // wait for last operations
    wait_wip(flash_dev);
    pi_spi_close(&flash_dev->qspi_dev);
    pi_l2_free(flash_dev, sizeof(spi_flash_t));
}


#if defined(PMSIS_DRIVERS)

int spiflash_erase_chip(struct pi_device *bsp_flash_dev)
{
    spi_flash_t *flash_dev = (spi_flash_t*) bsp_flash_dev->data;

    pi_device_t *qspi_dev = &flash_dev->qspi_dev;
    // flash chip erase (optional!)
    pi_spi_send(qspi_dev, (void*)g_write_enable, 8,
            SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    pi_spi_send(qspi_dev, (void*)g_chip_erase, 8,
            SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    wait_wip(flash_dev);
    return 0;
}

int spiflash_erase(struct pi_device *bsp_flash_dev, uint32_t addr, int size)
{
    spi_flash_t *flash_dev  =(spi_flash_t*) bsp_flash_dev->data;
    uint32_t flash_addr = addr;
    uint32_t sector_size = flash_dev->sector_size;
    pi_device_t *qspi_dev = &flash_dev->qspi_dev;

    uint8_t *cmd_buf = (uint8_t*)flash_dev->ucode_buffer;
    for(uint32_t c_size = 0; c_size < (unsigned int)size; c_size += sector_size)
    {
        uint32_t curr_flash_addr = flash_addr+c_size;
        cmd_buf[0] = QSPIF_ERASE_SECTOR_CMD;
        cmd_buf[1] = (curr_flash_addr & 0x00FF0000)>>16;
        cmd_buf[2] = (curr_flash_addr & 0x0000FF00)>>8;
        cmd_buf[3] = (curr_flash_addr & 0x000000FF)>>0;
        pi_spi_send(qspi_dev, (void*)g_write_enable, 8,
                SPI_LINES_FLAG | PI_SPI_CS_AUTO);
        pi_spi_send(qspi_dev, (void*)cmd_buf, 8*QSPIF_ERASE_SIZE,
                SPI_LINES_FLAG | PI_SPI_CS_AUTO);
        wait_wip(flash_dev);
    }
    return 0;
}

int spiflash_erase_sector(struct pi_device *bsp_flash_dev, uint32_t addr)
{
    spi_flash_t *flash_dev  = (spi_flash_t*)bsp_flash_dev->data;
    uint32_t flash_addr = addr;
    uint32_t sector_size = flash_dev->sector_size;
    
    return spiflash_erase(bsp_flash_dev, addr, sector_size);
}

static int spiflash_read(struct pi_device *bsp_flash_dev, uint32_t addr, void *data,
        uint32_t size)
{
    spi_flash_t *flash_dev  =(spi_flash_t*) bsp_flash_dev->data;
    pi_device_t *qspi_dev = &flash_dev->qspi_dev;

    uint32_t flash_addr = addr;
    uint32_t sector_size = flash_dev->sector_size;

    uint8_t *l2_buff = pi_l2_malloc(QSPIF_PAGE_SIZE);
    uint8_t *ucode = (uint8_t*)flash_dev->ucode_buffer;
    volatile uint32_t *ucode_u32 = (uint32_t*)ucode;
    if(!l2_buff)
    {
        printf("MALLOC FAILED\n");
        return -1;
    }
    uint32_t size_left = size;
    uint32_t curr_size = 0;
    uint32_t curr_pos = 0;
    while(size_left)
    {
        if(size_left >= QSPIF_PAGE_SIZE)
        {
            curr_size = QSPIF_PAGE_SIZE;
            size_left -= QSPIF_PAGE_SIZE;
        }
        else
        {
            curr_size = size_left;
            size_left = 0;
        }
        uint32_t curr_addr = flash_addr+curr_pos;
        ucode_u32[0] = pi_spi_get_config(qspi_dev);
        ucode_u32[1] = SPI_CMD_SOT(0);
        ucode_u32[2] = SPI_CMD_TX_DATA(8*1, 0, 0);
        ucode[12] = QSPIF_QIO_FAST_READ_CMD;// use QO for gvsoc instead
        ucode_u32[4] = SPI_CMD_TX_DATA(8*3, 1, 0);
        ucode[20] = (curr_addr >> 16 )& 0xFFUL;
        ucode[21] = (curr_addr >> 8) & 0xFFUL;
        ucode[22] = curr_addr & 0xFFUL;
        ucode_u32[6] = SPI_CMD_DUMMY(DUMMY_CYCLES);
        ucode_u32[7] = SPI_CMD_RX_DATA(curr_size*8, 1, 0);// use 4 lines to recv
        ucode_u32[8] = SPI_CMD_EOT(1);
        // any write/erase op must be preceeded by a WRITE ENABLE op, 
        // with full CS cycling
        pi_spi_receive_with_ucode(qspi_dev, (void*)l2_buff, (curr_size)*8,
                PI_SPI_LINES_SINGLE | PI_SPI_CS_AUTO, QSPIF_RCV_UCODE_SIZE,
                ucode);

        memcpy(data+curr_pos, l2_buff, curr_size);
        curr_pos += curr_size;
    }
    pi_l2_free(l2_buff, QSPIF_PAGE_SIZE);

    return 0;

}

static int spiflash_program(struct pi_device *bsp_flash_dev, uint32_t flash_addr,
      const void *data, uint32_t size)
{
    spi_flash_t *flash_dev  = (spi_flash_t*) bsp_flash_dev->data;
    pi_device_t *qspi_dev = &flash_dev->qspi_dev;

    uint8_t *l2_buff = pi_l2_malloc(QSPIF_PAGE_SIZE);
    if(!l2_buff)
    {
        printf("Malloc failed!\n");
        return -1;
    }
    uint32_t size_left = size;
    uint32_t curr_size = 0;
    uint32_t curr_pos = 0;
    
    uint8_t *ucode = (uint8_t*)flash_dev->ucode_buffer;
    uint32_t *ucode_u32 = (uint32_t*)ucode;
    if((flash_addr & 0xFF) && (((flash_addr & 0xFF)+size_left) > 0x100))
    {
        curr_pos  = 0;
        curr_size = 0x100 - (flash_addr & 0xFF);
        size_left -= curr_size;
        // any write/erase op must be preceeded by a WRITE ENABLE op,
        // with full CS cycling
        pi_spi_send(qspi_dev, (void*)g_write_enable, 8,
                SPI_LINES_FLAG | PI_SPI_CS_AUTO);
        memcpy(l2_buff, data+curr_pos, curr_size);
        ucode_u32[0] = pi_spi_get_config(qspi_dev);
        ucode_u32[1] = SPI_CMD_SOT(0);
        ucode_u32[2] = SPI_CMD_TX_DATA(8*4, 0, 0);
        ucode[12] = QSPIF_QIO_PAGE_PROG_CMD;//0x38;
        ucode[13] = ((flash_addr+curr_pos) & 0x00FF0000)>>16;
        ucode[14] = ((flash_addr+curr_pos) & 0x0000FF00)>>8;
        ucode[15] = ((flash_addr+curr_pos) & 0x000000FF)>>0;
        ucode_u32[4] = SPI_CMD_TX_DATA(curr_size*8, 1, 0);
        pi_spi_send_with_ucode(qspi_dev, (void*)l2_buff, (curr_size)*8,
                SPI_LINES_FLAG | PI_SPI_CS_AUTO, QSPIF_PPQ_UCODE_SIZE, ucode);
        wait_wip(flash_dev);
        curr_pos += curr_size;
    }

    while(size_left)
    {
        if(size_left >= QSPIF_PAGE_SIZE)
        {
            curr_size = QSPIF_PAGE_SIZE;
            size_left -= QSPIF_PAGE_SIZE;
        }
        else
        {
            curr_size = size_left;
            size_left = 0;
        }
        memcpy(l2_buff, data+curr_pos, curr_size);
        // any write/erase op must be preceeded by a WRITE ENABLE op,
        // with full CS cycling
        pi_spi_send(qspi_dev, (void*)g_write_enable, 8,
                SPI_LINES_FLAG | PI_SPI_CS_AUTO);
        ucode_u32[0] = pi_spi_get_config(qspi_dev);
        ucode_u32[1] = SPI_CMD_SOT(0);
        ucode_u32[2] = SPI_CMD_TX_DATA(8*4, 0, 0);
        ucode[12] = QSPIF_QIO_PAGE_PROG_CMD;
        ucode[13] = ((flash_addr+curr_pos) & 0x00FF0000)>>16;
        ucode[14] = ((flash_addr+curr_pos) & 0x0000FF00)>>8;
        ucode[15] = ((flash_addr+curr_pos) & 0x000000FF)>>0;
        ucode_u32[4] = SPI_CMD_TX_DATA(curr_size*8, 1, 0);
        pi_spi_send_with_ucode(qspi_dev, (void*)l2_buff, (curr_size)*8,
                SPI_LINES_FLAG | PI_SPI_CS_AUTO, QSPIF_PPQ_UCODE_SIZE, ucode);
        wait_wip(flash_dev);
        curr_pos += curr_size;
    }
    pi_l2_free(l2_buff, QSPIF_PAGE_SIZE);
    pi_l2_free(ucode, QSPIF_PPQ_UCODE_SIZE);
    return 0;
}


int spiflash_reg_set(struct pi_device *bsp_flash_dev, uint32_t reg, uint8_t *value)
{]
    spi_flash_t *flash_dev  = (spi_flash_t*)bsp_flash_dev->data;
    uint32_t sector_size = flash_dev->sector_size;
    pi_device_t *qspi_dev = &flash_dev->qspi_dev;

    uint8_t *ucode = (uint8_t*)flash_dev->ucode_buffer;
    uint32_t *ucode_u32 = (uint32_t*)ucode;

    pi_spi_send(qspi_dev, (void*)g_write_enable, 8,
                SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    ucode[0] = (uint8_t) reg;
    ucode[1] = *value;
    pi_spi_send(qspi_dev, (void*)ucode, 16, SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    return 0;
}

int spiflash_reg_get(struct pi_device *bsp_flash_dev, uint32_t reg, uint8_t *value)
{
    spi_flash_t *flash_dev  =(spi_flash_t*) bsp_flash_dev->data;
    uint32_t sector_size = flash_dev->sector_size;
    pi_device_t *qspi_dev = &flash_dev->qspi_dev;

    uint8_t *ucode = (uint8_t*)flash_dev->ucode_buffer;
    uint32_t *ucode_u32 = (uint32_t*)ucode;

    pi_spi_send(qspi_dev, (void*)g_write_enable, 8,
                SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    ucode[0] = (uint8_t) reg;
    pi_spi_send(qspi_dev, (void*)ucode, 8, SPI_LINES_FLAG | PI_SPI_CS_KEEP);
    pi_spi_receive(qspi_dev, (void*)value, 8, SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    return 0;
}

int spiflash_copy(struct pi_device *device, uint32_t flash_addr, void *buffer,
        uint32_t size, int ext2loc)
{
    int ret = 0;
    if(!ext2loc)
    {
        ret = spiflash_program(device, flash_addr, buffer, size);
    }
    else
    {
        ret = spiflash_read(device, flash_addr, buffer, size);
    }
    return ret;
}

int spiflash_copy_2d(struct pi_device *device, uint32_t flash_addr,
        void *buffer, uint32_t size, uint32_t stride, uint32_t length,
        int ext2loc)
{
    // not yet implemented
    return -1;
}

static int32_t spiflash_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    return 0;
}

static void spiflash_read_async(struct pi_device *device, uint32_t addr, void *data,
        uint32_t size, pi_task_t *task)
{
    spiflash_read(device,addr,data,size);
    pi_task_release(task);
}

static void spiflash_program_async(struct pi_device *device, uint32_t addr,
        const void *data, uint32_t size, pi_task_t *task)
{
    spiflash_program(device,addr,data,size);
    pi_task_release(task);
}

static void spiflash_erase_chip_async(pi_device_t *device, pi_task_t *task)
{
    spiflash_erase_chip(device);
    pi_task_release(task);
}

static void spiflash_erase_sector_async(struct pi_device *device, uint32_t addr,
        pi_task_t *task)
{
    spiflash_erase_sector(device,addr);
    pi_task_release(task);
}

static void spiflash_erase_async(struct pi_device *device, uint32_t addr,
    int size, pi_task_t *task)
{
    spiflash_erase(device,addr,size);
    pi_task_release(task);
}

static void spiflash_reg_set_async(struct pi_device *device, uint32_t addr,
        uint8_t *value, pi_task_t *task)
{
    spiflash_reg_set(device,addr,value);
    pi_task_release(task);
}

static void spiflash_reg_get_async(struct pi_device *device, uint32_t addr,
        uint8_t *value, pi_task_t *task)
{
    spiflash_reg_set(device,addr,value);
    pi_task_release(task);
}

static int spiflash_copy_async(struct pi_device *device, uint32_t flash_addr,
        void *buffer, uint32_t size, int ext2loc, pi_task_t *task)
{
    int ret = spiflash_copy(device, flash_addr, buffer, size, ext2loc);
    pi_task_release(task);
    return ret;
}

static int spiflash_copy_2d_async(struct pi_device *device, uint32_t flash_addr,
        void *buffer, uint32_t size, uint32_t stride, uint32_t length,
        int ext2loc, pi_task_t *task)
{
    int ret = spiflash_copy_2d(device, flash_addr, buffer, size, stride, length,
            ext2loc);
    pi_task_release(task);
    return ret;
}

#else


static inline int spiflash_erase_chip(struct pi_device *device)
{
    pi_task_t task;
    spiflash_erase_chip_async(device, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}


static inline int spiflash_erase_sector(struct pi_device *device, uint32_t pi_flash_addr)
{
    pi_task_t task;
    spiflash_erase_sector_async(device, pi_flash_addr, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}


static inline int spiflash_erase(struct pi_device *device, uint32_t pi_flash_addr, int size)
{
    pi_task_t task;
    pi_task_block(&task);
    spiflash_erase_async(device, pi_flash_addr, size, &task);
    pi_task_wait_on(&task);
    return 0;
}


static int spiflash_read(struct pi_device *device, uint32_t pi_flash_addr, void *data, uint32_t size)
{
    pi_task_t task;
    spiflash_read_async(device, pi_flash_addr, data, size, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}

    
static int spiflash_program(struct pi_device *bsp_flash_dev, uint32_t flash_addr,
      const void *data, uint32_t size)
{
    pi_task_t task;
    spiflash_program_async(bsp_flash_dev, flash_addr, data, size, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}


static inline int spiflash_reg_set(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value)
{
    pi_task_t task;
    spiflash_reg_set_async(device, pi_flash_addr, value, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}


static inline int spiflash_reg_get(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value)
{
    pi_task_t task;
    spiflash_reg_get_async(device, pi_flash_addr, value, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}


int spiflash_copy(struct pi_device *device, uint32_t flash_addr, void *buffer,
        uint32_t size, int ext2loc)
{
    pi_task_t task;
    spiflash_copy_async(device, flash_addr, buffer, size, ext2loc, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}


int spiflash_copy_2d(struct pi_device *device, uint32_t flash_addr,
        void *buffer, uint32_t size, uint32_t stride, uint32_t length,
        int ext2loc)
{
    pi_task_t task;
    spiflash_copy_2d_async(device, flash_addr, buffer, size, stride, length, ext2loc, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}


static int32_t spiflash_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    return 0;
}


static void spiflash_set_reg_exec(spi_flash_t *spiflash, unsigned int addr, unsigned short value)
{
    pi_device_t *qspi_dev = &spiflash->qspi_dev;

    uint8_t *ucode = (uint8_t*)spiflash->ucode_buffer;
    uint32_t *ucode_u32 = (uint32_t*)ucode;

    pi_spi_send(qspi_dev, (void*)g_write_enable, 8,
                SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    ucode[0] = (uint8_t)addr;
    ucode[1] = value;
    pi_spi_send(qspi_dev, (void*)ucode, 16, SPI_LINES_FLAG | PI_SPI_CS_AUTO);
}


static unsigned short spiflash_get_reg_exec(spi_flash_t *spiflash, unsigned int addr)
{
    pi_device_t *qspi_dev = &spiflash->qspi_dev;

    uint8_t *ucode = (uint8_t*)spiflash->ucode_buffer;
    uint32_t *ucode_u32 = (uint32_t*)ucode;

    pi_spi_send(qspi_dev, (void*)g_write_enable, 8,
                SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    ucode[0] = (uint8_t)addr;
    pi_spi_send(qspi_dev, (void*)ucode, 8, SPI_LINES_FLAG | PI_SPI_CS_KEEP);
    pi_spi_receive(qspi_dev, (void*)spiflash->cmd_buf, 8, SPI_LINES_FLAG | PI_SPI_CS_AUTO);

    return spiflash->cmd_buf[0];
}


// Handle the termination of the current operation. This will trigger the user task and continue with the next
// pending operation.
static void spiflash_handle_pending_task(void *arg)
{
    struct pi_device *device = (struct pi_device *)arg;
    spi_flash_t *spiflash = (spi_flash_t *)device->data;

    pi_task_push(spiflash->pending_task);
    spiflash->pending_task = NULL;

    pi_task_t *task = spiflash->waiting_first;

    if (task)
    {
    #if defined(PMSIS_DRIVERS)
        spiflash->waiting_first = task->next;
    #else
        spiflash->waiting_first = task->implem.next;
    #endif  /* PMSIS_DRIVERS */
    }

    if (task)
    {
    #if defined(PMSIS_DRIVERS)
        if (task->data[0] == STALL_TASK_PROGRAM)
        {
            spiflash_program_async(device, task->data[1], (void *)task->data[2], task->data[3], task);
        }
        else if (task->data[0] == STALL_TASK_ERASE_CHIP)
        {
            spiflash_erase_chip_async(device, task);
        }
        else if (task->data[0] == STALL_TASK_ERASE_SECTOR)
        {
            spiflash_erase_sector_async(device, task->data[1], task);
        }
        else if (task->data[0] == STALL_TASK_REG_SET)
        {
            spiflash_reg_set_async(device, task->data[1], (uint8_t *)task->data[2], task);
        }
        else if (task->data[0] == STALL_TASK_REG_GET)
        {
            spiflash_reg_get_async(device, task->data[1], (uint8_t *)task->data[2], task);
        }
        else if (task->data[0] == STALL_TASK_READ)
        {
            spiflash_read_async(device, task->data[1], (void *)task->data[2], task->data[3], task);
        }
        else if (task->data[0] == STALL_TASK_READ_2D)
        {
            spiflash_copy_2d_async(device, task->data[1], (void *)task->data[2], task->data[3], task->data[4], task->data[5], task);
        }
    #else
        if (task->implem.data[0] == STALL_TASK_PROGRAM)
        {
            spiflash_program_async(device, task->implem.data[1], (void *)task->implem.data[2], task->implem.data[3], task);
        }
        else if (task->implem.data[0] == STALL_TASK_ERASE_CHIP)
        {
            spiflash_erase_chip_async(device, task);
        }
        else if (task->implem.data[0] == STALL_TASK_ERASE_SECTOR)
        {
            spiflash_erase_sector_async(device, task->implem.data[1], task);
        }
        else if (task->implem.data[0] == STALL_TASK_REG_SET)
        {
            spiflash_reg_set_async(device, task->implem.data[1], (uint8_t *)task->implem.data[2], task);
        }
        else if (task->implem.data[0] == STALL_TASK_REG_GET)
        {
            spiflash_reg_get_async(device, task->implem.data[1], (uint8_t *)task->implem.data[2], task);
        }
        else if (task->implem.data[0] == STALL_TASK_READ)
        {
            spiflash_read_async(device, task->implem.data[1], (void *)task->implem.data[2], task->implem.data[3], task);
        }
        else if (task->implem.data[0] == STALL_TASK_READ_2D)
        {
            spiflash_copy_2d_async(device, task->implem.data[1], (void *)task->implem.data[2], task->implem.data[3], 1, task->implem.data[4], task->implem.data[5], task);
        }
    #endif  /* PMSIS_DRIVERS */
    }
}


// This is called to check if an operation is already pending. If so the new one is enqueued, and 1 is returned.
// Otherwise, the specified task is registered to be triggered at the end of the operation and 0 is returned.
static int spiflash_stall_task(spi_flash_t *spiflash, pi_task_t *task, uint32_t id, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
    #if defined(PMSIS_DRIVERS)
    if (spiflash->pending_task != NULL)
    {
        task->data[0] = id;
        task->data[1] = arg0;
        task->data[2] = arg1;
        task->data[3] = arg2;
        task->data[4] = arg3;
        task->data[5] = arg4;
        task->next = NULL;

        if (spiflash->waiting_first)
        {
        spiflash->waiting_last->next = task;
        }
        else
        {
        spiflash->waiting_first = task;
        }

        spiflash->waiting_last = task;

        return 1;
    }
    #else

    if (spiflash->pending_task != NULL)
    {
        task->implem.data[0] = id;
        task->implem.data[1] = arg0;
        task->implem.data[2] = arg1;
        task->implem.data[3] = arg2;
        task->implem.data[4] = arg3;
        task->implem.data[5] = arg4;

        if (spiflash->waiting_first)
        spiflash->waiting_last->implem.next = task;
        else
        spiflash->waiting_first = task;

        spiflash->waiting_last = task;
        task->implem.next = NULL;

        return 1;
    }
    #endif  /* PMSIS_DRIVERS */

    spiflash->pending_task = task;

  return 0;
}


// This is called to continue a pending program operation as it has to be cut
// into bursts with a sleep in between
static void spiflash_program_resume(void *arg)
{
    struct pi_device *device = (struct pi_device *)arg;
    spi_flash_t *spiflash = (spi_flash_t *)device->data;

    if (spiflash->pending_size == 0)
    {
        // Case where we are done, just unblock the used task and trigger
        // the first waiting operation
        spiflash_handle_pending_task(device);
    }
    else
    {
        // Iterate one page. Be careful to not cross the page, or the operation
        // will wrap around
        unsigned int iter_size = QSPIF_PAGE_SIZE - (spiflash->pending_flash_addr & (QSPIF_PAGE_SIZE-1));
        if (iter_size > spiflash->pending_size)
            iter_size = spiflash->pending_size;

        uint32_t flash_addr = spiflash->pending_flash_addr;
        uint32_t data = spiflash->pending_data;

        spiflash->pending_flash_addr += iter_size;
        spiflash->pending_data += iter_size;
        spiflash->pending_size -= iter_size;

        // any write/erase op must be preceeded by a WRITE ENABLE op,
        // with full CS cycling
        pi_spi_send(&spiflash->qspi_dev, (void*)g_write_enable, 8,
                SPI_LINES_FLAG | PI_SPI_CS_AUTO);

        // The SPI copy has been configured with proper ucode already, no need to take care
        pi_spi_copy_async(&spiflash->qspi_dev, flash_addr, (void *)data, iter_size,
            PI_SPI_COPY_LOC2EXT | PI_SPI_CS_AUTO | PI_SPI_LINES_QUAD,
            pi_task_callback(&spiflash->task, spiflash_check_program, device));
    }
}


// This callback is called either once a program operation is done or after a specific delay (to wait for program
// completion in flash) to check if the current program operation can be resumed.
static void spiflash_check_program(void *arg)
{
    struct pi_device *device = (struct pi_device *)arg;
    spi_flash_t *spiflash = (spi_flash_t *)device->data;

    if (get_wip(spiflash))
    {
        pi_task_push_delayed_us(pi_task_callback(&spiflash->task, spiflash_check_program, device), 5000);
    }
    else
    {
        spiflash_program_resume(device);
    }
}


static void spiflash_program_async(struct pi_device *device, uint32_t flash_addr, const void *data, uint32_t size, pi_task_t *task)
{
    spi_flash_t *spiflash = (spi_flash_t *)device->data;

    if (spiflash_stall_task(spiflash, task, STALL_TASK_PROGRAM, flash_addr, (uint32_t)data, size, 0, 0))
        return;

    spiflash->pending_flash_addr = flash_addr;
    spiflash->pending_data = (uint32_t)data;
    spiflash->pending_size = size;

    spiflash_program_resume(device);
}


static void spiflash_read_async(struct pi_device *device, uint32_t addr, void *data, uint32_t size, pi_task_t *task)
{
    spi_flash_t *spiflash = (spi_flash_t *)device->data;

    if (spiflash_stall_task(spiflash, task, STALL_TASK_READ, addr, (uint32_t)data, size, 0, 0))
        return;

    // The SPI copy has been configured with proper ucode already, no need to take care
    pi_spi_copy_async(&spiflash->qspi_dev, addr, data, size, PI_SPI_COPY_EXT2LOC | PI_SPI_CS_AUTO | PI_SPI_LINES_QUAD, pi_task_callback(&spiflash->task, spiflash_handle_pending_task, device));
}


static void spiflash_erase_chip_async(struct pi_device *device, pi_task_t *task)
{
    spi_flash_t *spiflash = (spi_flash_t *)device->data;
    pi_device_t *qspi_dev = &spiflash->qspi_dev;

    if (spiflash_stall_task(spiflash, task, STALL_TASK_ERASE_CHIP, 0, 0, 0, 0, 0))
        return;

    pi_spi_send(qspi_dev, (void*)g_write_enable, 8,
            SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    pi_spi_send(qspi_dev, (void*)g_chip_erase, 8,
            SPI_LINES_FLAG | PI_SPI_CS_AUTO);

    spiflash_handle_pending_task(device);
}



// This callback is called either once aan erase operation is done or after a specific delay (to wait for erase
// completion in flash) to check if the current erase operation can be resumed.
static void spiflash_check_erase(void *arg)
{
    struct pi_device *device = (struct pi_device *)arg;
    spi_flash_t *spiflash = (spi_flash_t *)device->data;

    if (get_wip(spiflash))
    {
        pi_task_push_delayed_us(pi_task_callback(&spiflash->task, spiflash_check_erase, device), 5000);
    }
    else
    {
        spiflash_erase_resume((void *)device);
    }
}


// Do the actual erase of 1 sector
static void spiflash_erase_sector_exec(struct pi_device *device, uint32_t addr)
{
    spi_flash_t *spiflash = (spi_flash_t *)device->data;
    pi_device_t *qspi_dev = &spiflash->qspi_dev;

    uint8_t *cmd_buf = (uint8_t*)spiflash->ucode_buffer;

    cmd_buf[0] = QSPIF_ERASE_SECTOR_CMD;
    cmd_buf[1] = (addr & 0x00FF0000)>>16;
    cmd_buf[2] = (addr & 0x0000FF00)>>8;
    cmd_buf[3] = (addr & 0x000000FF)>>0;
    pi_spi_send(qspi_dev, (void*)g_write_enable, 8,
            SPI_LINES_FLAG | PI_SPI_CS_AUTO);
    pi_spi_send(qspi_dev, (void*)cmd_buf, 8*QSPIF_ERASE_SIZE,
            SPI_LINES_FLAG | PI_SPI_CS_AUTO);

    // Typical sector erase time is 930ms
    pi_task_push_delayed_us(pi_task_callback(&spiflash->task, spiflash_check_erase, device), 5000);
}



static void spiflash_erase_sector_async(struct pi_device *device, uint32_t addr, pi_task_t *task)
{
    spi_flash_t *spiflash = (spi_flash_t *)device->data;
    pi_device_t *qspi_dev = &spiflash->qspi_dev;

    if (spiflash_stall_task(spiflash, task, STALL_TASK_ERASE_SECTOR, addr, 0, 0, 0, 0))
        return;

    spiflash->pending_flash_addr = addr & ~(SECTOR_SIZE - 1);
    spiflash->pending_size = SECTOR_SIZE;

    spiflash_erase_resume(device);
}



// This is called to continue a pending erase operation as it has to be cut
// into sector erase with a sleep in between
static void spiflash_erase_resume(void *arg)
{
    struct pi_device *device = (struct pi_device *)arg;
    spi_flash_t *spiflash = (spi_flash_t *)device->data;

    if (spiflash->pending_size == 0)
    {
        spiflash_handle_pending_task(device);
    }
    else
    {
        unsigned int iter_size = SECTOR_SIZE - (spiflash->pending_flash_addr & (SECTOR_SIZE - 1));
        if (iter_size > spiflash->pending_size)
            iter_size = spiflash->pending_size;

        uint32_t flash_addr = spiflash->pending_flash_addr;

        spiflash->pending_flash_addr += iter_size;
        spiflash->pending_size -= iter_size;

        spiflash_erase_sector_exec(device, flash_addr);
    }
}


static void spiflash_erase_async(struct pi_device *device, uint32_t addr, int size, pi_task_t *task)
{
    spi_flash_t *spiflash = (spi_flash_t *)device->data;

    if (spiflash_stall_task(spiflash, task, STALL_TASK_ERASE_SECTOR, addr, size, 0, 0, 0))
    {
        return;
    }

    spiflash->pending_flash_addr = addr;
    spiflash->pending_size = size;

    spiflash_erase_resume(device);
}


static void spiflash_reg_set_async(struct pi_device *device, uint32_t addr, uint8_t *value, pi_task_t *task)
{
    spi_flash_t *spiflash = (spi_flash_t *)device->data;

    if (spiflash_stall_task(spiflash, task, STALL_TASK_REG_SET, addr, (uint32_t)value, 0, 0, 0))
        return;

    spiflash_set_reg_exec(spiflash, addr, *(uint16_t *)value);

    spiflash_handle_pending_task(device);
}



static void spiflash_reg_get_async(struct pi_device *device, uint32_t addr, uint8_t *value, pi_task_t *task)
{
    spi_flash_t *spiflash = (spi_flash_t *)device->data;

    if (spiflash_stall_task(spiflash, task, STALL_TASK_REG_GET, addr, (uint32_t)value, 0, 0, 0))
        return;

    *(uint16_t *)value = spiflash_get_reg_exec(spiflash, addr);

    spiflash_handle_pending_task(device);
}



static int spiflash_copy_async(struct pi_device *device, uint32_t flash_addr,
        void *buffer, uint32_t size, int ext2loc, pi_task_t *task)
{
    if (!ext2loc)
        spiflash_program_async(device, flash_addr, buffer, size, task);
    else
        spiflash_read_async(device, flash_addr, buffer, size, task);

    return 0;
}

static int spiflash_copy_2d_async(struct pi_device *device, uint32_t flash_addr,
        void *buffer, uint32_t size, uint32_t stride, uint32_t length,
        int ext2loc, pi_task_t *task)
{
    spi_flash_t *spiflash = (spi_flash_t *)device->data;

    if (!ext2loc)
        return -1;

    pi_spi_copy_2d_async(&spiflash->qspi_dev, flash_addr, buffer, size, stride, length,
            PI_SPI_COPY_EXT2LOC | PI_SPI_CS_AUTO | PI_SPI_LINES_QUAD,
            pi_task_callback(&spiflash->task, spiflash_handle_pending_task, device));

    return 0;
}

#endif

static pi_flash_api_t spiflash_api = {
  .open                 = &spiflash_open,
  .close                = &spiflash_close,
  .ioctl                = &spiflash_ioctl,
  .read_async           = &spiflash_read_async,
  .program_async        = &spiflash_program_async,
  .erase_chip_async     = &spiflash_erase_chip_async,
  .erase_sector_async   = &spiflash_erase_sector_async,
  .erase_async          = &spiflash_erase_async,
  .reg_set_async        = &spiflash_reg_set_async,
  .reg_get_async        = &spiflash_reg_get_async,
  .copy_async           = &spiflash_copy_async,
  .copy_2d_async        = &spiflash_copy_2d_async,
  .read                 = &spiflash_read,
  .program              = &spiflash_program,
  .erase_chip           = &spiflash_erase_chip,
  .erase_sector         = &spiflash_erase_sector,
  .erase                = &spiflash_erase,
  .reg_set              = &spiflash_reg_set,
  .reg_get              = &spiflash_reg_get,
  .copy                 = &spiflash_copy,
  .copy_2d              = &spiflash_copy_2d,
};

void pi_spiflash_conf_init(struct pi_spiflash_conf *conf)
{
    conf->flash.api = &spiflash_api;
    bsp_spiflash_conf_init(conf);
    __flash_conf_init(&conf->flash);
}
