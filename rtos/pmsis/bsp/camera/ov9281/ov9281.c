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
#include "bsp/camera/ov9281.h"
#include "ov9281.h"

typedef struct {
    uint16_t addr;
    uint8_t value;
} i2c_req_t;


typedef struct {
    struct pi_ov9281_conf conf;
    struct pi_device csi2_device;
    struct pi_device i2c_device;

    struct pi_device gpio_port;

    i2c_req_t i2c_req;
    uint32_t i2c_read_value;

    int is_awake;
} ov9281_t;


typedef i2c_req_t ov9281_reg_init_t;



/*
 * Xclk 24Mhz
 * 150fps = 800 Mbps * 1 lane / (728*915*8)
 * mipi_datarate per lane 800Mbps
 * Method: 4:1 subsampling
 */
static ov9281_reg_init_t  ov9281_640x400[] = {
    {0x0103, 0x01},
    {0x0302, 0x32}, // PLL1_mul
    {0x030d, 0x50}, // FLL2_mul
    {0x030e, 0x02}, // FLL2_sys_div
    {0x3001, 0x00},
    {0x3004, 0x00},
    {0x3005, 0x00},
    {0x3006, 0x04},
    {0x3011, 0x0a},
    {0x3013, 0x18},
    {0x3022, 0x01},
    {0x3023, 0x00},
    {0x302c, 0x00},
    {0x302f, 0x00},
    {0x3030, 0x10}, // 04
    {0x3039, 0x32},
    {0x303a, 0x00},
    {0x303f, 0x01},
    {0x3500, 0x00},
    {0x3501, 0x2a},
    {0x3502, 0x90},
    {0x3503, 0x08},
    {0x3505, 0x8c},
    {0x3507, 0x03},
    {0x3508, 0x00},
    {0x3509, 0x10},
    {0x3610, 0x80},
    {0x3611, 0xa0},
    {0x3620, 0x6f},
    {0x3632, 0x56},
    {0x3633, 0x78},
    {0x3662, 0x03}, //1lane RAW10: 0x01, 1lane RAW8: 0x03, 2lane RAW10: 0x05
    {0x3666, 0x00},
    {0x366f, 0x5a},
    {0x3680, 0x84},
    {0x3712, 0x80},
    {0x372d, 0x22},
    {0x3731, 0x80},
    {0x3732, 0x30},
    {0x3778, 0x00},
    {0x377d, 0x22},
    {0x3788, 0x02},
    {0x3789, 0xa4},
    {0x378a, 0x00},
    {0x378b, 0x4a},
    {0x3799, 0x20},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x00},
    {0x3804, 0x05},
    {0x3805, 0x0f},
    {0x3806, 0x03},
    {0x3807, 0x2f},

    {0x3808, 0x02}, // 640
    {0x3809, 0x80},
    {0x380a, 0x01}, // 400
    {0x380b, 0x90},

    {0x380c, 0x02},
    {0x380d, 0xd8},
    {0x380e, 0x03},
    {0x380f, 0x93},
    {0x3810, 0x00},
    {0x3811, 0x04},
    {0x3812, 0x00},
    {0x3813, 0x04},
    {0x3814, 0x31},
    {0x3815, 0x22},
    {0x3820, 0x60},
    {0x3821, 0x01},
    {0x3881, 0x42},
    {0x38b1, 0x00},
    {0x3920, 0xff},
    {0x4003, 0x40},
    {0x4008, 0x02},
    {0x4009, 0x05},
    {0x400c, 0x00},
    {0x400d, 0x03},
    {0x4010, 0x40},
    {0x4043, 0x40},
    {0x4307, 0x30},
    {0x4317, 0x00},
    {0x4501, 0x00},
    {0x4507, 0x03},
    {0x4509, 0x80},
    {0x450a, 0x08},
    {0x4601, 0x04},
    {0x470f, 0x00},
    {0x4f07, 0x00},
    {0x4800, 0x00},
    {0x5000, 0x9f},
    {0x5001, 0x00},
    {0x5e00, 0x00},
    {0x5d00, 0x07},
    {0x5d01, 0x00},
    {REG_NULL, 0x00},
};

/*
 * Xclk 24Mhz
 * 60fps - 75fps = 800 Mbps * 1 lane / (1456*910*8)
 * mipi_datarate per lane 800Mbps
 * Method: cropping
 * 
 * Default conf (VGA resolution)
 */
static ov9281_reg_init_t  ov9281_640x480[] = {
    {0x0103, 0x01},
    {0x0302, 0x32}, // PLL1_mul
    {0x030d, 0x50}, // FLL2_mul
    {0x030e, 0x02}, // FLL2_sys_div
    {0x3001, 0x00},
    {0x3004, 0x00},
    {0x3005, 0x00},
    {0x3006, 0x04},
    {0x3011, 0x0a},
    {0x3013, 0x18},
    {0x3022, 0x01},
    {0x3030, 0x10}, // 04
    {0x3039, 0x32},
    {0x303a, 0x00},
    {0x3500, 0x00},
    {0x3501, 0x2a},
    {0x3502, 0x90},
    {0x3503, 0x08},
    {0x3505, 0x8c},
    {0x3507, 0x03},
    {0x3508, 0x00},
    {0x3509, 0x10},
    {0x3610, 0x80},
    {0x3611, 0xa0},
    {0x3620, 0x6f},
    {0x3632, 0x56},
    {0x3633, 0x78},
    {0x3662, 0x03}, //1lane RAW10: 0x01, 1lane RAW8: 0x03, 2lane RAW10: 0x05
    {0x3666, 0x00},
    {0x366f, 0x5a},
    {0x3680, 0x84},
    {0x3712, 0x80},
    {0x372d, 0x22},
    {0x3731, 0x80},
    {0x3732, 0x30},
    {0x3778, 0x00},
    {0x377d, 0x22},
    {0x3788, 0x02},
    {0x3789, 0xa4},
    {0x378a, 0x00},
    {0x378b, 0x4a},
    {0x3799, 0x20},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x00},
    {0x3804, 0x05},
    {0x3805, 0x0f},
    {0x3806, 0x03},
    {0x3807, 0x2f},

    {0x3808, 0x02}, // 640
    {0x3809, 0x80},
    {0x380a, 0x01}, // 480
    {0x380b, 0xe0},

    {0x380c, 0x05},
    {0x380d, 0xb0},
    {0x380e, 0x03},
    {0x380f, 0x8e},
    {0x3810, 0x00},
    {0x3811, 0x08},
    {0x3812, 0x00},
    {0x3813, 0x08},
    {0x3814, 0x11},
    {0x3815, 0x11},
    {0x3820, 0x40},
    {0x3821, 0x00},
    {0x3881, 0x42},
    {0x38b1, 0x00},
    {0x3920, 0xff},
    {0x4003, 0x40},
    {0x4008, 0x04},
    {0x4009, 0x0b},
    {0x400c, 0x00},
    {0x400d, 0x07},
    {0x4010, 0x40},
    {0x4043, 0x40},
    {0x4307, 0x30},
    {0x4317, 0x00},
    {0x4501, 0x00},
    {0x4507, 0x00},
    {0x4509, 0x00},
    {0x450a, 0x08},
    {0x4601, 0x04},
    {0x470f, 0x00},
    {0x4f07, 0x00},
    {0x4800, 0x00},
    {0x5000, 0x9f},
    {0x5001, 0x00},
    {0x5e00, 0x00},
    {0x5d00, 0x07},
    {0x5d01, 0x00},
    {REG_NULL, 0x00},
};

/*
 * Xclk 24Mhz
 * 60fps - 75fps = 800 Mbps * 1 lane / (1456*910*8)
 * mipi_datarate per lane 800Mbps
 * Method: cropping
 */
static ov9281_reg_init_t  ov9281_1280x800[] = {
    {0x0103, 0x01},
    {0x0302, 0x32}, // PLL1_mul
    {0x030d, 0x50}, // FLL2_mul
    {0x030e, 0x02}, // FLL2_sys_div
    {0x3001, 0x00},
    {0x3004, 0x00},
    {0x3005, 0x00},
    {0x3006, 0x04},
    {0x3011, 0x0a},
    {0x3013, 0x18},
    {0x3022, 0x01},
    {0x3030, 0x10}, // 04
    {0x3039, 0x32},
    {0x303a, 0x00},
    {0x3500, 0x00},
    {0x3501, 0x2a},
    {0x3502, 0x90},
    {0x3503, 0x08},
    {0x3505, 0x8c},
    {0x3507, 0x03},
    {0x3508, 0x00},
    {0x3509, 0x10},
    {0x3610, 0x80},
    {0x3611, 0xa0},
    {0x3620, 0x6f},
    {0x3632, 0x56},
    {0x3633, 0x78},
    {0x3662, 0x03}, //1lane RAW10: 0x01, 1lane RAW8: 0x03, 2lane RAW10: 0x05
    {0x3666, 0x00},
    {0x366f, 0x5a},
    {0x3680, 0x84},
    {0x3712, 0x80},
    {0x372d, 0x22},
    {0x3731, 0x80},
    {0x3732, 0x30},
    {0x3778, 0x00},
    {0x377d, 0x22},
    {0x3788, 0x02},
    {0x3789, 0xa4},
    {0x378a, 0x00},
    {0x378b, 0x4a},
    {0x3799, 0x20},
    {0x3800, 0x00},
    {0x3801, 0x00},
    {0x3802, 0x00},
    {0x3803, 0x00},
    {0x3804, 0x05},
    {0x3805, 0x0f},
    {0x3806, 0x03},
    {0x3807, 0x2f},

    {0x3808, 0x05}, // 1280
    {0x3809, 0x00},
    {0x380a, 0x03}, // 800
    {0x380b, 0x20},

    {0x380c, 0x05},
    {0x380d, 0xb0},
    {0x380e, 0x03},
    {0x380f, 0x8e},
    {0x3810, 0x00},
    {0x3811, 0x08},
    {0x3812, 0x00},
    {0x3813, 0x08},
    {0x3814, 0x11},
    {0x3815, 0x11},
    {0x3820, 0x40},
    {0x3821, 0x00},
    {0x3881, 0x42},
    {0x38b1, 0x00},
    {0x3920, 0xff},
    {0x4003, 0x40},
    {0x4008, 0x04},
    {0x4009, 0x0b},
    {0x400c, 0x00},
    {0x400d, 0x07},
    {0x4010, 0x40},
    {0x4043, 0x40},
    {0x4307, 0x30},
    {0x4317, 0x00},
    {0x4501, 0x00},
    {0x4507, 0x00},
    {0x4509, 0x00},
    {0x450a, 0x08},
    {0x4601, 0x04},
    {0x470f, 0x00},
    {0x4f07, 0x00},
    {0x4800, 0x00},
    {0x5000, 0x9f},
    {0x5001, 0x00},
    {0x5e00, 0x00},
    {0x5d00, 0x07},
    {0x5d01, 0x00},
    {REG_NULL, 0x00},
};


static inline int is_i2c_active()
{
    return 1;
}


static void __ov9281_reg_write(ov9281_t *ov9281, uint16_t addr, uint8_t value)
{
    if (is_i2c_active())
    {
        ov9281->i2c_req.value = value;
        ov9281->i2c_req.addr = ((addr >> 8) & 0xff) | ((addr & 0xff) << 8);
#ifdef I3C_I2C
        pi_i3c_write(&ov9281->i2c_device, (uint8_t *)&ov9281->i2c_req, 3);
#else
        pi_i2c_write(&ov9281->i2c_device, (uint8_t *)&ov9281->i2c_req, 3, PI_I2C_XFER_STOP);
#endif
    }
}

static uint8_t __ov9281_reg_read(ov9281_t *ov9281, uint16_t addr)
{
    if (is_i2c_active())
    {
        ov9281->i2c_req.addr = ((addr >> 8) & 0xff) | ((addr & 0xff) << 8);
#ifdef I3C_I2C
        pi_i3c_write(&ov9281->i2c_device, (uint8_t *)&ov9281->i2c_req.addr, 2);
        pi_i3c_read(&ov9281->i2c_device, (uint8_t *)&ov9281->i2c_read_value, 1);
#else
        pi_i2c_write(&ov9281->i2c_device, (uint8_t *)&ov9281->i2c_req.addr, 2, PI_I2C_XFER_STOP);
        pi_i2c_read(&ov9281->i2c_device, (uint8_t *)&ov9281->i2c_read_value, 1, PI_I2C_XFER_STOP);
#endif
        return *(volatile uint8_t *)&ov9281->i2c_read_value;
    }
    else
    {
        return 0;
    }
}

static void __ov9281_init_regs(ov9281_t *ov9281)
{
#ifdef SIZE_640_400
    for (int32_t i = 0; i < (int32_t)(sizeof(ov9281_640x400) / sizeof(ov9281_reg_init_t)); i++)
    {
        __ov9281_reg_write(ov9281, ov9281_640x400[i].addr, ov9281_640x400[i].value);
    }
#else
#ifdef SIZE_1280_800
    for (int32_t i = 0; i < (int32_t)(sizeof(ov9281_1280x800) / sizeof(ov9281_reg_init_t)); i++)
    {
        __ov9281_reg_write(ov9281, ov9281_1280x800[i].addr, ov9281_1280x800[i].value);
    }
#else
    for (int32_t i = 0; i < (int32_t)(sizeof(ov9281_640x480) / sizeof(ov9281_reg_init_t)); i++)
    {
        __ov9281_reg_write(ov9281, ov9281_640x480[i].addr, ov9281_640x480[i].value);
    }
#endif
#endif // SIZE_640_400

#ifdef DEBUG
#ifdef SIZE_640_400
    uint8_t reg_value = 0;
    for (int32_t i = 0; i < (int32_t)(sizeof(ov9281_640x400) / sizeof(ov9281_reg_init_t)); i++)
    {
        reg_value = __ov9281_reg_read(ov9281, ov9281_640x400[i].addr);
        if (reg_value != ov9281_640x400[i].value)
            printf("error reg: @%X = %X (expected: %X)\n", ov9281_640x400[i].addr, reg_value, ov9281_640x400[i].value);
    }
#else
#ifdef SIZE_1280_800
    uint8_t reg_value = 0;
    for (int32_t i = 0; i < (int32_t)(sizeof(ov9281_1280x800) / sizeof(ov9281_reg_init_t)); i++)
    {
        reg_value = __ov9281_reg_read(ov9281, ov9281_1280x800[i].addr);
        if (reg_value != ov9281_1280x800[i].value)
            printf("error reg: @%X = %X (expected: %X)\n", ov9281_1280x800[i].addr, reg_value, ov9281_1280x800[i].value);
    }
#else
    uint8_t reg_value = 0;
    for (int32_t i = 0; i < (int32_t)(sizeof(ov9281_640x480) / sizeof(ov9281_reg_init_t)); i++)
    {
        reg_value = __ov9281_reg_read(ov9281, ov9281_640x480[i].addr);
        if (reg_value != ov9281_640x480[i].value)
            printf("error reg: @%X = %X (expected: %X)\n", ov9281_640x480[i].addr, reg_value, ov9281_640x480[i].value);
    }
#endif // SIZE_1280_800
#endif // SIZE_640_400
#endif // DEBUG
}

static int __ov9281_stream_on(ov9281_t *ov9281)
{

    if (!ov9281->is_awake)
    {
        __ov9281_reg_write(ov9281, OV9281_REG_CTRL_MODE, OV9281_MODE_STREAMING);
        ov9281->is_awake = 1;
    }
    return 0;
}

static int __ov9281_stream_off(ov9281_t *ov9281)
{
    if (ov9281->is_awake)
    {
        __ov9281_reg_write(ov9281, OV9281_REG_CTRL_MODE, OV9281_MODE_SW_STANDBY);
        ov9281->is_awake = 0;
    }

    return 0;
}

int __ov9281_s_stream(ov9281_t *ov9281, int enable)
{
    if (enable)
        return __ov9281_stream_on(ov9281);
    else
        return __ov9281_stream_off(ov9281);

    return 0;
}

int __ov9281_detect(ov9281_t *ov9281)
{
    uint8_t read;

    __ov9281_reg_write(ov9281, OV9281_SW_RESET, 0x01);

    read = __ov9281_reg_read(ov9281, OV9281_REG_CHIPID_H);

    if (read != 0x92) {
        printf("ID High expected 0x92 got %x\n", read);
        return -1;
    }


    read = __ov9281_reg_read(ov9281, OV9281_REG_CHIPID_L);

    if (read != 0x81) {
        printf("ID Low expected 0x81 got %x\n", read);
        return -1;
    }

    __ov9281_reg_write(ov9281, OV9281_SW_RESET, 0x00);

    return 0;
}


int32_t __ov9281_open(struct pi_device *device)
{
    struct pi_ov9281_conf *conf = (struct pi_ov9281_conf *)device->config;

    ov9281_t *ov9281 = (ov9281_t *)pmsis_l2_malloc(sizeof(ov9281_t));
    if (ov9281 == NULL) return -1;

    device->data = (void *)ov9281;

    if (bsp_ov9281_open(conf))
        goto error;

#ifdef CUSTOM_BSP
    struct pi_gpio_conf gpio_conf;
    pi_gpio_conf_init(&gpio_conf);
    pi_open_from_conf(&ov9281->gpio_port, &gpio_conf);
    if (pi_gpio_open(&ov9281->gpio_port))
        goto error;

    pi_gpio_pin_configure(&ov9281->gpio_port, conf->reset_gpio, PI_GPIO_OUTPUT);
    pi_gpio_pin_configure(&ov9281->gpio_port, conf->pwdn_gpio, PI_GPIO_OUTPUT);

    pi_gpio_pin_write(&ov9281->gpio_port, conf->pwdn_gpio, 0);
    pi_time_wait_us(1000);
    pi_gpio_pin_write(&ov9281->gpio_port, ov9281->conf.reset_gpio, 0);
    pi_time_wait_us(20000);
#endif

    struct pi_csi2_conf csi2_conf;
    pi_csi2_conf_init(&csi2_conf);
    csi2_conf.itf = conf->csi2_itf;
    pi_open_from_conf(&ov9281->csi2_device, &csi2_conf);

    if (pi_csi2_open(&ov9281->csi2_device))
        goto error;

#ifdef I3C_I2C
    struct pi_i3c_conf i2c_conf;
    pi_i3c_conf_init(&i2c_conf);
#else
    struct pi_i2c_conf i2c_conf;
    pi_i2c_conf_init(&i2c_conf);
#endif
    i2c_conf.cs = OV9281_DEV_ADDR;
    i2c_conf.itf = conf->i2c_itf;
    i2c_conf.max_baudrate = 400000;

    pi_open_from_conf(&ov9281->i2c_device, &i2c_conf);

#ifdef I3C_I2C
    if (pi_i3c_open(&ov9281->i2c_device))
#else
    if (pi_i2c_open(&ov9281->i2c_device))
#endif
        goto error2;

    pi_csi2_set_pixel_clk_div(&ov9281->csi2_device, 0);
    pi_csi2_set_apb_clk_div(&ov9281->csi2_device, 10);

    pi_csi2_set_virtual_channel(&ov9281->csi2_device, 0);
    pi_csi2_set_lane(&ov9281->csi2_device, 1);

    ov9281->is_awake = 0;

    if(__ov9281_detect(ov9281))
    {
        goto error2;
    }
    __ov9281_init_regs(ov9281);

    return 0;

error2:
    pi_csi2_close(&ov9281->csi2_device);
error:
    pmsis_l2_malloc_free(ov9281, sizeof(ov9281_t));
    return -1;
}

static void __ov9281_close(struct pi_device *device)
{
    ov9281_t *ov9281 = (ov9281_t *)device->data;
    __ov9281_stream_off(ov9281);
    pi_csi2_close(&ov9281->csi2_device);
    pmsis_l2_malloc_free(ov9281, sizeof(ov9281_t));
}

static int __ov9281_sensor_power(ov9281_t *ov9281, int on)
{
    if (on) {

    } else {

    }

    return 0;
}

static int32_t __ov9281_control(struct pi_device *device, pi_camera_cmd_e cmd, void *arg)
{
    int irq = disable_irq();

    ov9281_t *ov9281 = (ov9281_t *)device->data;

    switch (cmd)
    {
    case PI_CAMERA_CMD_ON:
        __ov9281_sensor_power(ov9281, 1);
        break;

    case PI_CAMERA_CMD_OFF:
        __ov9281_sensor_power(ov9281, 0);
        break;

    case PI_CAMERA_CMD_START:
        __ov9281_s_stream(ov9281, 1);
        pi_csi2_control_start(&ov9281->csi2_device);
        break;

    case PI_CAMERA_CMD_STOP:
        __ov9281_s_stream(ov9281, 0);
        pi_csi2_control_stop(&ov9281->csi2_device);
        break;

    default:
        break;
    }

    restore_irq(irq);

    return 0;
}

void __ov9281_capture_async(struct pi_device *device, void *buffer, uint32_t bufferlen, pi_task_t *task)
{
    ov9281_t *ov9281 = (ov9281_t *)device->data;

    pi_csi2_capture_async(&ov9281->csi2_device, buffer, bufferlen, task);
}



int32_t __ov9281_reg_set(struct pi_device *device, uint32_t addr, uint8_t *value)
{
    ov9281_t *ov9281 = (ov9281_t *)device->data;
    __ov9281_reg_write(ov9281, addr, *value);
    return 0;
}



int32_t __ov9281_reg_get(struct pi_device *device, uint32_t addr, uint8_t *value)
{
    ov9281_t *ov9281 = (ov9281_t *)device->data;
    *value = __ov9281_reg_read(ov9281, addr);
    return 0;
}



static pi_camera_api_t ov9281_api =
{
    .open           = &__ov9281_open,
    .close          = &__ov9281_close,
    .control        = &__ov9281_control,
    .capture_async  = &__ov9281_capture_async,
    .reg_set        = &__ov9281_reg_set,
    .reg_get        = &__ov9281_reg_get
};



void pi_ov9281_conf_init(struct pi_ov9281_conf *conf)
{
    conf->camera.api = &ov9281_api;
    conf->vc = 0;
    bsp_ov9281_conf_init(conf);
    __camera_conf_init(&conf->camera);
}
