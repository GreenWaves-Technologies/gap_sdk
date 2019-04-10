/* 
 * Copyright (C) 2015 ETH Zurich and University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou (germain.haugou@gmail.com)
 */

#include "boot.h"
#include "hal/pulp.h"
#include <stddef.h>

#if defined(PMU_VERSION) && PMU_VERSION == 1
#include "hal/pmu_v1.h"
#endif

#ifdef USE_AES
#include "AesLib.h"
#endif

#define PLP_L2_DATA      __attribute__((section(".ram")))
#define PLP_FC_DATA __attribute__((section(".fcTcdm")))

#define BOOT_STACK_SIZE  1024
#define MAX_NB_AREA 16

#if FLASH_BLOCK_SIZE > HYPER_FLASH_BLOCK_SIZE
#define BLOCK_SIZE FLASH_BLOCK_SIZE
#else
#define BLOCK_SIZE HYPER_FLASH_BLOCK_SIZE
#endif

#define PADFUN0_OFFSET          0x140
#define PADFUN1_OFFSET          0x144
#define PADFUN2_OFFSET          0x148
#define PADFUN3_OFFSET          0x14C

typedef struct {
  unsigned char flashBuffer[FLASH_BLOCK_SIZE];
  unsigned int udma_buffer[256];
  int spi_flash_id;
  int step;
  flash_v2_header_t header;
  flash_v2_mem_area_t memArea[MAX_NB_AREA];
  char udma_uart_tx_buffer[1];
  unsigned char stack[BOOT_STACK_SIZE];
  int encrypted;
  int hyperflash;
  int blockSize;
  int qpi;

} boot_code_t;

typedef struct {
#ifdef USE_AES
  aes_data_t aes_data;
  unsigned char Key[4][MAXBC];
  unsigned char IV[2][MAXBC];
  int aes_init;
#endif
} boot_code_fc_t;

PLP_L2_DATA static boot_code_t    bootCode;
PLP_FC_DATA static boot_code_fc_t bootCodeFc;

static void __attribute__((noreturn)) bootFromOther(int platform, int encrypted);

static void boot_abort() {
#ifdef EU_VERSION
  eu_evt_maskWaitAndClr(0);
#endif
}

#if PULP_CHIP == CHIP_GAP
static int config_pin(unsigned int pad_muxN, int alternate)
{
    unsigned int regN, val;
    if (pad_muxN >= 64) return -1; 
    
    pad_muxN -= 8;

    regN = pad_muxN >> 4;
    val = alternate << ((pad_muxN & 0xf) << 1);

    val = val | IP_READ(ARCHI_APB_SOC_CTRL_ADDR, PADFUN0_OFFSET + regN*4);
    IP_WRITE(ARCHI_APB_SOC_CTRL_ADDR, PADFUN0_OFFSET + regN*4, val);
      return 0;
}
#endif


#ifdef DEBUG

static void uart_sendchar(boot_code_t *data, const char c) {
  // setup next transfer
  data->udma_uart_tx_buffer[0] = c;
  plp_udma_enqueue(UDMA_UART_TX_ADDR(0), (int)data->udma_uart_tx_buffer, 1, UDMA_CHANNEL_CFG_EN);
  wait_soc_event(ARCHI_SOC_EVENT_UART_TX(0));
}

static void putch(boot_code_t *data, char c) {
  uart_sendchar(data, c);
}

static inline void step_ok(boot_code_t *data) {
  putch(data, 'A' + data->step);  
  data->step++;
  putch(data, '\n');  
}

static inline void step_ko(boot_code_t *data) {
  putch(data, '!');  
  putch(data, '\n');  
}

static inline void step_done(boot_code_t *data) {
  putch(data, '.');  
  putch(data, '\n');  
}

#endif




void wait_soc_event()
{
#if defined(ITC_VERSION)
  hal_itc_enable_set(1 << ARCHI_FC_EVT_SOC_EVT);
  while(!((hal_itc_status_value_get() >> ARCHI_FC_EVT_SOC_EVT) & 1)) {
    hal_itc_wait_for_interrupt();
  }
  hal_itc_fifo_pop();
  hal_itc_status_clr(1 << ARCHI_FC_EVT_SOC_EVT);
  hal_itc_enable_clr(1 << ARCHI_FC_EVT_SOC_EVT);
#elif defined(EU_VERSION)
  eu_evt_maskWaitAndClr(1 << ARCHI_FC_EVT_SOC_EVT);
  eu_soc_events_pop();
  // The event must be cleared again as it is generated as soon as the fifo is not empty
  eu_evt_clr(1 << ARCHI_FC_EVT_SOC_EVT);
#endif
}

static void flash_read(boot_code_t *data, unsigned int flashAddr, unsigned int l2Addr, unsigned int size)
{
#if defined(UDMA_VERSION)
  if (!data->hyperflash) {
    unsigned int *buffer = data->udma_buffer;
    int buff_size;
    if (data->qpi) {
      buff_size = 8 * 4;
      *(volatile int *)&buffer[0] = SPI_CMD_SOT       (0);
      *(volatile int *)&buffer[1] = SPI_CMD_SEND_CMD  (0xEC, 8, 0);
      *(volatile int *)&buffer[2] = SPI_CMD_SEND_ADDR (32, 1);
      *(volatile int *)&buffer[3] = SPI_CMD_SEND_ADDR_VALUE(flashAddr);
      *(volatile int *)&buffer[4] = SPI_CMD_SEND_CMD  (0x00, 8, 1);
      *(volatile int *)&buffer[5] = SPI_CMD_DUMMY     (15);
      *(volatile int *)&buffer[6] = SPI_CMD_RX_DATA(size*8, 1, SPI_CMD_BYTE_ALIGN_ENA);
      *(volatile int *)&buffer[7] = SPI_CMD_EOT       (1);
    } else {
      buff_size = 7 * 4;
      *(volatile int *)&buffer[0] = SPI_CMD_SOT       (0);
      *(volatile int *)&buffer[1] = SPI_CMD_SEND_CMD  (0x0C, 8, 0);
      *(volatile int *)&buffer[2] = SPI_CMD_SEND_ADDR (32, 0);
      *(volatile int *)&buffer[3] = SPI_CMD_SEND_ADDR_VALUE(flashAddr);
      *(volatile int *)&buffer[4] = SPI_CMD_DUMMY     (15);
      *(volatile int *)&buffer[5] = SPI_CMD_RX_DATA(size*8, 0, SPI_CMD_BYTE_ALIGN_ENA);
      *(volatile int *)&buffer[6] = SPI_CMD_EOT       (1);
    }

    plp_udma_enqueue(UDMA_SPIM_RX_ADDR(0), l2Addr, size, UDMA_CHANNEL_CFG_EN | UDMA_CHANNEL_CFG_SIZE_32);
    plp_udma_enqueue(UDMA_SPIM_TX_ADDR(0), (unsigned int)data->udma_buffer, buff_size, UDMA_CHANNEL_CFG_EN | UDMA_CHANNEL_CFG_SIZE_32);
    wait_soc_event();

  } else {
#ifdef PLP_UDMA_HAS_HYPER
    hal_hyper_flash_ext_addr_set((flashAddr));
    plp_udma_enqueue(UDMA_HYPER_RX_ADDR(0), l2Addr, size, UDMA_CHANNEL_CFG_EN | UDMA_CHANNEL_CFG_SIZE_32);
    wait_soc_event();
#endif
  }
#endif
}

#if defined(UDMA_VERSION)
static int setReg(unsigned int *buffer, int index, unsigned addrAndVal, int eotRaise)
{
  *(volatile int *)&buffer[0] = SPI_CMD_SOT      (0);
  *(volatile int *)&buffer[1] = SPI_CMD_SEND_CMD (0x06, 8, 0);    //write enable 
  *(volatile int *)&buffer[2] = SPI_CMD_EOT      (0);
  *(volatile int *)&buffer[3] = SPI_CMD_SOT      (0);           
  *(volatile int *)&buffer[4] = SPI_CMD_SEND_CMD (0x0071, 8, 0);
  *(volatile int *)&buffer[5] = SPI_CMD_SEND_ADDR(32, 0);
  *(volatile int *)&buffer[6] = addrAndVal;
  *(volatile int *)&buffer[7] = SPI_CMD_EOT      (eotRaise);
  return 8;
}

static void flash_enqueueCommand(boot_code_t *data, unsigned short value, unsigned int addr) {
  short *buffer = (short *)data->udma_buffer;
  *buffer = value;
  hal_hyper_flash_ext_addr_set(addr<<1);
  plp_udma_enqueue(UDMA_HYPER_TX_ADDR(0), (int)buffer, 2, UDMA_CHANNEL_CFG_EN | UDMA_CHANNEL_CFG_SIZE_32);
  wait_soc_event();


}
#endif

static void flash_checkAndConf(boot_code_t *data)
{
#if defined(UDMA_VERSION)
  if (!data->hyperflash) {
    int index = 0;
    if(data->qpi) {
      // CR1V config: quad IO mode (only allows quad mode operations but instruction is still normal mode)
      index += setReg(&data->udma_buffer[index], index, 0x80000202, 0);
    }
    
    // CR2V config: 15 dummy cycles, 4 bytes address, no QPI mode
    index += setReg(&data->udma_buffer[index], index, 0x8000030f, 1);

    plp_udma_enqueue(UDMA_SPIM_TX_ADDR(0), (unsigned int)data->udma_buffer, index*4, UDMA_CHANNEL_CFG_EN | UDMA_CHANNEL_CFG_SIZE_32);
    wait_soc_event();
  } else {

#ifdef PLP_UDMA_HAS_HYPER
#if PULP_CHIP == CHIP_GAP

    config_pin(PLP_PAD_HYPER_CKN, PLP_PAD_GPIO_ALTERNATE3);
    config_pin(PLP_PAD_HYPER_CK,  PLP_PAD_GPIO_ALTERNATE3);
    config_pin(PLP_PAD_HYPER_DQ0, PLP_PAD_GPIO_ALTERNATE3);
    config_pin(PLP_PAD_HYPER_DQ1, PLP_PAD_GPIO_ALTERNATE3);
    config_pin(PLP_PAD_HYPER_DQ2, PLP_PAD_GPIO_ALTERNATE3);
    config_pin(PLP_PAD_HYPER_DQ3, PLP_PAD_GPIO_ALTERNATE3);
    config_pin(PLP_PAD_HYPER_DQ4, PLP_PAD_GPIO_ALTERNATE3);
    config_pin(PLP_PAD_HYPER_DQ5, PLP_PAD_GPIO_ALTERNATE3);
    config_pin(PLP_PAD_HYPER_DQ6, PLP_PAD_GPIO_ALTERNATE3);
    config_pin(PLP_PAD_HYPER_DQ7, PLP_PAD_GPIO_ALTERNATE3);
    config_pin(PLP_PAD_HYPER_CSN0, PLP_PAD_GPIO_ALTERNATE3);
    config_pin(PLP_PAD_HYPER_CSN1, PLP_PAD_GPIO_ALTERNATE3);
    config_pin(PLP_PAD_HYPER_RWDS, PLP_PAD_GPIO_ALTERNATE3);

    hal_hyper_start_ip();

    // Set memory base address with RAM = 16M
    hal_hyper_udma_mbr0_set(REG_MBR0);
    hal_hyper_udma_mbr1_set(REG_MBR1>>24);

    /* Device type of connected memory */
    // HyperRAM
    hal_hyper_udma_dt0_set(1);
    // HyperFlash
    hal_hyper_udma_dt1_set(0);

    // When using flash, this bit should set to 0, always memory access
    hal_hyper_udma_crt1_set(MEM_ACCESS);

    flash_enqueueCommand(data, 0xAA, 0x555);
    flash_enqueueCommand(data, 0x55, 0x2AA);
    flash_enqueueCommand(data, 0x38, 0x555);
    flash_enqueueCommand(data, 0x8e0b, 0x0);

#else

    plp_udma_cg_set(plp_udma_cg_get() | (1<<UDMA_HYPER_ID(0)));

    // Set memory base address with RAM = 16M
    hal_hyper_udma_mbr0_set(REG_MBR0);
    hal_hyper_udma_mbr1_set(REG_MBR1>>24);

    /* Device type of connected memory */
    // HyperRAM
    hal_hyper_udma_dt0_set(1);
    // HyperFlash
    hal_hyper_udma_dt1_set(0);

    // When using flash, this bit should set to 0, always memory access
    hal_hyper_udma_crt1_set(MEM_ACCESS);

    hal_hyper_udma_crt0_set(MEM_ACCESS);

#endif
#endif

  }
#endif
}

void wait_clock_ref(int nbTicks) {
#if defined(ITC_VERSION)
  hal_itc_enable_set(1<<ARCHI_FC_EVT_CLK_REF);
  for (int i=0; i<nbTicks; i++) {
    hal_itc_wait_for_interrupt();
    hal_itc_status_clr(1<<ARCHI_FC_EVT_CLK_REF);
  }
  hal_itc_enable_clr(1<<ARCHI_FC_EVT_CLK_REF);
#elif defined(EU_VERSION)
  unsigned int i;
  soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_REF_CLK_RISE);
  for (int i=0; i<nbTicks; i++) {
    wait_soc_event();
  }
  soc_eu_fcEventMask_clearEvent(ARCHI_SOC_EVENT_REF_CLK_RISE);
#endif
}

static void init(boot_code_t *data)
{

  data->step = 0;

#ifdef DEBUG
  plp_udma_cg_set((1<<UDMA_UART_ID(0))); 
  plp_uart_setup(0, UART_CLK_DIVIDER);
#endif

#ifdef UDMA_VERSION
  plp_udma_cg_set(plp_udma_cg_get() | (1<<ARCHI_UDMA_SPIM_ID(0))); 
#endif

  //spi_setup(4, 0, 0, SPI_TYPE_WORD); // divide sys clock by 4

#ifdef SOC_EU_VERSION
  // Deactivate all soc event to be able to wait on specific ones
  soc_eu_eventMask_set(SOC_FC_MASK_MSB, 0xffffffff);
  soc_eu_eventMask_set(SOC_FC_MASK_LSB, 0xffffffff);
  soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_SPIM0_EOT);
  soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_UART_TX(0));
#ifdef ARCHI_UDMA_HAS_HYPER
  soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_HYPER_RX(0));
  soc_eu_fcEventMask_setEvent(ARCHI_SOC_EVENT_HYPER_TX(0));
#endif
#endif

  // wait some time to have proper power up of external flash
  wait_clock_ref(20);

}

static void deinit(boot_code_t *data) {
#ifdef UDMA_VERSION
  plp_uart_disable(0);
  soc_eu_eventMask_set(SOC_FC_MASK_MSB, 0xffffffff);
  soc_eu_eventMask_set(SOC_FC_MASK_LSB, 0xffffffff);
  plp_udma_cg_set(0);
#endif
}

void *memcpy(void *dest, const void *src, size_t len) {
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}

static void load_section(boot_code_t *data, flash_v2_mem_area_t *area, int encrypted) {
  unsigned int flash_addr = area->start;
  unsigned int area_addr = area->ptr;
  unsigned int size = area->size;
  unsigned int i;

  int isL2Section = area_addr >= 0x1C000000 && area_addr < 0x1D000000;

  for (i = 0; i < area->blocks; i++) { // 4KB blocks

    unsigned int iterSize = data->blockSize;
    if (iterSize > size) iterSize = (size + 3) & 0xfffffffc;

    if (isL2Section) {
      flash_read(data, flash_addr, area_addr, iterSize);
    } else {
      flash_read(data, flash_addr, (unsigned int)(long)data->flashBuffer, iterSize);
      memcpy((void *)(long)area_addr, (void *)(long)data->flashBuffer, iterSize);
    }

    area_addr  += iterSize;
    flash_addr += iterSize;
    size       -= iterSize;
  }

#if PULP_CHIP == CHIP_GAP
  if (encrypted) {
    AesCtrCipher(&bootCodeFc.aes_data, (unsigned char *)area->ptr, (unsigned char *)area->ptr, bootCodeFc.Key, bootCodeFc.IV, area->size, bootCodeFc.aes_init);
    bootCodeFc.aes_init = 0;
  }
#endif

}

static inline void __attribute__((noreturn)) jump_to_address(unsigned int address) {
  void (*entry)() = (void (*)())((long)address);
  entry();
  while(1);
}

static inline void __attribute__((noreturn)) jump_to_entry(flash_v2_header_t *header) {

#if defined(PLP_FC_HAS_ICACHE)
  // TODO This should not be needed as the code from boot rom should be differentiated
  // from the one in L2 by the cache. Otherwise it is anyway not enough
  flush_all_icache_banks();
#endif

#if PULP_CHIP != CHIP_GAP
#ifdef APB_SOC_VERSION
  apb_soc_bootaddr_set(header->bootaddr);
#endif
#endif

  jump_to_address(header->entry);
  while(1);
}

__attribute__((noreturn)) void changeStack(boot_code_t *data, unsigned int entry, unsigned int stack);

static void getMemAreas(boot_code_t *data)
{
  flash_read(data, 0, (unsigned int)(long)&data->header, sizeof(data->header));

  int nbArea = data->header.nbAreas;
  if (nbArea >= MAX_NB_AREA) {
    nbArea = MAX_NB_AREA;
  }

  if (nbArea) flash_read(data, sizeof(flash_v2_header_t), (unsigned int)(long)data->memArea, nbArea*sizeof(flash_v2_mem_area_t));
}

static __attribute__((noreturn)) void loadBinaryAndStart(boot_code_t *data)
{

  getMemAreas(data);

  int encrypted = data->encrypted;

  if (encrypted) {

#ifdef EFUSE_VERSION
    plp_efuse_startRead();

    for (int i=0; i<GAP_EFUSE_AES_KEY_NB_REGS; i++) {
      ((unsigned char *)bootCodeFc.Key)[i] = plp_efuse_aesKey_get(i);
    }
    for (int i=0; i<GAP_EFUSE_AES_IV_NB_REGS; i++) {
      ((unsigned char *)bootCodeFc.IV)[i] = plp_efuse_aesIv_get(i);
    }
    plp_efuse_sleep();
#endif

#if PULP_CHIP == CHIP_GAP
    AesBuildLUT(&bootCodeFc.aes_data);
#endif
  }

  unsigned int i;
  for (i=0; i<data->header.nbAreas; i++) {
    load_section(data, &data->memArea[i], encrypted);
  }

#if PULP_CHIP == CHIP_GAP
  if (encrypted) {  
    // In case the code is not valid, just sleep for ever to avoid doing
    // bad things due to corrupted binary
    // For that we check that the entry point vector is a jump
    if (((*(volatile unsigned int *)data->header.entry) & 0x00001fff) != 0x6f) {
      while(1) {
        eu_evt_maskWaitAndClr(0);
      }
    }
  }
#endif

  deinit(data);

  jump_to_entry(&data->header);
}

static __attribute__((noreturn)) void loadBinaryAndStart_newStack(boot_code_t *data)
{
  changeStack(data, (unsigned int)(long)loadBinaryAndStart, ((unsigned int)(long)data->stack) + BOOT_STACK_SIZE);
}


static boot_code_t *findDataFit(boot_code_t *data)
{
  unsigned int addr = 0x1c000000;
  unsigned int i;

  for (i=0; i<data->header.nbAreas; i++) {
    flash_v2_mem_area_t *area = &data->memArea[i];
    if ((addr >= area->ptr && addr < area->ptr + area->size)
      || (addr < area->ptr && addr + sizeof(boot_code_t) > area->ptr)) {
        addr = ((area->ptr + area->size) + data->blockSize - 1) & ~(data->blockSize - 1);
      }
  }
  return (boot_code_t *)(long)addr;
}

static void bootFromRom(int encrypted, int hyperflash, int qpi)
{
  boot_code_t *data = &bootCode;

  data->hyperflash = hyperflash;
  if (hyperflash) data->blockSize = HYPER_FLASH_BLOCK_SIZE;
  else data->blockSize = FLASH_BLOCK_SIZE;
  data->qpi = qpi;

  init(data);

  //__builtin_pulp_write_base_off_v(((1<<4)<<16)|(0x3<<1)|(1), PMU_DLC_BASE_ADDRESS, 0x0);
  //wait_soc_event();

  flash_checkAndConf(data);

  getMemAreas(data);

  boot_code_t *newData = findDataFit(data);
  newData->encrypted = encrypted;
  newData->hyperflash = hyperflash;
  newData->qpi = qpi;
  bootCodeFc.aes_init = 1;
  if (hyperflash) newData->blockSize = HYPER_FLASH_BLOCK_SIZE;
  else newData->blockSize = FLASH_BLOCK_SIZE;

  loadBinaryAndStart_newStack(newData);

}

static void __attribute__((noreturn)) bootFromSpi(int platform, int encrypted)
{        
  if (platform == APB_SOC_PLT_FPGA) {

    // On FPGA, as the external JTAG reg is not accessible and we do not need to configure the pads,
    // We just wait until the loader writes to the internal jtag register
    while(1) {
      unsigned int value = apb_soc_jtag_reg_read();
      if (apb_soc_jtag_reg_loc(value) & 1) break;
    }

    // And finally jump to the loaded code
    jump_to_address(0x1c000080);

  } else {

#if PULP_CHIP == CHIP_GAP

    // Configure pads alternate to enable SPI slave
    config_pin( 8 + 8, 2);
    config_pin( 9 + 8, 2);
    config_pin(27 + 8, 2);
    config_pin(28 + 8, 2);
    config_pin(29 + 8, 2);
    config_pin(37 + 8, 2);

    // Notify external env that pads are configured, this will trigger binary loading through SPI slave
    apb_soc_jtag_reg_write(1);

    // Now wait until the external env notify us that it is finished
    while(1) {
      unsigned int value = apb_soc_jtag_reg_read();
      if (apb_soc_jtag_reg_ext(value) & 1) break;
      wait_clock_ref(1);
    }

    // Set back default alternate to not disturb the loaded binary
    config_pin( 8 + 8, 0);
    config_pin( 9 + 8, 0);
    config_pin(27 + 8, 0);
    config_pin(28 + 8, 0);
    config_pin(29 + 8, 0);
    config_pin(37 + 8, 0);

    // And finally jump to the loaded code
    jump_to_address(0x1c000080);

#endif

  }

  while(1);
}

static void __attribute__((noreturn)) bootFromJtag(int encrypted)
{        
  // Notify external env that we are ready for the loading
  apb_soc_jtag_reg_write(1);

  // Just wait until the external env notify us that the binary is loaded
  while(1) {
    unsigned int value = apb_soc_jtag_reg_read();
    if (apb_soc_jtag_reg_ext(value) & 1) break;
    wait_clock_ref(1);
  }

  // And jump to the loaded code
  jump_to_address(0x1c008080);

}

static __attribute__((noreturn)) void choose_boot_mode(int mode, int platform, int encrypted)
{
  switch (mode) {

    case APB_SOC_BOOT_SPI:
      bootFromSpi(platform, encrypted);
      break;

    case APB_SOC_BOOT_JTAG:
      bootFromJtag(encrypted);
      break;

    case APB_SOC_BOOT_OTHER:
      bootFromOther(platform, encrypted);
      break;

    case APB_SOC_BOOT_HYPER:
      bootFromRom(encrypted, 1, 0);
      break;

    case APB_SOC_BOOT_SPIM:
      bootFromRom(encrypted, 0, 0);
      break;

    case APB_SOC_BOOT_SPIM_QPI:
    case APB_SOC_BOOT_ROM:
    default:
      bootFromRom(encrypted, 0, 1);
      break;
  }  
  while(1);
}

// Special case where the efuse has not been programmed
// This is mainly used for development, check the jtag register to see what to do
static void __attribute__((noreturn)) bootFromOther(int platform, int encrypted)
{  
  // First jtag bit tell us what to do
  unsigned int value = apb_soc_jtag_reg_ext(apb_soc_jtag_reg_read());
  if (value) {
    // Value is not zero, check it to see how to boot
    unsigned int mode = ARCHI_REG_FIELD_GET(value, 1, APB_SOC_JTAG_REG_EXT_WIDTH-1);

    choose_boot_mode(mode, platform, encrypted);

  }

  // Nothing was specified or something incorrect, just do the normal boot from rom
  bootFromRom(encrypted, 0, 1);

  while(1);
}

void __attribute__((noreturn)) main() 
{

#if PULP_CHIP == CHIP_GAP
  // Check if we do an L2 retentive boot, in this case, don't do any loading, just dump to the entry point
  PMU_RetentionStateT PMURetentionState;
  PMURetentionState.Raw = GetRetentiveState();
  if (PMURetentionState.Fields.BootType == RETENTIVE_BOOT) {
    jump_to_address(0x1c000080);
  }
#endif

#ifdef EFUSE_VERSION

  // The way we should boot is specified in the efuse
  plp_efuse_startRead();
  unsigned int info = plp_efuse_info_get();
#ifdef EFUSE_VERSION
    // Check if we need to wait for xtal stabilization, only if specified in efuse and we come from cold boot
    if (PMURetentionState.Fields.BootType == COLD_BOOT && plp_efuse_wait_xtal_get(info)) {
      if (!WaitStableXtal(plp_efuse_wait_xtal_delta_get(), plp_efuse_wait_xtal_min_get(), plp_efuse_wait_xtal_max_get())) boot_abort();
    }
#endif
  int bootMode = plp_efuse_bootmode_get(info);
  int platform = plp_efuse_platform_get(info);  
  int encrypted = plp_efuse_encrypted_get(info);
  plp_efuse_sleep();

  choose_boot_mode(bootMode, platform, encrypted);

#else

#if defined(APB_SOC_VERSION) && APB_SOC_VERSION == 3

#if PULP_CHIP == CHIP_WOLFE
  bootFromRom(0, apb_soc_pads_config_bootself_get(), 1);
#else
  bootFromOther(0, 0);
#endif

#else

#if defined(APB_SOC_VERSION) && APB_SOC_VERSION >= 4

  bootFromOther(APB_SOC_PLT_OTHER, 0);

#else

  bootFromRom(0, 0, 1);

#endif

#endif

#endif  

  while(1);
}
