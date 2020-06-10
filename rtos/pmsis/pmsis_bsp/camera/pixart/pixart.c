/*
 * Copyright (C) 2020 GreenWaves Technologies
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
#include "bsp/camera/pixart.h"
#include "pixart.h"


typedef struct {
  uint32_t addr;
  uint32_t value;
} spi_req_t;

typedef struct {
  struct pi_pixart_conf conf;
  struct pi_device cpi_device;
  struct pi_device spi_device;
  struct pi_device pwm_device;
  struct pi_device gpio_port;
  spi_req_t spi_req;
} pixart_t;

typedef struct {
  uint8_t addr;
  uint8_t data;
} pixart_reg_init_t;

static pixart_reg_init_t __pixart_reg_init[] =
{
  {0x7F, 0x00}, // bank select 0
  {0x0C, 0x1D}, // soft reset
  {0x7F, 0x00}, // bank select 0
  //{0x07, 0x00}, // Hsynclnv/Vsynclnv/PxClklnv/AAvg_V/AAvg_H/VFlip/HFlip
  {0x07, 0x80}, // Hsynclnv/Vsynclnv/PxClklnv/AAvg_V/AAvg_H/VFlip/HFlip
  {0x08, 0x03}, // Askip_V/Askip_H
  {0x0B, 0x0E},
  {0x0D, 0xC5}, // BLC/R_ABC_startline/R_ABC_En
  {0x0F, 0x32}, // BINV_UB
  //{0x11, 0x41}, // R_fgh
  {0x11, 0x61}, // R_fgh
  {0x12, 0xBD}, // R_ggh
  {0x13, 0xD2},
  {0x14, 0xFE},
  {0x15, 0x00},
  {0x17, 0x02},
  {0x1A, 0x07},
  {0x1B, 0x08},
  {0x20, 0x00},
  {0x25, 0x78},
  {0x29, 0x28},
  {0x2B, 0x06},
  {0x2F, 0x0E},
  {0x30, 0x0E},
  {0x34, 0x0F},
  {0x35, 0x0F},
  {0x3A, 0x28},
  {0x45, 0x17},
  {0x46, 0x17},
  //{0x48, 0x10}, // Expo[7:0]
  //{0x49, 0x27}, // Expo[15:8]
  //{0x4A, 0x00}, // Expo[23:16]
  {0x48, 0x10}, // Expo[7:0]
  {0x49, 0x51}, // Expo[15:8]
  {0x4A, 0x00}, // Expo[23:16]
  {0x4D, 0x0D},
  {0x4E, 0x20},
  {0x62, 0x12},
  {0x64, 0x02},
  {0x67, 0x0A},
  {0x69, 0x0A},
  {0x6C, 0x0B},
  {0x6E, 0x0B},
  {0x71, 0x0A},
  {0x73, 0x1D},
  {0x75, 0x1E},
  {0x77, 0x0B},
  {0x7F, 0x01}, // Bank select 1
  {0x01, 0x14},
  {0x02, 0x02},
  {0x04, 0x96},
  {0x05, 0x03},
  {0x06, 0x46},
  {0x0D, 0x9F}, // BLACINV Control 1/T_BLACINV_EnH/
  {0x0E, 0x11}, // BLACINV Control 2/T_SIG_REF
  {0x0F, 0x48},
  {0x10, 0x10},
  {0x11, 0x00},
  {0x12, 0x05}, // T_GPIO_OPDRV/T_HSYNC_OPDRV/T_VSYNC_OPDRV/T_PXD_OPDRV
  {0x15, 0x00},
  {0x16, 0x01},
  {0x17, 0x67},
  {0x18, 0xD0},
  {0x21, 0x14},
  {0x22, 0x80},
  {0x23, 0x80}, // Mode select trigger/exit low power mode
  {0x2F, 0x30},
  {0x35, 0x64},
  {0x39, 0x03},
  {0x3A, 0x03},
  //{0x45, 0x02},
  {0x46, 0x06}, // BLC Function 2/R_ABC_SampleSize
  //{0x4A, 0x01}, // Vsync_front_delay[7:0]
  //{0x4B, 0x10}, // Vsync_rear_delay[7:0]
  {0x4A, 0xFF}, // Vsync_front_delay[7:0]
  {0x4B, 0x00}, // Vsync_rear_delay[7:0]
  {0x00, 0x01}, // Update
};


static inline int is_spi_active(void)
{
#if defined(ARCHI_PLATFORM_RTL)

  // SPI driver is not yet working on some chips, at least this works on gvsoc.
  // Also there is noSPI connection to camera model on RTL
#if PULP_CHIP == CHIP_GAP9 || PULP_CHIP == CHIP_VEGA || PULP_CHIP == CHIP_ARNOLD || PULP_CHIP == CHIP_PULPISSIMO || PULP_CHIP == CHIP_PULPISSIMO_V1
  return 0;
#else
  return rt_platform() != ARCHI_PLATFORM_RTL;
#endif

#else

  return 1;

#endif
}

static void __pixart_reg_write(pixart_t *pixart, uint8_t addr, uint8_t value)
{
  if (is_spi_active())
  {
    /* wtrite command must keep MSB 0. */
    pixart->spi_req.addr = (addr & 0x7F);
    pixart->spi_req.value = value;
    pi_spi_send(&pixart->spi_device, (uint8_t *)&pixart->spi_req.addr, 8, PI_SPI_CS_KEEP);
    pi_spi_send(&pixart->spi_device, (uint8_t *)&pixart->spi_req.value, 8, PI_SPI_CS_AUTO);
  }
}

static uint8_t __pixart_reg_read(pixart_t *pixart, uint8_t addr)
{
  if (is_spi_active())
  {
    /* read command must keep MSB 1. */
    pixart->spi_req.addr = (addr | 0x80);
    pi_spi_send(&pixart->spi_device, (uint8_t *)&pixart->spi_req.addr, 8, PI_SPI_CS_KEEP);
    pi_spi_receive(&pixart->spi_device, (uint8_t *)&pixart->spi_req.value, 8, PI_SPI_CS_AUTO);
    return pixart->spi_req.value;
  }

  return 0;
}

static void __pixart_regs_init(pixart_t *pixart)
{
  for (uint8_t i = 0; i < (sizeof(__pixart_reg_init) / sizeof(pixart_reg_init_t)); i++)
  {
    __pixart_reg_write(pixart, __pixart_reg_init[i].addr, __pixart_reg_init[i].data);
  }

  /* wait for time to ensure init success. */
  pi_time_wait_us(1000);
}

static void __pixart_on(pixart_t *pixart)
{
  /* enable 4V/2V5 to power on camera. */
  pi_gpio_pin_write(&pixart->gpio_port, pixart->conf.gpio_ctl.gpio_power_4V, 1);
  pi_gpio_pin_write(&pixart->gpio_port, pixart->conf.gpio_ctl.gpio_power_2V5, 1);
}

static void __pixart_off(pixart_t *pixart)
{
  /* close 4V/2V5 to power off camera. */
  pi_gpio_pin_write(&pixart->gpio_port, pixart->conf.gpio_ctl.gpio_power_4V, 0);
  pi_gpio_pin_write(&pixart->gpio_port, pixart->conf.gpio_ctl.gpio_power_2V5, 0);
}

static void __pixart_mode(pixart_t *pixart, uint8_t mode)
{
  __pixart_reg_write(pixart, PIXART_BANK_SELECT_REG, 0x01);
  __pixart_reg_write(pixart, PIXART_SWITCH_MODE_CTL1_REG, (mode != PIXART_TRIGGER_MODE) ? 0x00 : 0x80);
  __pixart_reg_write(pixart, PIXART_UPDATE_REG, 0x01);
}

static int __pixart_start(pixart_t *pixart)
{
  /* poweron camera. */
  __pixart_on(pixart);

  /* wait time > 500us to complete poweron process. */
  pi_time_wait_us(1000);

  /* reset camera from pad. */
  //pi_gpio_pin_write(&pixart->gpio_port, pixart->conf.gpio_ctl.gpio_reset, 1);

  /* wait time > 500us to complete reset process. */
  //pi_time_wait_us(1000);

  uint16_t id;
  do
  {
    /* soft reset pixart. */
    __pixart_reg_write(pixart, PIXART_GLOBAL_RESET_REG, PIXART_SOFT_RESET);

    pi_time_wait_us(100);

    id = ((__pixart_reg_read(pixart, PIXART_CHIP_ID_REG_H) << 8) | __pixart_reg_read(pixart, PIXART_CHIP_ID_REG_L));

  } while (id == 0xFFFF); //while (id != PIXART_CHIP_ID);

  /* init register for pixart and then pixart is ready. */
  __pixart_regs_init(pixart);

  return 0;
}

static void __pixart_trigger_snapshot(pixart_t *pixart)
{
  /* in trigger mode to start run. */
  __pixart_reg_write(pixart, PIXART_SWITCH_MODE_CTL2_REG, 0x8C);
  __pixart_reg_write(pixart, PIXART_SWITCH_MODE_CTL2_REG, 0x0C);
}

static int32_t __pixart_open(struct pi_device *device)
{
  struct pi_pixart_conf *conf = (struct pi_pixart_conf *)device->config;

  /* init bsp gpio. */
  bsp_pixart_open(conf);

  pixart_t *pixart = (pixart_t *)pmsis_l2_malloc(sizeof(pixart_t));
  if (pixart == NULL)
    return -1;

  memcpy(&pixart->conf, conf, sizeof(*conf));
  device->data = (void *)pixart;

  /* configure cpi interface. */
  struct pi_cpi_conf cpi_conf;
  pi_cpi_conf_init(&cpi_conf);
  cpi_conf.itf = conf->cpi_itf;
  pi_open_from_conf(&pixart->cpi_device, &cpi_conf);
  if (pi_cpi_open(&pixart->cpi_device))
    goto error;

  pi_cpi_set_format(&pixart->cpi_device, PI_CPI_FORMAT_BYPASS_BIGEND);

  /* configure spi interface to use for command. */
  struct pi_spi_conf spi_conf;
  pi_spi_conf_init(&spi_conf);
  spi_conf.itf = conf->spi_itf;
  spi_conf.cs = conf->spi_cs;
  spi_conf.wordsize = PI_SPI_WORDSIZE_8;
  spi_conf.big_endian = 1;
  spi_conf.max_baudrate = PIXART_SPI_BAUDRATE;
  spi_conf.polarity = 0;
  spi_conf.phase = 0;
  pi_open_from_conf(&pixart->spi_device, &spi_conf);
  if (pi_spi_open(&pixart->spi_device))
    goto error2;

  /* configure pwm to support clk to camera. */
  struct pi_pwm_conf pwm_conf;
  pi_pwm_conf_init(&pwm_conf);
  pwm_conf.pwm_id = conf->pwm_id;
  pwm_conf.ch_id = conf->pwm_channel;
  pwm_conf.timer_conf &= ~PI_PWM_CLKSEL_REFCLK_32K;
  pwm_conf.timer_conf |= PI_PWM_CLKSEL_FLL;
  pi_open_from_conf(&pixart->pwm_device, &pwm_conf);
  if (pi_pwm_open(&pixart->pwm_device))
    goto error3;

  pi_pwm_duty_cycle_set(&pixart->pwm_device, PIXART_MCLK_FREQ, 50);
  pi_pwm_timer_start(&pixart->pwm_device);

  /* configure gpio to control power and reset functions. */
  struct pi_gpio_conf gpio_conf;
  pi_gpio_conf_init(&gpio_conf);
  pi_open_from_conf(&pixart->gpio_port, &gpio_conf);
  if (pi_gpio_open(&pixart->gpio_port))
    goto error4;

  pi_gpio_pin_configure(&pixart->gpio_port, conf->gpio_ctl.gpio_power_4V, PI_GPIO_OUTPUT | PI_GPIO_PULL_DISABLE);
  pi_gpio_pin_configure(&pixart->gpio_port, conf->gpio_ctl.gpio_power_2V5, PI_GPIO_OUTPUT | PI_GPIO_PULL_DISABLE);
  //pi_gpio_pin_configure(&pixart->gpio_port, conf->gpio_ctl.gpio_reset, PI_GPIO_OUTPUT | PI_GPIO_PULL_DISABLE);
  __pixart_off(pixart);
  //pi_gpio_pin_write(&pixart->gpio_port, pixart->conf.gpio_ctl.gpio_reset, 0);

  /* init camera. */
  if (__pixart_start(pixart) != 0)
    goto error5;

  return 0;

error5:
  // TODO does not exist yet
  //pi_gpio_close(&pixart->gpio_port);
error4:
  pi_pwm_close(&pixart->pwm_device);
error3:
  pi_spi_close(&pixart->spi_device);
error2:
  pi_cpi_close(&pixart->cpi_device);
error:
  pmsis_l2_malloc_free(pixart, sizeof(pixart_t));

  return -1;
}

static void __pixart_close(struct pi_device *device)
{
  /* close periph and free memery. */
  pixart_t *pixart = (pixart_t *)device->data;
  pi_pwm_close(&pixart->pwm_device);
  pi_spi_close(&pixart->spi_device);
  pi_cpi_close(&pixart->cpi_device);
  pmsis_l2_malloc_free(pixart, sizeof(pixart_t));
}

static int32_t __pixart_control(struct pi_device *device, pi_camera_cmd_e cmd, void *arg)
{
  pixart_t *pixart = (pixart_t *)device->data;

  switch (cmd)
  {
    case PI_CAMERA_CMD_ON:
      __pixart_on(pixart);
      break;

    case PI_CAMERA_CMD_OFF:
      __pixart_off(pixart);
      break;

    case PI_CAMERA_CMD_START:
      pi_cpi_control_start(&pixart->cpi_device);
      break;

    case PI_CAMERA_CMD_STOP:
      /* switch from contiune mode to trigger mode(or stop output datastream). */
      __pixart_mode(pixart, PIXART_TRIGGER_MODE);
      pi_cpi_control_stop(&pixart->cpi_device);
      break;

    default:
      break;
  }

  return 0;
}

static void __pixart_capture_async(struct pi_device *device, void *buffer, uint32_t bufferlen, pi_task_t *task)
{
  pixart_t *pixart = (pixart_t *)device->data;

  pi_cpi_capture_async(&pixart->cpi_device, buffer, bufferlen, task);

  /* switch to continue mode and start output datastream. */
  __pixart_mode(pixart, PIXART_CONTINUE_MODE);
}

static int32_t __pixart_reg_set(struct pi_device *device, uint32_t addr, uint8_t *value)
{
  pixart_t *pixart = (pixart_t *)device->data;
  __pixart_reg_write(pixart, addr, *(uint16_t *)value);
  return 0;
}

static int32_t __pixart_reg_get(struct pi_device *device, uint32_t addr, uint8_t *value)
{
  pixart_t *pixart = (pixart_t *)device->data;
  *(uint16_t *)value = __pixart_reg_read(pixart, addr);
  return 0;
}

static pi_camera_api_t pixart_api =
{
  .open           = &__pixart_open,
  .close          = &__pixart_close,
  .control        = &__pixart_control,
  .capture_async  = &__pixart_capture_async,
  .reg_set        = &__pixart_reg_set,
  .reg_get        = &__pixart_reg_get
};

void pi_pixart_conf_init(struct pi_pixart_conf *conf)
{
  conf->camera.api = &pixart_api;
  bsp_pixart_conf_init(conf);
  conf->format = PI_CAMERA_QVGA;
  conf->skip_pads_config = 0;
  __camera_conf_init(&conf->camera);
}
