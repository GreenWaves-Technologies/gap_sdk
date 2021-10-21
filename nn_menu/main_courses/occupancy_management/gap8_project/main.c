/*
 * Copyright 2020 GreenWaves Technologies, SAS
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

#include <stdio.h>
#include "BleNotifier.h"
#include "Gap.h"
#include "lynredKernels.h"
#include "SSDKernels.h"
#include "SSDParams.h"
#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/gapoc_b_v2.h"
#include "bsp/camera.h"
#include "bsp/fs.h"

#if defined(QSPI)
#define FLASH_NAME "QSPI"
#include "bsp/flash/spiflash.h"
#else
#define FLASH_NAME "HYPER"
#include "bsp/flash/hyperflash.h"
#endif

#include "bsp/camera/thermeye.h"

#include "bsp/fs.h"
#include <bsp/fs/hostfs.h>

#include "shutterless/PreFiltering.h"
#include "shutterless/PreFilteringCluster.h"
#include "ImageDraw.h"
#include "setup.h"

#define MOUNT           1
#define UNMOUNT         0
#define CID             0

#define GPIO_USER_LED 0


#if defined(QSPI)
struct pi_device QspiRam;
AT_QSPIFLASH_FS_EXT_ADDR_TYPE lynred_L3_Flash;
#else
struct pi_device HyperRam;
AT_HYPERFLASH_FS_EXT_ADDR_TYPE lynred_L3_Flash;
#endif


#define FIX2FP(Val, Precision)    ((float) (Val) / (float) (1<<(Precision)))

// Softmax always outputs Q15 short int even from 8 bit input
short int *output8, *output1, *output2, *output3, *output4, *output5, *output6, *output7,*tmp_buffer_classes,*tmp_buffer_boxes;


PI_L2 int16_t* img_offset;
PI_L2 int16_t* ImageIn;

#define BUFFER_SIZE 1024
static struct pi_device cam;
static struct pi_device cluster_dev;

extern PI_L2 Alps * anchor_layer_2;
extern PI_L2 Alps * anchor_layer_3;
extern PI_L2 Alps * anchor_layer_4;
extern PI_L2 Alps * anchor_layer_5;

PI_L2 bboxs_t bbxs;

void open_flash_filesystem(struct pi_device *flash, struct pi_device *fs)
{
    struct pi_fs_conf fsconf;
    struct pi_spiflash_conf flash_conf;

    /* Init & open flash. */
    #if defined(QSPI)
    pi_spiflash_conf_init(&flash_conf);
    #else
    pi_hyperflash_conf_init(&flash_conf);
    #endif
    pi_open_from_conf(flash, &flash_conf);
    if (pi_flash_open(flash))
    {
        printf("Error flash open !\n");
        pmsis_exit(-1);
    }

    pi_fs_conf_init(&fsconf);

    //fsconf.type = PI_FS_HOST;
    fsconf.flash = flash;
    fsconf.type = PI_FS_READ_ONLY;

    pi_open_from_conf(fs, &fsconf);

    if (pi_fs_mount(fs)){
        printf("Error FS mounting !\n");
        pmsis_exit(-2);
    }
}

void close_flash_filesystem(struct pi_device *flash, struct pi_device *fs)
{
    pi_fs_unmount(fs);
    pi_flash_close(flash);
}

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

static int initNN(){

    #ifndef INPUT_FILE
    PRINTF("Loading Offset Image from Flash...\n");
    pi_fs_file_t *file = NULL;
    char *name = "Calibration.bin";
    int32_t size = 0;
    uint32_t size_total = 0;

    img_offset  = (unsigned short int *) pmsis_l2_malloc(80 * 80 * sizeof(short int));
    char * buff =  img_offset;

    if (img_offset==NULL )
    {
        PRINTF("Failed to allocate Memory for image Offset\n");
        pmsis_exit(-4);
    }

    struct pi_device flash;
    struct pi_device fs;

    open_flash_filesystem(&flash, &fs);

    file = pi_fs_open(&fs, name, 0);
    if (file == NULL)
    {
        printf("File %s open failed !\n", name);
        pmsis_exit(-4);
    }

    do
    {
        //Read from filesystem(on flash) to a buffer in L2 memory.
        size = pi_fs_read(file, buff+size_total, BUFFER_SIZE);
        size_total += size;
    } while (size_total < file->size);

    pi_fs_close(file);

    close_flash_filesystem(&flash,&fs);

    #endif
    //Allocating Space for Bouding Boxes
    bbxs.bbs = pmsis_l2_malloc(sizeof(bbox_t)*MAX_BB);

    if(bbxs.bbs==NULL){
        return 1;
    }

    bbxs.num_bb = 0;

    initAnchorLayer_2();
    initAnchorLayer_3();
    initAnchorLayer_4();
    initAnchorLayer_5();

    return 0;

}


void convertCoordBboxes(bboxs_t *boundbxs,float scale){

    for (int counter=0;counter< boundbxs->num_bb;counter++){
        //Set Alive for non max this should be done somewhere else
        boundbxs->bbs[counter].alive=1;
        boundbxs->bbs[counter].x = (int)(FIX2FP(boundbxs->bbs[counter].x,20) * scale);
        boundbxs->bbs[counter].y = (int)(FIX2FP(boundbxs->bbs[counter].y,20) * scale);
        boundbxs->bbs[counter].w = (int)(FIX2FP(boundbxs->bbs[counter].w,26) * scale);
        boundbxs->bbs[counter].h = (int)(FIX2FP(boundbxs->bbs[counter].h,26) * scale);
        boundbxs->bbs[counter].x = boundbxs->bbs[counter].x - (boundbxs->bbs[counter].w/2);
        boundbxs->bbs[counter].y = boundbxs->bbs[counter].y - (boundbxs->bbs[counter].h/2);
        if(boundbxs->bbs[counter].x<0) boundbxs->bbs[counter].x =0;
        if(boundbxs->bbs[counter].y<0) boundbxs->bbs[counter].y =0;
        if(boundbxs->bbs[counter].x>79) boundbxs->bbs[counter].x =79;
        if(boundbxs->bbs[counter].y>79) boundbxs->bbs[counter].y =79;
    }
}

void printBboxes(bboxs_t *boundbxs){
    PRINTF("\n\n======================================================");
    PRINTF("\nDetected Bounding boxes                                 ");
    PRINTF("\n======================================================\n");

    PRINTF("BoudingBox:  score     cx     cy     w     h    class");
    PRINTF("\n------------------------------------------------------\n");

    for (int counter=0;counter< boundbxs->num_bb;counter++){
        if(boundbxs->bbs[counter].alive)
            PRINTF("bbox [%02d] : %.5f     %03d    %03d     %03d    %03d     %02d\n",
                counter,
                FIX2FP(boundbxs->bbs[counter].score,15 ),
                boundbxs->bbs[counter].x,
                boundbxs->bbs[counter].y,
                boundbxs->bbs[counter].w,
                boundbxs->bbs[counter].h,
                boundbxs->bbs[counter].class);
    }//
}


void drawBboxes(bboxs_t *boundbxs, uint8_t *img){

     for (int counter=0;counter< boundbxs->num_bb;counter++){
        if(boundbxs->bbs[counter].alive){
            DrawRectangle(img, 80, 80, boundbxs->bbs[counter].x, boundbxs->bbs[counter].y, boundbxs->bbs[counter].w, boundbxs->bbs[counter].h, 255);
        }
    }
}

void printBboxes_forPython(bboxs_t *boundbxs){
    PRINTF("\n\n======================================================");
    PRINTF("\nThis can be copy-pasted to python to draw BoudingBoxs   ");
    PRINTF("\n\n");

    for (int counter=0;counter< boundbxs->num_bb;counter++){
        if(boundbxs->bbs[counter].alive)
            PRINTF("rect = patches.Rectangle((%d,%d),%d,%d,linewidth=1,edgecolor='r',facecolor='none')\nax.add_patch(rect)\n",
                boundbxs->bbs[counter].x,
                boundbxs->bbs[counter].y,
                boundbxs->bbs[counter].w,
                boundbxs->bbs[counter].h);
    }//
}

int rect_intersect_area( short a_x, short a_y, short a_w, short a_h,
                         short b_x, short b_y, short b_w, short b_h ){

    #define MIN(a,b) ((a) < (b) ? (a) : (b))
    #define MAX(a,b) ((a) > (b) ? (a) : (b))

    int x = MAX(a_x,b_x);
    int y = MAX(a_y,b_y);

    int size_x = MIN(a_x+a_w,b_x+b_w) - x;
    int size_y = MIN(a_y+a_h,b_y+b_h) - y;

    if(size_x <=0 || size_x <=0)
        return 0;
    else
        return size_x*size_y;

    #undef MAX
    #undef MIN
}


void non_max_suppress(bboxs_t * boundbxs){

    int idx,idx_int;

    //Non-max supression
     for(idx=0;idx<boundbxs->num_bb;idx++){
        //check if rect has been removed (-1)
        if(boundbxs->bbs[idx].alive==0)
            continue;

        for(idx_int=0;idx_int<boundbxs->num_bb;idx_int++){

            if(boundbxs->bbs[idx_int].alive==0 || idx_int==idx)
                continue;

            //check the intersection between rects
            int intersection = rect_intersect_area(boundbxs->bbs[idx].x,boundbxs->bbs[idx].y,boundbxs->bbs[idx].w,boundbxs->bbs[idx].h,
                                                   boundbxs->bbs[idx_int].x,boundbxs->bbs[idx_int].y,boundbxs->bbs[idx_int].w,boundbxs->bbs[idx_int].h);

            if(intersection >= NON_MAX_THRES){ //is non-max
                //supress the one that has lower score that is alway the internal index, since the input is sorted
                boundbxs->bbs[idx_int].alive =0;
            }
        }
    }
}


int initL3Buffers(){

    /* Init & open ram. */
#if defined(QSPI)
    struct pi_device *ram= &QspiRam;
    static struct pi_spiram_conf conf;
    pi_spiram_conf_init(&conf);
#else
    struct pi_device *ram=&HyperRam;
    static struct pi_hyperram_conf conf;
    pi_hyperram_conf_init(&conf);
#endif
    pi_open_from_conf(ram, &conf);
    if (pi_ram_open(ram))
    {
        printf("Error ram open !\n");
        return -1;
    }

    pi_ram_alloc(ram, &output1, 40 * 40 * 16 * sizeof(short int));
    pi_ram_alloc(ram, &output2, 20 * 20 * 16 * sizeof(short int));
    pi_ram_alloc(ram, &output3, 10 * 10 * 16 * sizeof(short int));
    pi_ram_alloc(ram, &output4,  5 *  5 * 16 * sizeof(short int));

    pi_ram_alloc(ram, &output5, 40 * 40 * 32 * sizeof(short int));
    pi_ram_alloc(ram, &output6, 20 * 20 * 32 * sizeof(short int));
    pi_ram_alloc(ram, &output7, 10 * 10 * 32 * sizeof(short int));
    pi_ram_alloc(ram, &output8,  5 *  5 * 32 * sizeof(short int));

    pi_ram_alloc(ram, &tmp_buffer_classes, 40 * 40 * 16 * sizeof(short int));
    pi_ram_alloc(ram, &tmp_buffer_boxes  , 40 * 40 * 32 * sizeof(short int));

    if(output1==NULL || output2==NULL || output3==NULL || output4==NULL || output5==NULL || output6==NULL || output7==NULL || output8==NULL )
    {
        printf("Error Allocating OUTPUTs in L3\n");
        return -1;
    }
    if(tmp_buffer_classes==NULL || tmp_buffer_classes==NULL)
    {
        printf("Error Allocating SSD Temp buffers in L3\n");
        return -1;
    }

    //leave ram open for AT

    return 0;
}

static void RunNN()
{


    unsigned int ti,ti_nn,ti_ssd;

    gap_cl_starttimer();
    gap_cl_resethwtimer();
    ti = gap_cl_readhwtimer();

    lynredCNN(ImageIn,
              output1,
              output2,
              output3,
              output4,
              output5,
              output6,
              output7,
              output8);

    ti_nn = gap_cl_readhwtimer()-ti;

    /////////////////////
    //SSD Code
    /////////////////////

    //Set Boundinx Boxes to 0
    bbxs.num_bb = 0;
    //TODO Add a check to be sure that ssd L1 is smaller than L1
    SSDKernels_L1_Memory = lynred_L1_Memory;

    //Processing Classes and Boxes
    SDD3Dto2DSoftmax_40_40_16(output1,tmp_buffer_classes,OUTPUT1_Q,2);
    SDD3Dto2D_40_40_32(output5,tmp_buffer_boxes,0,0);
    Predecoder40_40(tmp_buffer_classes, tmp_buffer_boxes, anchor_layer_2, &bbxs,OUTPUT5_Q);

    SDD3Dto2DSoftmax_20_20_16(output2,tmp_buffer_classes,OUTPUT2_Q,2);
    SDD3Dto2D_20_20_32(output6,tmp_buffer_boxes,0,0);
    Predecoder20_20(tmp_buffer_classes, tmp_buffer_boxes, anchor_layer_3, &bbxs,OUTPUT6_Q);

    SDD3Dto2DSoftmax_10_10_16(output3,tmp_buffer_classes,OUTPUT3_Q,2);
    SDD3Dto2D_10_10_32(output7,tmp_buffer_boxes,0,0);
    Predecoder10_10(tmp_buffer_classes, tmp_buffer_boxes, anchor_layer_4, &bbxs,OUTPUT7_Q);

    SDD3Dto2DSoftmax_5_5_16(output4,tmp_buffer_classes,OUTPUT4_Q,2);
    SDD3Dto2D_5_5_32(output8,tmp_buffer_boxes,0,0);
    Predecoder5_5(tmp_buffer_classes, tmp_buffer_boxes, anchor_layer_5, &bbxs,OUTPUT8_Q);


    bbox_t temp;
    int changed=0;
    do{
        changed=0;
        for(int i=0; i<bbxs.num_bb-1;i++){
            if(bbxs.bbs[i].score < bbxs.bbs[i+1].score){
                temp          = bbxs.bbs[i];
                bbxs.bbs[i]   = bbxs.bbs[i+1];
                bbxs.bbs[i+1] = temp;
                changed=1;
            }
        }
    }while(changed);

    convertCoordBboxes(&bbxs,80);
    non_max_suppress(&bbxs);

    ti_ssd = gap_cl_readhwtimer()-ti_nn-ti;

    #if !defined SILENT

    printBboxes(&bbxs);
    printBboxes_forPython(&bbxs);
    #endif

    PRINTF("Cycles NN : %10d\n",ti_nn);
    PRINTF("Cycles SSD: %10d\n",ti_ssd);

}


char bleDetString[200];
char tmpString[200];
int dt;
int old_dt;
float thres;
void sendResultsToBle(bboxs_t *boundbxs){

    int stringLenght = 0;
    int AliveBBs=0;
    for (int counter=0;counter< boundbxs->num_bb;counter++){
        if(boundbxs->bbs[counter].alive){
            AliveBBs++;
        }
    }
    if(AliveBBs>MAX_OUT_BB) AliveBBs=MAX_OUT_BB;

    stringLenght+=sprintf(bleDetString,"%d;",AliveBBs);

    for (int counter=0;counter< boundbxs->num_bb;counter++){
        if(boundbxs->bbs[counter].alive){

            boundbxs->bbs[counter].x = boundbxs->bbs[counter].x + (boundbxs->bbs[counter].w/2);
            boundbxs->bbs[counter].y = boundbxs->bbs[counter].y + (boundbxs->bbs[counter].h/2);
            stringLenght+=sprintf(tmpString,"%dx%d;",boundbxs->bbs[counter].x, boundbxs->bbs[counter].y);
            strcat(bleDetString,tmpString);
        }
    }

    //stringLenght+=sprintf(tmpString,"Gap8 Power Consuption %f mW/FPS",((float)(1/(50000000.f/12000000)) * 16.800));
    stringLenght+=sprintf(tmpString,"A Project from GreenWaves and Lynred");
    strcat(bleDetString,tmpString);

    //printf("%s",bleDetString);
    //printf("\n");
    //printf("String Size: %d\n",stringLenght);

    dt = handleDetections(bleDetString,stringLenght);
    if(dt<10)dt=10;
    if(dt!=old_dt){
        old_dt=dt;
        thres = ((float)old_dt)/100;
        anchor_layer_2->confidence_thr = FP2FIX(thres,15);
        anchor_layer_3->confidence_thr = FP2FIX(thres,15);
        anchor_layer_4->confidence_thr = FP2FIX(thres,15);
        anchor_layer_5->confidence_thr = FP2FIX(thres,15);

    }
}

int read_raw_image(char* filename, int16_t* buffer,int w,int h){
    struct pi_fs_conf conf;
    static struct pi_device fs;
    static pi_fs_file_t *file;
    unsigned int ReadSize=0;

    pi_fs_conf_init(&conf);
    conf.type = PI_FS_HOST;
    pi_open_from_conf(&fs, &conf);

    if (pi_fs_mount(&fs))
        return -2;

    file = pi_fs_open(&fs, filename, PI_FS_FLAGS_READ);
    if (file == NULL) return -3;


    {
        char *TargetImg = buffer;
        unsigned int RemainSize = w*h*sizeof(int16_t);

        while (RemainSize > 0)
        {
            unsigned int Chunk = Min(4096, RemainSize);
            unsigned R = pi_fs_read(file,TargetImg, Chunk);
            ReadSize+=R;
            if (R!=Chunk) break;
            TargetImg += Chunk; RemainSize -= Chunk;
        }
    }

    pi_fs_close(file);
    pi_fs_unmount(&fs);

    printf("Image %s, [W: %d, H: %d], Gray, Size: %d bytes, Loaded sucessfully\n", filename, w, h, ReadSize);

    return 0;

}

/* This SLEEP only works in pulpos for now
 * TODO: need to be support in freeRTOS when new api available
 */

#ifdef SLEEP
#define RTC_TIME 5
void go_to_sleep(){

    rt_rtc_conf_t rtc_conf;
    rt_rtc_t *rtc;

    rtc_conf.clkDivider = 0xF000;
    rt_rtc_cntDwn_t cntDwn = { RTC_TIME, 0 };

    rtc = rt_rtc_open(&rtc_conf, NULL);
    if (rtc == NULL) return -1;

    rt_pm_wakeup_clear_all();

    rt_rtc_control(rtc, RTC_CNTDOWN_SET, (void *)&cntDwn, NULL);
    rt_rtc_control(rtc, RTC_CNTDOWN_START, NULL, NULL);

    if(rt_pm_state_switch(RT_PM_STATE_DEEP_SLEEP, RT_PM_STATE_FAST)==-1)
        printf("Error....\n");

}
#endif


int32_t fixed_shutterless(int16_t* img_input_fp16,int16_t* img_offset_fp16,int w, int h, uint8_t q_output){
    
    int16_t min,max;
    int16_t out_min = 0;
    int32_t out_max = 255;
    int32_t out_space = (out_max-out_min);
   
    //Optmized shutterless running on cluster (cluster must be open ahead and have enough free memory)
    int error = shutterless_fixed_cl(&cluster_dev,img_input_fp16,img_offset_fp16,40,&min,&max);
    //Calling shutterless running on fabric controller
    //int error = shutterless_fixed_fc(img_input_fp16,img_offset_fp16,40,&min,&max);

    float div = 1./(max-min);
    int32_t div_fix = FP2FIX(div ,15);

    //Normalizing to 8 bit and changing fixed point format for NN.
    for(int i=0;i<w*h;i++){
        img_input_fp16[i]= (int16_t)(((out_space)* ((((((int32_t)img_input_fp16[i])-(int32_t)min))*div_fix)))>>(15-q_output+8));
    }

    return error;
}

int32_t float_shutterless(int16_t* img_input_fp16,int16_t* img_offset_fp16,int w, int h, uint8_t q_output, float gamma){
    int min,max;
    int32_t out_min = 0;
    int32_t out_max = 255;
    
    int error = shutterless_float(img_input_fp16,img_offset_fp16,50,&min,&max);    
        
    for(int i=0;i<w*h;i++){
        img_input_fp16[i]= (int16_t)((out_max-out_min)* (pow(((float)img_input_fp16[i]-min)/(max-min),gamma) + out_min)) ;
        img_input_fp16[i]= img_input_fp16[i] << (q_output-8);
    }
    return error;
}



#define USER_GPIO 18

void peopleDetection(void)
{
    char *ImageName = "../../../samples/im4.pgm";
    char *RawImageName = "../../../raw_samples/dump_out_imgs/im_gap_20210125-14_15_04.bin";

    //To configure and use User LED
    //pi_pad_e pad = (GPIO_USER_LED >> PI_GPIO_NUM_SHIFT);
    //uint32_t pin = (GPIO_USER_LED & PI_GPIO_NUM_MASK);
    //pi_pad_set_function(pad, PI_PAD_FUNC0);
    //pi_gpio_pin_configure(NULL, GPIO_USER_LED, PI_GPIO_OUTPUT);
    //pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);

    //To configure and use CONN3 pin8 on Gapoc_b boards
    //That is connected to pad 24 gpio18 of Gap8
    //this can be use as analysis pin (i.e. connect and external osilloscope and check chip activity)
    //pi_pad_set_function(PI_PAD_32_A13_TIMER0_CH1, PI_PAD_32_A13_GPIO_A18_FUNC1);
    //pi_gpio_pin_configure(NULL, USER_GPIO, PI_GPIO_OUTPUT);
    //pi_gpio_pin_write(NULL, USER_GPIO, 1);

    unsigned int Wi, Hi;
    //Input image size
    unsigned int W = 80, H = 80;
    unsigned int save_index=0;
    PRINTF("Entering main controller\n");

    
    unsigned char *ImageInChar = (unsigned char *) pmsis_l2_malloc( W * H * sizeof(int16_t));
    if (ImageInChar == 0)
    {
        PRINTF("Failed to allocate Memory for Image (%d bytes)\n", W * H * sizeof(int16_t));
        return 1;
    }
    ImageIn = (int16_t *)ImageInChar;

    #ifdef INPUT_FILE
    //Reading Image from Bridge
    PRINTF("Loading Image from File\n");
    if ((ReadImageFromFile(ImageName, &Wi, &Hi, ImageInChar, W * H * sizeof(unsigned char)) == 0) || (Wi != W) || (Hi != H))
    {
        PRINTF("Failed to load image %s or dimension mismatch Expects [%dx%d], Got [%dx%d]\n", ImageName, W, H, Wi, Hi);
        pmsis_exit(-1);
    }


    for (int i = W * H - 1; i >= 0; i--)
    {
        ImageIn[i] = ImageInChar[i] << INPUT1_Q - 8; //Input is naturally a Q8
    }

    #elif defined INPUT_RAW_FILE
    //Load 16 bits raw image

    if(read_raw_image(RawImageName, ImageIn,W,H)){
        PRINTF("Failed to load raw image\n");
        pmsis_exit(-1);
    }
    #endif

    /* Configure And open cluster. */
    struct pi_cluster_conf cl_conf;
    cl_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        PRINTF("Cluster open failed !\n");
        pmsis_exit(-7);
    }

    pi_freq_set(PI_FREQ_DOMAIN_FC,250000000);
    pi_freq_set(PI_FREQ_DOMAIN_CL,175000000);

    PRINTF("Init NN\n");
    if(initNN())
    {
        PRINTF("NN Init exited with an error\n");
        return 1;
    }

    PRINTF("Allocating Buffers in L3\n");
    if (initL3Buffers())
    {
        PRINTF("L3 Allocation exited with an error\n");
        return 1;
    }

    PRINTF("Constructor\n");
    if (lynredCNN_Construct(0))
    {
        PRINTF("Graph constructor exited with an error\n");
        return 1;
    }
    //Deallocating L1 to be used by other cluster calls
    if(lynredCNN_Destruct(1)){
        printf("Error deallocating L1 for cluster...\n");
        pmsis_exit(-1);
    }

    //SSD Allocations
    SSDKernels_L2_Memory = pmsis_l2_malloc(_SSDKernels_L2_Memory_SIZE);
    if(SSDKernels_L2_Memory==NULL)
    {
        printf("SSD L2 allocation error\n");
        pmsis_exit(-3);
    }

    //Pad Workaround:

    #if !defined(INPUT_RAW_FILE) && !defined(INPUT_FILE)
    PRINTF("Opening camera\n");
    if (open_camera_thermeye(&cam))
    {
        PRINTF("Thermal Eye camera open failed !\n");
        pmsis_exit(-1);
    }

    #ifdef OFFSET_IMAGE_EVERY_BOOT
    //This taking the offset each time we turn on the board
    PRINTF("Shooting offset, cover sensor with a black body!\n");
    pi_gpio_pin_write(NULL, GPIO_USER_LED, 0);
    pi_time_wait_us(2 * 1000 * 1000);
    pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
    pi_camera_capture(&cam, img_offset, W*H * sizeof(uint16_t));
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
    pi_gpio_pin_write(NULL, GPIO_USER_LED, 1);
    PRINTF("Offset image taken!\n");
    #endif
    #endif

    #ifdef USE_BLE
    PRINTF("Init BLE\n");
    int status;
    status = initHandler();

    if(status)
    {
        PRINTF("User manager init failed!\n");
        pmsis_exit(-5);
    }
    #endif

    PRINTF("Running NN\n");
    struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    if(task==NULL) {
        PRINTF("Alloc Error! \n");
        pmsis_exit(-7);
    }
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = RunNN;
    task->arg = (void *) NULL;
    task->stack_size = (uint32_t) STACK_SIZE;
    task->slave_stack_size = (uint32_t) SLAVE_STACK_SIZE;


    char iterate=1;
    while(iterate){


        #if defined(INPUT_RAW_FILE) || defined(INPUT_FILE)
        iterate=0;
        #else
        PRINTF("Taking Picture!\n");
        //pi_gpio_pin_write(NULL, USER_GPIO, 0);
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam, ImageIn, W*H*sizeof(int16_t));
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        #endif

        #ifndef INPUT_FILE

        //PRINTF("Calling shutterless filtering\n");
        //pi_perf_conf(1 << PI_PERF_ACTIVE_CYCLES);
        //pi_perf_reset(); pi_perf_start();
        
        //The shutterless floating point version was done just for reference...very slow on gap.
        //if(float_shutterless(ImageIn, img_offset,W,H,INPUT1_Q,1)){

        if(fixed_shutterless(ImageIn, img_offset,W,H,INPUT1_Q)){
            PRINTF("Error Calling prefiltering, exiting...\n");
            pmsis_exit(-8);
        }
        
        //pi_perf_stop();
        //int Ti = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
        //PRINTF("Cycles shutterless: %10d\n",Ti);

        #endif
        PRINTF("Call cluster\n");
        //pi_gpio_pin_write(NULL, USER_GPIO , 1);
        //Calling warm constructor to allocate only L1
        if(lynredCNN_Construct(1)){
            printf("Error allocating L1 for cluster...\n");
            pmsis_exit(-1);
        }
        pi_cluster_send_task_to_cl(&cluster_dev, task);
        //Calling warm destructor to deallocate only L1
        lynredCNN_Destruct(1);

        #ifdef SAVE_TO_PC
        char string_buffer[50];
        sprintf(string_buffer, "../../../dump_out_imgs/img_%04ld.pgm", save_index);
        unsigned char *img_out_ptr = ImageIn;
        for(int i=0;i<W*H;i++) img_out_ptr[i] = (uint8_t) (ImageIn[i] >> (INPUT1_Q - 8));
        drawBboxes(&bbxs,img_out_ptr);
        WriteImageToFile(string_buffer, W, H, img_out_ptr);
        save_index++;
        #endif

        #if defined USE_BLE
        sendResultsToBle(&bbxs);
        #ifndef SAVE_TO_PC
        pi_time_wait_us(2 * 1000 * 1000);
        #endif
        #endif

        //This is not the optimized deep sleep, when should take care of pad setting in sleep
        //and to properly shutdown all external devices
        //go_to_sleep();
    }

    lynredCNN_Destruct(0);

    // Close the cluster
    pi_cluster_close(&cluster_dev);

    PRINTF("Ended\n");
    pmsis_exit(0);
}


int main(void)
{
    PRINTF("\n\n\t *** Therm Eye ***\n");
    PRINTF("\n\t   %s Version\n\n", FLASH_NAME);
    return pmsis_kickoff((void *) peopleDetection);
}
