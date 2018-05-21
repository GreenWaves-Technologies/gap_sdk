
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
    char*	Image_Name;
    unsigned int 	Image_Size;
    unsigned int	Image_Ready;
    unsigned int    Flash_Addr;
} fileDescriptor;

volatile RT_L2_DATA unsigned int flasher_ready=0 ;
volatile RT_L2_DATA unsigned int flasher_finished=0 ;

volatile RT_L2_DATA fileDescriptor flasherHeader;
RT_L2_DATA unsigned char *L2_Buffer;

static void debugInit(){
    //flasherHeader.Image_Name = "../../../test_FlashImg/flashImg.raw";
    //flasherHeader.Image_Size = 10122563;
    flasherHeader.Image_Name = "../../../test_FlashImg/FlashImage.dat";
    flasherHeader.Image_Size = 565696;
    //flasherHeader.Image_Name = "../../../test_FlashImg/testFlashImg.raw";
    //flasherHeader.Image_Size = 40886;
    flasherHeader.Image_Ready = 1;
    flasherHeader.Flash_Addr = 0;
}

int flashImageInit(){
    int debug = 1;
    if(!debug) while (((volatile fileDescriptor *) &flasherHeader)->Image_Ready == 0) {}
    else debugInit();
    printf("Connecting to bridge\n");
    rt_bridge_connect(NULL);
    printf("Connection done\n");
    int file = rt_bridge_open(flasherHeader.Image_Name, 0, 0, NULL);
    if(file == -1) return -1;
    else return file;
}

static void bufferInit(unsigned int size){
    for (unsigned int i=0; i<size; i++)
        L2_Buffer[i] = 0;
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
    printf ("GAP: Flasher Version %s start\n", VERSION);

    rt_event_sched_init(&sched);
    if (rt_event_alloc(&sched, 4)) return -1;

    int file = flashImageInit();
    if(file == -1){
        printf("flashImage read failed\n");
        return -1;
    }
    imageSize = ((flasherHeader.Image_Size + 3) & ~3);
    printf("Flash Image Size: %d\n", imageSize);

    if (imageSize < MAX_BUFF_SIZE){
        L2_Buffer = (unsigned char *) rt_alloc(RT_ALLOC_L2_CL_DATA, imageSize);
        lastSize = imageSize;
    }else{
        L2_Buffer = (unsigned char *) rt_alloc(RT_ALLOC_L2_CL_DATA, MAX_BUFF_SIZE);
        lastSize = imageSize % MAX_BUFF_SIZE;
        if(lastSize) nIter = imageSize / MAX_BUFF_SIZE + 1;
        else nIter = imageSize / MAX_BUFF_SIZE;
    }
    if(!L2_Buffer) return -1;

    rt_flash_conf_t flash_conf;
    rt_flash_conf_init(&flash_conf);

    flash = rt_flash_open("hyperflash", &flash_conf, NULL);
    if (flash == NULL){
        printf("GAP: hyper flash open failed\n");
        return -1;
    }
    hyperflash_init(flash);

    flasher_ready = 1 ;
    printf("GAP: Flasher Init Finished, %d Iter will be processed\n", nIter) ;
    flashAddr = flasherHeader.Flash_Addr;

    for (int i=0; i<nIter; i++){
        if (lastSize && i == (nIter-1)){
            bufferInit(lastSize);
            res = rt_bridge_read(file, L2_Buffer, lastSize, NULL);
            if (!res) return -1;
            write_bloc_to_hyperflash(flash, flashAddr, L2_Buffer, lastSize) ;
        }else{
            bufferInit(MAX_BUFF_SIZE);
            res = rt_bridge_read(file, L2_Buffer, MAX_BUFF_SIZE, NULL);
            if (!res) return -1;
            write_bloc_to_hyperflash(flash, flashAddr, L2_Buffer, MAX_BUFF_SIZE) ;
            flashAddr += MAX_BUFF_SIZE;
        }
    }
    printf("GAP: Flasher Finish received\n") ;

    hyper_close();

    return error;
}


