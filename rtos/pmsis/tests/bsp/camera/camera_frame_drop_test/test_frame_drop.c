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

static pi_task_t task;
static uint8_t *imgBuff0;
static uint8_t *imgBuff1;
static struct pi_device cam;
static uint8_t pixel_size = PIXEL_SIZE;
static uint8_t drop = DROP;

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

static void change_cpi_sequence (uint8_t seq)
{
    *(volatile unsigned int *)(long)(CPI_BASE+0x30) = seq & 0x3;
}

static void change_frame_drop (uint8_t nb_frame_drop)
{
    uint32_t glob_reg_value = *(volatile unsigned int *)(long)(CPI_BASE+0x20);
    glob_reg_value = glob_reg_value & (~0x7F); // clean the frame drop bits
    glob_reg_value = glob_reg_value | (nb_frame_drop << 1); // set frame drop size
    glob_reg_value = glob_reg_value | 0x1; // set frame drop enable
    *(volatile unsigned int *)(long)(CPI_BASE+0x20) = glob_reg_value;
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
    else if (format == 1)
        return ((w+h+X+Y)&0xFF);
    else
        printf("error: no this format\n");

    return -1;
}

static uint32_t pixel_value_rebuild(uint8_t *imgBuff, uint16_t w, uint16_t h)
{
    uint32_t value = 0;

#if defined(BYPASS)
    if(pixel_size == 2)
    {
        value = ((imgBuff[(h*CAMERA_WIDTH + w)*2 + 0]&0xFF)<<8) | (imgBuff[(h*CAMERA_WIDTH + w)*2 + 1]&0xFF);
    }
    else
    {
        value = imgBuff[h*CAMERA_WIDTH + w];
    }
#else
    uint8_t b = (imgBuff[(h*CAMERA_WIDTH + w)*3 + 0]&0xFF);
    uint8_t g = (imgBuff[(h*CAMERA_WIDTH + w)*3 + 1]&0xFF);
    uint8_t r = (imgBuff[(h*CAMERA_WIDTH + w)*3 + 2]&0xFF);
#if defined(RGB565)
    value = (((r&0xF8)|(g>>5))&0xFF)<<8 | (((g&0x1C)<<3 | (b>>3) )& 0xFF);
#elif defined(RGB555) 
    value = ((((r&0xF8)>>1)|(g>>6))<<8 | ((g&0x38)<<2 | (b>>3))) & 0x7FFF;
#elif defined(RGB444) 
    value = (((r>>4)<<8) | (((g>>4)<<4) | (b>>4))) & 0xFFF;
#elif defined(RGB888)
    value = ((r<<16) | (g<<8) | b) & 0xFFFFFF;
#endif
#endif

    return value;
}

static int picture_check()
{
    uint32_t ref_pix = 0;
    uint32_t pixel_buf0 = 0;
    uint32_t pixel_buf1 = 0;
    uint8_t  r=0, g=0, b=0;
    uint8_t  frame_id0 = imgBuff0[0];
    uint8_t  frame_id1 = imgBuff1[0];

#if defined(RGB888)                                                                                 
    uint8_t _p_size = 3;                                                                            
#elif defined(BYPASS)                                                                                               
    uint8_t _p_size = 1;                                                                           
#else
    uint8_t _p_size = 2;                                                                           
#endif

    if (frame_id0 != frame_id1)
    {
	printf("frame id0 : %x, frame_id1 : %x\n", frame_id0, frame_id1);
   	return 1; 
    }
    for (uint16_t h=0; h<CAMERA_HEIGHT; h++)
    {
        for (uint16_t w=(h==0?1:0); w<CAMERA_WIDTH; w++)
        {
            ref_pix = pixel_value(_p_size, w+X, h+Y)
#if defined(RGB555)
		    &0x7FFF
#elif defined(RGB444) 
		    &0xFFF
#endif
		    ;
            pixel_buf0 = pixel_value_rebuild(imgBuff0, w, h)
#if defined(RGB555)
		    &0x7FFF
#elif defined(RGB444) 
		    &0xFFF
#endif
		    ;
            pixel_buf1 = pixel_value_rebuild(imgBuff1, w, h)
#if defined(RGB555)
		    &0x7FFF
#elif defined(RGB444) 
		    &0xFFF
#endif
		    ;

            if(ref_pix != pixel_buf0 || ref_pix != pixel_buf1)
            {
                printf("pixel: (w=%d, h=%d) = %x | %x (ref: %x)\n", w+X, h+Y, pixel_buf0, pixel_buf1, ref_pix);
                return 1;
            }
        }
    }
    return 0;
}

static int32_t open_camera_himax(struct pi_device *device)
{
    struct pi_himax_conf cam_conf;
    pi_himax_conf_init(&cam_conf);

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
    imgBuff1 = (uint8_t *) pmsis_l2_malloc(CAMERA_WIDTH * CAMERA_HEIGHT * pixel_size);
    if (imgBuff0 == NULL || imgBuff1 == NULL)
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
    change_cpi_datasize ((pixel_size-1)? (pixel_size-1) : pixel_size);

#if defined(RGB565)
    change_cpi_img_format(PI_CPI_FORMAT_RGB565);
#elif defined(RGB555) 
    change_cpi_img_format(PI_CPI_FORMAT_RGB555);
#elif defined(RGB444) 
    change_cpi_img_format(PI_CPI_FORMAT_RGB444);
#elif defined(RGB888) 
    change_cpi_img_format(PI_CPI_FORMAT_RGB888);
#else
    change_cpi_img_format(PI_CPI_FORMAT_BYPASS_BIGEND);
#endif

#ifdef DEBUG_REG 
    dump_reg();
#endif
    change_frame_drop(drop);

    for (int i=0; i<2; i++)
    {
	pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
	pi_camera_capture(&cam, imgBuff0, CAMERA_WIDTH * CAMERA_HEIGHT * pixel_size);
	pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);

	pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
	pi_camera_capture(&cam, imgBuff1, CAMERA_WIDTH * CAMERA_HEIGHT * pixel_size);
	pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);

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
