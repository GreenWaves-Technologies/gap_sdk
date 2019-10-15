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
            Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "pmsis.h"
#include "pmsis/rtos/os_frontend_api/pmsis_time.h"
#include "bsp/bsp.h"
#include "bsp/camera/himax.h"
#include "himax.h"


typedef struct {
  uint16_t addr;
  uint8_t value;
} i2c_req_t;


typedef struct {
  struct pi_device cpi_device;
  struct pi_device i2c_device;
  i2c_req_t i2c_req;
  uint32_t i2c_read_value;
  int is_awake;
} himax_t;



typedef struct {
    unsigned short addr;
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
  {0x3059, 0x1E},
  {0x3064, 0x00},
  {0x3065, 0x04},             //  pad pull 0

  {HIMAX_BLC_CFG, 0x43},            //  BLC_on, IIR

  {0x1001, 0x43},             //  BLC dithering en
  {0x1002, 0x43},             //  blc_darkpixel_thd
  {0x0350, 0x00},             //  Dgain Control
  {HIMAX_BLI_EN, 0x01},             //  BLI enable
  {0x1003, 0x00},             //  BLI Target [Def: 0x20]

  {HIMAX_DPC_CTRL, 0x01},           //  DPC option 0: DPC off   1 : mono   3 : bayer1   5 : bayer2
  {0x1009, 0xA0},             //  cluster hot pixel th
  {0x100A, 0x60},             //  cluster cold pixel th
  {HIMAX_SINGLE_THR_HOT, 0x90},     //  single hot pixel th
  {HIMAX_SINGLE_THR_COLD, 0x40},    //  single cold pixel th
  {0x1012, 0x00},             //  Sync. shift disable
  {0x2000, 0x07},
  {0x2003, 0x00},
  {0x2004, 0x1C},
  {0x2007, 0x00},
  {0x2008, 0x58},
  {0x200B, 0x00},
  {0x200C, 0x7A},
  {0x200F, 0x00},
  {0x2010, 0xB8},
  {0x2013, 0x00},
  {0x2014, 0x58},
  {0x2017, 0x00},
  {0x2018, 0x9B},

  {HIMAX_AE_CTRL,        0x01},      //Automatic Exposure Gain Control
  {HIMAX_AE_TARGET_MEAN, 0x3C},      //AE target mean [Def: 0x3C]
  {HIMAX_AE_MIN_MEAN,    0x0A},      //AE min target mean [Def: 0x0A]

  {HIMAX_INTEGRATION_H,  0x00},      //Integration H [Def: 0x01]
  {HIMAX_INTEGRATION_L,  0x60},      //Integration L [Def: 0x08]
  {HIMAX_ANALOG_GAIN,    0x00},      //Analog Global Gain
  {HIMAX_DAMPING_FACTOR, 0x20},      //Damping Factor [Def: 0x20]
  {HIMAX_DIGITAL_GAIN_H, 0x01},      //Digital Gain High [Def: 0x01]
  {HIMAX_DIGITAL_GAIN_L, 0x00},      //Digital Gain Low [Def: 0x00]

  {0x2103, 0x03},

  {0x2104, 0x05},
  {0x2105, 0x01},

  {0x2106, 0x54},

  {0x2108, 0x03},
  {0x2109, 0x04},

  {0x210B, 0xC0},
  {0x210E, 0x00}, //Flicker Control
  {0x210F, 0x00},
  {0x2110, 0x3C},
  {0x2111, 0x00},
  {0x2112, 0x32},

  {0x2150, 0x30},
  {0x0340, 0x02},
  {0x0341, 0x16},
  {0x0342, 0x01},
  {0x0343, 0x78},
  {0x3010, 0x00},
  {0x0383, 0x01},
  {0x0387, 0x01},
  {0x0390, 0x00},
  {0x3011, 0x70},
  {0x3059, 0x02},
  {0x3060, 0x0B},
//    {0x3060, 0x25}, //Clock gating and clock divisors
  {0x3068, 0x20}, //PCLK0 polarity
  {HIMAX_IMG_ORIENTATION, 0x00}, // change the orientation
  {0x0104, 0x01}
};



static inline int is_i2c_active()
{
#if defined(ARCHI_PLATFORM_RTL)

  // I2C driver is not yet working on some chips, at least this works on gvsoc.
  // Also there is noI2C connection to camera model on RTL
#if PULP_CHIP == CHIP_GAP9 || PULP_CHIP == CHIP_VEGA || PULP_CHIP == CHIP_ARNOLD || PULP_CHIP == CHIP_PULPISSIMO || PULP_CHIP == CHIP_PULPISSIMO_V1
  return 0;
#else
  return rt_platform() != ARCHI_PLATFORM_RTL;
#endif

#else

  return 1;

#endif
}



static void __himax_reg_write(himax_t *himax, unsigned int addr, uint8_t value)
{
  if (is_i2c_active())
  {
    himax->i2c_req.value = value;
    himax->i2c_req.addr = ((addr >> 8) & 0xff) | ((addr & 0xff) << 8);
    pi_i2c_write(&himax->i2c_device, (uint8_t *)&himax->i2c_req, 3, PI_I2C_XFER_STOP);
  }
}



static uint8_t __himax_reg_read(himax_t *himax, unsigned int addr)
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
  unsigned int i;
  for(i=0; i<(sizeof(__himax_reg_init)/sizeof(himax_reg_init_t)); i++)
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



int32_t __himax_open(struct pi_device *device)
{
  struct pi_himax_conf *conf = (struct pi_himax_conf *)device->config;

  himax_t *himax = (himax_t *)pmsis_l2_malloc(sizeof(himax_t));
  if (himax == NULL) return -1;

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

  __himax_reset(himax);

  __himax_init_regs(himax);

  __himax_wakeup(himax);

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

  himax_t *himax = (himax_t *)device->data;

  switch (cmd)
  {
    case PI_CAMERA_CMD_ON:
      __himax_wakeup(himax);
      break;

    case PI_CAMERA_CMD_OFF:
      __himax_standby(himax);
      break;

    case PI_CAMERA_CMD_START:
      pi_cpi_control_start(&himax->cpi_device);
      break;

    case PI_CAMERA_CMD_STOP:
      pi_cpi_control_stop(&himax->cpi_device);
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
  bsp_himax_conf_init(conf);
  __camera_conf_init(&conf->camera);
}
