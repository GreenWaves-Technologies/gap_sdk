
//#include "pmsis/chips/gap8/pmu.h"
//#include "pmsis/drivers/pmu.h"

#include "stdio.h"

/* PMSIS includes. */
#include "pmsis.h"

/* PMSIS BSP includes. */
#include "bsp/bsp.h"
#include "epeas_cis001_board.h"
#include "bsp/camera.h"

#include "bsp/ram.h"
#include "bsp/ram/spiram.h"

/* gaplib includes */
#include "gaplib/ImgIO.h"

#include "bsp/camera/epeas_cis001.h"
#include "epeas_cis001.h"


/* constants */
#ifdef IMG_QVGA
#define CAMERA_WIDTH  ( 320 )
#define CAMERA_HEIGHT ( 240 )
#elif defined IMG_QQVGA
#define CAMERA_WIDTH  ( 160 )
#define CAMERA_HEIGHT ( 120 )
#else 
#define CAMERA_WIDTH  ( 640 )
#define CAMERA_HEIGHT ( 480 )
#endif

#define ROI1_X_START    1
#define ROI1_X_END      5
#define ROI1_Y_START    1
#define ROI1_Y_END      5

#define ROI2_X_START    10
#define ROI2_X_END      14
#define ROI2_Y_START    10
#define ROI2_Y_END      10

#define ROI1_WIDTH      (ROI1_X_START - ROI1_X_END + 1)*32
#define ROI1_HEIGHT     (ROI1_Y_START - ROI1_Y_END + 1)*32

#define ROI2_WIDTH      (ROI2_X_START - ROI2_X_END + 1)*32
#define ROI2_HEIGHT     (ROI2_Y_START - ROI2_Y_END + 1)*32


/* statics */

PI_L2 static uint8_t *qqvga_buffers;
PI_L2 static uint8_t *roi1_buffers;
PI_L2 static uint8_t *roi2_buffers;


static struct pi_device cam;

static pi_task_t task;


static int32_t open_camera_epeas_cis001(struct pi_device *device)
{
    struct pi_epeas_cis001_conf cam_conf;
    pi_epeas_cis001_conf_init(&cam_conf);
    #ifdef IMG_QVGA
    cam_conf.format = PI_CAMERA_QVGA;
    #elif defined IMG_QQVGA
    cam_conf.format = PI_CAMERA_QQVGA;
    #else
    cam_conf.format = PI_CAMERA_VGA;
    #endif
    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    {
        return -1;
    }
    return 0;
}


static int32_t open_camera(struct pi_device *device)
{
    return open_camera_epeas_cis001(device);
}

static void dump_cam_configuration_register(void)
{
    uint8_t val_read;
    uint16_t temp;


    printf("\n");
    printf("-------- Dump Configuration Registers --------\n");

    pi_camera_reg_get(&cam,ADDR_RESOLUTION_REGISTER,&val_read);
    printf("Resolution : 0x%x \n",val_read);

    pi_camera_reg_get(&cam,ADDR_MODE_REGISTER,&val_read);
    printf("Mode : %d \n",val_read);

    pi_camera_reg_get(&cam,ADDR_EXPOSURE_B0_REGISTER,&val_read);
    temp = val_read;
    pi_camera_reg_get(&cam,ADDR_EXPOSURE_B1_REGISTER,&val_read);
    temp += (val_read)<<8;
    printf("Exposure Time : %d \n",temp);

    pi_camera_reg_get(&cam,ADDR_ANALOG_GAIN_REGISTER,&val_read);
    printf("Analog Gain : %d \n",val_read);

    pi_camera_reg_get(&cam,ADDR_CCLK_CAL_REGISTER,&val_read);
    printf("CCLK Calibration : %d \n",val_read);

    printf("-------- End Dump Configuration Registers --------\n");
    printf("\n");


}


static void write_cam_configuration_register(uint8_t addr, uint8_t value)
{

    pi_camera_reg_set(&cam,addr, &value);

}

static void config_qqvga(void)
{
    write_cam_configuration_register(ADDR_ROI_EN_REGISTER, 0);
    write_cam_configuration_register(ADDR_RESOLUTION_REGISTER, 2);
}

static void config_roi(void)
{
    write_cam_configuration_register(ADDR_RESOLUTION_REGISTER, 0);
    write_cam_configuration_register(ADDR_ROI_EN_REGISTER, 1);
}

static void config_roi_size(uint8_t roi_x_start, uint8_t roi_x_end, uint8_t roi_y_start, uint8_t roi_y_end)
{
        write_cam_configuration_register(ADDR_ROI_X_START_REGISTER, roi_x_start);
        write_cam_configuration_register(ADDR_ROI_X_END_REGISTER, roi_x_end);
        write_cam_configuration_register(ADDR_ROI_Y_START_REGISTER, roi_y_start);
        write_cam_configuration_register(ADDR_ROI_Y_END_REGISTER, roi_y_end);
}

static void auto_cal_cclk(int target, int threshold)
{
    
    uint16_t res=0;
    uint16_t cal_max=120;
    uint16_t cal_min=5;
    uint16_t cal=15;
    uint16_t iter=0;
    uint8_t previous_mode=0;
    uint8_t a,b=0;
    
    pi_camera_reg_get(&cam,ADDR_MODE_REGISTER,&previous_mode);
    
    
    //set cam in cclk mode
    write_cam_configuration_register(ADDR_MODE_REGISTER, 5);
    
    // start at cal=15
    write_cam_configuration_register(ADDR_CCLK_CAL_REGISTER, cal);
    pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
    pi_time_wait_us(1000); 
    //printf("cclk cal done\n"); // print something instead of seleeping
    iter++;
    
    //res=read_cam_configuration_register(ADDR_CAL_CLK_BUFF_B0_REGISTER)
    //res+=(read_cam_configuration_register(ADDR_CAL_CLK_BUFF_B1_REGISTER)*256);
    pi_camera_reg_get(&cam,ADDR_CAL_CLK_BUFF_B0_REGISTER,&a);
    pi_camera_reg_get(&cam,ADDR_CAL_CLK_BUFF_B1_REGISTER,&b);
    res=a+(b<<8);
    //printf("res: %i\n",res); 
    
    
    while (((res < target -threshold) || (res > target+threshold)) && (iter<8)){
        
        write_cam_configuration_register(ADDR_CCLK_CAL_REGISTER, cal);
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        pi_time_wait_us(5000); 
        //printf("cclk cal done\n"); // print something instead of seleeping
        
        pi_camera_reg_get(&cam,ADDR_CAL_CLK_BUFF_B0_REGISTER,&a);
        pi_camera_reg_get(&cam,ADDR_CAL_CLK_BUFF_B1_REGISTER,&b);
        res=a+(b<<8);
        //printf("res: %i\n",res); 
        
        //updated stuff
        if(res< (target -threshold)){         
            //printf("too low: %i %i %i %i",res,cclk,lower,upper);
            cal_min=cal;
            cal=(cal_min+cal_max)/2;
            //printf(" -> %i  %i %i\r\n\r\n",cclk,lower,upper);
        } else if(res> (target+threshold)){
            //printf("too high: %i %i %i %i",res,cclk,lower,upper);
            cal_max=cal;
            cal=(cal_min+cal_max)/2;
            //printf(" -> %i %i %i \r\n\r\n",cclk,lower,upper);
        } else {
            //printf("ok !!\r\n");
        }    
        iter++;
    }
    
    //printf("res: %i\n",res); 
    // put camera in the mode it was
    write_cam_configuration_register(ADDR_MODE_REGISTER, previous_mode);
    
    return;
}


static void mode_drs()
{
     // configure ccclk
    auto_cal_cclk(350, 5);
    //mode
    write_cam_configuration_register(ADDR_MODE_REGISTER, 1);
}

static void mode_so()
{
    // configure ccclk
    auto_cal_cclk(250, 5);
    //mode
    write_cam_configuration_register(ADDR_MODE_REGISTER, 0);
    
}

static void mode_hdr_drs()
{
     // configure ccclk
    auto_cal_cclk(350, 5);
    //mode
    write_cam_configuration_register(ADDR_MODE_REGISTER, 12);
}

static void mode_hdr_so()
{
    // configure ccclk
    auto_cal_cclk(250, 5);
    //mode
    write_cam_configuration_register(ADDR_MODE_REGISTER, 11);
    
}




static void test_camera_double_buffer(void)
{
    printf("Launching test ...\n");
    int32_t errors = 0;

    /*Force DC-DC in PWM mode to minimize jitter clock*/
    // Set some custom FC frequency :
    pi_freq_set(PI_FREQ_DOMAIN_FC, 240 * 1000000) ;
    printf("Frequency now = %d Hz\n\n", (int)pi_freq_get(PI_FREQ_DOMAIN_FC));
    // Set some known voltage before switch 
    pi_pmu_voltage_set( PI_PMU_DOMAIN_FC, 1199 );
    // FORCE DC-DC PWM Mode
    uint32_t DC_ForcePWM_RegAdd = 0x1A10414C;
    (*(volatile unsigned int *)(long)(DC_ForcePWM_RegAdd)) = 1;
    // Set a different voltage AFTER this to validate 
    pi_pmu_voltage_set( PI_PMU_DOMAIN_FC, 1200 );


    /*Memory allocation for L2 buffer image and ROI*/


    qqvga_buffers = (uint8_t *) pmsis_l2_malloc(CAMERA_WIDTH*CAMERA_HEIGHT);
        if(qqvga_buffers == NULL)pmsis_exit(-1);

    roi1_buffers = (uint8_t *)  pmsis_l2_malloc(ROI1_WIDTH*ROI1_HEIGHT);
        if(roi1_buffers == NULL)pmsis_exit(-1);

    roi2_buffers = (uint8_t *)  pmsis_l2_malloc(ROI2_WIDTH*ROI2_HEIGHT);
        if(roi2_buffers == NULL)pmsis_exit(-1);


    /* open camera */

    printf("Opening camera ...\n");
    errors = open_camera(&cam);
    if (errors)
    {
        printf("Failed to open camera : %ld\n", errors);
        pmsis_exit(-2);
    }


    /* Configuration */

    mode_drs();           // calibration from default driver configuration

    //exposure for linear mode
    //write_cam_configuration_register(ADDR_EXPOSURE_B0_REGISTER, 0);
    //write_cam_configuration_register(ADDR_EXPOSURE_B1_REGISTER, 3);
    
    //exposure for HDR  mode
    //write_cam_configuration_register(ADDR_EXPOSURE_B0_REGISTER, 100);
    //write_cam_configuration_register(ADDR_EXPOSURE_B1_REGISTER, 7);

    //write_cam_configuration_register(ADDR_ANALOG_GAIN_REGISTER, 6);
    //write_cam_configuration_register(ADDR_FLIP_REGISTER, 3);


    int idx =0;
    printf("Starting image capture...\n");
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
    
    while (idx++ < 10)
    {

        
        config_qqvga();

        pi_camera_capture_async(&cam, qqvga_buffers, CAMERA_WIDTH*CAMERA_HEIGHT, pi_task_block(&task));
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);

        int tm = pi_time_get_us();

        pi_task_wait_on(&task);
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);


        pi_time_wait_us(30000);

        config_roi();

        config_roi_size(ROI1_X_START, ROI1_X_END, ROI1_Y_START, ROI1_Y_END);

        pi_camera_capture_async(&cam, roi1_buffers, ROI1_WIDTH*ROI1_HEIGHT, pi_task_block(&task));
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_task_wait_on(&task);
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);


        config_roi_size(ROI2_X_START, ROI2_X_END, ROI2_Y_START, ROI2_Y_END);

        pi_camera_capture_async(&cam, roi2_buffers, ROI2_WIDTH*ROI2_HEIGHT, pi_task_block(&task));
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_task_wait_on(&task);
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);

        tm = pi_time_get_us() - tm;
        printf("Inference finished in %d us\n", tm);

        

        char filename[30];

        sprintf(filename, "../../../pgmfile%d.pgm", idx );
        WriteImageToFile(filename, 160, 120, sizeof(uint8_t), qqvga_buffers, GRAY_SCALE_IO);

        sprintf(filename, "../../../pgmfile%d_roi1.pgm", idx  );
        WriteImageToFile(filename, ROI1_WIDTH, ROI1_HEIGHT, sizeof(uint8_t), roi1_buffers, GRAY_SCALE_IO);

        sprintf(filename, "../../../pgmfile%d_roi2.pgm", idx  );
        WriteImageToFile(filename, ROI2_WIDTH, ROI2_HEIGHT, sizeof(uint8_t), roi2_buffers, GRAY_SCALE_IO);

    }

    printf("Exiting...\n");
    pmsis_exit(errors);
}

int main(void)
{
    printf("\n\t*** PMSIS camera e-Peas CIS001 software ROI example ***\n\n");
    return pmsis_kickoff((void *) test_camera_double_buffer);
}
