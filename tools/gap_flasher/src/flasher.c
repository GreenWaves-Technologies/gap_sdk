
/*
 * Copyright (c) 2017 GreenWaves Technologies SAS
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



/*
 * This test toggle gpio
 */

#include <stdio.h>
#include "rt/rt_api.h"
#include "hyper_flash.h"
#include "hyper_flash_commands.h"

#define MAX_BUFF_SIZE (350*1024)
#define VERSION "1.0"

static rt_flash_t *flash;
static rt_event_sched_t sched;

typedef struct {
    unsigned int	Image_Ready;
    unsigned int	Flasher_Ready;
    unsigned int    Flash_Addr;
    unsigned int    IterTimes;
    unsigned int    BufferSize;
    unsigned char   *L2_Buffer;
} fileDescriptor;
volatile RT_L2_DATA fileDescriptor flasherHeader;

#ifdef fileIO
RT_L2_DATA char*	        Image_Name;
RT_L2_DATA unsigned int 	Image_Size;

static void flasherInit(){
    Image_Name = "../../../test_FlashImg/testFlashImg.raw";
    Image_Size = 40886;
    flasherHeader.Flash_Addr = 0;
}

int flashImageInit(){
    flasherInit();
    rt_bridge_connect(NULL);
    int file = rt_bridge_open(Image_Name, 0, 0, NULL);
    if(file == -1) return -1;
    else return file;
}
#endif

static void bufferInit(unsigned int size){
    for (unsigned int i=0; i<size; i++)
        flasherHeader.L2_Buffer[i] = 0;
}

static void headerInit(){
    flasherHeader.Image_Ready = 0;
    flasherHeader.Flasher_Ready = 0;
    flasherHeader.Flash_Addr = 0;
    flasherHeader.IterTimes = 0;
    flasherHeader.L2_Buffer = NULL;
}

int main()
{
    int error = 0;
    int imageSize = 0;
    int readSize = 0;
    int nIter = 0;
    int lastSize = 0;
    int res = 0;
    unsigned int flashAddr = 0;
    headerInit();
    printf ("[Flasher]: Flasher Version %s start\n", VERSION);

    rt_event_sched_init(&sched);
    if (rt_event_alloc(&sched, 4)) return -1;

#ifdef fileIO
    int file = flashImageInit();
    if(file == -1){
        printf("[Flasher]: flashImage read failed\n");
        return -1;
    }
    imageSize = ((Image_Size + 3) & ~3);

    if (imageSize < MAX_BUFF_SIZE){
        flasherHeader.L2_Buffer = (unsigned char *) rt_alloc(RT_ALLOC_L2_CL_DATA, imageSize);
        lastSize = imageSize;
        nIter = 1;
    }
    else
#endif
    {
        flasherHeader.L2_Buffer = (unsigned char *) rt_alloc(RT_ALLOC_L2_CL_DATA, MAX_BUFF_SIZE);
        flasherHeader.Flasher_Ready = 1;
#ifdef fileIO
        lastSize = imageSize % MAX_BUFF_SIZE;
        if(lastSize) nIter = imageSize / MAX_BUFF_SIZE + 1;
        else nIter = imageSize / MAX_BUFF_SIZE;
#endif
    }
    if(!flasherHeader.L2_Buffer) return -1;

    rt_flash_conf_t flash_conf;
    rt_flash_conf_init(&flash_conf);

    flash = rt_flash_open("hyperflash", &flash_conf, NULL);
    if (flash == NULL){
        printf("[Flasher]: hyper flash open failed\n");
        return -1;
    }
    hyperflash_init(flash);

#ifndef fileIO
    // Waiting for the bridge write the buffer to the descriptor
    while (((volatile fileDescriptor *) &flasherHeader)->Image_Ready == 0) {}
    flasherHeader.Flasher_Ready = 0;
    nIter = flasherHeader.IterTimes;
    imageSize = flasherHeader.BufferSize;
#endif
    flashAddr = flasherHeader.Flash_Addr;

    for (int i=0; i<nIter; i++){
#ifdef fileIO
        if (lastSize && i == (nIter-1)){
            bufferInit(lastSize);
            res = rt_bridge_read(file, flasherHeader.L2_Buffer, lastSize, NULL);
            if (!res) return -1;
            write_bloc_to_hyperflash(flash, flashAddr, flasherHeader.L2_Buffer, lastSize) ;
        }else{
            bufferInit(MAX_BUFF_SIZE);
            res = rt_bridge_read(file, flasherHeader.L2_Buffer, MAX_BUFF_SIZE, NULL);
            if (!res) return -1;
            write_bloc_to_hyperflash(flash, flashAddr, flasherHeader.L2_Buffer, MAX_BUFF_SIZE) ;
            flashAddr += MAX_BUFF_SIZE;
        }
#else
        if(i!=0){
            while (((volatile fileDescriptor *) &flasherHeader)->Image_Ready == 0) {}
            flasherHeader.Flasher_Ready = 0;
            imageSize = flasherHeader.BufferSize;
        }
        write_bloc_to_hyperflash(flash, flashAddr, flasherHeader.L2_Buffer, imageSize) ;
        flashAddr += imageSize;
        flasherHeader.Image_Ready = 0;
        flasherHeader.Flasher_Ready = 1;
#endif
    }
    flasherHeader.Flasher_Ready = 1;

    printf("[Flasher]: Flasher Finish\n") ;

    hyper_close();

    return error;
}


