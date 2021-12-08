
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
//#define CAMERA_HEIGHT ( 100 )
#define CAMERA_PIXEL_SIZE (1)
#define ITER_SIZE  (0x12C00)

/* statics */

static uint8_t* img_buffers[2];
static volatile int current_idx = 0;
static volatile int next_idx = 0;

static struct pi_device cam;

static struct pi_device ram;
static uint32_t l3_buff;
static pi_task_t ctrl_tasks[2];
static pi_task_t ram_tasks[2];

static int remaining_size;
static int saved_size;
static volatile int done;
static int nb_transfers;
static unsigned char current_buff;
static int current_size[2];
static int current_task;

static void handle_transfer_end(void *arg);
static void handle_ram_end(void *arg);

// This is called to enqueue new transfers

static void enqueue_transfer()
{
    // We can enqueue new transfers if there are still a part of the image to
    // capture and less than 2 transfers are pending (the dma supports up to 2 transfers
    // at the same time)
    while (remaining_size > 0 && nb_transfers < 2)
    {
            int iter_size = ITER_SIZE;
            if (remaining_size < iter_size)
                iter_size = remaining_size;

            pi_task_t *task = &ctrl_tasks[current_task];
            // Enqueue a transfer. The callback will be called once the transfer is finished
            // so that  a new one is enqueued while another one is already running
            pi_camera_capture_async(&cam, img_buffers[current_task], iter_size, pi_task_callback(task, handle_transfer_end, (void *) current_task));

            current_size[current_task] = iter_size;
            remaining_size -= iter_size;
            nb_transfers++;
            current_task ^= 1;     
    }
}


static void handle_transfer_end(void * arg)
{
    nb_transfers--;
    unsigned last = (unsigned) arg;
    int size = current_size[last];

    enqueue_transfer();

    pi_task_t *cb_tx = &ram_tasks[last];
    pi_ram_write_async(&ram, (l3_buff+saved_size), img_buffers[last], (uint32_t) size, pi_task_callback(cb_tx, handle_ram_end, NULL));

    saved_size += size;
}

static void handle_ram_end(void *arg)
{
    if (nb_transfers == 0 && saved_size == (CAMERA_WIDTH*CAMERA_HEIGHT*CAMERA_PIXEL_SIZE))
        done = 1;
}



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

static void process_image(int idx)
{
    char imgName[50];
    sprintf(imgName, "../../../img_OUT_%ld.ppm", idx);
    WriteImageToFileL3(&ram, imgName, CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_PIXEL_SIZE, l3_buff, GRAY_SCALE_IO);
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

    struct pi_spiram_conf conf;
    pi_spiram_conf_init(&conf);
    pi_open_from_conf(&ram, &conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_exit(-3);
    }

    if (pi_ram_alloc(&ram, &l3_buff, (uint32_t) (CAMERA_WIDTH*CAMERA_HEIGHT*CAMERA_PIXEL_SIZE)))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-4);
    }

    /* allocate memory for images */

    for(int i = 0; i<2; i++)
    {
        img_buffers[i] = (uint8_t *) pmsis_l2_malloc(ITER_SIZE);
        if (img_buffers[i] == NULL)
        {
            printf("Failed to allocate Memory for Image %d \n", i);
            pmsis_exit(-1);
        }
    }

    remaining_size = (CAMERA_WIDTH*CAMERA_HEIGHT*CAMERA_PIXEL_SIZE);    
    nb_transfers = 0;
    current_buff = 0;
    current_task = 0;
    saved_size = 0;
    done = 0;

    /* open camera */

    printf("Opening camera ...\n");

    errors = open_camera(&cam);
    if (errors)
    {
        printf("Failed to open camera : %ld\n", errors);
        pmsis_exit(-2);
    }


    /* Configuration */

    mode_drs();           // default driver used

    //exposure for linear mode
    //write_cam_configuration_register(ADDR_EXPOSURE_B0_REGISTER, 0);
    //write_cam_configuration_register(ADDR_EXPOSURE_B1_REGISTER, 3);
    
    //exposure for HDR  mode
    //write_cam_configuration_register(ADDR_EXPOSURE_B0_REGISTER, 100);
    //write_cam_configuration_register(ADDR_EXPOSURE_B1_REGISTER, 7);

    //write_cam_configuration_register(ADDR_ANALOG_GAIN_REGISTER, 6);
    //write_cam_configuration_register(ADDR_FLIP_REGISTER, 3);


    dump_cam_configuration_register();

    int idx =0;
    printf("Starting image capture...\n");
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
    while (idx++ < 10)
    {

        enqueue_transfer();

        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        while(!done) pi_yield();
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        printf("Capture done\n");

        process_image(idx);

        remaining_size = (CAMERA_WIDTH*CAMERA_HEIGHT*CAMERA_PIXEL_SIZE);
        nb_transfers = 0;
        current_buff = 0;
        current_task = 0;
        saved_size = 0;
        done = 0;
    }

    printf("Exiting...\n");
    pmsis_exit(errors);
}

int main(void)
{
    printf("\n\t*** PMSIS camera e-Peas CIS001 software double buffering ***\n\n");
    return pmsis_kickoff((void *) test_camera_double_buffer);
}
