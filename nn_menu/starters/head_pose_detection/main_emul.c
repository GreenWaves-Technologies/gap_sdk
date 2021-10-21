/*
 * Copyright (c) 2019-2020 GreenWaves Technologies SAS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of GreenWaves Technologies SAS nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Gap.h"

#include "gaplib/ImgIO.h"
#include "ResizeBasicKernels.h"

#include "vww17Kernels.h"

#define MAX(a, b) (((a)>(b))?(a):(b))

#ifdef ALREADY_RESIZED
    #define INPUT_WIDTH         AT_INPUT_WIDTH
    #define INPUT_HEIGHT        AT_INPUT_HEIGHT
#else
    #define INPUT_WIDTH         320
    #define INPUT_HEIGHT        240
#endif
#define INPUT_SIZE              (INPUT_WIDTH*INPUT_HEIGHT*1)
#define AT_INPUT_SIZE           (AT_INPUT_WIDTH*AT_INPUT_HEIGHT*1)
#define NEW_WIDTH               220
#define NEW_HEIGHT              165

char *ImageName;
unsigned char Input_Image[INPUT_SIZE];
unsigned char Image_Resized[NEW_WIDTH*AT_INPUT_HEIGHT];
char Net_Input[AT_INPUT_SIZE];
char Out_Head[1];
char Out_Look[1];
AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;

int body()
{
    char string_buffer[64];

    PRINTF("Start ReID Demo Application\n");

    PRINTF("vww17CNN_Construct..\n");
    if (__PREFIX(CNN_Construct)())
    {
        printf("Graph constructor exited with an error\n");
        return 1;
    }    
    PRINTF("vww17CNN_Construct..done\n");

    PRINTF("Reading image\n");
    if (ReadImageFromFile(ImageName, INPUT_WIDTH, INPUT_HEIGHT, 1, Input_Image, INPUT_SIZE*sizeof(char), IMGIO_OUTPUT_CHAR, 0)) {
        printf("Failed to load image %s\n", ImageName);
        return 1;
    }

    #ifdef ALREADY_RESIZED
        vww17CNN(Input_Image, Out_Head, Out_Look);
    #else
        KerResizeBilinear_ArgT ResizeArg;
            ResizeArg.In             = Input_Image;
            ResizeArg.Win            = INPUT_WIDTH;
            ResizeArg.Hin            = INPUT_HEIGHT;
            ResizeArg.Out            = Image_Resized;
            ResizeArg.Wout           = NEW_WIDTH;
            ResizeArg.Hout           = NEW_HEIGHT;
            ResizeArg.HTileOut       = NEW_HEIGHT;
            ResizeArg.FirstLineIndex = 0;
        PRINTF("Resizing...");
        KerResizeBilinear(&ResizeArg);

        int offset_w = (NEW_WIDTH - AT_INPUT_WIDTH) >> 1;
        int offset_h = (NEW_HEIGHT - AT_INPUT_HEIGHT) >> 1;
        int idx=0;
        for(int i=0; i<AT_INPUT_HEIGHT; i++){
            for (int j=0; j<AT_INPUT_WIDTH; j++){
                Net_Input[idx++] = Image_Resized[(i+offset_h)*NEW_WIDTH+j+offset_w];
            }
        }
        vww17CNN(Net_Input, Out_Head, Out_Look);
    #endif

    printf("input:\t%s\tHEAD(%d)\tLOOK(%d)\n", ImageName, Out_Head[0], Out_Look[0]);

    vww17CNN_Destruct();
    return 0;
}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        PRINTF("Usage: ./exe [image_file]\n");
        exit(-1);
    }
    PRINTF("\n\n\t *** OCR SSD Emul ***\n\n");
    for (int i=1; i<argc; i++){
        ImageName = argv[i];
        body();
    }
    return 0;
}
