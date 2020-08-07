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
 */

#include "pmsis.h"
#include "pmsis/rtos/os_frontend_api/pmsis_time.h"
#include "bsp/bsp.h"
#include "bsp/camera/ov5640.h"
#include "ov5640.h"

typedef struct {
  uint16_t addr;
  uint8_t value;
} i2c_req_t;


typedef struct {
  struct pi_ov5640_conf conf;
  struct pi_device cpi_device;
  struct pi_device i2c_device;

  struct pi_device gpio_port;

  i2c_req_t i2c_req;
  uint32_t i2c_read_value;

  int is_awake;
} ov5640_t;


typedef i2c_req_t ov5640_reg_init_t;


static ov5640_reg_init_t __ov5640_reg_init[] =
{
    {0x3008, 0x42}, // software power down, bit[6]
    {0x3103, 0x03}, // system clock from PLL, bit[1]
    {0x3017, 0xff}, // FREX, Vsync, HREF, PCLK, D[9:6] output enable
    {0x3018, 0xff}, // D[5:0], GPIO[1:0] output enable
    {0x3034, 0x18}, // MIPI 10-bit
    {0x3035, 0x41}, //0x41, // PLL
    {0x3036, 0x90}, // PLL
    {0x3037, 0x03}, //0x13, // PLL root divider, bit[4], PLL pre-divider, bit[3:0]
    {0x3108, 0x01}, // PCLK root divider, bit[5:4], SCLK2x root divider, bit[3:2], SCLK root divider, bit[1:0]
    {0x3630, 0x36},
    {0x3631, 0x0e},
    {0x3632, 0xe2},
    {0x3633, 0x12},
    {0x3621, 0xe0},
    {0x3704, 0xa0},
    {0x3703, 0x5a},
    {0x3715, 0x78},
    {0x3717, 0x01},
    {0x370b, 0x60},
    {0x3705, 0x1a},
    {0x3905, 0x02},
    {0x3906, 0x10},
    {0x3901, 0x0a},
    {0x3731, 0x12},
    {0x3600, 0x08}, // VCM control
    {0x3601, 0x33}, // VCM control
    {0x302d, 0x60}, // system control
    {0x3620, 0x52},
    {0x371b, 0x20},
    {0x471c, 0x50},
    {0x3a13, 0x43}, // pre-gain = 1.047x
    {0x3a18, 0x00}, // gain ceiling
    {0x3a19, 0xf8}, // gain ceiling = 15.5x
    {0x3635, 0x13},
    {0x3636, 0x03},
    {0x3634, 0x40},
    {0x3622, 0x01},
    {0x3c01, 0x34}, // Band auto, bit[7]
    {0x3c04, 0x28}, // threshold low sum
    {0x3c05, 0x98}, // threshold high sum
    {0x3c06, 0x00}, // light meter 1 threshold[15:8]
    {0x3c07, 0x07}, // light meter 1 threshold[7:0]
    {0x3c08, 0x00}, // light meter 2 threshold[15:8]
    {0x3c09, 0x1c}, // light meter 2 threshold[7:0]
    {0x3c0a, 0x9c}, // sample number[15:8]
    {0x3c0b, 0x40}, // sample number[7:0]
    {0x3810, 0x00}, // Timing Hoffset[11:8]
    {0x3811, 0x10}, // Timing Hoffset[7:0]
    {0x3812, 0x00}, // Timing Voffset[10:8]
    {0x3708, 0x64},
    {0x4001, 0x02}, // BLC start from line 2
    {0x4005, 0x1a}, // BLC always update
    {0x3000, 0x00}, // enable blocks
    {0x3004, 0xff}, // enable clocks
    {0x300e, 0x58}, // MIPI power down, DVP enable
    {0x302e, 0x00},
    //{0x4300, 0x23}, // rgb888
    {0x4300, 0x61}, // rgb565
    {0x501F, 0x01},
    {0x3820, 0x40}, // flip
    {0x3821, 0x01}, // mirror
    {0x3814, 0x71}, // timing X inc
    {0x3815, 0x35}, // timing Y inc
    {0x3800, 0x00}, // HS
    {0x3801, 0x00}, // HS
    {0x3802, 0x00}, // VS
    {0x3803, 0x00}, // VS
    {0x3804, 0x0a}, // HW (HE)
    {0x3805, 0x3f}, // HW (HE)
    {0x3806, 0x07}, // VH (VE)
    {0x3807, 0x9f}, // VH (VE)
    {0x3808, (320 >> 8)}, // DVPHO
    {0x3809, (320 & 0xff)}, // DVPHO
    {0x380a, (260 >> 8)}, // DVPVO
    {0x380b, (260 & 0xff)}, // DVPVO
    {0x380c, 0x07}, // HTS
    {0x380d, 0x58}, // HTS
    {0x380e, 0x01}, // VTS
    {0x380f, 0xf0}, // VTS
    {0x3810, 0x00}, // HTS
    {0x3811, 0x08}, // HTS
    {0x3812, 0x00}, // VTS
    {0x3813, 0x02}, // VTS
    {0x3618, 0x00},
    {0x3612, 0x29},
    {0x3709, 0x52},
    {0x370c, 0x03},
    {0x3a02, 0x02}, // 60Hz max exposure
    {0x3a03, 0xe0}, // 60Hz max exposure
    {0x3a14, 0x02}, // 50Hz max exposure
    {0x3a15, 0xe0}, // 50Hz max exposure
    {0x4004, 0x02}, // BLC line number
    {0x3002, 0x1c}, // reset JFIFO, SFIFO, JPG
    {0x3006, 0xc3}, // disable clock of JPEG2x, JPEG
    {0x4713, 0x03}, // JPEG mode 3
    {0x4407, 0x04}, // Quantization scale
    {0x460b, 0x37},
    {0x460c, 0x20},
    {0x4837, 0x16}, // MIPI global timing
    {0x3824, 0x04}, // PCLK manual divider
    {0x5001, 0xA3}, // SDE on, scale on, UV average off, color matrix on, AWB on
    {0x3503, 0x00}, // AEC/AGC on
    {0x440e, 0x00},
    {0x5000, 0xa7}, // Lenc on, raw gamma on, BPC on, WPC on, CIP on
    {0x3a0f, 0x30}, // stable range in high
    {0x3a10, 0x28}, // stable range in low
    {0x3a1b, 0x30}, // stable range out high
    {0x3a1e, 0x26}, // stable range out low
    {0x3a11, 0x60}, // fast zone high
    {0x3a1f, 0x14}, // fast zone low
    {0x5800, 0x23},
    {0x5801, 0x14},
    {0x5802, 0x0f},
    {0x5803, 0x0f},
    {0x5804, 0x12},
    {0x5805, 0x26},
    {0x5806, 0x0c},
    {0x5807, 0x08},
    {0x5808, 0x05},
    {0x5809, 0x05},
    {0x580a, 0x08},
    {0x580b, 0x0d},
    {0x580c, 0x08},
    {0x580d, 0x03},
    {0x580e, 0x00},
    {0x580f, 0x00},
    {0x5810, 0x03},
    {0x5811, 0x09},
    {0x5812, 0x07},
    {0x5813, 0x03},
    {0x5814, 0x00},
    {0x5815, 0x01},
    {0x5816, 0x03},
    {0x5817, 0x08},
    {0x5818, 0x0d},
    {0x5819, 0x08},
    {0x581a, 0x05},
    {0x581b, 0x06},
    {0x581c, 0x08},
    {0x581d, 0x0e},
    {0x581e, 0x29},
    {0x581f, 0x17},
    {0x5820, 0x11},
    {0x5821, 0x11},
    {0x5822, 0x15},
    {0x5823, 0x28},
    {0x5824, 0x46},
    {0x5825, 0x26},
    {0x5826, 0x08},
    {0x5827, 0x26},
    {0x5828, 0x64},
    {0x5829, 0x26},
    {0x582a, 0x24},
    {0x582b, 0x22},
    {0x582c, 0x24},
    {0x582d, 0x24},
    {0x582e, 0x06},
    {0x582f, 0x22},
    {0x5830, 0x40},
    {0x5831, 0x42},
    {0x5832, 0x24},
    {0x5833, 0x26},
    {0x5834, 0x24},
    {0x5835, 0x22},
    {0x5836, 0x22},
    {0x5837, 0x26},
    {0x5838, 0x44},
    {0x5839, 0x24},
    {0x583a, 0x26},
    {0x583b, 0x28},
    {0x583c, 0x42},
    {0x583d, 0xce}, // lenc BR offset
    {0x5180, 0xff}, // AWB B block
    {0x5181, 0xf2}, // AWB control
    {0x5182, 0x00}, // [7:4] max local counter, [3:0] max fast counter
    {0x5183, 0x14}, // AWB advanced
    {0x5184, 0x25},
    {0x5185, 0x24},
    {0x5186, 0x09},
    {0x5187, 0x09},
    {0x5188, 0x09},
    {0x5189, 0x75},
    {0x518a, 0x54},
    {0x518b, 0xe0},
    {0x518c, 0xb2},
    {0x518d, 0x42},
    {0x518e, 0x3d},
    {0x518f, 0x56},
    {0x5190, 0x46},
    {0x5191, 0xf8}, // AWB top limit
    {0x5192, 0x04}, // AWB bottom limit
    {0x5193, 0x70}, // red limit
    {0x5194, 0xf0}, // green limit
    {0x5195, 0xf0}, // blue limit
    {0x5196, 0x03}, // AWB control
    {0x5197, 0x01}, // local limit
    {0x5198, 0x04},
    {0x5199, 0x12},
    {0x519a, 0x04},
    {0x519b, 0x00},
    {0x519c, 0x06},
    {0x519d, 0x82},
    {0x519e, 0x38}, // AWB control
    {0x5480, 0x01}, // Gamma bias plus on, bit[0]
    {0x5481, 0x08},
    {0x5482, 0x14},
    {0x5483, 0x28},
    {0x5484, 0x51},
    {0x5485, 0x65},
    {0x5486, 0x71},
    {0x5487, 0x7d},
    {0x5488, 0x87},
    {0x5489, 0x91},
    {0x548a, 0x9a},
    {0x548b, 0xaa},
    {0x548c, 0xb8},
    {0x548d, 0xcd},
    {0x548e, 0xdd},
    {0x548f, 0xea},
    {0x5490, 0x1d},
    {0x5381, 0x1e}, // CMX1 for Y
    {0x5382, 0x5b}, // CMX2 for Y
    {0x5383, 0x08}, // CMX3 for Y
    {0x5384, 0x0a}, // CMX4 for U
    {0x5385, 0x7e}, // CMX5 for U
    {0x5386, 0x88}, // CMX6 for U
    {0x5387, 0x7c}, // CMX7 for V
    {0x5388, 0x6c}, // CMX8 for V
    {0x5389, 0x10}, // CMX9 for V
    {0x538a, 0x01}, // sign[9]
    {0x538b, 0x98}, // sign[8:1]
    {0x5580, 0x06}, // saturation on, bit[1]
    {0x5583, 0x40},
    {0x5584, 0x10},
    {0x5589, 0x10},
    {0x558a, 0x00},
    {0x558b, 0xf8},
    {0x501d, 0x40}, // enable manual offset of contrast
    {0x5300, 0x08}, // CIP sharpen MT threshold 1
    {0x5301, 0x30}, // CIP sharpen MT threshold 2
    {0x5302, 0x10}, // CIP sharpen MT offset 1
    {0x5303, 0x00}, // CIP sharpen MT offset 2
    {0x5304, 0x08}, // CIP DNS threshold 1
    {0x5305, 0x30}, // CIP DNS threshold 2
    {0x5306, 0x08}, // CIP DNS offset 1
    {0x5307, 0x16}, // CIP DNS offset 2
    {0x5309, 0x08}, // CIP sharpen TH threshold 1
    {0x530a, 0x30}, // CIP sharpen TH threshold 2
    {0x530b, 0x04}, // CIP sharpen TH offset 1
    {0x530c, 0x06}, // CIP sharpen TH offset 2
    {0x5025, 0x00},
    {0x3008, 0x02}, // wake up from standby, bit[6]
    {0x4740, 0x21}, //VSYNC active HIGH

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



static void __ov5640_reg_write(ov5640_t *ov5640, uint16_t addr, uint8_t value)
{
  if (is_i2c_active())
  {
    ov5640->i2c_req.value = value;
    ov5640->i2c_req.addr = ((addr >> 8) & 0xff) | ((addr & 0xff) << 8);
    pi_i2c_write(&ov5640->i2c_device, (uint8_t *)&ov5640->i2c_req, 3, PI_I2C_XFER_STOP);
  }
}



static uint8_t __ov5640_reg_read(ov5640_t *ov5640, uint16_t addr)
{
  if (is_i2c_active())
  {
    ov5640->i2c_req.addr = ((addr >> 8) & 0xff) | ((addr & 0xff) << 8);
    pi_i2c_write(&ov5640->i2c_device, (uint8_t *)&ov5640->i2c_req.addr, 2, PI_I2C_XFER_STOP);
    pi_i2c_read(&ov5640->i2c_device, (uint8_t *)&ov5640->i2c_read_value, 1, PI_I2C_XFER_STOP);
    return *(volatile uint8_t *)&ov5640->i2c_read_value;
  }
  else
  {
    return 0;
  }
}


static void __ov5640_init_regs(ov5640_t *ov5640)
{
    int32_t i;
    for(i=0; i<(int32_t)(sizeof(__ov5640_reg_init)/sizeof(ov5640_reg_init_t)); i++)
    {
        __ov5640_reg_write(ov5640, __ov5640_reg_init[i].addr, __ov5640_reg_init[i].value);
    }
#ifdef DEBUG
    uint8_t reg_value = 0;
    for(i=0; i<(sizeof(__ov5640_reg_init)/sizeof(ov5640_reg_init_t)); i++)
    {
        reg_value = __ov5640_reg_read(ov5640, __ov5640_reg_init[i].addr);
        if (reg_value != __ov5640_reg_init[i].value)
            printf("error reg: @%X = %X (expected: %X)\n", __ov5640_reg_init[i].addr, reg_value, __ov5640_reg_init[i].value);
    }
#endif
}



static void __ov5640_reset(ov5640_t *ov5640)
{
    __ov5640_reg_write(ov5640, 0x3103, 0x11);
    //HW reset: pull the RESETB pin to 0
    // SW reset
    __ov5640_reg_write(ov5640, 0x3008, 0x82);
    // Datasheet: Wait 1ms after reset
    pi_time_wait_us(10000);
#ifdef CUSTOM_BSP
    pi_gpio_pin_write(&ov5640->gpio_port, ov5640->conf.reset_gpio, 1);
#endif
}


static void __ov5640_mode(ov5640_t *ov5640, uint8_t mode)
{
    uint8_t value=0;
    value = __ov5640_reg_read(ov5640, 0x3008);
    if (mode == OV5640_STREAMING)
        value = 0x02;
    else if (mode == OV5640_STANDBY)
        value |= 0x42; // SW PWDN

    __ov5640_reg_write(ov5640, 0x3008, value);
}



static void __ov5640_wakeup(ov5640_t *ov5640)
{
  if (!ov5640->is_awake)
  {
#ifdef CUSTOM_BSP
    pi_gpio_pin_write(&ov5640->gpio_port, ov5640->conf.pwdn_gpio, 0);
    pi_time_wait_us(10000);
    pi_gpio_pin_write(&ov5640->gpio_port, ov5640->conf.reset_gpio, 1);
    pi_time_wait_us(10000);
#endif

    __ov5640_mode(ov5640, OV5640_STREAMING);
    ov5640->is_awake = 1;
  }
}



static void __ov5640_standby(ov5640_t *ov5640)
{
  if (ov5640->is_awake)
  {
    __ov5640_mode(ov5640, OV5640_STANDBY);

#ifdef CUSTOM_BSP
    pi_gpio_pin_write(&ov5640->gpio_port, ov5640->conf.pwdn_gpio, 1);
#endif

    ov5640->is_awake = 0;
  }
}



int32_t __ov5640_open(struct pi_device *device)
{
    struct pi_ov5640_conf *conf = (struct pi_ov5640_conf *)device->config;

    ov5640_t *ov5640 = (ov5640_t *)pmsis_l2_malloc(sizeof(ov5640_t));
    if (ov5640 == NULL) return -1;

    device->data = (void *)ov5640;

    if (bsp_ov5640_open(conf))
        goto error;

#ifdef CUSTOM_BSP
    struct pi_gpio_conf gpio_conf;
    pi_gpio_conf_init(&gpio_conf);
    pi_open_from_conf(&ov5640->gpio_port, &gpio_conf);
    if (pi_gpio_open(&ov5640->gpio_port))
        goto error;

    pi_gpio_pin_configure(&ov5640->gpio_port, conf->reset_gpio, PI_GPIO_OUTPUT);
    pi_gpio_pin_configure(&ov5640->gpio_port, conf->pwdn_gpio, PI_GPIO_OUTPUT);

    pi_gpio_pin_write(&ov5640->gpio_port, conf->pwdn_gpio, 0);
    pi_time_wait_us(1000);
    pi_gpio_pin_write(&ov5640->gpio_port, ov5640->conf.reset_gpio, 0);
    pi_time_wait_us(20000);
#endif

    struct pi_cpi_conf cpi_conf;
    pi_cpi_conf_init(&cpi_conf);
    cpi_conf.itf = conf->cpi_itf;
    pi_open_from_conf(&ov5640->cpi_device, &cpi_conf);

    if (pi_cpi_open(&ov5640->cpi_device))
        goto error;

    struct pi_i2c_conf i2c_conf;
    pi_i2c_conf_init(&i2c_conf);
    i2c_conf.cs = 0x78;
    i2c_conf.itf = conf->i2c_itf;
    i2c_conf.max_baudrate = 400000;
    pi_open_from_conf(&ov5640->i2c_device, &i2c_conf);

    if (pi_i2c_open(&ov5640->i2c_device))
        goto error2;

    pi_cpi_set_format(&ov5640->cpi_device, PI_CPI_FORMAT_BYPASS_LITEND);

    uint8_t cam_id[2];
    cam_id[0] = __ov5640_reg_read(ov5640, 0x300A);
    cam_id[1] = __ov5640_reg_read(ov5640, 0x300B);

    if(cam_id[0] != 0x56 || cam_id[1] != 0x40)
    {
        printf("camera open failed: cam_id 0x%X%X is wrong\n", cam_id[1], cam_id[0] );
        goto error2;
    }

    __ov5640_reset(ov5640);
    __ov5640_init_regs(ov5640);

    return 0;

error2:
    pi_cpi_close(&ov5640->cpi_device);
error:
    pmsis_l2_malloc_free(ov5640, sizeof(ov5640_t));
    return -1;
}



static void __ov5640_close(struct pi_device *device)
{
    ov5640_t *ov5640 = (ov5640_t *)device->data;
    __ov5640_standby(ov5640);
    pi_cpi_close(&ov5640->cpi_device);
    pmsis_l2_malloc_free(ov5640, sizeof(ov5640_t));
}



static int32_t __ov5640_control(struct pi_device *device, pi_camera_cmd_e cmd, void *arg)
{
    int irq = disable_irq();

    ov5640_t *ov5640 = (ov5640_t *)device->data;

    switch (cmd)
    {
        case PI_CAMERA_CMD_ON:
            __ov5640_wakeup(ov5640);
            break;

        case PI_CAMERA_CMD_OFF:
            __ov5640_standby(ov5640);
            break;

        case PI_CAMERA_CMD_START:
            pi_cpi_control_start(&ov5640->cpi_device);
            break;

        case PI_CAMERA_CMD_STOP:
            pi_cpi_control_stop(&ov5640->cpi_device);
            break;

        default:
            break;
    }

    restore_irq(irq);

    return 0;
}

void __ov5640_capture_async(struct pi_device *device, void *buffer, uint32_t bufferlen, pi_task_t *task)
{
    ov5640_t *ov5640 = (ov5640_t *)device->data;

	pi_cpi_capture_async(&ov5640->cpi_device, buffer, bufferlen, task);
}



int32_t __ov5640_reg_set(struct pi_device *device, uint32_t addr, uint8_t *value)
{
    ov5640_t *ov5640 = (ov5640_t *)device->data;
    __ov5640_reg_write(ov5640, addr, *value);
    return 0;
}



int32_t __ov5640_reg_get(struct pi_device *device, uint32_t addr, uint8_t *value)
{
    ov5640_t *ov5640 = (ov5640_t *)device->data;
    *value = __ov5640_reg_read(ov5640, addr);
    return 0;
}



static pi_camera_api_t ov5640_api =
{
    .open           = &__ov5640_open,
    .close          = &__ov5640_close,
    .control        = &__ov5640_control,
    .capture_async  = &__ov5640_capture_async,
    .reg_set        = &__ov5640_reg_set,
    .reg_get        = &__ov5640_reg_get
};



void pi_ov5640_conf_init(struct pi_ov5640_conf *conf)
{
    conf->camera.api = &ov5640_api;
    conf->skip_pads_config = 0;
    bsp_ov5640_conf_init(conf);
    __camera_conf_init(&conf->camera);
}
