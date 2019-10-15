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

#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/camera/mt9v034.h"
#include "mt9v034.h"



typedef struct {
  uint8_t addr;
  uint16_t value;
} __attribute__((packed)) i2c_req_t;



typedef struct {
  struct pi_mt9v034_conf conf;
  struct pi_device cpi_device;
  struct pi_device i2c_device;
  struct pi_device gpio_port;
  i2c_req_t i2c_req;
} mt9v034_t;



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



static void __mt9v034_reg_write(mt9v034_t *mt9v034, uint8_t addr, uint16_t value)
{
  if (is_i2c_active())
  {
    mt9v034->i2c_req.value = ((value >> 8) & 0xff) | ((value & 0xff) << 8);
    mt9v034->i2c_req.addr = addr;
    pi_i2c_write(&mt9v034->i2c_device, (uint8_t *)&mt9v034->i2c_req, 3, PI_I2C_XFER_STOP);
  }
}


static uint16_t __mt9v034_reg_read(mt9v034_t *mt9v034, uint8_t addr)
{
  if (is_i2c_active())
  {
    mt9v034->i2c_req.addr = addr;
    pi_i2c_write(&mt9v034->i2c_device, (uint8_t *)&mt9v034->i2c_req, 1, PI_I2C_XFER_NO_STOP);
    pi_i2c_read(&mt9v034->i2c_device, (uint8_t *)&mt9v034->i2c_req.value, 2, PI_I2C_XFER_STOP);
    uint16_t value = mt9v034->i2c_req.value;
    return ((value >> 8) & 0xff) | ((value & 0xff) << 8);
  }
  return 0;
}



static int __mt9v034_start(mt9v034_t *mt9v034)
{
  // GPIO_CIS_EXP   // Make sure trigger input is low -- normally redundant
  pi_gpio_pin_write(&mt9v034->gpio_port, mt9v034->conf.trigger_gpio, 0);

  // Enable 3V3A/3V3D
  pi_gpio_pin_write(&mt9v034->gpio_port, mt9v034->conf.power_gpio, 1); // GPIO_CIS_PWRON

  // Wait for chip ready
  {
    uint16_t id;
    do
    {
      id = __mt9v034_reg_read(mt9v034, MT9V034_CHIP_ID_REG);
    } while (id == 0xFFFF);  // Getting 0xFFFF when I2C not yet ready

    if (id != MT9V034_CHIP_ID)
    {
      printf("Error - Unexpected I2C device address from CIS, id got: %x\n", id);
      return -1;
    }
  }

  // Make sure camera won't start outputting valid pixels yet: go to snapshot mode (trigger input asssumed low)
  __mt9v034_reg_write(mt9v034, MT9V034_CHIP_CONTROL, MT9V034_CHIP_CONTROL_SNAPSHOT_MODE | MT9V034_CHIP_CONTROL_DVP_OFF| MT9V034_CHIP_CONTROL_SIMULTANEOUS);

  // Reset chip (except I2C config) to abort any capture started in default master mode before config change (helps save power)
  __mt9v034_reg_write(mt9v034, MT9V034_RESET, MT9V034_SOFT_RESET);

  // Replace default values of some reserved registers as per recommendations ginven in datasheet Rev.D, Table 8 p14
  // Need different settings in case of binning ?
  if( (__mt9v034_reg_read(mt9v034, MT9V034_COARSE_SHUTTER_WIDTH_A)==0) && (__mt9v034_reg_read(mt9v034, MT9V034_FINE_SHUTTER_WIDTH_A)<456) )
  {
    __mt9v034_reg_write(mt9v034, MT9V034_RESERVED_REG20, 0x01c7); // exception to below general case
  }
  else
  {
    __mt9v034_reg_write(mt9v034, MT9V034_RESERVED_REG20, 0x03C7); // rather than default = 0x01C1
    __mt9v034_reg_write(mt9v034, MT9V034_RESERVED_REG13, 0x2D2E); // rather than default = 0x01C1
  }

  __mt9v034_reg_write(mt9v034, MT9V034_RESERVED_REG24, 0x001b); // rather than default = 0x0010
  __mt9v034_reg_write(mt9v034, MT9V034_RESERVED_REG2B, 0x0003); // rather than default = 0x0004
  __mt9v034_reg_write(mt9v034, MT9V034_RESERVED_REG2F, 0x0003); // rather than default = 0x0004

  int binning = 0;
  int width = 640;

  if (mt9v034->conf.format == PI_CAMERA_QVGA)
  {
    binning = 1;
    width = 320;
  }
  if (mt9v034->conf.format == PI_CAMERA_QQVGA)
  {
    binning = 2;
    width = 160;
  }

  __mt9v034_reg_write(mt9v034, MT9V034_READ_MODE_A,
    (binning << MT9V034_READ_MODE_ROW_BIN_SHIFT) |
    (binning << MT9V034_READ_MODE_COLUMN_BIN_SHIFT) |
    (mt9v034->conf.column_flip << MT9V034_READ_MODE_COLUMN_FLIP_SHIFT) |
    (mt9v034->conf.row_flip << MT9V034_READ_MODE_ROW_FLIP_SHIFT)
  );

  if (binning)
  {
    // !!! In binning mode, mt9v034 appears to provide pixels with difft timing -- requiring clock inversion !!
    __mt9v034_reg_write(mt9v034,  MT9V034_PIXEL_CLOCK, MT9V034_PIXEL_CLOCK_INV_PXL_CLK);

    // Correct Pixel Count in Histogram
    __mt9v034_reg_write(mt9v034,  MT9V034_HISTOGRAM_PIXCOUNT, MT9V034_HISTOGRAM_PIXCOUNT_DEF/((1+binning)*(1+binning)));
  }

  __mt9v034_reg_write(mt9v034, MT9V034_COLUMN_START_A, 56+1);     // first column index is 1
  __mt9v034_reg_write(mt9v034, MT9V034_WINDOW_WIDTH_A, 640);
  __mt9v034_reg_write(mt9v034, MT9V034_ROW_START_A, 4);         // CAUTION: First 4 rows are dark rows, not to be read out // !!! A VOIR ////
  __mt9v034_reg_write(mt9v034, MT9V034_WINDOW_HEIGHT_A, 480);


  // Adjust target brightness
  // TODO -- This could be offered as user-definable setting
  #define BRIGHTNESS_1_64     MT9V034_AEC_AGC_DESIRED_BIN_DEF     // keep at MT9V034_AEC_AGC_DESIRED_BIN_DEF=58 for default brightness (Min=1, Max=64)
  __mt9v034_reg_write(mt9v034,  MT9V034_AEC_AGC_DESIRED_BIN, BRIGHTNESS_1_64 );

  //Enable High Dynamic Range, this is very good for image flow
  __mt9v034_reg_write(mt9v034,  MT9V034_HDR, MT9V034_HDR_ON );

  //Set Black level control to auto
  __mt9v034_reg_write(mt9v034,  MT9V034_BLACK_LEVEL_CTRL, MT9V034_BLACK_LEVEL_AUTO );



  // Program Horizontal and Vertical Blanking accordingly:
  // Keep Frame Start Blanking and Frame End Blanking to their default values
  // Short row timing is not possible (see Table 5 Note 1 of DS rev D),
  // minimum total row time is 704pclk (hal width +blanking)
  // --> Decide to always keep total row time = 752+94 (max line width + default HBI) = 846pclk @26.66MHz -- keep same for 27MHz
  #define TOTAL_ROW_TIME 704 // was 846  // = max line width + default HBI
  __mt9v034_reg_write(mt9v034, MT9V034_HORIZONTAL_BLANKING_A, TOTAL_ROW_TIME - width);
  //__mt9v034_reg_write(mt9v034, MT9V034_VERTICAL_BLANKING_A, 1000);

  // -- Switch camera to suitable mode (GPIO_CIS_EXP assumed low, normally already done!]
  __mt9v034_reg_write(mt9v034, MT9V034_CHIP_CONTROL, MT9V034_CHIP_CONTROL_SNAPSHOT_MODE | MT9V034_CHIP_CONTROL_DVP_ON | MT9V034_CHIP_CONTROL_SIMULTANEOUS);


  return 0;
}


static void __mt9v034_on(mt9v034_t *mt9v034)
{
  // Enable 3V3A/3V3D
  pi_gpio_pin_write(&mt9v034->gpio_port, mt9v034->conf.power_gpio, 1);
}



static void __mt9v034_off(mt9v034_t *mt9v034)
{
  // Stop 3V3A/3V3D
  pi_gpio_pin_write(0, mt9v034->conf.power_gpio, 0);
}



static void __mt9v034_trigger_snapshot(mt9v034_t *mt9v034)
{
  // -- GPIO_CIS_EXP snapshot trigger signal
  pi_gpio_pin_write(&mt9v034->gpio_port, mt9v034->conf.trigger_gpio, 1);   // generate rising edge to trigger snapshot
  pi_gpio_pin_write(&mt9v034->gpio_port, mt9v034->conf.trigger_gpio, 0);   // generate rising edge to trigger snapshot
}



static int32_t __mt9v034_open(struct pi_device *device)
{
  struct pi_mt9v034_conf *conf = (struct pi_mt9v034_conf *)device->config;

  mt9v034_t *mt9v034 = (mt9v034_t *)pmsis_l2_malloc(sizeof(mt9v034_t));
  if (mt9v034 == NULL) return -1;

  memcpy(&mt9v034->conf, conf, sizeof(*conf));
  device->data = (void *)mt9v034;

  struct pi_cpi_conf cpi_conf;
  pi_cpi_conf_init(&cpi_conf);
  cpi_conf.itf = conf->cpi_itf;
  pi_open_from_conf(&mt9v034->cpi_device, &cpi_conf);

  if (pi_cpi_open(&mt9v034->cpi_device))
    goto error;

  struct pi_i2c_conf i2c_conf;
  pi_i2c_conf_init(&i2c_conf);
  i2c_conf.cs = 0x90;
  i2c_conf.itf = 1;
  pi_open_from_conf(&mt9v034->i2c_device, &i2c_conf);

  if (pi_i2c_open(&mt9v034->i2c_device))
    goto error2;

  struct pi_gpio_conf gpio_conf;
  pi_gpio_conf_init(&gpio_conf);

  pi_open_from_conf(&mt9v034->gpio_port, &gpio_conf);

  if (pi_gpio_open(&mt9v034->gpio_port))
    goto error3;

  pi_gpio_pin_configure(&mt9v034->gpio_port, conf->trigger_gpio, PI_GPIO_OUTPUT);

  pi_gpio_pin_configure(&mt9v034->gpio_port, conf->power_gpio, PI_GPIO_OUTPUT);

  if (__mt9v034_start(mt9v034) != 0)
    goto error4;

  if (bsp_mt9v034_open(conf))
    goto error4;

  pi_cpi_set_format(&mt9v034->cpi_device, PI_CPI_FORMAT_BYPASS_BIGEND);

  return 0;

error4:
  // TODO does not exist yet
  //pi_gpio_close(&mt9v034->gpio_port);
error3:
  pi_i2c_close(&mt9v034->i2c_device);
error2:
  pi_cpi_close(&mt9v034->cpi_device);
error:
  pmsis_l2_malloc_free(mt9v034, sizeof(mt9v034_t));
  return -1;
}



static void __mt9v034_close(struct pi_device *device)
{
  mt9v034_t *mt9v034 = (mt9v034_t *)device->data;
  pi_cpi_close(&mt9v034->cpi_device);
  pmsis_l2_malloc_free(mt9v034, sizeof(mt9v034_t));
}



static int32_t __mt9v034_control(struct pi_device *device, pi_camera_cmd_e cmd, void *arg)
{
  mt9v034_t *mt9v034 = (mt9v034_t *)device->data;

  switch (cmd)
  {
    case PI_CAMERA_CMD_ON:
      __mt9v034_on(mt9v034);
      break;

    case PI_CAMERA_CMD_OFF:
      __mt9v034_off(mt9v034);
      break;

    case PI_CAMERA_CMD_START:
      pi_cpi_control_start(&mt9v034->cpi_device);
      __mt9v034_trigger_snapshot(mt9v034);
      break;

    case PI_CAMERA_CMD_STOP:
      pi_cpi_control_stop(&mt9v034->cpi_device);
      break;

    default:
      break;
  }

  return 0;
}



static void __mt9v034_capture_async(struct pi_device *device, void *buffer, uint32_t bufferlen, pi_task_t *task)
{
  mt9v034_t *mt9v034 = (mt9v034_t *)device->data;

  pi_cpi_capture_async(&mt9v034->cpi_device, buffer, bufferlen, task);
}

static int32_t __mt9v034_reg_set(struct pi_device *device, uint32_t addr, uint8_t *value)
{
  mt9v034_t *mt9v034 = (mt9v034_t *)device->data;
  __mt9v034_reg_write(mt9v034, addr, *(uint16_t *)value);
  return 0;
}



static int32_t __mt9v034_reg_get(struct pi_device *device, uint32_t addr, uint8_t *value)
{
  mt9v034_t *mt9v034 = (mt9v034_t *)device->data;
  *(uint16_t *)value = __mt9v034_reg_read(mt9v034, addr);
  return 0;
}

static pi_camera_api_t MT9V034_api =
{
  .open           = &__mt9v034_open,
  .close          = &__mt9v034_close,
  .control        = &__mt9v034_control,
  .capture_async  = &__mt9v034_capture_async,
  .reg_set        = &__mt9v034_reg_set,
  .reg_get        = &__mt9v034_reg_get
};



void pi_mt9v034_conf_init(struct pi_mt9v034_conf *conf)
{
  conf->camera.api = &MT9V034_api;
  bsp_mt9v034_conf_init(conf);
  conf->format = PI_CAMERA_QVGA;
  conf->column_flip = 1;
  conf->row_flip = 0;
  conf->skip_pads_config = 0;
  __camera_conf_init(&conf->camera);
}
