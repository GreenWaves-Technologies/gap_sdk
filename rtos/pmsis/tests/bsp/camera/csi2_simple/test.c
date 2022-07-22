/*
 * Copyright (C) 2019 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/camera.h"
#include "gaplib/ImgIO.h"
#ifdef OV5647
#include "bsp/camera/ov5647.h"
#else
#include "bsp/camera/ov9281.h"
#endif


#ifdef SIZE_640_400
    #define WIDTH    640
    #define HEIGHT   400
#else
#ifdef SIZE_1280_800
    #define WIDTH    1280
    #define HEIGHT   800
#else
#ifdef QVGA
    #define WIDTH    320
    #define HEIGHT   240
#else
    #define WIDTH    640
    #define HEIGHT   480
#endif
#endif // SIZE_1280_800
#endif // SIZE_640_400

#ifdef OV5647
#ifdef RAW8
#define BUFFER_SIZE   WIDTH*HEIGHT*2 // For now only 10 bits config works
// We get data on 10 bits and delete the 2 LSB after
// Therefore we still need 2 bytes when getting pixels from camera
#else
#define BUFFER_SIZE   WIDTH*HEIGHT*2
#endif // RAW8
#else
#define BUFFER_SIZE   WIDTH*HEIGHT
#endif

pi_device_t i2c = {0};

PI_L2 unsigned char *buff[2];

#ifdef LOGS
#define PRINTF printf
#else
#define PRINTF(...) ((void) 0)
#endif


static int open_camera(struct pi_device *device)
{
	PRINTF("Opening CSI2 camera\n");
#ifdef OV5647
	struct pi_ov5647_conf cam_conf;
	pi_ov5647_conf_init(&cam_conf);
#else
	struct pi_ov9281_conf cam_conf;
	pi_ov9281_conf_init(&cam_conf);
#endif

	#if !defined(__GAP8__)
	// Set pull-down to I2C0 pads to avoid 'X' propagation in netlist tests
	uint32_t *pad_cfg_10 = (uint32_t *) (ARCHI_APB_SOC_CTRL_ADDR + 0x58);
	// Activate the pull-down of the I2C SCK and SDA pads to avoid
	// 'x' propagation and test failure in netlist tests
	*pad_cfg_10 = (*pad_cfg_10) | (1 << 8) | (1 << 0);
	#endif  /* __GAP8__ */

	pi_open_from_conf(device, &cam_conf);
	if (pi_camera_open(device))
		return -1;

	return 0;
}

int write_reg8(pi_device_t *dev, uint8_t addr, uint8_t value)
{
    uint8_t buffer[2] = { addr, value };
    if (pi_i2c_write(dev, buffer, 2, PI_I2C_XFER_START | PI_I2C_XFER_STOP))
    {
        printf("Error when writing \n");
        return -1;
    }

    return 0;
}

static uint8_t read_reg8(pi_device_t *dev, uint8_t addr)
{
    uint8_t result;
    pi_i2c_write_read(dev, &addr, &result, 1, 1);
    return result;
}

// Setup FXL6408 GPIO Expander
// Datasheet https://www.onsemi.com/pdf/datasheet/fxl6408-d.pdf
static int fxl6408_setup()
{
    PRINTF("Setup fxl6408\n");

    struct pi_i2c_conf conf;
    pi_i2c_conf_init(&conf);
    conf.itf = 3;
    conf.max_baudrate = 100000;
    pi_i2c_conf_set_slave_addr(&conf, 0x88, 0);
    PRINTF("*** check 1 ***\n");

    pi_open_from_conf(&i2c, &conf);
    PRINTF("*** check 2 ***\n");

    if (pi_i2c_open(&i2c))
    {
        printf("*** TEST FAILED ***\n");
        return -1;
    }
    PRINTF("*** check 3 ***\n");

    // Turn-on fxl6408
    write_reg8(&i2c, 0x01, 0x1); // SW reset the GPIO expander
    write_reg8(&i2c, 0x03, 0x1); // Direction: GPIO0 -> Output
    write_reg8(&i2c, 0x05, 0x1); // Output state: GPIO0 -> High
    write_reg8(&i2c, 0x07, 0x0); // Output state follow the 0x05 (above)
    PRINTF("*** check 4 ***\n");

    PRINTF("*** SETUP fxl6408 OK ***\n");
    return 0;
}

static int test_entry()
{
    #ifdef OV5647
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, 40*1000*1000);
    pi_freq_set(PI_FREQ_DOMAIN_FC, 40*1000*1000);
    #else
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, 25*1000*1000);
    pi_freq_set(PI_FREQ_DOMAIN_FC, 25*1000*1000);
    #endif


    PRINTF("Entering main controller\n");

    // Going to I2C3 to access IO expander
    pi_pad_set_function(PI_PAD_046, PI_PAD_FUNC2); // I2C3 SDA
    pi_pad_set_function(PI_PAD_047, PI_PAD_FUNC2); // I2C3 SCL
    PRINTF("***** Step 1 OK *****\n");


    struct pi_device gpio_ic_3v3;
    struct pi_gpio_conf gpio_conf = {0};
    pi_gpio_e gpio_pin_o = PI_GPIO_A00;

    pi_gpio_pin_write(&gpio_ic_3v3, gpio_pin_o, 1);
    pi_time_wait_us(1000000);
    PRINTF("***** Step 2 OK *****\n");


    fxl6408_setup();
    PRINTF("***** Step 3 OK *****\n");


    // Coming back to I3C to open the camera
	pi_pad_set_function(PI_PAD_046, PI_PAD_FUNC0); // I3C SDA
    pi_pad_set_function(PI_PAD_047, PI_PAD_FUNC0); // I3C SCL
	PRINTF("***** Step 4 OK *****\n");


    struct pi_device camera;
    pi_task_t task;

    if (open_camera(&camera))
    {
        printf("Failed to open camera\n");
        goto error;
    }

    buff[0] = pmsis_l2_malloc(BUFFER_SIZE);
    if (buff[0] == NULL) goto error;

    pi_camera_capture_async(&camera, buff[0], BUFFER_SIZE, pi_task_block(&task));
    pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);

    pi_task_wait_on(&task);

    pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);


    pi_camera_close(&camera);
    pi_i2c_close(&i2c);
    pi_gpio_pin_write(&gpio_ic_3v3, PI_GPIO_A00, 0);

    printf("\nTest success\n");

#ifdef RAW8
    for (int i = 0; i < HEIGHT; i++) { // Put pixels on 8 bits instead of 10 to go on 1 byte encoding only
        for (int j = 0; j < WIDTH; j++) {
            // Shifts bits to delete the 2 LSB, on the 10 useful bits
            buff[0][i * WIDTH + j] = buff[0][(i * WIDTH + j) *2 +1] << 6 | (buff[0][(i * WIDTH + j) *2] >> 2);
        }
    }
#endif

#ifdef SAVE_PIC
    printf("\nSaving image...\n");
#ifdef OV5647
#ifdef RAW8
    WriteImageToFile("Out.ppm", WIDTH, HEIGHT, sizeof(uint8_t), buff[0], BYPASS_IO);
#else
    // bayer RAW10,  2 bytes, only 10bit effective
    WriteImageToFile("Out.ppm", WIDTH, HEIGHT, sizeof(uint16_t), buff[0], BYPASS_IO);
#endif // RAW8
#else
    WriteImageToFile("Out.ppm", WIDTH, HEIGHT, sizeof(uint8_t), buff[0], GRAY_SCALE_IO);
#endif
#endif

    return 0;

error:
    printf("Test failure\n");
    return -1;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
