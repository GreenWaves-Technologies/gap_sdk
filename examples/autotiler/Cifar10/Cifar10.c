/*
 * Copyright (C) 2020 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

/* PMSIS includes. */
#include "pmsis.h"

/* Autotiler includes. */
#include "Gap.h"
#include "Cifar10Kernels.h"
#include "CifarData.h"

/* Variables used. */
#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTF printf
#else
#define DEBUG_PRINTF(...) ((void) 0)
#endif

#ifdef COEF_L2
#include "coef/CifarCoeff.h"
#else
#include "bsp/bsp.h"
#include "bsp/fs.h"
#include "bsp/fs/readfs.h"
#include "bsp/flash/hyperflash.h"

#define  BUFFER_SIZE   (1024)

struct pi_readfs_conf conf_fs;
struct pi_hyperflash_conf conf_flash;
struct pi_hyperram_conf conf_ram;
struct pi_device flash;
struct pi_device fs;
struct pi_device HyperRam;
pi_fs_file_t *file;
int16_t *buff;
#endif  /* COEF_L2 */

uint16_t *Filter_Layer[3] = {0};
uint16_t *Bias_Layer[3]= {0};
uint32_t Filter_Layer_Size[3] = {0};
uint32_t Bias_Layer_Size[3]= {0};
int16_t *Out_Layer[3];
uint32_t Out_Layer_Size[3] = {0};

int ConvAt(short *In, short int *Filter, unsigned int X, unsigned int Y, unsigned int W, unsigned int H, unsigned int Norm)
{
    int i, j;
    int Acc = 0;
    int K = 5;

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

unsigned int CheckSum(short int *In, int Size)
{
    int i;
    unsigned S=0;

    for (i=0; i<Size; i++) S += In[i];
    return S;
}

void Check(char *Mess, short int *Planes, int NPlane, int W, int H)
{
    int i;

    printf("Check sum for %s\n", Mess);

    for (i=0; i<NPlane; i++) {
        printf("\t%2d: %x\n", i, CheckSum(Planes + i*(W*H), W*H));
    }
}

#if !defined(COEF_L2)
void CopyFileFromFlashToL3(struct pi_device *fs, char *file_name, uint32_t *hyper_buff, uint32_t *hyper_size)
{
    DEBUG_PRINTF("Loading layer \"%s\" from FS to L3\n", file_name);
    file = pi_fs_open(fs, file_name, 0);
    if (file == NULL)
    {
        printf("File open failed !\n");
        pmsis_exit(-1);
    }

    if (pi_ram_alloc(&HyperRam, hyper_buff, file->size))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-2);
    }
    *hyper_size = file->size;
    DEBUG_PRINTF("Hyperram alloc : %x %d\n", *hyper_buff, file->size);

    uint32_t size_total = 0;
    uint32_t size = 0;
    pi_task_t task;
    do
    {
        size = pi_fs_read_async(file, buff, BUFFER_SIZE, pi_task_block(&task));
        pi_task_wait_on(&task);
        size = ((size + 3) & ~3);
        if (size)
        {
            pi_ram_write(&HyperRam, (uint32_t) (*hyper_buff+size_total), buff, size);
            size_total += size;
        }
    } while (size_total < file->size);
    DEBUG_PRINTF("Loading layer \"%s\" from FS to L3, hyper %x size = %d\n", file_name, *hyper_buff, size_total);

    pi_fs_seek(file, 0);
    pi_fs_close(file);
}
#endif  /* COEF_L2 */

static void RunCifar10(void *arg)
{
    DEBUG_PRINTF("Cluster: Start to run Cifar10\n");

    Conv5x5MaxPool2x2_SW_0(ImageIn,
                           Filter_Layer[0],
                           Bias_Layer[0],
                           Out_Layer[0]);

    Conv5x5MaxPool2x2_SW_1(Out_Layer[0],
                           Filter_Layer[1],
                           Bias_Layer[1],
                           Out_Layer[1]);

    LinearLayerReLU_1(Out_Layer[1],
                      Filter_Layer[2],
                      Bias_Layer[2],
                      Out_Layer[2]);

    DEBUG_PRINTF("Cluster: End run Cifar10\n");
}

void test_cifar10(void)
{
    printf("Entering main controller\n");
    uint8_t CheckResults = 1;

    /* Output result size. */
    Out_Layer_Size[0] = (14 * 14 * sizeof(int16_t) * 8);
    Out_Layer_Size[1] = (5 * 5 * sizeof(int16_t) * 12);
    Out_Layer_Size[2] = (1 * 1 * sizeof(int16_t) * 10);

    #if !defined(COEF_L2)
    buff = (int16_t *) pmsis_l2_malloc(BUFFER_SIZE);
    if (buff == NULL)
    {
        printf("Buffer alloc failed !\n");
        pmsis_exit(-1);
    }

    pi_hyperram_conf_init(&conf_ram);
    pi_open_from_conf(&HyperRam, &conf_ram);
    if (pi_ram_open(&HyperRam))
    {
        printf("Error ram open !\n");
        pmsis_exit(-2);
    }

    pi_hyperflash_conf_init(&conf_flash);
    pi_open_from_conf(&flash, &conf_flash);
    if (pi_flash_open(&flash))
    {
        printf("Error flash open !\n");
        pmsis_exit(-3);
    }

    pi_readfs_conf_init(&conf_fs);
    conf_fs.fs.flash = &flash;
    pi_open_from_conf(&fs, &conf_fs);

    int32_t err = pi_fs_mount(&fs);
    if (err)
    {
        printf("Error FS mounting : %d !\n", err);
        pmsis_exit(-4);
    }
    printf("FS mounted.\n");

    CopyFileFromFlashToL3(&fs, "Cifar10_Filter0.dat", &Filter_Layer[0], &Filter_Layer_Size[0]);
    CopyFileFromFlashToL3(&fs, "Cifar10_Bias0.dat",   &Bias_Layer[0], &Bias_Layer_Size[0]);
    CopyFileFromFlashToL3(&fs, "Cifar10_Filter1.dat", &Filter_Layer[1], &Filter_Layer_Size[1]);
    CopyFileFromFlashToL3(&fs, "Cifar10_Bias1.dat",   &Bias_Layer[1], &Bias_Layer_Size[1]);
    CopyFileFromFlashToL3(&fs, "Cifar10_Filter2.dat", &Filter_Layer[2], &Filter_Layer_Size[2]);
    CopyFileFromFlashToL3(&fs, "Cifar10_Bias2.dat",   &Bias_Layer[2], &Bias_Layer_Size[2]);

    pi_fs_unmount(&fs);
    printf("FS unmounted.\n");
    pi_flash_close(&flash);

    if (pi_ram_alloc(&HyperRam, (uint32_t *) &Out_Layer[0], Out_Layer_Size[0]))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-5);
    }
    #else
    /* Bias & Filters. */
    Bias_Layer[0] = Bias_Layer0;
    Bias_Layer[1] = Bias_Layer1;
    Bias_Layer[2] = Bias_Layer2;
    Filter_Layer[0] = Filter_Layer0;
    Filter_Layer[1] = Filter_Layer1;
    Filter_Layer[2] = Filter_Layer2;

    Out_Layer[0] = (int16_t *) pmsis_l2_malloc(Out_Layer_Size[0]);
    if (Out_Layer[0] == NULL)
    {
        printf("Failed to allocated memory, giving up.\n");
        pmsis_exit(-5);
    }
    #endif  /* COEF_L2 */
    else
    {
        printf("Allocating %d: OK -> %x\n", Out_Layer_Size[0], Out_Layer[0]);
    }

    Out_Layer[1] = (int16_t *) pmsis_l2_malloc(Out_Layer_Size[1]);
    Out_Layer[2] = (int16_t *) pmsis_l2_malloc(Out_Layer_Size[2]);
    if ((Out_Layer[1] == NULL) && (Out_Layer[2] == NULL))
    {
        printf("Failed to allocated memory, giving up.\n");
        pmsis_exit(-5);
    }
    else
    {
        printf("Allocating %d: OK -> %x\n", Out_Layer_Size[1], Out_Layer[1]);
        printf("Allocating %d: OK -> %x\n", Out_Layer_Size[2], Out_Layer[2]);
    }

    /* Configure And open cluster. */
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    cl_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-7);
    }

    Cifar10_L2_Memory = pmsis_l2_malloc(_Cifar10_L2_Memory_SIZE);
    if (Cifar10_L2_Memory == NULL)
    {
        printf("L2 Memory Allocation Error! Quit...\n");
        pmsis_exit(-8);
    }

    /* Allocate the predetermined memory in the shared L1 memory that the cluster can act on. */
    Cifar10_L1_Memory = pmsis_l1_malloc(_Cifar10_L1_Memory_SIZE);
    if (Cifar10_L1_Memory == NULL)
    {
        printf("Memory Allocation Error! Quit...\n");
        pmsis_exit(-8);
    }

    struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = RunCifar10;
    task->arg = NULL;
    task->stack_size = 2048*2;

    pi_cluster_send_task_to_cl(&cluster_dev, task);

    pmsis_l1_malloc_free(Cifar10_L1_Memory, _Cifar10_L1_Memory_SIZE);

    pi_cluster_close(&cluster_dev);

    if (CheckResults)
    {
        printf("L1: ");
        Check("SW   Layer0", Out_Layer[0], 8, 14, 14);
        printf("L2: ");
        Check("SW   Layer1", Out_Layer[1], 12, 5, 5);
        printf("L3: ");
        Check("SW   Layer2", Out_Layer[2], 10, 1, 1);
    }

    #if !defined(COEF_L2)
    for (uint32_t i = 0; i < 3; i++)
    {
        pi_ram_free(&HyperRam, Filter_Layer[i], Filter_Layer_Size[i]);
        pi_ram_free(&HyperRam, Bias_Layer[i], Bias_Layer_Size[i]);
    }
    pi_ram_free(&HyperRam, (uint32_t) Out_Layer[0], Out_Layer_Size[0]);
    pi_ram_close(&HyperRam);
    pmsis_l2_malloc_free(buff, BUFFER_SIZE);
    #else
    pmsis_l2_malloc_free(Out_Layer[0], Out_Layer_Size[0]);
    #endif  /* COEF_L2 */
    pmsis_l2_malloc_free(Out_Layer[1], Out_Layer_Size[1]);
    pmsis_l2_malloc_free(Out_Layer[2], Out_Layer_Size[2]);


    printf("Test success\n");
    pmsis_exit(0);
}

int main(void)
{
    printf("\n\n\t *** PMSIS Cifar10 Test ***\n\n");
    return pmsis_kickoff((void *) test_cifar10);
}

