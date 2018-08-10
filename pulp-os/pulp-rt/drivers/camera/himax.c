/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Yao ZHANG, GreenWaves Technologies (yao.zhang@greenwaves-technologies.com)
 *          Francesco PACI, GreenWaves Technologies (francesco.paci@greenwaves-technologies.com)
 */

#include "rt/rt_api.h"

//TODO: this sequence could be optimized
static himax_reg_cfg_t himaxRegInit[] = {
    {BLC_TGT, 0x08},            //  BLC target :8  at 8 bit mode
    {BLC2_TGT, 0x08},           //  BLI target :8  at 8 bit mode
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

    {BLC_CFG, 0x43},            //  BLC_on, IIR

    {0x1001, 0x43},             //  BLC dithering en
    {0x1002, 0x43},             //  blc_darkpixel_thd
    {0x0350, 0x00},             //  Dgain Control
    {BLI_EN, 0x01},             //  BLI enable
    {0x1003, 0x00},             //  BLI Target [Def: 0x20]

    {DPC_CTRL, 0x01},           //  DPC option 0: DPC off   1 : mono   3 : bayer1   5 : bayer2
    {0x1009, 0xA0},             //  cluster hot pixel th
    {0x100A, 0x60},             //  cluster cold pixel th
    {SINGLE_THR_HOT, 0x90},     //  single hot pixel th
    {SINGLE_THR_COLD, 0x40},    //  single cold pixel th
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

    {AE_CTRL,        0x01},      //Automatic Exposure Gain Control
    {AE_TARGET_MEAN, 0x3C},      //AE target mean [Def: 0x3C]
    {AE_MIN_MEAN,    0x0A},      //AE min target mean [Def: 0x0A]

    {INTEGRATION_H,  0x00},      //Integration H [Def: 0x01]
    {INTEGRATION_L,  0x60},      //Integration L [Def: 0x08]
    {ANALOG_GAIN,    0x00},      //Analog Global Gain
    {DAMPING_FACTOR, 0x20},      //Damping Factor [Def: 0x20]
    {DIGITAL_GAIN_H, 0x01},      //Digital Gain High [Def: 0x01]
    {DIGITAL_GAIN_L, 0x00},      //Digital Gain Low [Def: 0x00]

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
    {0x3010, 0x01},
    {0x0383, 0x01},
    {0x0387, 0x01},
    {0x0390, 0x00},
    {0x3011, 0x70},
    {0x3059, 0x02},
    {0x3060, 0x01},
//    {0x3060, 0x25}, //Clock gating and clock divisors
    {0x3068, 0x20}, //PCLK0 polarity
    {IMG_ORIENTATION, 0x01}, // change the orientation
    {0x0104, 0x01}
};

typedef struct {
    union {
        struct {
            uint16_t addr;
            uint8_t value;
        } wr;
        struct {
            uint16_t addr;
        } rd;
    };
} i2c_req_t;

static RT_L2_DATA i2c_req_t i2c_req;
RT_L2_DATA unsigned char valRegHimax;

// TODO: write a status var for cam
RT_FC_DATA unsigned char camera_isAwaked;


void himaxRegWrite(rt_camera_t *cam, unsigned int addr, unsigned char value){
    if (rt_platform() == ARCHI_PLATFORM_FPGA || rt_platform() == ARCHI_PLATFORM_BOARD){
        i2c_req.wr.value = value;
        i2c_req.wr.addr = ((addr >> 8) & 0xff) | ((addr & 0xff) << 8);
        rt_i2c_write(cam->i2c, (unsigned char *)&i2c_req, 3, 0, NULL);
    }
}

unsigned char himaxRegRead(rt_camera_t *cam, unsigned int addr){
    if (rt_platform() == ARCHI_PLATFORM_FPGA || rt_platform() == ARCHI_PLATFORM_BOARD){
        i2c_req.rd.addr = ((addr >> 8) & 0xff) | ((addr & 0xff) << 8);
        rt_i2c_write(cam->i2c, (unsigned char *)&i2c_req, 2, 1, NULL);
        rt_i2c_read(cam->i2c, &valRegHimax, 1, 0, NULL);
    }
    return valRegHimax;
}

static void _himaxBoot(rt_camera_t *cam){
    unsigned int i;
    for(i=0; i<(sizeof(himaxRegInit)/sizeof(himax_reg_cfg_t)); i++){
        himaxRegWrite(cam, himaxRegInit[i].addr, himaxRegInit[i].data);
        rt_trace(RT_TRACE_CAM, "%x = %x \n", himaxRegInit[i].addr, himaxRegRead(cam, himaxRegInit[i].addr));
    }
}

static void _himaxMode(rt_camera_t *cam, unsigned char mode){
    himaxRegWrite(cam, MODE_SELECT, mode);
}

static void _himaxWakeUP (rt_camera_t *cam){
    if (!camera_isAwaked){
        _himaxMode(cam, HIMAX_Streaming);
        camera_isAwaked = 1;
    }
}

void _himaxReset(rt_camera_t *cam){
    himaxRegWrite(cam, SW_RESET, HIMAX_RESET);
    while (himaxRegRead(cam, MODE_SELECT) != HIMAX_Standby){
        himaxRegWrite(cam, SW_RESET, HIMAX_RESET);
        rt_time_wait_us(50);
    }
}

void _himaxStandby(rt_camera_t *cam){
    _himaxMode(cam, HIMAX_Standby);
}

void himaxGrayScale(rt_camera_t *cam, unsigned char value){
    himaxRegWrite(cam, BLC_CFG, ENABLE);
    himaxRegWrite(cam, BLC_TGT, value);
    himaxRegWrite(cam, BLI_EN, ENABLE);
    himaxRegWrite(cam, BLC2_TGT, value);
}

void himaxFrameRate(rt_camera_t *cam){
    himaxRegWrite(cam, FRAME_LEN_LINES_H, 0x02);
    himaxRegWrite(cam, FRAME_LEN_LINES_L, 0x1C);
    himaxRegWrite(cam, LINE_LEN_PCK_H, 0x01);
    himaxRegWrite(cam, LINE_LEN_PCK_L, 0x72);
}

static void _himaxParamInit(rt_camera_t *dev_cam, rt_cam_conf_t *cam_conf){
    cam_conf->cpiCfg = UDMA_CHANNEL_CFG_SIZE_16;
    memcpy(&dev_cam->conf, cam_conf, sizeof(rt_cam_conf_t));
}

// TODO: For each case, should add the configuration of camera if necessary.
static void _himaxConfig(rt_cam_conf_t *cam){

    plp_udma_cg_set(plp_udma_cg_get() | (1<<ARCHI_UDMA_CAM_ID(0)));   // Activate CAM channel
    plpUdmaCamCustom_u _cpi;
    _cpi.raw = 0;
    switch (cam->resolution){
        case QQVGA:
            break;
        case QVGA:
        default:
            _cpi.cfg_size.row_length = ((QVGA_W+4)/2-1);
    }
    hal_cpi_size_set(0, _cpi.raw);

    _cpi.raw = 0;

    switch (cam->format){
        case HIMAX_MONO_COLOR:
        case HIMAX_RAW_BAYER:
            _cpi.cfg_glob.format = BYPASS_BIGEND;
            break;
        default:
            rt_warning("[CAM Himax] No this format, set the format as default: RAW_BAYER\n");
            _cpi.cfg_glob.format = BYPASS_BIGEND;
            break;
    }
    _cpi.cfg_glob.framedrop_enable = cam->frameDrop_en & MASK_1BIT;
    _cpi.cfg_glob.framedrop_value = cam->frameDrop_value & MASK_6BITS;
    _cpi.cfg_glob.frameslice_enable = cam->slice_en & MASK_1BIT;
    _cpi.cfg_glob.shift = cam->shift & MASK_4BITS;
    _cpi.cfg_glob.enable = DISABLE;

    hal_cpi_glob_set(0, _cpi.raw);
    plp_udma_cg_set(plp_udma_cg_get() & ~(1<<ARCHI_UDMA_CAM_ID(0)));
}

void __rt_himax_close(rt_camera_t *dev_cam, rt_event_t *event){
    int irq = rt_irq_disable();
    _camera_stop();
    if (rt_platform() == ARCHI_PLATFORM_FPGA || rt_platform() == ARCHI_PLATFORM_BOARD)
        rt_i2c_close(dev_cam->i2c, NULL);
    rt_free(RT_ALLOC_FC_DATA, (void*)dev_cam, sizeof(rt_camera_t));
    plp_udma_cg_set(plp_udma_cg_get() & ~(1<<ARCHI_UDMA_CAM_ID(0)));
    if (event) __rt_event_enqueue(event);
    rt_irq_restore(irq);
}

void __rt_himax_control(rt_camera_t *dev_cam, rt_cam_cmd_e cmd, void *_arg){
    rt_trace(RT_TRACE_DEV_CTRL, "[CAM] Control command (cmd: %d)\n", cmd);
    unsigned int *arg = (unsigned int *)_arg;
    int irq = rt_irq_disable();
    switch (cmd){
        case CMD_RESOL:
            dev_cam->conf.resolution = *arg;
            break;
        case CMD_FORMAT:
            dev_cam->conf.format = *arg;
            break;
        case CMD_FPS:
            dev_cam->conf.fps = *arg;
            break;
        case CMD_SLICE:
            {
                rt_img_slice_t *slicer = (rt_img_slice_t *) arg;
                _camera_extract(&dev_cam->conf, slicer);
            }
            break;
        case CMD_SHIFT:
            _camera_normlize(&dev_cam->conf, arg);
            break;
        case CMD_FRAMEDROP:
            _camera_drop_frame(&dev_cam->conf, arg);
            break;
        case CMD_INIT:
            _himaxWakeUP(dev_cam);
            _himaxConfig(&dev_cam->conf);
            break;
        case CMD_START:
            _camera_start();
            break;
        case CMD_PAUSE:
            _camera_stop();
            break;
        case CMD_STOP:
            _himaxStandby(dev_cam);
            _camera_stop();
            camera_isAwaked = 0;
            break;
        default:
            rt_warning("[CAM] This Command %d is not disponible for Himax camera\n", cmd);
            break;
    }
    rt_irq_restore(irq);
}

static void __rt_camera_conf_init(rt_camera_t *dev, rt_cam_conf_t* cam){
    _himaxParamInit(dev, cam);
}

rt_camera_t* __rt_himax_open(rt_dev_t *dev, rt_cam_conf_t* cam, rt_event_t*event){

    rt_trace(RT_TRACE_DEV_CTRL, "[CAM] Opening Himax camera\n");

    rt_camera_t *camera = NULL;
    if (dev == NULL) return NULL;
    camera = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_camera_t));
    if (camera == NULL) return NULL;

    camera->dev = dev;
    if (dev->channel != -1)
        camera->channel = dev->channel & 0xf;
    else
        camera->channel = dev->itf + ARCHI_UDMA_CAM_ID(dev->itf);

    __rt_camera_conf_init(camera, cam);

    if (rt_platform() == ARCHI_PLATFORM_FPGA || rt_platform() == ARCHI_PLATFORM_BOARD){

        rt_i2c_conf_init(&camera->i2c_conf);
        camera->i2c_conf.cs = 0x48;
#if PULP_CHIP == CHIP_GAP
        camera->i2c_conf.id = 1;
#else
        camera->i2c_conf.id = 0;
#endif
        camera->i2c_conf.max_baudrate = 200000;

        camera->i2c = rt_i2c_open(NULL, &camera->i2c_conf, NULL);
        if (camera->i2c == NULL) printf ("Filed to open I2C\n");
        // the I2C of Himax freq: 400kHz max.

   }

    soc_eu_fcEventMask_setEvent(ARCHI_UDMA_CAM_ID(0)*2);
    _himaxReset(camera);
    _himaxBoot(camera);

    if (event) __rt_event_enqueue(event);
    return camera;
}

void __rt_himax_capture(rt_camera_t *dev_cam, void *buffer, size_t bufferlen, rt_event_t *event)
{
    rt_trace(RT_TRACE_CAM, "[CAM HIMAX] Capture (buffer: %p, size: 0x%x)\n", buffer, bufferlen);

    int irq = rt_irq_disable();

    rt_event_t *call_event = __rt_wait_event_prepare(event);

    rt_periph_copy_init(&call_event->copy, 0);

    rt_periph_copy(&call_event->copy, UDMA_CHANNEL_ID(dev_cam->channel) + 0, (unsigned int) buffer, bufferlen, dev_cam->conf.cpiCfg, call_event);

    __rt_wait_event_check(event, call_event);

    rt_irq_restore(irq);
}

rt_cam_dev_t himax_desc = {
    .open      = &__rt_himax_open,
    .close     = &__rt_himax_close,
    .control   = &__rt_himax_control,
    .capture   = &__rt_himax_capture
};

RT_FC_BOOT_CODE void __attribute__((constructor)) __rt_himax_init()
{
  camera_isAwaked = 0;
}
