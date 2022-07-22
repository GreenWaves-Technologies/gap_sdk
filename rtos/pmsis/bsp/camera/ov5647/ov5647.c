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
#include "bsp/camera/ov5647.h"
#include "ov5647.h"

typedef struct {
    uint16_t addr;
    uint8_t value;
} i2c_req_t;


typedef struct {
    struct pi_ov5647_conf conf;
    struct pi_device csi2_device;
    struct pi_device i2c_device;

    struct pi_device gpio_port;

    i2c_req_t i2c_req;
    uint32_t i2c_read_value;

    int is_awake;
} ov5647_t;


typedef i2c_req_t ov5647_reg_init_t;



static ov5647_reg_init_t sensor_oe_disable_regs[] = {
    {0x3000, 0x00},
    {0x3001, 0x00},
    {0x3002, 0x00},
};

static ov5647_reg_init_t sensor_oe_enable_regs[] = {
    {0x3000, 0x0f},
    {0x3001, 0xff},
    {0x3002, 0xe4},
};


/*
 * PLL1 Clock Tree:
 *
 * +-< XVCLK
 * |
 * +-+ pll_pre_div (0x3037 [3:0], special values: 5: 1.5, 7: 2.5)
 *   |
 *   +-+ pll_mul (0x3036 [7:0])
 *     |
 *     +-+ sys_div (0x3035 [7:4])
 *       |
 *       +-+ mipi_div (0x3035 [3:0])
 *       | |
 *       | +-> MIPI_SCLK
 *       | |
 *       | +-+ mipi_phy_div (2)
 *       |   |
 *       |   +-> MIPI_CLK
 *       |
 *       +-+ root_div (0x3037 [4])
 *         |
 *         +-+ bit_div (0x3034 [3:0], 8 bits: 2, 10 bits: 2.5, other: 1)
 *           |
 *           +-+ sclk_div (0x3106 [3:2])
 *             |
 *             +-> SCLK
 *             |
 *             +-+ mipi_div (0x3035, 1: PCLK = SCLK)
 *               |
 *               +-> PCLK
 */

/*
 * Xclk 24Mhz
 * 15fps = 80 MHz * 1 lane / (2700*1974)
 * mipi_datarate per lane 800Mbps
 * 
 * Default config (VGA resolution)
 */
static ov5647_reg_init_t ov5647_640x480[] = {
    {0x0100, 0x00}, // OV    // GWT
    {0x0103, 0x01},
    {0x3034, 0x1a}, // 18    // 1a (mipi 10 bits) 18 (mipi 8 bits)
    {0x3035, 0x11}, // 11;21 // 11
    {0x3036, 0x64}, // 38;46 // 64
    {0x303c, 0x11},
    {0x3106, 0xf9}, // f5    // f9
    {0x3821, 0x06}, // 07    // 06
    {0x3820, 0x00}, // 41    // 00
    {0x3827, 0xec},
    {0x370c, 0x03}, // 0f    // 03
    {0x3612, 0x5b}, // 59    // 5b
    {0x3618, 0x04}, // 00    // 04
    {0x5000, 0x06},
    {0x5001, 0x00}, // none  // 00
    {0x5002, 0x40},
    {0x5003, 0x08},
    {0x5a00, 0x08},
    {0x3000, 0x00}, // ff    // 00
    {0x3001, 0x00}, // ff    // 00
    {0x3002, 0x00}, // ff    // 00
    {0x3016, 0x08},
    {0x3017, 0xe0},
    {0x3018, 0x24}, // 44    // 24
    {0x301c, 0xf8}, // fc;f8 // f8
    {0x301d, 0xf0},
    {0x3a18, 0x00},
    {0x3a19, 0xf8},
    {0x3c01, 0x80},
    {0x3b07, 0x0c},
    {0x3708, 0x64},
    {0x3709, 0x12}, // 52    // 12

    {0x3800, 0x00},
    {0x3801, 0x10},
    {0x3802, 0x00},
    {0x3803, 0x00},

    {0x3804, 0x0a},
    {0x3805, 0x3f}, // 2f    // 3f
    {0x3806, 0x07},
    {0x3807, 0xa3}, // 9f    // a3

    {0x3808, 0x02}, // 640 (change in common.h the CAM_H_RES)
    {0x3809, 0x80},
    {0x380a, 0x01}, // 480 (change in common.h the CAM_V_RES)
    {0x380b, 0xe0},

    {0x380c, 0x07},                // 2700
    {0x380d, 0x3c},
    {0x380e, 0x02}, // 03;01 // 02 // 1974
    {0x380f, 0xf0}, // f0;f8 // d0
    {0x3811, 0x08}, // none  // 08
    {0x3813, 0x02}, // none  // 02
    {0x3814, 0x35}, // 71    // 35
    {0x3815, 0x35}, // 71    // 35

    {0x350A, 0x00}, // none  // 00
    {0x350B, 0x10}, // none  // 10
    {0x3500, 0x00}, // none  // 00
    {0x3501, 0x13}, // none  // 13
    {0x3502, 0xB0}, // none  // b0
    {0x3212, 0xA0}, // none  // a0

    {0x3630, 0x2e},
    {0x3632, 0xe2},
    {0x3633, 0x23},
    {0x3634, 0x44},
    {0x3636, 0x06},
    {0x3620, 0x64},
    {0x3621, 0xe0},
    {0x3600, 0x37},
    {0x3704, 0xa0},
    {0x3703, 0x5a},
    {0x3715, 0x78},
    {0x3717, 0x01},
    {0x3731, 0x02},
    {0x370b, 0x60},
    {0x3705, 0x1a},
    {0x3f05, 0x02},
    {0x3f06, 0x10},
    {0x3f01, 0x0a},
    {0x3a08, 0x01},
    {0x3a09, 0x28}, // 2e    // 28
    {0x3a0a, 0x00},
    {0x3a0b, 0xf6}, // fb    // f6 (dft)
    {0x3a0d, 0x08}, // 02    // 08 (dft)
    {0x3a0e, 0x06}, // 01    // 06 (dft)
    {0x3a0f, 0x58},
    {0x3a10, 0x50},
    {0x3a1b, 0x58},
    {0x3a1e, 0x50},
    {0x3a11, 0x60},
    {0x3a1f, 0x28},
    {0x4001, 0x02},
    {0x4004, 0x04}, // 02    // 04
    {0x4000, 0x09},
    //{0x4805, 0x50}, // none  // none
    {0x4837, 0x19}, // none  // 19
    {0x4050, 0x6e},
    {0x4051, 0x8f},
    {0x0100, 0x01},
};

/*
 * Xclk 24Mhz
 * 15fps = 80 MHz * 1 lane / (2700*1974)
 * mipi_datarate per lane 800Mbps
 */
static ov5647_reg_init_t ov5647_640x400[] = {
    {0x0100, 0x00},
    {0x0103, 0x01},
    {0x3034, 0x1a}, // 1a (mipi 10 bits) 18 (mipi 8 bits)
    {0x3035, 0x11},
    {0x3036, 0x64},
    {0x303c, 0x11},
    {0x3106, 0xf9},
    {0x3821, 0x06},
    {0x3820, 0x00},
    {0x3827, 0xec},
    {0x370c, 0x03},
    {0x3612, 0x5b},
    {0x3618, 0x04},
    {0x5000, 0x06},
    {0x5001, 0x00},
    {0x5002, 0x40},
    {0x5003, 0x08},
    {0x5a00, 0x08},
    {0x3000, 0x00},
    {0x3001, 0x00},
    {0x3002, 0x00},
    {0x3016, 0x08},
    {0x3017, 0xe0},
    {0x3018, 0x24},
    {0x301c, 0xf8},
    {0x301d, 0xf0},
    {0x3a18, 0x00},
    {0x3a19, 0xf8},
    {0x3c01, 0x80},
    {0x3b07, 0x0c},
    {0x3708, 0x64},
    {0x3709, 0x12},

    {0x3800, 0x00},
    {0x3801, 0x10},
    {0x3802, 0x00},
    {0x3803, 0x00},

    {0x3804, 0x0a},
    {0x3805, 0x3f},
    {0x3806, 0x07},
    {0x3807, 0xa3},

    {0x3808, 0x02}, // 640
    {0x3809, 0x80},
    {0x380a, 0x01}, // 400
    {0x380b, 0x90},

    {0x380c, 0x07}, // 2700
    {0x380d, 0x3c},
    {0x380e, 0x02}, // 1974
    {0x380f, 0xf0},
    {0x3811, 0x08},
    {0x3813, 0x02},
    {0x3814, 0x35},
    {0x3815, 0x35},

    {0x350A, 0x00},
    {0x350B, 0x10},
    {0x3500, 0x00},
    {0x3501, 0x13},
    {0x3502, 0xB0},
    {0x3212, 0xA0},

    {0x3630, 0x2e},
    {0x3632, 0xe2},
    {0x3633, 0x23},
    {0x3634, 0x44},
    {0x3636, 0x06},
    {0x3620, 0x64},
    {0x3621, 0xe0},
    {0x3600, 0x37},
    {0x3704, 0xa0},
    {0x3703, 0x5a},
    {0x3715, 0x78},
    {0x3717, 0x01},
    {0x3731, 0x02},
    {0x370b, 0x60},
    {0x3705, 0x1a},
    {0x3f05, 0x02},
    {0x3f06, 0x10},
    {0x3f01, 0x0a},
    {0x3a08, 0x01},
    {0x3a09, 0x28},
    {0x3a0a, 0x00},
    {0x3a0b, 0xf6},
    {0x3a0d, 0x08},
    {0x3a0e, 0x06},
    {0x3a0f, 0x58},
    {0x3a10, 0x50},
    {0x3a1b, 0x58},
    {0x3a1e, 0x50},
    {0x3a11, 0x60},
    {0x3a1f, 0x28},
    {0x4001, 0x02},
    {0x4004, 0x04},
    {0x4000, 0x09},
    {0x4837, 0x19},
    {0x4050, 0x6e},
    {0x4051, 0x8f},
    {0x0100, 0x01},
};

/*
 * Xclk 24Mhz
 * 15fps = 80 MHz * 1 lane / (2700*1974)
 * mipi_datarate per lane 800Mbps
 * QVGA resolution
 */
static ov5647_reg_init_t ov5647_320x240[] = {
    {0x0100, 0x00},
    {0x0103, 0x01},
    {0x3034, 0x1a}, // 1a (mipi 10 bits) 18 (mipi 8 bits)
    {0x3035, 0x11},
    {0x3036, 0x64},
    {0x303c, 0x11},
    {0x3106, 0xf9},
    {0x3821, 0x06},
    {0x3820, 0x00},
    {0x3827, 0xec},
    {0x370c, 0x03},
    {0x3612, 0x5b},
    {0x3618, 0x04},
    {0x5000, 0x06},
    {0x5001, 0x00},
    {0x5002, 0x40},
    {0x5003, 0x08},
    {0x5a00, 0x08},
    {0x3000, 0x00},
    {0x3001, 0x00},
    {0x3002, 0x00},
    {0x3016, 0x08},
    {0x3017, 0xe0},
    {0x3018, 0x24},
    {0x301c, 0xf8},
    {0x301d, 0xf0},
    {0x3a18, 0x00},
    {0x3a19, 0xf8},
    {0x3c01, 0x80},
    {0x3b07, 0x0c},
    {0x3708, 0x64},
    {0x3709, 0x12},

    {0x3800, 0x00},
    {0x3801, 0x10},
    {0x3802, 0x00},
    {0x3803, 0x00},

    {0x3804, 0x0a},
    {0x3805, 0x3f},
    {0x3806, 0x07},
    {0x3807, 0xa3},

    {0x3808, 0x01}, // 320
    {0x3809, 0x40},
    {0x380a, 0x00}, // 240
    {0x380b, 0xf0},

    {0x380c, 0x07}, // 2700
    {0x380d, 0x3c},
    {0x380e, 0x02}, // 1974
    {0x380f, 0xf0},
    {0x3811, 0x08},
    {0x3813, 0x02},
    {0x3814, 0x35},
    {0x3815, 0x35},

    {0x350A, 0x00},
    {0x350B, 0x10},
    {0x3500, 0x00},
    {0x3501, 0x13},
    {0x3502, 0xB0},
    {0x3212, 0xA0},

    {0x3630, 0x2e},
    {0x3632, 0xe2},
    {0x3633, 0x23},
    {0x3634, 0x44},
    {0x3636, 0x06},
    {0x3620, 0x64},
    {0x3621, 0xe0},
    {0x3600, 0x37},
    {0x3704, 0xa0},
    {0x3703, 0x5a},
    {0x3715, 0x78},
    {0x3717, 0x01},
    {0x3731, 0x02},
    {0x370b, 0x60},
    {0x3705, 0x1a},
    {0x3f05, 0x02},
    {0x3f06, 0x10},
    {0x3f01, 0x0a},
    {0x3a08, 0x01},
    {0x3a09, 0x28},
    {0x3a0a, 0x00},
    {0x3a0b, 0xf6},
    {0x3a0d, 0x08},
    {0x3a0e, 0x06},
    {0x3a0f, 0x58},
    {0x3a10, 0x50},
    {0x3a1b, 0x58},
    {0x3a1e, 0x50},
    {0x3a11, 0x60},
    {0x3a1f, 0x28},
    {0x4001, 0x02},
    {0x4004, 0x04},
    {0x4000, 0x09},
    {0x4837, 0x19},
    {0x4050, 0x6e},
    {0x4051, 0x8f},
    {0x0100, 0x01},
};

static inline int is_i2c_active()
{
    return 1;
}


static void __ov5647_reg_write(ov5647_t *ov5647, uint16_t addr, uint8_t value)
{
    if (is_i2c_active())
    {
        ov5647->i2c_req.value = value;
        ov5647->i2c_req.addr = ((addr >> 8) & 0xff) | ((addr & 0xff) << 8);
#ifdef I3C_I2C
        pi_i3c_write(&ov5647->i2c_device, (uint8_t *)&ov5647->i2c_req, 3);
#else
        pi_i2c_write(&ov5647->i2c_device, (uint8_t *)&ov5647->i2c_req, 3, PI_I2C_XFER_STOP);
#endif
    }
}

static uint8_t __ov5647_reg_read(ov5647_t *ov5647, uint16_t addr)
{
    if (is_i2c_active())
    {
        ov5647->i2c_req.addr = ((addr >> 8) & 0xff) | ((addr & 0xff) << 8);
#ifdef I3C_I2C
        pi_i3c_write(&ov5647->i2c_device, (uint8_t *)&ov5647->i2c_req.addr, 2);
        pi_i3c_read(&ov5647->i2c_device, (uint8_t *)&ov5647->i2c_read_value, 1);
#else
        pi_i2c_write(&ov5647->i2c_device, (uint8_t *)&ov5647->i2c_req.addr, 2, PI_I2C_XFER_STOP);
        pi_i2c_read(&ov5647->i2c_device, (uint8_t *)&ov5647->i2c_read_value, 1, PI_I2C_XFER_STOP);
#endif
        return *(volatile uint8_t *)&ov5647->i2c_read_value;
    }
    else
    {
        return 0;
    }
}

static void __ov5647_init_regs(ov5647_t *ov5647)
{
#ifdef SIZE_640_400
    for (int32_t i = 0; i < (int32_t)(sizeof(ov5647_640x400) / sizeof(ov5647_reg_init_t)); i++)
    {
        __ov5647_reg_write(ov5647, ov5647_640x400[i].addr, ov5647_640x400[i].value);
    }
#else
#ifdef QVGA
    for (int32_t i = 0; i < (int32_t)(sizeof(ov5647_320x240) / sizeof(ov5647_reg_init_t)); i++)
    {
        __ov5647_reg_write(ov5647, ov5647_320x240[i].addr, ov5647_320x240[i].value);
    }
#else
    for (int32_t i = 0; i < (int32_t)(sizeof(ov5647_640x480) / sizeof(ov5647_reg_init_t)); i++)
    {
        __ov5647_reg_write(ov5647, ov5647_640x480[i].addr, ov5647_640x480[i].value);
    }
#endif
#endif // SIZE_640_400

#ifdef DEBUG
#ifdef SIZE_640_400
    uint8_t reg_value = 0;
    for (int32_t i = 0; i < (int32_t)(sizeof(ov5647_640x400) / sizeof(ov5647_reg_init_t)); i++)
    {
        reg_value = __ov5647_reg_read(ov5647, ov5647_640x400[i].addr);
        if (reg_value != ov5647_640x400[i].value)
            printf("error reg: @%X = %X (expected: %X)\n", ov5647_640x400[i].addr, reg_value, ov5647_640x400[i].value);
    }
#else
#ifdef QVGA
    uint8_t reg_value = 0;
    for (int32_t i = 0; i < (int32_t)(sizeof(ov5647_320x240) / sizeof(ov5647_reg_init_t)); i++)
    {
        reg_value = __ov5647_reg_read(ov5647, ov5647_320x240[i].addr);
        if (reg_value != ov5647_320x240[i].value)
            printf("error reg: @%X = %X (expected: %X)\n", ov5647_320x240[i].addr, reg_value, ov5647_320x240[i].value);
    }
#else
    uint8_t reg_value = 0;
    for (int32_t i = 0; i < (int32_t)(sizeof(ov5647_640x480) / sizeof(ov5647_reg_init_t)); i++)
    {
        reg_value = __ov5647_reg_read(ov5647, ov5647_640x480[i].addr);
        if (reg_value != ov5647_640x480[i].value)
            printf("error reg: @%X = %X (expected: %X)\n", ov5647_640x480[i].addr, reg_value, ov5647_640x480[i].value);
    }
#endif // QVGA
#endif // SIZE_640_400
#endif // DEBUG
}

static int __ov5647_stream_on(ov5647_t *ov5647, int clock_ncont)
{

    if (!ov5647->is_awake)
    {
        uint8_t val = MIPI_CTRL00_BUS_IDLE;

        if (clock_ncont)
            val |= MIPI_CTRL00_CLOCK_LANE_GATE |
                MIPI_CTRL00_LINE_SYNC_ENABLE;

        __ov5647_reg_write(ov5647, OV5647_REG_MIPI_CTRL00, val);

        __ov5647_reg_write(ov5647, OV5647_REG_FRAME_OFF_NUMBER, 0x00);

        __ov5647_reg_write(ov5647, OV5640_REG_PAD_OUT, 0x00);

        ov5647->is_awake = 1;
    }
    return 0;
}

static int __ov5647_stream_off(ov5647_t *ov5647)
{
    if (ov5647->is_awake)
    {
        __ov5647_reg_write(ov5647, OV5647_REG_MIPI_CTRL00, MIPI_CTRL00_CLOCK_LANE_GATE
                     | MIPI_CTRL00_BUS_IDLE | MIPI_CTRL00_CLOCK_LANE_DISABLE);

        __ov5647_reg_write(ov5647, OV5647_REG_FRAME_OFF_NUMBER, 0x0f);

        __ov5647_reg_write(ov5647, OV5640_REG_PAD_OUT, 0x01);

        ov5647->is_awake = 0;
    }

    return 0;
}

int __ov5647_s_stream(ov5647_t *ov5647, int enable)
{
    if (enable)
        return __ov5647_stream_on(ov5647, 0);
    else
        return __ov5647_stream_off(ov5647);

    return 0;
}

static int __ov5647_sw_standby(ov5647_t *ov5647, int standby)
{
    uint8_t rdval = __ov5647_reg_read(ov5647, OV5647_SW_STANDBY);

    if (standby)
        rdval &= ~0x01;
    else
        rdval |= 0x01;

    __ov5647_reg_write(ov5647, OV5647_SW_STANDBY, rdval);

    return 0;
}

static int __ov5647_set_virtual_channel(ov5647_t *ov5647, int channel)
{
    uint8_t channel_id;

    channel_id = __ov5647_reg_read(ov5647, OV5647_REG_MIPI_CTRL14);

    channel_id &= ~(3 << 6);

    __ov5647_reg_write(ov5647, OV5647_REG_MIPI_CTRL14, channel_id | (channel << 6));

    return 0;
}

int __ov5647_sensor_init(ov5647_t *ov5647 )
{
    uint8_t rdval = __ov5647_reg_read(ov5647, OV5647_SW_STANDBY);

    __ov5647_init_regs(ov5647);

    __ov5647_set_virtual_channel(ov5647, 0);

    uint8_t resetval = __ov5647_reg_read(ov5647, OV5647_SW_STANDBY);

    if (!(resetval & 0x01)) {
        printf("Device was in SW standby\n");
        __ov5647_reg_write(ov5647, OV5647_SW_STANDBY, 0x01);
    }

    __ov5647_s_stream(ov5647, 0);

    return 0;
}

static int __ov5647_sensor_power(ov5647_t *ov5647, int on)
{
    if (on) {

        for(int i=0; i<(int32_t)(sizeof(sensor_oe_enable_regs)/sizeof(ov5647_reg_init_t)); i++)
        {
            __ov5647_reg_write(ov5647, sensor_oe_enable_regs[i].addr, sensor_oe_enable_regs[i].value);
        }

        __ov5647_sensor_init(ov5647);
    } else {
        for(int i=0; i<(int32_t)(sizeof(sensor_oe_disable_regs)/sizeof(ov5647_reg_init_t)); i++)
        {
            __ov5647_reg_write(ov5647, sensor_oe_disable_regs[i].addr, sensor_oe_disable_regs[i].value);
        }

        __ov5647_sw_standby(ov5647, 1);
    }

    return 0;
}

int __ov5647_detect(ov5647_t *ov5647)
{
    uint8_t read;

    __ov5647_reg_write(ov5647, OV5647_SW_RESET, 0x01);

    read = __ov5647_reg_read(ov5647, OV5647_REG_CHIPID_H);

    if (read != 0x56) {
        printf("ID High expected 0x56 got %x\n", read);
        return -1;
    }


    read = __ov5647_reg_read(ov5647, OV5647_REG_CHIPID_L);

    if (read != 0x47) {
        printf("ID Low expected 0x47 got %x\n", read);
        return -1;
    }

    __ov5647_reg_write(ov5647, OV5647_SW_RESET, 0x00);

    return 0;
}


int __ov5647_get_sysclk(ov5647_t *ov5647)
{
    /* calculate sysclk */
    int xvclk = 24;
    int sysclk, temp1;
    int pre_div02x, div_cnt7b, sdiv0, pll_rdiv, bit_div2x, sclk_div, VCO, mipi_div;
    int pre_div02x_map[] = {2, 2, 4, 6, 8, 3, 12, 5, 16, 2, 2, 2, 2, 2, 2, 2};
    int sdiv0_map[] = {16, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    int pll_rdiv_map[] = {1, 2};
    int bit_div2x_map[] = {2, 2, 2, 2, 2, 2, 2, 2, 4, 2, 5, 2, 2, 2, 2, 2};
    int sclk_div_map[] = {1, 2, 4, 1};
    uint8_t temp;

    temp = __ov5647_reg_read(ov5647, 0x3037);
    temp1 = temp & 0x0f;
    pre_div02x = pre_div02x_map[temp1];
    temp1 = (temp >> 4) & 0x01;
    pll_rdiv = pll_rdiv_map[temp1];
    temp = __ov5647_reg_read(ov5647, 0x3036);

    div_cnt7b = temp;

    VCO = xvclk * 2 / pre_div02x * div_cnt7b;
    temp = __ov5647_reg_read(ov5647, 0x3035);
    mipi_div = sdiv0_map[temp & 0x0f];
    temp1 = temp >> 4;
    sdiv0 = sdiv0_map[temp1];
    temp = __ov5647_reg_read(ov5647, 0x3034);
    temp1 = temp & 0x0f;

    printf("%dbit\n", temp1);

    bit_div2x = bit_div2x_map[temp1];
    temp = __ov5647_reg_read(ov5647, 0x3106);
    temp1 = (temp >> 2) & 0x03;
    sclk_div = sclk_div_map[temp1];
    sysclk = VCO * 2 / sdiv0 / pll_rdiv / bit_div2x / sclk_div;

    int mipi_clk = VCO / sdiv0 / mipi_div / 2;

    printf("MIPI_CLK = %d MHz\n", mipi_clk);

    return sysclk;
}

int32_t __ov5647_open(struct pi_device *device)
{
    struct pi_ov5647_conf *conf = (struct pi_ov5647_conf *)device->config;

    ov5647_t *ov5647 = (ov5647_t *)pmsis_l2_malloc(sizeof(ov5647_t));
    if (ov5647 == NULL) return -1;

    device->data = (void *)ov5647;

    if (bsp_ov5647_open(conf))
        goto error;

#ifdef CUSTOM_BSP
    struct pi_gpio_conf gpio_conf;
    pi_gpio_conf_init(&gpio_conf);
    pi_open_from_conf(&ov5647->gpio_port, &gpio_conf);
    if (pi_gpio_open(&ov5647->gpio_port))
        goto error;

    pi_gpio_pin_configure(&ov5647->gpio_port, conf->reset_gpio, PI_GPIO_OUTPUT);
    pi_gpio_pin_configure(&ov5647->gpio_port, conf->pwdn_gpio, PI_GPIO_OUTPUT);

    pi_gpio_pin_write(&ov5647->gpio_port, conf->pwdn_gpio, 0);
    pi_time_wait_us(1000);
    pi_gpio_pin_write(&ov5647->gpio_port, ov5647->conf.reset_gpio, 0);
    pi_time_wait_us(20000);
#endif

    struct pi_csi2_conf csi2_conf;
    pi_csi2_conf_init(&csi2_conf);
    csi2_conf.itf = conf->csi2_itf;
    pi_open_from_conf(&ov5647->csi2_device, &csi2_conf);

    if (pi_csi2_open(&ov5647->csi2_device))
        goto error;

#ifdef I3C_I2C
    struct pi_i3c_conf i2c_conf;
    pi_i3c_conf_init(&i2c_conf);
#else
    struct pi_i2c_conf i2c_conf;
    pi_i2c_conf_init(&i2c_conf);
#endif
    i2c_conf.cs = OV5647_DEV_ADDR;
    i2c_conf.itf = conf->i2c_itf;
    i2c_conf.max_baudrate = 400000;
    pi_open_from_conf(&ov5647->i2c_device, &i2c_conf);

#ifdef I3C_I2C
    if (pi_i3c_open(&ov5647->i2c_device))
#else
    if (pi_i2c_open(&ov5647->i2c_device))
#endif
        goto error2;

    pi_csi2_set_pixel_clk_div(&ov5647->csi2_device, 0);
    pi_csi2_set_apb_clk_div(&ov5647->csi2_device, 10);

    pi_csi2_set_virtual_channel(&ov5647->csi2_device, 0);
    pi_csi2_set_lane(&ov5647->csi2_device, 1);

    ov5647->is_awake = 0;

    if(__ov5647_detect(ov5647))
    {
        goto error2;
    }

    __ov5647_sensor_power(ov5647, 1);

    return 0;

error2:
    pi_csi2_close(&ov5647->csi2_device);
error:
    pmsis_l2_malloc_free(ov5647, sizeof(ov5647_t));
    return -1;
}

static void __ov5647_close(struct pi_device *device)
{
    ov5647_t *ov5647 = (ov5647_t *)device->data;

    __ov5647_stream_off(ov5647);
    __ov5647_sensor_power(ov5647, 0);

    pi_csi2_close(&ov5647->csi2_device);
    pmsis_l2_malloc_free(ov5647, sizeof(ov5647_t));
}

static int32_t __ov5647_control(struct pi_device *device, pi_camera_cmd_e cmd, void *arg)
{
    int irq = disable_irq();

    ov5647_t *ov5647 = (ov5647_t *)device->data;

    switch (cmd)
    {
    case PI_CAMERA_CMD_ON:
        __ov5647_sensor_power(ov5647, 1);
        break;

    case PI_CAMERA_CMD_OFF:
        __ov5647_sensor_power(ov5647, 0);
        break;

    case PI_CAMERA_CMD_START:
        __ov5647_s_stream(ov5647, 1);
        pi_csi2_control_start(&ov5647->csi2_device);
        break;

    case PI_CAMERA_CMD_STOP:
        __ov5647_s_stream(ov5647, 0);
        pi_csi2_control_stop(&ov5647->csi2_device);
        break;

    default:
        break;
    }

    restore_irq(irq);

    return 0;
}

void __ov5647_capture_async(struct pi_device *device, void *buffer, uint32_t bufferlen, pi_task_t *task)
{
    ov5647_t *ov5647 = (ov5647_t *)device->data;

    pi_csi2_capture_async(&ov5647->csi2_device, buffer, bufferlen, task);
}



int32_t __ov5647_reg_set(struct pi_device *device, uint32_t addr, uint8_t *value)
{
    ov5647_t *ov5647 = (ov5647_t *)device->data;
    __ov5647_reg_write(ov5647, addr, *value);
    return 0;
}



int32_t __ov5647_reg_get(struct pi_device *device, uint32_t addr, uint8_t *value)
{
    ov5647_t *ov5647 = (ov5647_t *)device->data;
    *value = __ov5647_reg_read(ov5647, addr);
    return 0;
}



static pi_camera_api_t ov5647_api =
{
    .open           = &__ov5647_open,
    .close          = &__ov5647_close,
    .control        = &__ov5647_control,
    .capture_async  = &__ov5647_capture_async,
    .reg_set        = &__ov5647_reg_set,
    .reg_get        = &__ov5647_reg_get
};



void pi_ov5647_conf_init(struct pi_ov5647_conf *conf)
{
    conf->camera.api = &ov5647_api;
    conf->vc = 0;
    bsp_ov5647_conf_init(conf);
    __camera_conf_init(&conf->camera);
}
