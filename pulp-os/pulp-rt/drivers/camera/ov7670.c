/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#include "rt/rt_api.h"

static RT_L2_DATA ov7670_reg_cfg_t _qvga_conf[] = {
    {REG_COM14, 0x19},
    {0x70, 0x4A},
    {0x71, 0x35},
    {0x72, 0x11},
    {0x73, 0xf1},
    {REG_HSTART,0x16},
    {REG_HSTOP,0x04},
    {REG_HREF,0x24},
    {REG_VSTART,0x02},
    {REG_VSTOP,0x7a},
    {REG_VREF,0x0a},

    //	{0xff, 0xff},	/* END MARKER */
};

static RT_L2_DATA ov7670_reg_cfg_t _rgb565_conf[] = {
    {0x3a, 0x04},
    {REG_COM7, COM7_RGB}, /* Selects RGB mode */
    {REG_RGB444, 0},	  /* No RGB444 please */
    {REG_COM1, 0x0},
    {REG_COM15, COM15_RGB565|COM15_R00FF},
    {REG_COM9, 0x6A},	 /* 128x gain ceiling; 0x8 is reserved bit */
    {0x4f, 0xb3},		 /* "matrix coefficient 1" */
    {0x50, 0xb3},		 /* "matrix coefficient 2" */
    {0x51, 0},		 /* vb */
    {0x52, 0x3d},		 /* "matrix coefficient 4" */
    {0x53, 0xa7},		 /* "matrix coefficient 5" */
    {0x54, 0xe4},		 /* "matrix coefficient 6" */
    {REG_COM13, /*COM13_GAMMA|*/COM13_UVSAT},
    //    {0x55, 0x60},
    //    {0x56, 0x80},
    //	{0xff, 0xff},	/* END MARKER */
    {REG_BRIGHT, 0xC0}
};

static RT_L2_DATA ov7670_reg_cfg_t _yuv422_conf[] = {
    {REG_COM7, 0x0},	/* Selects YUV mode */
    {REG_RGB444, 0},	/* No RGB444 please */
    {REG_COM1, 0},
    {REG_COM15, COM15_R00FF},
    {REG_COM9, 0x6A},	/* 128x gain ceiling; 0x8 is reserved bit */
    {0x4f, 0x80},		/* "matrix coefficient 1" */
    {0x50, 0x80},		/* "matrix coefficient 2" */
    {0x51, 0},		    /* vb */
    {0x52, 0x22},		/* "matrix coefficient 4" */
    {0x53, 0x5e},		/* "matrix coefficient 5" */
    {0x54, 0x80},		/* "matrix coefficient 6" */
    {REG_COM13,/*COM13_GAMMA|*/COM13_UVSAT},

    {0x55, 0x80},
    {0x56, 0x80},
    //    {0x11, 0x12},
    //	{0xff, 0xff},		/* END MARKER */
    {REG_BRIGHT, 0xC0}
};

typedef struct {
    union {
        struct {
            uint8_t addr;
            uint8_t value;
        } wr;
        struct {
            uint8_t addr;
        } rd;
    };
} i2c_req_t;

static RT_L2_DATA i2c_req_t i2c_req;
RT_L2_DATA unsigned char valRegOV7670;
RT_L2_DATA unsigned int regAddrOV7670;
RT_L2_DATA unsigned char __ov7670Inited;


void ov7670RegWrite(rt_camera_t *cam, unsigned char addr, unsigned char value, rt_event_t *event){
    if (rt_platform() == ARCHI_PLATFORM_FPGA || rt_platform() == ARCHI_PLATFORM_BOARD){
        i2c_req.wr.value = value;
        i2c_req.wr.addr = addr;
        rt_i2c_write(cam->i2c, (unsigned char *)&i2c_req, sizeof(i2c_req.wr), 0, NULL);
    }
}

unsigned int ov7670RegRead(rt_camera_t *cam, unsigned char addr, rt_event_t *event){
    if (rt_platform() == ARCHI_PLATFORM_FPGA || rt_platform() == ARCHI_PLATFORM_BOARD){
        i2c_req.rd.addr = addr;
        rt_i2c_write(cam->i2c, (unsigned char *)&i2c_req, sizeof(i2c_req.rd), 1, NULL);
        rt_i2c_read(cam->i2c, &valRegOV7670, 1, 0, NULL);
    }
    return valRegOV7670;
}


static void regDump(rt_camera_t *cam){
    for (unsigned int i=0; i<=0xC9; i++)
        printf("addr: %X, value: %X\n", i, ov7670RegRead(cam, i, NULL));
}

void ov7670_reset(rt_camera_t *cam){
    ov7670RegWrite(cam, REG_COM7, COM7_RESET, NULL);
    for (volatile int i=0; i<50000; i++);
}

static void _ov7670RegInit(rt_camera_t *cam){
    unsigned int i;
    ov7670_reset(cam);
    /*
       for(i=0; i<(sizeof(ov7670RegConf)/sizeof(ov7670_reg_cfg_t)); i++){
       ov7670RegWrite(cam, ov7670RegConf[i].addr, ov7670RegConf[i].data, NULL);
       }
       */
    rt_trace(RT_TRACE_DEV_CTRL, "Cam ov7670 inited\n");
}

static void _ov7670_rgb565(rt_camera_t *cam){
    unsigned int i;
    for(i=0; i<(sizeof(_rgb565_conf)/sizeof(ov7670_reg_cfg_t)); i++){
        ov7670RegWrite(cam, _rgb565_conf[i].addr, _rgb565_conf[i].data, NULL);
    }
    unsigned char value = ov7670RegRead(cam, REG_CLKRC, NULL);
    // TODO, to be replaced by a wait xx us function.
    for(i=0; i<5000; i++);
    ov7670RegWrite(cam, REG_CLKRC, value, NULL);
}

static void _ov7670_yuv(rt_camera_t *cam){
    for(unsigned int i=0; i<(sizeof(_yuv422_conf)/sizeof(ov7670_reg_cfg_t)); i++){
        ov7670RegWrite(cam, _yuv422_conf[i].addr, _yuv422_conf[i].data, NULL);
    }
}

static void _ov7670ParamInit(rt_camera_t *dev_cam, rt_cam_conf_t *cam_conf){
    memcpy(&dev_cam->conf, cam_conf, sizeof(rt_cam_conf_t));
}

// TODO: For each case, should add the configuration of camera if necessary.
static void _ov7670ConfigAndEnable(rt_camera_t *cam){
    plpUdmaCamCustom_u _cpi;
    _cpi.raw = 0;
    switch (cam->conf.resolution){
        case QVGA:
            goto qvga;
        case QQVGA:
            break;

        default:
qvga:
            if (!__ov7670Inited){
                ov7670RegWrite(cam, REG_COM3, 0x4, NULL);
                for(unsigned int i=0; i<(sizeof(_qvga_conf)/sizeof(ov7670_reg_cfg_t)); i++)
                    ov7670RegWrite(cam, _qvga_conf[i].addr, _qvga_conf[i].data, NULL);
            }
            _cpi.cfg_size.row_length = (QVGA_W-1) & MASK_16BITS;

    }
    hal_cpi_size_set(0, _cpi.raw);
    _cpi.raw = 0;

    switch (cam->conf.format){
        case OV7670_RGB565:
            if (!__ov7670Inited)
                _ov7670_rgb565(cam);
            _cpi.cfg_glob.format = RGB565;
            break;
        case OV7670_RGB555:
            //TODO: configure ov7670 as rgb555
            _cpi.cfg_glob.format = RGB555;
            break;
        case OV7670_RGB444:
            //TODO: configure ov7670 as rgb444
            _cpi.cfg_glob.format = RGB444;
            break;
        case OV7670_YUV422:
            if (!__ov7670Inited)
                _ov7670_yuv(cam);
            _cpi.cfg_glob.format = BYPASS_LITEND;
            break;
        case OV7670_MONO_COLOR:
            if (!__ov7670Inited)
                _ov7670_yuv(cam);
            _cpi.cfg_glob.format = BYPASS_LITEND;
            cam->conf.cpiCfg = UDMA_CHANNEL_CFG_SIZE_8;
            break;
    }
    _cpi.cfg_glob.framedrop_enable = cam->conf.frameDrop_en & MASK_1BIT;
    _cpi.cfg_glob.framedrop_value = cam->conf.frameDrop_value & MASK_6BITS;
    _cpi.cfg_glob.frameslice_enable = cam->conf.slice_en & MASK_1BIT;
    _cpi.cfg_glob.shift = cam->conf.shift & MASK_4BITS;
    _cpi.cfg_glob.enable = ENABLE;

    hal_cpi_glob_set(0, _cpi.raw);
    __ov7670Inited = 1;
}

void __rt_ov7670_close(rt_camera_t *dev_cam, rt_event_t *event){
    int irq = rt_irq_disable();
    _camera_stop();
    rt_camera_t *cam = (rt_camera_t *) dev_cam;
    rt_free(RT_ALLOC_FC_DATA, (void*)dev_cam, sizeof(rt_camera_t));
    plp_udma_cg_set(plp_udma_cg_get() & ~(1<<ARCHI_UDMA_CAM_ID(0)));
    __ov7670Inited = 0;
    rt_irq_restore(irq);
}

void __rt_ov7670_control(rt_camera_t *dev_cam, rt_cam_cmd_e cmd, void *_arg){
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
        case CMD_FILTER:
            {
                rt_img_filter_t *filter = (rt_img_filter_t *) arg;
                _camera_filter(&dev_cam->conf, filter);
            }
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
        case CMD_START:
        case CMD_INIT:
            _ov7670ConfigAndEnable(dev_cam);
            break;
        case CMD_STOP:
        case CMD_PAUSE:
            _camera_stop();
            break;
    }
    rt_irq_restore(irq);
}

static void rt_ov7670_conf_init(rt_camera_t *dev_cam, rt_cam_conf_t* cam){
    _ov7670ParamInit(dev_cam, cam);
}

rt_camera_t* __rt_ov7670_open(rt_dev_t *dev, rt_cam_conf_t* cam, rt_event_t*event){
    rt_trace(RT_TRACE_DEV_CTRL, "[CAM] Opening ov7670 camera\n");

    rt_camera_t *camera = NULL;
    if (dev == NULL) return NULL;
    camera = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_camera_t));
    if (camera == NULL) return NULL;

    camera->dev = dev;
    camera->channel = dev->channel & 0xf;
    rt_ov7670_conf_init(camera, cam);

    if (rt_platform() == ARCHI_PLATFORM_FPGA || rt_platform() == ARCHI_PLATFORM_BOARD){
        rt_i2c_conf_init(&camera->i2c_conf);
        camera->i2c_conf.cs = 0x42;
        camera->i2c_conf.id = 1;
        camera->i2c_conf.max_baudrate = 200000;

        camera->i2c = rt_i2c_open(NULL, &camera->i2c_conf, NULL);
        if (camera->i2c == NULL) printf ("Filed to open I2C\n");
    }

    soc_eu_fcEventMask_setEvent(ARCHI_UDMA_CAM_ID(0)*2);
    _ov7670RegInit(camera);
    if (event) rt_event_push(event);
    return camera;
}

void __rt_ov7670_capture(rt_camera_t *dev_cam, void *buffer, size_t bufferlen, rt_event_t *event)
{
    rt_trace(RT_TRACE_CAM, "[CAM OV7670] Capture (buffer: %p, size: 0x%x)\n", buffer, bufferlen);

    int irq = rt_irq_disable();

    rt_event_t *call_event = __rt_wait_event_prepare(event);

    rt_periph_copy_init(&call_event->copy, 0);

    rt_periph_copy(&call_event->copy, UDMA_CHANNEL_ID(dev_cam->channel) + 0, (unsigned int) buffer, bufferlen, dev_cam->conf.cpiCfg, call_event);

    __rt_wait_event_check(event, call_event);

    rt_irq_restore(irq);
}

rt_cam_dev_t ov7670_desc = {
    .open      = &__rt_ov7670_open,
    .close     = &__rt_ov7670_close,
    .control   = &__rt_ov7670_control,
    .capture   = &__rt_ov7670_capture
};

RT_FC_BOOT_CODE void __attribute__((constructor)) __rt_ov7670_init()
{
  __ov7670Inited = 0;
}
