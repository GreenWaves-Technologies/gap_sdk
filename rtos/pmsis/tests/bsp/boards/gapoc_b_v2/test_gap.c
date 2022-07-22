/* PMSIS includes. */
#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/camera.h"

//#define ASYNC

/* App includes. */
#include "gaplib/ImgIO.h"

#define DEBUG_PRINTF(...) ((void) 0)

static int32_t open_camera_thermeye(struct pi_device *device)
{
    struct pi_thermeye_conf cam_conf;
    pi_thermeye_conf_init(&cam_conf);
    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    {
        return -1;
    }
    return 0;
}

void set_pad_to_f(){
    //PAD FUN REG 0
    //b31                                                  b0
    //A41 B38 A42 B39 A37 A43 B40 A44 B1 A2 B2 A3 B4 A5 B3 A4
    uint32_t* reg0 = 0x1A104140;
    //PAD FUN REG 1
    //b31                                                                     b0
    //A7 B6 | B14 A15 | B13 A14 | B12 A13 | B11 D1 | B34 A36 | A38 B36 | A40 B37
    uint32_t* reg1 = 0x1A104144;
    //PAD FUN REG 1
    //b31                                                    b0
    //B23 A26 A24 A25 B22 B9 A16 B15 A9 B7 A8 B8 A10 B10 A11 D2
    uint32_t* reg2 = 0x1A104148;

    *reg0 = 0xffffffff;
    *reg1 = 0xffffffff;
    *reg2 = 0xffffffff;
    
    PRINTF("PAD SET to : %x %x %x\n",*reg0,*reg1,*reg2); 
}

void set_pad_to_0(){
    //PAD FUN REG 0
    //b31                                                  b0
    //A41 B38 A42 B39 A37 A43 B40 A44 B1 A2 B2 A3 B4 A5 B3 A4
    uint32_t* reg0 = 0x1A104140;
    //PAD FUN REG 1
    //b31                                                                     b0
    //A7 B6 | B14 A15 | B13 A14 | B12 A13 | B11 D1 | B34 A36 | A38 B36 | A40 B37
    uint32_t* reg1 = 0x1A104144;
    //PAD FUN REG 1
    //b31                                                    b0
    //B23 A26 A24 A25 B22 B9 A16 B15 A9 B7 A8 B8 A10 B10 A11 D2
    uint32_t* reg2 = 0x1A104148;

    *reg0 = 0x00000000;
    *reg1 = 0x00000000;
    *reg2 = 0x00000000;
    
    PRINTF("PAD SET to : %x %x %x\n",*reg0,*reg1,*reg2); 
}

void test_therm_eye()
{
    PRINTF("Entering test thermeye\n");
    
    #define IMG_WIDTH  ( 80 )
    #define IMG_HEIGHT ( 80 )
    #define IMG_SIZE   ( IMG_HEIGHT * IMG_WIDTH )

    struct pi_device cam;

    uint16_t *calib_buffer = pmsis_l2_malloc(IMG_SIZE*sizeof(uint16_t));
    uint16_t *img_buffer   = pmsis_l2_malloc(IMG_SIZE*sizeof(uint16_t));
    int16_t  *ref_buffer   = pmsis_l2_malloc(IMG_SIZE*sizeof(uint16_t));
    int16_t  *dif_buff     = pmsis_l2_malloc(IMG_SIZE*sizeof(uint16_t));
    uint8_t  *scaled_buff  = pmsis_l2_malloc(IMG_SIZE*sizeof(uint8_t));

    if (open_camera_thermeye(&cam))
    {
        printf("Thermal Eye camera open failed !\n");
        pmsis_exit(-1);
    }
    pi_task_t cb = {0};
    pi_task_block(&cb);

    PRINTF("Cover sensor until end of calibration(led will switch off).\n");
    /* Calibration step. */
    pi_time_wait_us(3 * 1000 * 1000);
    pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);

    #if defined(ASYNC)
    pi_task_block(&cb);
    pi_camera_capture_async(&cam, calib_buffer, IMG_SIZE * sizeof(uint16_t), &cb);
    pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
    pi_task_wait_on(&cb);
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
    #else
    pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
    pi_camera_capture(&cam, calib_buffer, IMG_SIZE * sizeof(uint16_t));
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
    #endif  /* ASYNC */

    uint32_t frame_avg = 0;
    for (uint32_t i = 0; i < IMG_SIZE; i++)
    {
        frame_avg += calib_buffer[i];
    }
    PRINTF("\nFrame avg %ld nb_pixels %ld\n", frame_avg, (uint32_t) IMG_SIZE);
    frame_avg = frame_avg / (uint32_t) IMG_SIZE;
    for (uint32_t i = 0; i < IMG_SIZE; i++)
    {
        ref_buffer[i] = calib_buffer[i] - frame_avg;
    }

    PRINTF("Calibration done !\n");
    /* Wait some time after calibration. */
    pi_time_wait_us(2 * 1000 * 1000);

    uint32_t save_index = 0;
    uint32_t loop = 1;
    while (loop<3)
    {
        PRINTF("\nCapture image %ld\n", loop);

        /* Capture image. */
        #if defined(ASYNC)
        pi_task_block(&cb);
        pi_camera_capture_async(&cam, img_buffer, IMG_SIZE * sizeof(uint16_t), &cb);
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_task_wait_on(&cb);
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        #else
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam, img_buffer, IMG_SIZE * sizeof(uint16_t));
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        #endif  /* ASYNC */

        /* Process image. */
        int32_t min_diff = (1 << 14);
        int32_t max_diff = -(1 << 14);

        for (uint32_t i = 0; i < IMG_SIZE; i++)
        {
            dif_buff[i] = img_buffer[i] - ref_buffer[i];

            if (dif_buff[i] < 0 )
            {
                DEBUG_PRINTF("Negative value !\n");
                dif_buff[i] = 0;
            }
            if (dif_buff[i] > (1 << 14))
            {
                DEBUG_PRINTF("Saturation !\n");
                dif_buff[i] = (1 << 14);
            }

            if (dif_buff[i] > max_diff)
            {
                max_diff = dif_buff[i];
            }
            else if (dif_buff[i] < min_diff)
            {
                min_diff = dif_buff[i];
            }
        }
        uint32_t min_max_swing = max_diff - min_diff;
        for (uint32_t i = 0; i < IMG_SIZE; i++)
        {
            scaled_buff[i] = ((dif_buff[i] - min_diff) * 255) / min_max_swing;
        }

        char string_buffer[50];
        sprintf(string_buffer, "../../../img_%ld.ppm", save_index);
        WriteImageToFile(string_buffer, IMG_WIDTH, IMG_HEIGHT, 1, scaled_buff,GRAY_SCALE_IO);
        save_index++;
 
        loop++;
        pi_time_wait_us(1000);
    }

    pi_camera_close(&cam);

    pmsis_l2_malloc_free(calib_buffer,IMG_SIZE*sizeof(uint16_t));
    pmsis_l2_malloc_free(img_buffer  ,IMG_SIZE*sizeof(uint16_t));
    pmsis_l2_malloc_free(ref_buffer  ,IMG_SIZE*sizeof(uint16_t));
    pmsis_l2_malloc_free(dif_buff    ,IMG_SIZE*sizeof(uint16_t));
    pmsis_l2_malloc_free(scaled_buff ,IMG_SIZE*sizeof(uint8_t));


    printf("Thermeye test ended...\n");

    return;
}

void test_gapoc_b_v2(){

    set_pad_to_f();
    test_therm_eye(); 
    set_pad_to_0();
    test_therm_eye();

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** Therm Eye ***\n\n");
    return pmsis_kickoff((void *) test_gapoc_b_v2);
}
