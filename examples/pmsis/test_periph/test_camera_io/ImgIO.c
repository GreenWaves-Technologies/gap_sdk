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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pmsis.h"
#include "ImgIO.h"
#include "bsp/bsp.h"
#include "bsp/fs.h"

#define PPM_HEADER 40

#define Max(a, b)               (((a)>(b))?(a):(b))
#define Min(a, b)               (((a)<(b))?(a):(b))

#define ALIGN(Value, Size)      (((Value)&((1<<(Size))-1))?((((Value)>>(Size))+1)<<(Size)):(Value))

#define CHUNK_SIZE 8192

#define PRINTF printf

static void progress_bar(char * OutString, int n, int tot)
{
    int tot_chars = 30;
    PRINTF("%s",OutString);
    PRINTF(" [");
    int chars = (n*tot_chars)/tot;

    for (int i=0; i<tot_chars; i++)
    {
        if (i<=chars)
        {
            PRINTF("#");
        }
        else
        {
            PRINTF(" ");
        }
    }
    PRINTF("]");
    PRINTF("\n");
}

static unsigned int SkipComment(unsigned char *Img, unsigned int Ind)
{
    while (Img[Ind] == '#')
    {
        while (Img[Ind] != '\n')
        {
            PRINTF("%c", Img[Ind]);
            Ind++;
        }
        Ind++;
    }
    return Ind;
}

static unsigned int ReadPPMHeader(unsigned char *ImgIn, unsigned int *W, unsigned int *H, unsigned int *IsRGB)
{
#define IS_DIGIT(C) (((C) >= '0') && ((C) <= '9'))
    unsigned int Val, Ind = 0;

    if ((ImgIn[0] == 'P') && (ImgIn[1] == '5') && (ImgIn[2] == '\n'))
    {
        *IsRGB = 0;
    }
    else if ((ImgIn[0] == 'P') && (ImgIn[1] == '6') && (ImgIn[2] == '\n'))
    {
        *IsRGB = 1;
    }
    else
    {
        return 0;
    }
    Ind = 3;

    Ind=SkipComment(ImgIn, Ind);
    while (!IS_DIGIT(ImgIn[Ind]))
    {
        Ind++;
    }
    Val = 0;
    while (IS_DIGIT(ImgIn[Ind]))
    {
        Val = Val*10 + (ImgIn[Ind] - 0x30);
        Ind++;
    }
    *W = Val;
    Ind=SkipComment(ImgIn, Ind);
    while (!IS_DIGIT(ImgIn[Ind]))
    {
        Ind++;
    }
    Val = 0;
    while (IS_DIGIT(ImgIn[Ind]))
    {
        Val = Val*10 + (ImgIn[Ind] - 0x30);
        Ind++;
    }
    *H = Val;

    Ind=SkipComment(ImgIn, Ind);
    while (!IS_DIGIT(ImgIn[Ind]))
    {
        Ind++;
    }
    Val = 0;
    while (IS_DIGIT(ImgIn[Ind]))
    {
        Val = Val*10 + (ImgIn[Ind] - 0x30);
        Ind++;
    }
    if (Val != 255)
    {
        return 0;
    }
    while (ImgIn[Ind] != 0xA)
    {
        Ind++;
    }

    return (Ind+1);
#undef IS_DIGIT
}

static unsigned int GetInputImageInfos(char *Name, unsigned int *W, unsigned int *H, unsigned int *IsRGB, unsigned int *HeaderSize)
{
    struct pi_fs_conf conf;
    pi_fs_conf_init(&conf);
    struct pi_device fs;

    conf.type = PI_FS_HOST;

    pi_open_from_conf(&fs, &conf);

    if (pi_fs_mount(&fs))
        return -2;

    static int Debug = 0;
    void *File = pi_fs_open(&fs, Name, PI_FS_FLAGS_READ);
    unsigned int Err = 0;

    *W = 0; *H = 0; *IsRGB = 0; *HeaderSize = 0;

    if (Debug)
    {
        PRINTF("File: %s open: %s\n", Name, File?"Ok":"Failed");
    }
    if (File)
    {
        unsigned char *Header = (unsigned char *) pmsis_l2_malloc(256);
        Err |= (Header == NULL);
        if (Err)
        {
            return Err;
        }
        if (pi_fs_read(File,Header, 256) == 256)
        {
            unsigned int i;
            *HeaderSize = ReadPPMHeader(Header, W, H, IsRGB);
            if (Debug)
            {
                PRINTF("Image %s:  [W: %d, H: %d] %s, HeaderSize: %d\n", Name, *W, *H, *IsRGB?"Color":"Gray", *HeaderSize);
                for (i=0; i<*HeaderSize; i++)
                {
                    PRINTF("%c", Header[i]);
                }
                PRINTF("\n");
            }
        }
        else
        {
            Err = 2;
        }
        pmsis_l2_malloc_free(Header, 256);
        pi_fs_close(File);
        pi_fs_unmount(&fs);
    }
    return Err;
}

unsigned char *ReadImageFromFile(char *ImageName, unsigned int *W, unsigned int *H, unsigned char *InBuffer, unsigned int BuffSize)
{
    void *File = NULL;
    unsigned int IsRGB, HeaderSize, Size, AlignedSize, ReadSize=0;
    unsigned char *ImagePtr = 0;
    int Allocated = 0;

    struct pi_fs_conf conf;
    pi_fs_conf_init(&conf);
    struct pi_device fs;

    conf.type = PI_FS_HOST;

    pi_open_from_conf(&fs, &conf);

    if (pi_fs_mount(&fs))
        return NULL;

    if (GetInputImageInfos(ImageName, W, H, &IsRGB, &HeaderSize))
    {
        PRINTF("Failed to get input images infos, %s\n", ImageName); goto Fail;
    }
    if (IsRGB)
    {
        PRINTF("Only Gray levels supported, found RGB\n"); goto Fail;
    }
    File = pi_fs_open(&fs, ImageName, PI_FS_FLAGS_READ);
    if (File == 0)
    {
        PRINTF("Failed to open file, %s\n", ImageName); goto Fail;
    }
    Size = (*W)*(*H)*(IsRGB?3:1);
    if (InBuffer && (BuffSize >= Size))
    {
        AlignedSize = Size;
        ImagePtr = InBuffer;
    }
    else
    {
        Allocated = 1;
        AlignedSize = ALIGN(Size, 2);
        ImagePtr = (unsigned char *) pmsis_l2_malloc(AlignedSize);
    }
    if (ImagePtr == 0)
    {
        PRINTF("Failed to allocate %d bytes for input image\n", AlignedSize); goto Fail;
    }
    pi_fs_seek(File,HeaderSize);
    {
        unsigned char *TargetImg = ImagePtr;
        unsigned int RemainSize = AlignedSize;

        while (RemainSize > 0)
        {
            unsigned int Chunk = Min(4096, RemainSize);
            unsigned R = pi_fs_read(File,TargetImg, Chunk);
            ReadSize+=R;
            if (R!=Chunk) break;
            TargetImg += Chunk; RemainSize -= Chunk;
        }
    }
    if (AlignedSize!=ReadSize)
    {
        PRINTF("Error, expects %d bytes but got %d\n", AlignedSize, ReadSize); goto Fail;
    }
    pi_fs_close(File);
    pi_fs_unmount(&fs);
    PRINTF("Image %s, [W: %d, H: %d], Gray, Size: %d bytes, Loaded sucessfully\n", ImageName, *W, *H, AlignedSize);

    return (ImagePtr);
Fail:
    if (ImagePtr && Allocated)
    {
        pmsis_l2_malloc_free(ImagePtr, AlignedSize);
    }
    pi_fs_close(File);
    pi_fs_unmount(&fs);
    PRINTF("Failed to load image %s from flash\n", ImageName);
    return 0;
}

static void WritePPMHeader(void *FD, unsigned int W, unsigned int H)
{
    unsigned int Ind = 0, x, i, L;
    unsigned char *Buffer = (unsigned char *) pmsis_l2_malloc(PPM_HEADER * sizeof(unsigned char));

    /* P5<cr>* */
    Buffer[Ind++] = 0x50; Buffer[Ind++] = 0x35; Buffer[Ind++] = 0xA;

    /* W <space> */
    x = W; L=0;
    while (x>0)
    {
        x = x/10;
        L++;
    }
    x = W; i = 1;
    while (x>0)
    {
        Buffer[Ind+L-i] = 0x30 + (x%10);
        i++;
        x=x/10;
    }
    Ind += L;
    Buffer[Ind++] = 0x20;

    /* H <cr> */
    x = H; L=0;
    while (x>0)
    {
        x = x/10;
        L++;
    }
    x = H; i = 1;
    while (x>0)
    {
        Buffer[Ind+L-i] = 0x30 + (x%10);
        i++;
        x=x/10;
    }
    Ind += L;
    Buffer[Ind++] = 0xA;

    /* 255 <cr> */
    Buffer[Ind++] = 0x32;
    Buffer[Ind++] = 0x35;
    Buffer[Ind++] = 0x35;
    Buffer[Ind++] = 0xA;

    for (unsigned int a=0; a<Ind; a++)
    {
        pi_fs_write(FD,&(Buffer[a]), sizeof(unsigned char));
    }

    pmsis_l2_malloc_free(Buffer, PPM_HEADER * sizeof(unsigned char));
}

int WriteImageToFile(char *ImageName, unsigned int W, unsigned int H, unsigned char *OutBuffer, unsigned char PixelSize)
{
    struct pi_fs_conf conf;
    pi_fs_conf_init(&conf);
    struct pi_device fs;

    conf.type = PI_FS_HOST;

    pi_open_from_conf(&fs, &conf);

    if (pi_fs_mount(&fs))
        return 0;

    void *File = pi_fs_open(&fs, ImageName, PI_FS_FLAGS_WRITE);

    int ret = 0;
//    WritePPMHeader(File,W,H);

	int steps = (W*H*PixelSize) / CHUNK_SIZE;

    for(int i=0;i<steps;i++){
        progress_bar("Writing image ",i,steps);
        ret+=pi_fs_write(File,OutBuffer +(CHUNK_SIZE*i), CHUNK_SIZE);
    }
    if(((W*H*PixelSize) % CHUNK_SIZE) != 0)
		ret+=pi_fs_write(File,OutBuffer+(CHUNK_SIZE*steps) , ((W*H*PixelSize) % CHUNK_SIZE)*sizeof(unsigned char));

    pi_fs_close(File);
    pi_fs_unmount(&fs);

    return ret;
}
