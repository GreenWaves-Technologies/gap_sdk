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
#include "bsp/bsp.h"
#include "bsp/camera/gc0308.h"
#include "gc0308.h"

typedef struct {
    uint8_t addr;
    uint8_t value;
} i2c_req_t;


typedef struct {
    struct pi_gc0308_conf conf;
    struct pi_device cpi_device;
    struct pi_device i2c_device;

    struct pi_device gpio_port;

    i2c_req_t i2c_req;
    uint32_t i2c_read_value;

    int is_awake;
} gc0308_t;


typedef i2c_req_t gc0308_reg_init_t;

static gc0308_reg_init_t __gc0308_reg_init[] =
{
    {0xfe,0x00},
    {0x01,0xcb},   //0x28
    {0x02,0x60},   //0x00
    {0x0f,0x18},	 //0x21
    {0xe2,0x00},
    {0xe3,0xFA},
    {0xe4,0x03},
    {0xe5,0xE8},
    {0xe6,0x03},
    {0xe7,0xE8},
    {0xe8,0x03},
    {0xe9,0xE8},
    {0xea,0x09},
    {0xeb,0xC4},
    {0xec,0x20},
    {0x05,0x00},
    {0x06,0x00},
    {0x07,0x00},
    {0x08,0x00},
    
    //{0x09,0x00}, 
    //{0x0a,0xF8},
    //
    //{0x0b,0x01},
    //{0x0c,0x48},

    {0x0d,0x02},
    {0x0e,0x02},
    {0x10,0x26},
    {0x11,0x0d},
    {0x12,0x2a},
    {0x13,0x00},
    {0x14,0x11},
    
    //{0x15,0x4a},
    
    {0x16,0x05},
    {0x17,0x01},
    {0x18,0x44},
    {0x19,0x44},
    {0x1a,0x2a},
    {0x1b,0x00},
    {0x1c,0x49},
    {0x1d,0x9a},
    {0x1e,0x61},
    {0x1f,0x16},
    {0x20,0x7f},
    {0x21,0xfa},
    {0x22,0x57},
    {0x24,0xa6},	//Only Y, can be changed to RGB or YUV
    {0x25,0x0f},
    {0x26,0x02},    // Vsync Low active, Hsync High active
    {0x28,0x00},
    {0x2d,0x0a},
    {0x2f,0x01},
    {0x30,0xf7},
    {0x31,0x50},
    {0x32,0x00},
    {0x33,0x28},
    {0x34,0x2a},
    {0x35,0x28},
    {0x39,0x04},
    {0x3a,0x20},
    {0x3b,0x20},
    {0x3c,0x00},
    {0x3d,0x00},
    {0x3e,0x00},
    {0x3f,0x00},
    
    
    {0x50,0x14}, // 0x14
    {0x52,0x41},
    {0x53,0x80},
    {0x55,0x80},
    {0x56,0x80},
    {0x8b,0x20},
    {0x8c,0x20},
    {0x8d,0x20},
    {0x8e,0x14},
    {0x8f,0x10},
    {0x90,0x14},
    {0x91,0x3c},
    {0x92,0x50},
    {0x5d,0x12},
    {0x5e,0x1a},
    {0x5f,0x24},
    {0x60,0x07},
    {0x61,0x15},
    {0x62,0x08}, // 0x08
    {0x64,0x03},  // 0x03
    {0x66,0xe8},
    {0x67,0x86},
    {0x68,0x82},
    {0x69,0x18},
    {0x6a,0x0f},
    {0x6b,0x00},
    {0x6c,0x5f},
    {0x6d,0x8f},
    {0x6e,0x55},
    {0x6f,0x38},
    {0x70,0x15},
    {0x71,0x33},
    {0x72,0xdc},
    {0x73,0x00},
    {0x74,0x02},
    {0x75,0x3f},
    {0x76,0x02},
    {0x77,0x38}, // 0x47
    {0x78,0x88},
    {0x79,0x81},
    {0x7a,0x81},
    {0x7b,0x22},
    {0x7c,0xff},
    {0x93,0x48},  //color matrix default
    {0x94,0x02},
    {0x95,0x07},
    {0x96,0xe0},
    {0x97,0x40},
    {0x98,0xf0},
    {0xb1,0x40},
    {0xb2,0x40},
    {0xb3,0x40}, //0x40
    {0xb6,0xe0},
    {0xbd,0x38},
    {0xbe,0x36},
    {0xd0,0xCB},
    {0xd1,0x10},
    {0xd2,0x90},
    {0xd3,0x48},
    {0xd5,0xF2},
    {0xd6,0x16},
    {0xdb,0x92},
    {0xdc,0xA5},
    {0xdf,0x23},
    {0xd9,0x00},
    {0xda,0x00},
    {0xe0,0x09},
    {0xed,0x04},
    {0xee,0xa0},
    {0xef,0x40},
    {0x80,0x03},

    {0x9F,0x10},
    {0xA0,0x20},
    {0xA1,0x38},
    {0xA2,0x4e},
    {0xA3,0x63},
    {0xA4,0x76},
    {0xA5,0x87},
    {0xA6,0xa2},
    {0xA7,0xb8},
    {0xA8,0xca},
    {0xA9,0xd8},
    {0xAA,0xe3},
    {0xAB,0xeb},
    {0xAC,0xf0},
    {0xAD,0xF8},
    {0xAE,0xFd},
    {0xAF,0xFF},

    {0xc0,0x00},
    {0xc1,0x10},
    {0xc2,0x1c},
    {0xc3,0x30},
    {0xc4,0x43},
    {0xc5,0x54},
    {0xc6,0x65},
    {0xc7,0x75},
    {0xc8,0x93},
    {0xc9,0xB0},
    {0xca,0xCB},
    {0xcb,0xE6},
    {0xcc,0xFF},
    {0xf0,0x02},
    {0xf1,0x01},
    {0xf2,0x02},
    {0xf3,0x30},
    {0xf7,0x12},
    {0xf8,0x0a},
    {0xf9,0x9f},
    {0xfa,0x78},
    {0xfe,0x01},
    {0x00,0xf5},
    {0x02,0x20},
    {0x04,0x10},
    {0x05,0x08},
    {0x06,0x20},
    {0x08,0x0a},
    {0x0a,0xa0},
    {0x0b,0x60},
    {0x0c,0x08},
    {0x0e,0x44},
    {0x0f,0x32},
    {0x10,0x41},
    {0x11,0x37},
    {0x12,0x22},
    {0x13,0x19},
    {0x14,0x44},
    {0x15,0x44},
    {0x16,0xc2},
    {0x17,0xA8},
    {0x18,0x18},
    {0x19,0x50},
    {0x1a,0xd8},
    {0x1b,0xf5},
    {0x70,0x40},
    {0x71,0x58},
    {0x72,0x30},
    {0x73,0x48},
    {0x74,0x20},
    {0x75,0x60},
    {0x77,0x20},
    {0x78,0x32},
    {0x30,0x03},
    {0x31,0x40},
    {0x32,0x10},
    {0x33,0xe0},
    {0x34,0xe0},
    {0x35,0x00},
    {0x36,0x80},
    {0x37,0x00},
    {0x38,0x04},
    {0x39,0x09},
    {0x3a,0x12},
    {0x3b,0x1C},
    {0x3c,0x28},
    {0x3d,0x31},
    {0x3e,0x44},
    {0x3f,0x57},
    {0x40,0x6C},
    {0x41,0x81},
    {0x42,0x94},
    {0x43,0xA7},
    {0x44,0xB8},
    {0x45,0xD6},
 //   {0x46,0xEE},
//    {0x47,0x0d},
    {0x62,0xf7},
    {0x63,0x68},
    {0x64,0xd3},
    {0x65,0xd3},
    {0x66,0x60},
    {0xfe,0x00},

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



static void __gc0308_reg_write(gc0308_t *gc0308, uint8_t addr, uint8_t value)
{
    if (is_i2c_active())
    {
        gc0308->i2c_req.value = value;
        gc0308->i2c_req.addr = (uint8_t)addr;
        pi_i2c_write(&gc0308->i2c_device, (uint8_t *)&gc0308->i2c_req, 2, PI_I2C_XFER_STOP);
    }
}



static uint8_t __gc0308_reg_read(gc0308_t *gc0308, uint8_t addr)
{

    if (is_i2c_active())
    {
        gc0308->i2c_req.addr = (addr & 0xFF);
        pi_i2c_write(&gc0308->i2c_device, (uint8_t *)&gc0308->i2c_req.addr, 1, PI_I2C_XFER_NO_STOP);
        pi_i2c_read(&gc0308->i2c_device, (uint8_t *)&gc0308->i2c_read_value, 1, PI_I2C_XFER_STOP);
        return *(volatile uint8_t *)&gc0308->i2c_read_value;
    }
    return 0;
}

static void __gc0308_init_regs(gc0308_t *gc0308)
{
    int32_t i;
    for(i=0; i<(int32_t)(sizeof(__gc0308_reg_init)/sizeof(gc0308_reg_init_t)); i++)
    {
        __gc0308_reg_write(gc0308, __gc0308_reg_init[i].addr, __gc0308_reg_init[i].value);
    }

#ifdef DEBUG
    uint8_t reg_value = 0;
    for(i=0; i<(sizeof(__gc0308_reg_init)/sizeof(gc0308_reg_init_t)); i++)
    {
        reg_value = __gc0308_reg_read(gc0308, __gc0308_reg_init[i].addr);
        if (reg_value != __gc0308_reg_init[i].value)
            printf("error reg: @%X = %X (expected: %X)\n", __gc0308_reg_init[i].addr, reg_value, __gc0308_reg_init[i].value);
    }
#endif

}



static void __gc0308_reset(gc0308_t *gc0308)
{
    //set to 0 power down pin
    pi_gpio_pin_write(&gc0308->gpio_port, gc0308->conf.reset_gpio, 0);
}


static void __gc0308_mode(gc0308_t *gc0308, uint8_t mode)
{
    // The camera standby mode is controlled by PWDN pin, which is not connected on GAPuino board.
    // Do nothing here
}



static void __gc0308_wakeup(gc0308_t *gc0308)
{
    if (!gc0308->is_awake)
    {
        // The camera standby mode is controlled by PWDN pin, which is not connected on GAPuino board.
        // Do nothing here
        gc0308->is_awake = 1;
    }
}



static void __gc0308_standby(gc0308_t *gc0308)
{
    if (gc0308->is_awake)
    {
        // The camera standby mode is controlled by PWDN pin, which is not connected on GAPuino board.
        // Do nothing here
        gc0308->is_awake = 0;
    }
}

static void __gc0308_set_qvga(gc0308_t *gc0308)
{

   // gc0308_t *gc0308 = (gc0308_t *)device->data;

    __gc0308_reg_write(gc0308,0xfe,0x01);
    __gc0308_reg_write(gc0308,0x54,0x22);  //1/2 subsample 
    __gc0308_reg_write(gc0308,0x55,0x03); 
    __gc0308_reg_write(gc0308,0x56,0x00); 
    __gc0308_reg_write(gc0308,0x57,0x00); 
    __gc0308_reg_write(gc0308,0x58,0x00); 
    __gc0308_reg_write(gc0308,0x59,0x00); 
    __gc0308_reg_write(gc0308,0xfe,0x00);
    __gc0308_reg_write(gc0308,0x46,0x80);//enable crop window mode
    __gc0308_reg_write(gc0308,0x47,0x00);  
    __gc0308_reg_write(gc0308,0x48,0x00);  
    __gc0308_reg_write(gc0308,0x49,0x00); 
    __gc0308_reg_write(gc0308,0x4a,0xf0);//240
    __gc0308_reg_write(gc0308,0x4b,0x01);  
    __gc0308_reg_write(gc0308,0x4c,0x40); //320

    return;
}

void __gc0308_set_crop(struct pi_device *device, uint8_t offset_x, uint8_t offset_y,uint16_t width,uint16_t height)
{
    uint8_t reg8;
    gc0308_t *gc0308 = (gc0308_t *)device->data;

    __gc0308_reg_write(gc0308,0xfe,0x00);
    //Enable Crop
    __gc0308_reg_write(gc0308, 0x46, 0x80);
    //set x offset
    __gc0308_reg_write(gc0308, 0x48, offset_x);
    //set y offset
    __gc0308_reg_write(gc0308, 0x47, offset_y);
    //set width
    reg8 = ( width & 0x00ff);
    __gc0308_reg_write(gc0308, 0x4C, reg8);
    reg8 = ( width & 0x0300) >> 8;
    __gc0308_reg_write(gc0308, 0x4B, reg8);
    //set height
    reg8 = ( height & 0x00ff);
    __gc0308_reg_write(gc0308, 0x4A, reg8);
    reg8 = ( height & 0x0100) >> 8;
    __gc0308_reg_write(gc0308, 0x49, reg8);
    
    return;   
}




int32_t __gc0308_open(struct pi_device *device)
{
    struct pi_gc0308_conf *conf = (struct pi_gc0308_conf *)device->config;

    gc0308_t *gc0308 = (gc0308_t *)pmsis_l2_malloc(sizeof(gc0308_t));
    if (gc0308 == NULL) return -1;

    memcpy(&gc0308->conf, conf, sizeof(*conf));
    device->data = (void *)gc0308;

    if (bsp_gc0308_open(conf))
        goto error;

    struct pi_gpio_conf gpio_reset_conf;
    pi_gpio_conf_init(&gpio_reset_conf);
    pi_open_from_conf(&gc0308->gpio_port, &gpio_reset_conf);
    if (pi_gpio_open(&gc0308->gpio_port))
        goto error;

    pi_gpio_pin_configure(&gc0308->gpio_port, conf->reset_gpio, PI_GPIO_OUTPUT);

    struct pi_cpi_conf cpi_conf;
    pi_cpi_conf_init(&cpi_conf);
    cpi_conf.itf = conf->cpi_itf;
    pi_open_from_conf(&gc0308->cpi_device, &cpi_conf);

    if (pi_cpi_open(&gc0308->cpi_device))
        goto error;

    struct pi_i2c_conf i2c_conf;
    pi_i2c_conf_init(&i2c_conf);
    i2c_conf.cs = 0x42;
    i2c_conf.itf = conf->i2c_itf;
    i2c_conf.max_baudrate = 200000;
    pi_open_from_conf(&gc0308->i2c_device, &i2c_conf);

    if (pi_i2c_open(&gc0308->i2c_device))
        goto error2;

    __gc0308_reset(gc0308);

    uint8_t cam_id = __gc0308_reg_read(gc0308, 0x00);
    if (cam_id != 0x9b) goto error2;

    __gc0308_init_regs(gc0308);

    if(gc0308->conf.format==PI_CAMERA_QVGA){
        __gc0308_set_qvga(gc0308);
    }

    if(gc0308->conf.color_mode==PI_CAMERA_GRAY8){
        pi_cpi_set_format(&gc0308->cpi_device, PI_CPI_FORMAT_BYPASS_BIGEND);
        __gc0308_reg_write(gc0308,0x24,0xb1); //set to YUV and get only Y
    }else{
        pi_cpi_set_format(&gc0308->cpi_device, PI_CPI_FORMAT_BYPASS_LITEND);
        //registers in this case are left by default to RGB565
    }


    return 0;

error2:
    pi_cpi_close(&gc0308->cpi_device);
error:
    pmsis_l2_malloc_free(gc0308, sizeof(gc0308_t));
    return -1;
}



static void __gc0308_close(struct pi_device *device)
{
    gc0308_t *gc0308 = (gc0308_t *)device->data;
    __gc0308_standby(gc0308);
    pi_cpi_close(&gc0308->cpi_device);
    pmsis_l2_malloc_free(gc0308, sizeof(gc0308_t));
}



static int32_t __gc0308_control(struct pi_device *device, pi_camera_cmd_e cmd, void *arg)
{
    int irq = disable_irq();

    gc0308_t *gc0308 = (gc0308_t *)device->data;

    switch (cmd)
    {
        case PI_CAMERA_CMD_ON:
            __gc0308_wakeup(gc0308);
            break;

        case PI_CAMERA_CMD_OFF:
            __gc0308_standby(gc0308);
            break;

        case PI_CAMERA_CMD_START:
            pi_cpi_control_start(&gc0308->cpi_device);
            break;

        case PI_CAMERA_CMD_STOP:
            pi_cpi_control_stop(&gc0308->cpi_device);
            break;

        default:
            break;
    }

    restore_irq(irq);

    return 0;
}

void __gc0308_capture_async(struct pi_device *device, void *buffer, uint32_t bufferlen, pi_task_t *task)
{
    gc0308_t *gc0308 = (gc0308_t *)device->data;

    pi_cpi_capture_async(&gc0308->cpi_device, buffer, bufferlen, task);
}

int32_t __gc0308_reg_set(struct pi_device *device, uint32_t addr, uint8_t *value)
{
    gc0308_t *gc0308 = (gc0308_t *)device->data;
    __gc0308_reg_write(gc0308, addr, *value);
    return 0;
}

int32_t __gc0308_reg_get(struct pi_device *device, uint32_t addr, uint8_t *value)
{
    gc0308_t *gc0308 = (gc0308_t *)device->data;
    *value = __gc0308_reg_read(gc0308, addr);
    return 0;
}


static pi_camera_api_t gc0308_api =
{
    .open           = &__gc0308_open,
    .close          = &__gc0308_close,
    .control        = &__gc0308_control,
    .capture_async  = &__gc0308_capture_async,
    .reg_set        = &__gc0308_reg_set,
    .reg_get        = &__gc0308_reg_get,
    .set_crop       = &__gc0308_set_crop,
};


void pi_gc0308_conf_init(struct pi_gc0308_conf *conf)
{
    conf->camera.api = &gc0308_api;
    conf->skip_pads_config = 0;
    bsp_gc0308_conf_init(conf);
    __camera_conf_init(&conf->camera);
    conf->format = PI_CAMERA_QVGA;
    conf->color_mode = PI_CAMERA_RGB565;
}
