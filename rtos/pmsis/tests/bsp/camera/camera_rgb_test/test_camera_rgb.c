#include "stdio.h"

/* PMSIS includes. */
#include "pmsis.h"

/* PMSIS BSP includes. */
#include "bsp/bsp.h"
#include "bsp/camera.h"

/* Demo includes. */
#ifdef SAVE_PIC 
#include "gaplib/ImgIO.h"
#endif

#ifdef SMALL_IMG  
#ifdef SLICE_MODE
#define X                   4 
#define Y                   6 
#define CAMERA_WIDTH        24
#define CAMERA_HEIGHT       18
#else /* NOT SLICE MODE */
#define X                   0 
#define Y                   0 

#define CAMERA_WIDTH        40
#define CAMERA_HEIGHT       40
#endif
#define ROWLEN              40

#else

#ifdef SLICE_MODE
#define X                   20 
#define Y                   60 
#define CAMERA_WIDTH        224
#define CAMERA_HEIGHT       180
#else /* NOT SLICE MODE */
#define X                   0 
#define Y                   0 

#define CAMERA_WIDTH        254
#define CAMERA_HEIGHT       254
#endif
#define ROWLEN              254
#endif
#define PIXEL_SIZE          3     // In RGB mode, the output will always be RGB888 (3bytes)


static pi_task_t task;
static uint8_t *imgBuff0;
static struct pi_device cam;
static uint8_t pixel_size = PIXEL_SIZE;

static char imgName[50];
static uint32_t idx = 0;

static void cam_handler(void *arg);
static void lcd_handler(void *arg);

static void dump_reg()
{
#define CPI_BASE	(0x1a102A00)
    printf("reg CFG_DEST: %x \n",       *(volatile unsigned int *)(long)(CPI_BASE+0x0));
    printf("reg CFG_DATASIZE: %x \n",   *(volatile unsigned int *)(long)(CPI_BASE+0x4));
    printf("reg CFG_GLOB: %x \n",       *(volatile unsigned int *)(long)(CPI_BASE+0x20));
    printf("reg CFG_LL: %x \n",         *(volatile unsigned int *)(long)(CPI_BASE+0x24));
    printf("reg CFG_UR: %x \n",         *(volatile unsigned int *)(long)(CPI_BASE+0x28));
    printf("reg CFG_SIZE: %x \n",       *(volatile unsigned int *)(long)(CPI_BASE+0x2C));
    printf("reg CFG_RGB: %x \n",        *(volatile unsigned int *)(long)(CPI_BASE+0x30));
    printf("reg CFG_POLARITY: %x \n",   *(volatile unsigned int *)(long)(CPI_BASE+0x34));
}

// Since this has been packaged deeply in the driver, we can use simple write to test.
static void change_cpi_datasize (uint32_t datasize)
{
    /* datasize : 00: 8bit 01: 16bit 10: 24bit 11: 32bit */
    *(volatile unsigned int *)(long)(CPI_BASE+0x4) = datasize & 0x3;
}


static void change_cpi_img_format (uint8_t format)
{
    uint32_t glob_reg_value = *(volatile unsigned int *)(long)(CPI_BASE+0x20);
    glob_reg_value = glob_reg_value & (~0x700); // clean the format bits
    glob_reg_value = glob_reg_value | (format << 8); // set new format
    *(volatile unsigned int *)(long)(CPI_BASE+0x20) = glob_reg_value;
}

static void change_cpi_rowlen (uint32_t rowlen)
{
    *(volatile unsigned int *)(long)(CPI_BASE+0x2C) = rowlen & 0xFFFF;
}

static void change_cpi_slice (uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    *(volatile unsigned int *)(long)(CPI_BASE+0x24) = ((y&0xFFFF)<<16 | x);
    *(volatile unsigned int *)(long)(CPI_BASE+0x28) = (((y+h)&0xFFFF)<<16 | (x+w));
}

static void change_cpi_sequence (uint8_t seq)
{
    *(volatile unsigned int *)(long)(CPI_BASE+0x30) = seq & 0x7;
}

static uint32_t pixel_value (int format, int w, int h)
{
    if (format == 2)
    {
        // construct the image 16bits with (x,y)
        return ((w&0xFF)<<8 | (h&0xFF));
    }
    else if (format == 3)
    {
        // construct the image 24 bits with (x, y, z), where z = x+y;
        return (((w&0xFF)<<16) | (h&0xFF) <<8 | ((w+h)&0xFF));
    }
    else
        printf("error: no this format\n");

    return -1;
}

static uint32_t pixel_value_rebuild(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t value = 0;
    //printf("r = %x, g = %x, b = %x\n", r,g,b&0xFF);
#if defined(RGB565)
    value = (((r&0xF8)|(g>>5))&0xFF)<<8 | (((g&0x1C)<<3 | (b>>3) )& 0xFF);
#elif defined(RGB555) 
    value = ((((r&0xF8)>>1)|(g>>6))<<8 | ((g&0x38)<<2 | (b>>3))) & 0x7FFF;
#elif defined(RGB444) 
    value = (((r>>4)<<8) | (((g>>4)<<4) | (b>>4))) & 0xFFF;
#elif defined(RGB888)
    value = ((r<<16) | (g<<8) | b) & 0xFFFFFF;
#endif
    return value;
}

static int picture_check()
{
    uint32_t ref_pix = 0;
    uint32_t pixel = 0;
    uint8_t r=0, g=0, b=0;

#if defined(RGB888)
    uint8_t _p_size = 3;
#else
    uint8_t _p_size = 2;
#endif

    for (uint h=0; h<CAMERA_HEIGHT; h++)
    {
        for (uint w=0; w<CAMERA_WIDTH; w++)
        {

// Since it's little endian when copy into L2 
#if defined(RGB)
            b = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 0]&0xFF);
            g = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 1]&0xFF);
            r = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 2]&0xFF);
#elif defined(RBG)
            g = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 0]&0xFF);
            b = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 1]&0xFF);
            r = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 2]&0xFF);
#elif defined(BRG)
            g = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 0]&0xFF);
            r = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 1]&0xFF);
            b = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 2]&0xFF);
#elif defined(BGR)
            r = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 0]&0xFF);
            g = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 1]&0xFF);
            b = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 2]&0xFF);
#elif defined(GRB)
            b = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 0]&0xFF);
            r = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 1]&0xFF);
            g = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 2]&0xFF);
#elif defined(GBR)
            r = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 0]&0xFF);
            b = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 1]&0xFF);
            g = (imgBuff0[(h*CAMERA_WIDTH + w)*3 + 2]&0xFF);
#endif

            ref_pix = pixel_value(_p_size, w+X, h+Y)
#if defined(RGB555)
		    &0x7FFF
#elif defined(RGB444) 
		    &0xFFF
#endif
		    ;
            pixel = pixel_value_rebuild(r, g, b);

            if(ref_pix != pixel)
            {
                printf("pixel: (w=%d, h=%d) = %x (ref: %x)\n", w+X, h+Y, pixel, ref_pix);
		printf("r = %x, g = %x, b = %x\n", r,g,b&0xFF);
                return 1;
            }
        }
    }
    return 0;
}

static void cam_handler(void *arg)
{
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
}

static int32_t open_camera_himax(struct pi_device *device)
{
    struct pi_himax_conf cam_conf;
    pi_himax_conf_init(&cam_conf);


#ifdef SLICE_MODE
    cam_conf.format = ROWLEN;  
    cam_conf.roi.slice_en = 1;
    cam_conf.roi.x = X;
    cam_conf.roi.y = Y;
    cam_conf.roi.w = CAMERA_WIDTH;
    cam_conf.roi.h = CAMERA_HEIGHT;
#endif

    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    {
        return -1;
    }

    return 0;
}

void test_camera_rgb(void)
{
    #if !defined(__GAP8__)
    // Set pull-down to I2C0 pads to avoid 'X' propagation in netlist tests
    uint32_t *pad_cfg_10 = (uint32_t *) (ARCHI_APB_SOC_CTRL_ADDR + 0x58);
    // Activate the pull-down of the I2C SCK and SDA pads to avoid
    // 'x' propagation and test failure in netlist tests
    *pad_cfg_10 = (*pad_cfg_10) | (1 << 8) | (1 << 0);
    #endif  /* __GAP8__ */

    printf("Entering main controller...\n");
    int32_t errors = 0;

    imgBuff0 = (uint8_t *) pmsis_l2_malloc(CAMERA_WIDTH * CAMERA_HEIGHT * pixel_size);
    if (imgBuff0 == NULL)
    {
        printf("Failed to allocate Memory for Image, asking for: %d * %d\n", CAMERA_WIDTH, CAMERA_HEIGHT);
        pmsis_exit(-1);
    }

    errors = open_camera_himax(&cam);
    if (errors)
    {
        printf("Failed to open camera : %ld\n", errors);
        pmsis_exit(-2);
    }
    printf("camera opened\n");


    // The transfert datasize will be 24bits - 10: 24bit
    change_cpi_datasize (pixel_size-1);
    change_cpi_rowlen(ROWLEN);

#if defined(RGB565)
    change_cpi_img_format(PI_CPI_FORMAT_RGB565);
#elif defined(RGB555) 
    change_cpi_img_format(PI_CPI_FORMAT_RGB555);
#elif defined(RGB444) 
    change_cpi_img_format(PI_CPI_FORMAT_RGB444);
#elif defined(RGB888) 
    change_cpi_img_format(PI_CPI_FORMAT_RGB888);
#endif

#if defined(RGB)
    change_cpi_sequence (0);
#elif defined(RBG)
    change_cpi_sequence (1);
#elif defined(GRB)
    change_cpi_sequence (2);
#elif defined(GBR)
    change_cpi_sequence (3);
#elif defined(BRG)
    change_cpi_sequence (4);
#elif defined(BGR)
    change_cpi_sequence (5);
#endif
    
#ifdef SLICE_MODE
    // Change the slice ROI and rowlen here, since the bsp is made for himax
    change_cpi_slice(X, Y, CAMERA_WIDTH, CAMERA_HEIGHT);
#endif

#ifdef DEBUG_REG 
    dump_reg();
#endif

    for (idx=0; idx<2; idx++)
    {
#if defined(ASYNC)
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        pi_task_callback(&task, cam_handler, NULL);
        pi_camera_capture_async(&cam, imgBuff0, CAMERA_WIDTH * CAMERA_HEIGHT * pixel_size, &task);
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_task_wait_on(&task);
#else
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam, imgBuff0, CAMERA_WIDTH * CAMERA_HEIGHT * pixel_size);
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);

#endif  /* ASYNC */

        errors += picture_check();
        if (errors)
            break;
    }

    printf("Exiting...%d errors\n", errors);
    pmsis_exit(errors);
}

int main(void)
{
    printf("\n\t*** PMSIS Camera with Himax ***\n\n");
    return pmsis_kickoff((void *) test_camera_rgb);
}
