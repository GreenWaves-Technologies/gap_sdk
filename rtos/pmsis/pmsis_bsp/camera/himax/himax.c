/*
 * Copyright (C) 2019 GreenWaves Technologies
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
 * Authors: Yao ZHANG, GreenWaves Technologies (yao.zhang@greenwaves-technologies.com)
 *          Francesco PACI, GreenWaves Technologies (francesco.paci@greenwaves-technologies.com)
 *          Germain HAUGOU, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "pmsis.h"
#include "pmsis/rtos/os_frontend_api/pmsis_time.h"
#include "bsp/bsp.h"
#include "bsp/camera/himax.h"
#include "himax.h"
#include <string.h>


typedef struct {
  uint16_t addr;
  uint8_t value;
} i2c_req_t;


typedef struct {
  struct pi_himax_conf conf;
  struct pi_device cpi_device;
  struct pi_device i2c_device;
  i2c_req_t i2c_req;
  uint32_t i2c_read_value;
  int is_awake;
} himax_t;



typedef struct {
    uint16_t addr;
    uint8_t data;
} himax_reg_init_t;



static himax_reg_init_t __himax_reg_init[] =
{
  {HIMAX_BLC_TGT, 0x08},            //  BLC target :8  at 8 bit mode
  {HIMAX_BLC2_TGT, 0x08},           //  BLI target :8  at 8 bit mode
  {0x3044, 0x0A},             //  Increase CDS time for settling
  {0x3045, 0x00},             //  Make symetric for cds_tg and rst_tg
  {0x3047, 0x0A},             //  Increase CDS time for settling
  {0x3050, 0xC0},             //  Make negative offset up to 4x
  {0x3051, 0x42},
  {0x3052, 0x50},
  {0x3053, 0x00},
  {0x3054, 0x03},             //  tuning sf sig clamping as lowest
  {0x3055, 0xF7},             //  tuning dsun
  {0x3056, 0xF8},             //  increase adc nonoverlap clk
  {0x3057, 0x29},             //  increase adc pwr for missing code
  {0x3058, 0x1F},             //  turn on dsun
  {HIMAX_BIT_CONTROL, 0x1E},
  {HIMAX_ANA_Register_14, 0x00},
  {HIMAX_OUTPUT_PIN_STATUS_CONTROL, 0x04},             //  pad pull 0

  {HIMAX_BLC_CFG, 0x43},            //  BLC_on, IIR

  {0x1001, 0x43},             //  BLC dithering en
  {0x1002, 0x43},             //  blc_darkpixel_thd
  {0x0350, 0x00},             //  Dgain Control
  {HIMAX_BLI_EN, 0x01},             //  BLI enable
  {HIMAX_BLC_TGT, 0x00},             //  BLI Target [Def: 0x20]

  {HIMAX_DPC_CTRL, 0x01},           //  DPC option 0: DPC off   1 : mono   3 : bayer1   5 : bayer2
  {0x1009, 0xA0},             //  cluster hot pixel th
  {0x100A, 0x60},             //  cluster cold pixel th
  {HIMAX_SINGLE_THR_HOT, 0x90},     //  single hot pixel th
  {HIMAX_SINGLE_THR_COLD, 0x40},    //  single cold pixel th

  {HIMAX_STATISTIC_CTRL, 0x07},
  {HIMAX_MD_LROI_Y_START_H, 0x00},
  {HIMAX_MD_LROI_Y_START_L, 0x58},
  {HIMAX_MD_LROI_Y_END_H, 0x00},
  {HIMAX_MD_LROI_Y_END_L, 0x9B},

  {HIMAX_AE_CTRL,        0x01},      //Automatic Exposure Gain Control
  {HIMAX_AE_TARGET_MEAN, 0x3C},      //AE target mean [Def: 0x3C]
  {HIMAX_AE_MIN_MEAN,    0x0A},      //AE min target mean [Def: 0x0A]

  {HIMAX_INTEGRATION_H,  0x00},      //Integration H [Def: 0x01]
  {HIMAX_INTEGRATION_L,  0x60},      //Integration L [Def: 0x08]
  {HIMAX_ANALOG_GAIN,    0x00},      //Analog Global Gain
  {HIMAX_DAMPING_FACTOR, 0x20},      //Damping Factor [Def: 0x20]
  {HIMAX_DIGITAL_GAIN_H, 0x01},      //Digital Gain High [Def: 0x01]
  {HIMAX_DIGITAL_GAIN_L, 0x00},      //Digital Gain Low [Def: 0x00]

  {HIMAX_CONVERGE_IN_TH, 0x03},
  {HIMAX_CONVERGE_OUT_TH, 0x05},

  {HIMAX_MAX_INTG_H, 0x01},
  {HIMAX_MAX_INTG_L, 0x54},

  {HIMAX_MAX_AGAIN_FULL, 0x03},
  {HIMAX_MAX_AGAIN_BIN2, 0x04},

  {HIMAX_MAX_DGAIN, 0xC0},
  {HIMAX_FS_CTRL, 0x00}, //Flicker Control
  {HIMAX_FS_60HZ_H, 0x00},
  {HIMAX_FS_60HZ_L, 0x3C},
  {HIMAX_FS_50HZ_H, 0x00},
  {HIMAX_FS_50HZ_L, 0x32},

  {HIMAX_MD_CTRL, 0x30},
  {HIMAX_FRAME_LEN_LINES_H, 0x02},
  {HIMAX_FRAME_LEN_LINES_L, 0x16},
  {HIMAX_LINE_LEN_PCK_H, 0x01},
  {HIMAX_LINE_LEN_PCK_L, 0x78},
  {HIMAX_QVGA_WIN_EN, 0x00},
  {HIMAX_READOUT_X, 0x01},
  {HIMAX_READOUT_Y, 0x01},
  {HIMAX_BINNING_MODE, 0x00},
  {HIMAX_SIX_BIT_MODE_EN, 0x70},
  {HIMAX_BIT_CONTROL, 0x02},
  {HIMAX_OSC_CLK_DIV, 0x0B},
//    {HIMAX_OSC_CLK_DIV, 0x25}, //Clock gating and clock divisors
  {HIMAX_PCLK_POLARITY, 0x20}, //PCLK0 polarity
  {HIMAX_IMG_ORIENTATION, 0x00}, // change the orientation
  {HIMAX_GRP_PARAM_HOLD, 0x01}, // hold
  {HIMAX_VSYNC_HSYNC_PIXEL_SHIFT_EN, 0x01}, // avoid 2 invalid pixel columns

  {HIMAX_MODE_SELECT, 0x00}, // standby mode
};



static inline int is_i2c_active()
{
#if defined(__PLATFORM_RTL__) || defined(__PLATFORM_GVSOC__)

  // I2C driver is not yet working on some chips, at least this works on gvsoc.
  // Also there is noI2C connection to camera model on RTL

  return 0;

#else

  return 1;

#endif
}



static void __himax_reg_write(himax_t *himax, uint16_t addr, uint8_t value)
{
  if (is_i2c_active())
  {
    himax->i2c_req.value = value;
    himax->i2c_req.addr = ((addr >> 8) & 0xff) | ((addr & 0xff) << 8);
    pi_i2c_write(&himax->i2c_device, (uint8_t *)&himax->i2c_req, 3, PI_I2C_XFER_STOP);
  }
}



static uint8_t __himax_reg_read(himax_t *himax, uint16_t addr)
{
  if (is_i2c_active())
  {
    himax->i2c_req.addr = ((addr >> 8) & 0xff) | ((addr & 0xff) << 8);
    pi_i2c_write(&himax->i2c_device, (uint8_t *)&himax->i2c_req, 2, PI_I2C_XFER_NO_STOP);
    pi_i2c_read(&himax->i2c_device, (uint8_t *)&himax->i2c_read_value, 1, PI_I2C_XFER_STOP);
    return *(volatile uint8_t *)&himax->i2c_read_value;
  }
  else
  {
    return 0;
  }
}



static void __himax_init_regs(himax_t *himax)
{
  int32_t i;
  for(i=0; i<(int32_t)(sizeof(__himax_reg_init)/sizeof(himax_reg_init_t)); i++)
  {
    __himax_reg_write(himax, __himax_reg_init[i].addr, __himax_reg_init[i].data);
  }
}



static void __himax_reset(himax_t *himax)
{
  __himax_reg_write(himax, HIMAX_SW_RESET, HIMAX_RESET);

  while (__himax_reg_read(himax, HIMAX_MODE_SELECT) != HIMAX_STANDBY)
  {
    __himax_reg_write(himax, HIMAX_SW_RESET, HIMAX_RESET);
    pi_time_wait_us(50);
  }
}

static void __himax_set_qqvga(himax_t *himax){
  __himax_reg_write(himax, HIMAX_BINNING_MODE, 0x03);
  __himax_reg_write(himax, HIMAX_READOUT_X, 0x03);
  __himax_reg_write(himax, HIMAX_READOUT_Y, 0x03);
}


static void __himax_mode(himax_t *himax, uint8_t mode)
{
  __himax_reg_write(himax, HIMAX_MODE_SELECT, mode);
}



static void __himax_wakeup(himax_t *himax)
{
  if (!himax->is_awake)
  {
    __himax_mode(himax, HIMAX_STREAMING);
    himax->is_awake = 1;
  }
}



static void __himax_standby(himax_t *himax)
{
  if (himax->is_awake)
  {
    __himax_mode(himax, HIMAX_STANDBY);
    himax->is_awake = 0;
  }
}


static int __himax_id_check(himax_t *himax)
{
#if defined(ARCHI_PLATFORM_RTL) || defined(__PLATFORM_RTL__)
  return 0;
#else
  uint8_t id_h, id_l;
  id_h = __himax_reg_read(himax, HIMAX_MODEL_ID_H);
  id_l = __himax_reg_read(himax, HIMAX_MODEL_ID_L);

  if (id_h != 0x01 || id_l != 0xB0)
  {
    printf("Himax id read error, should be 01b0, but get %x%x", id_h, id_l);
    return -1;
  }
  return 0;
#endif
}

static void __himax_aeg_init(himax_t *himax)
{
    __himax_wakeup(himax);
    pi_time_wait_us(100000);
    __himax_standby(himax);
}


int32_t __himax_open(struct pi_device *device)
{
  struct pi_himax_conf *conf = (struct pi_himax_conf *)device->config;

  himax_t *himax = (himax_t *)pmsis_l2_malloc(sizeof(himax_t));
  if (himax == NULL) return -1;
  memcpy(&himax->conf, conf, sizeof(*conf));

  device->data = (void *)himax;

  if (bsp_himax_open(conf))
    goto error;

  struct pi_cpi_conf cpi_conf;
  pi_cpi_conf_init(&cpi_conf);
  cpi_conf.itf = conf->cpi_itf;
  pi_open_from_conf(&himax->cpi_device, &cpi_conf);

  if (pi_cpi_open(&himax->cpi_device))
    goto error;

  struct pi_i2c_conf i2c_conf;
  pi_i2c_conf_init(&i2c_conf);
  i2c_conf.cs = 0x48;
  i2c_conf.itf = conf->i2c_itf;
  pi_open_from_conf(&himax->i2c_device, &i2c_conf);

  if (pi_i2c_open(&himax->i2c_device))
    goto error2;

  pi_cpi_set_format(&himax->cpi_device, PI_CPI_FORMAT_BYPASS_BIGEND);

#ifdef __GAP9__
  // The rowlen, ur_x and ll_x need to be div by 2 when pixel size is 1 byte.
  uint8_t div = 1;
#else
  // The rowlen, ur_x and ll_x need to be div by 2 and -1 when pixel size is 1 byte.
  // The div and -1 has been integrated in gap8 cpi driver.
  uint8_t div = 0;
#endif
  uint16_t rowlen=0;

  // If it's not the predefined format
  if(himax->conf.format>PI_CAMERA_QQVGA){
      rowlen = himax->conf.format;
  }else if(himax->conf.format==PI_CAMERA_QQVGA){
      rowlen = 160+2;
  }else{
      rowlen = 320+4;
  }

  pi_cpi_set_rowlen(&himax->cpi_device, rowlen>>div);

  if (himax->conf.roi.slice_en != 0)
    pi_cpi_set_slice(&himax->cpi_device, himax->conf.roi.x>>div,
            himax->conf.roi.y,
            himax->conf.roi.w>>div,
            himax->conf.roi.h);

#ifdef __GAP9__
  // Reverse the vsync, since the vsync of himax is always 1 during one frame.
#if !defined(ARCHI_PLATFORM_RTL) && !defined(__PLATFORM_RTL__)
  pi_cpi_set_sync_polarity(&himax->cpi_device, 1, 0);
#endif
#endif

  himax->is_awake = 0;

  __himax_reset(himax);

  if (is_i2c_active())
  {
    if (__himax_id_check(himax))
      goto error2;
  }

  __himax_init_regs(himax);
  if(himax->conf.format==PI_CAMERA_QQVGA){
    __himax_set_qqvga(himax);
  }

  return 0;

error2:
  pi_cpi_close(&himax->cpi_device);
error:
  pmsis_l2_malloc_free(himax, sizeof(himax_t));
  return -1;
}


//This is a copy of open with an additional argument, not very nice but effective
//Otherwise we would need to add an argument to open anche change all user code

int32_t __himax_reopen(struct pi_device *device, pi_camera_opts_e opts)
{
  struct pi_himax_conf *conf = (struct pi_himax_conf *)device->config;

  himax_t *himax = (himax_t *)pmsis_l2_malloc(sizeof(himax_t));
  if (himax == NULL) return -1;
  memcpy(&himax->conf, conf, sizeof(*conf));

  device->data = (void *)himax;

  if (bsp_himax_open(conf))
    goto error;

  struct pi_cpi_conf cpi_conf;
  pi_cpi_conf_init(&cpi_conf);
  cpi_conf.itf = conf->cpi_itf;
  pi_open_from_conf(&himax->cpi_device, &cpi_conf);

  if (pi_cpi_open(&himax->cpi_device))
    goto error;

  struct pi_i2c_conf i2c_conf;
  pi_i2c_conf_init(&i2c_conf);
  i2c_conf.cs = 0x48;
  i2c_conf.itf = conf->i2c_itf;
  pi_open_from_conf(&himax->i2c_device, &i2c_conf);

  if (pi_i2c_open(&himax->i2c_device))
    goto error2;

  pi_cpi_set_format(&himax->cpi_device, PI_CPI_FORMAT_BYPASS_BIGEND);

  himax->is_awake = 0;

  if(opts != PI_CAMERA_OPT_NO_REG_INIT){
    __himax_reset(himax);
    __himax_init_regs(himax);
  }

  if(himax->conf.format==PI_CAMERA_QQVGA){
    __himax_set_qqvga(himax);
  }

  return 0;

error2:
  pi_cpi_close(&himax->cpi_device);
error:
  pmsis_l2_malloc_free(himax, sizeof(himax_t));
  return -1;
}



static void __himax_close(struct pi_device *device)
{
  himax_t *himax = (himax_t *)device->data;
  __himax_standby(himax);
  pi_cpi_close(&himax->cpi_device);
  pmsis_l2_malloc_free(himax, sizeof(himax_t));
}



static int32_t __himax_control(struct pi_device *device, pi_camera_cmd_e cmd, void *arg)
{
  int irq = disable_irq();
  pi_camera_opts_e open_opt = (pi_camera_opts_e) arg;

  himax_t *himax = (himax_t *)device->data;

  switch (cmd)
  {
    case PI_CAMERA_CMD_ON:
      __himax_reopen(device, open_opt);
      break;

    case PI_CAMERA_CMD_OFF:
      __himax_standby(himax);
      break;

    case PI_CAMERA_CMD_START:
      pi_cpi_control_start(&himax->cpi_device);
      __himax_wakeup(himax);
      break;

    case PI_CAMERA_CMD_STOP:
      __himax_standby(himax);
      pi_cpi_control_stop(&himax->cpi_device);
      break;

    case PI_CAMERA_CMD_AEG_INIT:
      __himax_aeg_init(himax);
      break;

    default:
      break;
  }

  restore_irq(irq);

  return 0;
}



void __himax_capture_async(struct pi_device *device, void *buffer, uint32_t bufferlen, pi_task_t *task)
{
  himax_t *himax = (himax_t *)device->data;

  pi_cpi_capture_async(&himax->cpi_device, buffer, bufferlen, task);
}



int32_t __himax_reg_set(struct pi_device *device, uint32_t addr, uint8_t *value)
{
  himax_t *himax = (himax_t *)device->data;
  __himax_reg_write(himax, addr, *value);
  return 0;
}



int32_t __himax_reg_get(struct pi_device *device, uint32_t addr, uint8_t *value)
{
  himax_t *himax = (himax_t *)device->data;
  *value = __himax_reg_read(himax, addr);
  return 0;
}



static pi_camera_api_t himax_api =
{
  .open           = &__himax_open,
  .close          = &__himax_close,
  .control        = &__himax_control,
  .capture_async  = &__himax_capture_async,
  .reg_set        = &__himax_reg_set,
  .reg_get        = &__himax_reg_get
};

void pi_himax_conf_init(struct pi_himax_conf *conf)
{
  conf->camera.api = &himax_api;
  conf->skip_pads_config = 0;
  conf->format = PI_CAMERA_QVGA; //By Default QVGA
  conf->roi.slice_en = 0; //Disable the ROI by default.
  bsp_himax_conf_init(conf);
  __camera_conf_init(&conf->camera);
}
