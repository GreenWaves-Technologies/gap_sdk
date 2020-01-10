/*
 * Copyright 2019 GreenWaves Technologies, SAS
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

/* PMSIS includes. */
#include "pmsis.h"

/* App includes. */
#include "setup.h"

/* PMSIS BSP includes. */
#include "bsp/bsp.h"
#include "bsp/buffer.h"
#include "bsp/display.h"
#include "bsp/display/ili9341.h"


/* Autotiler includes. */
#if defined(USE_AUTOTILER)
#include "ResizeKernels.h"
#endif  /* USE_AUTOTILER */

/* Variables used. */
uint8_t *image_in = NULL;
uint8_t *image_out = NULL;
uint8_t *image_bw = NULL;

#if defined(USE_AUTOTILER)
typedef struct ArgCluster
{
    uint32_t Win;
    uint32_t Hin;
    uint32_t Wout;
    uint32_t Hout;
    uint8_t *ImageIn;
    uint8_t *ImageOut;
} ArgCluster_T;

static void cluster_camera(ArgCluster_T *ArgC)
{
    /* Launching resize on cluster. */
    ResizeImage(ArgC->ImageIn, ArgC->ImageOut);
}
#endif  /* USE_AUTOTILER */

void image_crop(uint8_t *input, uint32_t w_in, uint32_t h_in,
                uint8_t *output, uint32_t w_out, uint32_t h_out)
{
    uint32_t h_step = ((w_in - 1) << 16) / w_out;
    uint32_t w_step = ((h_in - 1) << 16) / h_out;
    uint32_t h_coef = h_step * 0;
    uint32_t base_r = h_coef >> 16;
    for (uint32_t r = 0; r < h_out; r++)
    {
        uint32_t offset_r = (h_coef >> 16) - base_r;
        uint32_t h_c2 = (h_coef >> 9) & 0x7F;
        uint32_t h_c1 = 0x80 - h_c2;
        uint32_t w_coef = 0 * w_step;
        for (uint32_t c = 0; c < w_out; c++)
        {
            uint32_t offset_c = (w_coef >> 16);
            uint32_t w_c2 = (w_coef >> 9) & 0x7F;
            uint32_t w_c1 = 0x80 - w_c2;
            uint32_t P1 = input[(offset_r + 0) * w_in + (offset_c + 0)];
            uint32_t P2 = input[(offset_r + 1) * w_in + (offset_c + 0)];
            uint32_t P3 = input[(offset_r + 0) * w_in + (offset_c + 1)];
            uint32_t P4 = input[(offset_r + 1) * w_in + (offset_c + 1)];
            output[r * w_out + c] = ((((P1 * h_c1) + (P2 * h_c2)) * w_c1) +
                                     (((P3 * h_c1) + (P4 * h_c2)) * w_c2)) >> 14;
            w_coef += w_step;
        }
        h_coef += h_step;
    }
}

int ConvAt(short *In, short int *Filter, unsigned int X, unsigned int Y, unsigned int W, unsigned int H, unsigned int Norm)
{
    unsigned int i, j;
    int Acc = 0;
    unsigned int K = 5;

    for (i=0; i<K; i++) {
        for (j=0; j<K; j++) {
            Acc += In[(X+i)*W+Y+j]*Filter[K*i+j];
        }
    }
    return (gap_clip(gap_roundnorm_reg(Acc, Norm), 15));
}


void DumpPlane(char *Mess, short int *Plane, unsigned int W, unsigned int H)
{
    unsigned int i, j;

    printf("----------------- %s ------------------------\n", Mess);
    for (i=0; i<H; i++) {
        printf("%2d: ", i);
        for (j=0; j<W; j++) {
            printf("%4x ", (unsigned short) Plane[i*W+j]);
        }
        printf("\n");
    }
    printf("-----------------------------------------\n");
}

void DumpPaddedCoeff(char *Name, short int *C, unsigned int NTap, unsigned int NFilter)
{
    unsigned int i, j;
    printf("L2_MEM short int %s[] = {\n", Name);
    for (i=0; i<NFilter; i++) {
        for (j=0; j<NTap; j++) {
            printf("%d, ", C[i*NTap+j]);
        }
        printf("0,\n");
    }
    printf("};\n");
}

int CheckSum(short int *In, int Size)
{
    int i;
    int S=0;

    for (i=0; i<Size; i++) S += In[i];
    return S;
}

void Check(char *Mess, short int *Planes, int NPlane, int W, int H)
{
    int i;
    printf("Check sum for %s\n", Mess);

    for (i=0; i<NPlane; i++) {
        printf("\t%2d: %d\n", i, CheckSum(Planes + i*(W*H), W*H));
    }
}

#if defined(HAVE_CAMERA)
#if defined(HIMAX)
static int32_t open_camera_himax(struct pi_device *device)
{
    struct pi_himax_conf cam_conf;
    pi_himax_conf_init(&cam_conf);
    cam_conf.format = PI_CAMERA_QVGA;
    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    {
        return -1;
    }
    return 0;
}
#else

#define MT9V034_BLACK_LEVEL_CTRL (0x47)
#define MT9V034_BLACK_LEVEL_AUTO (0 << 0)
#define MT9V034_AEC_AGC_ENABLE   (0xAF)
#define MT9V034_AEC_ENABLE_A     (1 << 0)
#define MT9V034_AGC_ENABLE_A     (1 << 1)
#define MT9V034_AEC_ENABLE_B     (1 << 8)
#define MT9V034_AGC_ENABLE_B     (1 << 9)

static int32_t open_camera_mt9v034(struct pi_device *device)
{
    struct pi_mt9v034_conf cam_conf;

    pi_mt9v034_conf_init(&cam_conf);
    cam_conf.format = PI_CAMERA_QVGA;
    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    {
        return -1;
    }
    uint16_t val = MT9V034_BLACK_LEVEL_AUTO;
    pi_camera_reg_set(device, MT9V034_BLACK_LEVEL_CTRL, &val);
    val = MT9V034_AEC_ENABLE_A|MT9V034_AGC_ENABLE_A;
    pi_camera_reg_set(device, MT9V034_AEC_AGC_ENABLE, &val);

    //max exposure def:0x01E0
    val = 0x0FE0;
    pi_camera_reg_set(device, 0xAD, &val);
    //Max Analog Gain def: 0x0040
    val = 0x0F40;
    pi_camera_reg_set(device, 0xAB, &val);

    return 0;
}
#endif  /* HIMAX */

static int32_t open_camera(struct pi_device *device)
{
    #if defined(HIMAX)
    return open_camera_himax(device);
    #else
    return open_camera_mt9v034(device);
    #endif  /* HIMAX */
    return -1;
}
#endif  /* HAVE_CAMERA */

#if defined(HAVE_DISPLAY)
static int open_display(struct pi_device *device)
{
    struct pi_ili9341_conf ili_conf;

    pi_ili9341_conf_init(&ili_conf);

    pi_open_from_conf(device, &ili_conf);

    if (pi_display_open(device))
        return -1;
    return 0;
}
#endif  /* HAVE_DISPLAY */

void test_cam(void)
{
    printf("Entering main controller\n");

    char string_buffer[64];
    struct pi_device camera;
    struct pi_device display;
    struct pi_device cluster_dev;
    struct pi_cluster_conf cluster_conf;
    struct pi_cluster_task task = {0};
    task.stack_size = (uint32_t) STACK_SIZE;

    uint32_t input_size = sizeof(uint8_t) * CAMERA_HEIGHT * CAMERA_WIDTH;
    uint32_t resize_size = sizeof(uint8_t) * RESIZE_HEIGHT * RESIZE_WIDTH;
    image_in = (uint8_t *) pmsis_l2_malloc(input_size);
    image_out = (uint8_t *) pmsis_l2_malloc(resize_size);

    if (image_in == NULL)
    {
        printf("image_in alloc failed: %d bytes !\n", input_size);
        pmsis_exit(-1);
    }

    if (image_out == NULL)
    {
        printf("image_out alloc failed: %d bytes !\n", resize_size);
        pmsis_exit(-2);
    }

    #if defined(HAVE_DISPLAY)
    if (open_display(&display))
    {
        printf("Failed to open display\n");
        pmsis_exit(-3);
    }
    pi_display_ioctl(&display, PI_ILI_IOCTL_ORIENTATION, (void *) PI_ILI_ORIENTATION_270);
    #endif  /* HAVE_DISPLAY */

    #if defined(HAVE_CAMERA)
    if (open_camera(&camera))
    {
        printf("Failed to open camera\n");
        pmsis_exit(-4);
    }
    #endif  /* HAVE_CAMERA */

    #if defined(HAVE_BRIDGE)
    char name[] = "../../../imgTest0.pgm";
    uint32_t w_in = 0, h_in = 0;
    if ((ReadImageFromFile(name, &w_in, &h_in, image_in, input_size) == 0) ||
        (w_in != CAMERA_WIDTH) || (h_in != CAMERA_HEIGHT))
    {
        printf("Failed to load image %s or dimension mismatch Expects [%dx%d], Got [%dx%d]\n", name, CAMERA_WIDTH, CAMERA_HEIGHT, w_in, h_in);
        pmsis_exit(-5);
    }
    #endif  /* HAVE_BRIDGE */

    #if defined(USE_AUTOTILER)
    printf("Initializing cluster\n");
    /* Configure And open cluster. */
    cluster_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cluster_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-6);
    }

    /* Allocating L1 memory for cluster */
    Resize_L1_Memory = (char *) pmsis_l1_malloc(_Resize_L1_Memory_SIZE);
    if (Resize_L1_Memory == 0)
    {
        printf("Failed to allocate %d bytes for L1_memory\n", _Resize_L1_Memory_SIZE);
        pmsis_exit(-7);
    }

    ArgCluster_T cluster_call;

    task.entry = (void *) cluster_camera;
    task.arg = &cluster_call;
    printf("Cluster done\n");
    #endif  /* USE_AUTOTILER */

    static pi_buffer_t RenderBuffer;
    static pi_buffer_t ImageInBuffer;

    // WIth Himax, propertly configure the buffer to skip boarder pixels
    #if defined(HIMAX)
    ImageInBuffer.data = image_in + CAMERA_WIDTH * 2 + 2;
    ImageInBuffer.stride = 4;
    pi_buffer_init(&ImageInBuffer, PI_BUFFER_TYPE_L2, image_in + CAMERA_WIDTH * 2 + 2);
    pi_buffer_set_stride(&ImageInBuffer, 4);

    RenderBuffer.data = image_out;
    pi_buffer_init(&RenderBuffer, PI_BUFFER_TYPE_L2, image_out);
    #else
    ImageInBuffer.data = image_in;
    pi_buffer_init(&ImageInBuffer, PI_BUFFER_TYPE_L2, image_in);

    pi_buffer_init(&RenderBuffer, PI_BUFFER_TYPE_L2, image_out);
    #endif  /* HIMAX */
    pi_buffer_set_format(&ImageInBuffer, CAMERA_WIDTH, CAMERA_HEIGHT, 1, PI_BUFFER_FORMAT_GRAY);
    pi_buffer_set_format(&RenderBuffer, RESIZE_WIDTH, RESIZE_HEIGHT, 1, PI_BUFFER_FORMAT_GRAY);

    uint32_t loop = 1, index = 0;
    while (loop)
    {
        #if defined(HAVE_CAMERA)
        pi_camera_control(&camera, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&camera, image_in, CAMERA_HEIGHT * CAMERA_WIDTH);
        pi_camera_control(&camera, PI_CAMERA_CMD_STOP, 0);
        //printf("Capture done\n");
        #else
        loop = 0;
        #endif  /* HAVE_CAMERA */

        #if defined(HAVE_DISPLAY)
        pi_display_write(&display, &ImageInBuffer, 0, 0, IMG_WIDTH, IMG_HEIGHT);
        #endif  /* HAVE_DISPLAY */

        #if defined(USE_AUTOTILER)
	   cluster_call.Win      = (uint32_t) CAMERA_WIDTH;
	   cluster_call.Hin      = (uint32_t) CAMERA_HEIGHT;
	   cluster_call.Wout     = (uint32_t) RESIZE_WIDTH;
	   cluster_call.Hout     = (uint32_t) RESIZE_HEIGHT;
	   cluster_call.ImageIn  = image_in;
	   cluster_call.ImageOut = image_out;
        pi_cluster_send_task_to_cl(&cluster_dev, &task);
        #else
        image_crop(image_in, (uint32_t) CAMERA_WIDTH, (uint32_t) CAMERA_HEIGHT,
                   image_out, (uint32_t) RESIZE_WIDTH, (uint32_t) RESIZE_HEIGHT);
        #endif  /* USE_AUTOTILER */

        #if defined(HAVE_DISPLAY)
        pi_display_write(&display, &RenderBuffer, LCD_OFF_X, LCD_OFF_Y, RESIZE_WIDTH, RESIZE_HEIGHT);
        #endif  /* HAVE_DISPLAY */

        #if defined(HAVE_BRIDGE)
        /* Original image. */
        sprintf(string_buffer, "../../../pics/num_%d.pgm", index);
        WriteImageToFile(string_buffer, CAMERA_WIDTH, CAMERA_HEIGHT, image_in);
        /* Resized image. */
        sprintf(string_buffer, "../../../pics/num_%d_resize.pgm", index);
        WriteImageToFile(string_buffer, RESIZE_WIDTH, RESIZE_HEIGHT, image_out);
        index++;
        #endif  /* HAVE_BRIDGE */
    }

    pmsis_exit(0);
}

int main()
{
    printf("\n\n\t *** PMSIS Resize Test ***\n\n");
    return pmsis_kickoff((void *) test_cam);
}
