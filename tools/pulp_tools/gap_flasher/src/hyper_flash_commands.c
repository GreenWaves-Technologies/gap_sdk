
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
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "rt/rt_api.h"
#include "hyper_flash.h"
#include "hyper_flash_commands.h"

//define MAX_BUFF_SIZE (350*1024)
#define PAGE_SIZE 256
#define BUFFER_SIZE             128
#define DEVICE_READY_OFFSET     7
#define ERASE_STATUS_OFFSET     5
#define PROGRAM_STATUS_OFFSET   4

// Write and read address
#define SA                      0x0000

typedef struct {
    unsigned short data;
    unsigned short addr;
} cmdSeq;

/* // Chip erase */
RT_L2_DATA  cmdSeq eraseSeq[6] = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x80, 0x555}, {0xAA, 0x555}, {0x55, 0x2AA}, {0x10, 0x555}};

// Sector erase
//RT_L2_DATA  cmdSeq eraseSeq[6] = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x80, 0x555}, {0xAA, 0x555}, {0x55, 0x2AA}, {0x30, SA}};
RT_L2_DATA  cmdSeq VCRSeq[4]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x38, 0x555}, {0x8e0b, 0x0}};
RT_L2_DATA  cmdSeq readSeq     = {0x70, 0x555};

// Write 512/4 = 128 word to Sector addr 0x4xxx
RT_L2_DATA  cmdSeq WBPSeq[4]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0x25, SA}, {BUFFER_SIZE - 1, SA}};
RT_L2_DATA  cmdSeq WPSeq[3]   = {{0xAA, 0x555}, {0x55, 0x2AA}, {0xA0, 0x555}};
RT_L2_DATA  cmdSeq BPCSeq      = {0x29, SA};


RT_L2_DATA  unsigned short status_reg = 0;

unsigned short read_reg(rt_flash_t  *flash)
{
    // Read status register
    hyper_flash_write(flash, (unsigned int) &readSeq.data, readSeq.addr << 1, 0);
    hyper_flash_read (flash, (unsigned int) &status_reg, 0, 2, 0);

    return status_reg;
}

void conf_flash(rt_flash_t  *flash)
{
    // Set VCR to 5 delay cycles
    for (int i = 0; i < 4; i++)
        hyper_flash_write(flash, (unsigned int) &VCRSeq[i].data, VCRSeq[i].addr << 1, 0);
}

void erase_page_in_flash(rt_flash_t  *flash)
{
    // Erase sector 0x0E00
    for ( int i = 0; i < 6; i++)
        hyper_flash_write(flash, (unsigned int) &eraseSeq[i].data, eraseSeq[i].addr << 1, 0);
}

void write_page_in_flash(rt_flash_t  *flash, unsigned char *l2_buff, unsigned int flash_addr, unsigned int len)
{
    int i = 0;
    // Write to Buffer command sequence
    for (i = 0; i < 3; i++)
        hyper_flash_write(flash, (unsigned int) &WPSeq[i].data, WPSeq[i].addr << 1, 0);

    // Load buffer data
    rt_event_t *event = rt_event_get_blocking(NULL);
    hyper_flash_burst_write(flash, (unsigned int) l2_buff, (flash_addr), len, event);
    rt_event_wait(event);
}

void read_page_from_flash(rt_flash_t  *flash, unsigned char *l2_buff, unsigned int flash_addr, unsigned int len)
{
    rt_event_t *event = rt_event_get_blocking(NULL);
    hyper_flash_read(flash, (unsigned int) l2_buff, flash_addr, len, event);
    rt_event_wait(event);
}

int wait_process_end(rt_flash_t *flash, unsigned int err_bit)
{
    unsigned short reg;
    // Wait the end of process
    // Status Register (SR)
    // bit 4 -> program status bit, 0-success ; 1-failure
    // bit 5 -> erase status bit,   0-success ; 1-failure
    // bit 7 -> device ready bit
    do {
        reg = read_reg(flash);

        if ( reg & (1 << err_bit))
        {
#ifdef DEBUG
            printf("[Flasher]: ERR bit = 1, an error occurs in  operation\n");
#endif
            return 1;
        }
    } while( !(reg & (1 << DEVICE_READY_OFFSET)));

    return 0;
}

void hyperflash_init(rt_flash_t *flash)
{
    // Enable uDMA HYPER
    if( hyper_init() ) printf("[Flasher]: init error.\n");

    // Configuration
    conf_flash(flash);
    printf("[Flasher]: conf finished.\n");

    // Erase chip
    erase_page_in_flash(flash);
    wait_process_end(flash,  ERASE_STATUS_OFFSET);

#ifdef DEBUG
    printf("[Flasher]: Erase finished.\n");
#endif
}

unsigned int calculCheckSum(unsigned char *L2_Buffer, unsigned int sizeData)
{
    unsigned int i;
    unsigned int check = 0;
    for (i=0; i<sizeData; i++){
        check += L2_Buffer[i];
    }
#ifdef DEBUG
    printf("[Flasher]: checkSum: %X\n", check);
#endif
    return check;
}

int write_bloc_to_hyperflash(rt_flash_t *flash, unsigned int flash_addr,  unsigned char *L2_Buffer, unsigned int sizeData)
{
    unsigned int i;
    unsigned int j;
    unsigned int nbrPage;
    unsigned int sizeLast = 0;
    unsigned int tmp_flash_addr ;
    unsigned int checkSum_tx = 0;
    unsigned int checkSum_rx = 0;

    if (sizeData%256){
        nbrPage = (sizeData/256) + 1;
        sizeLast = sizeData%256;
    }else{
        nbrPage = (sizeData/256);
    }
#ifdef DEBUG
    printf("[Flasher] : flash start address : %x \n", flash_addr) ;
    printf("[Flasher] : %x %x %x %x\n", L2_Buffer[0], L2_Buffer[1], L2_Buffer[2], L2_Buffer[3]) ;
    printf("[Flasher] : %d bytes of data, %d Page would be loaded, %d bytes in last page\n", sizeData, nbrPage, sizeLast);
#endif
    checkSum_tx = calculCheckSum(L2_Buffer, sizeData);

    // Write buffer
    tmp_flash_addr = flash_addr;
    for (i=0; i<nbrPage; i++){
        if (sizeLast && i == (nbrPage-1)){
            write_page_in_flash(flash, &L2_Buffer[i*PAGE_SIZE], tmp_flash_addr, sizeLast);
        }else
            write_page_in_flash(flash, &L2_Buffer[i*PAGE_SIZE], tmp_flash_addr, PAGE_SIZE);
        wait_process_end(flash,  PROGRAM_STATUS_OFFSET);
        tmp_flash_addr += PAGE_SIZE ;
    }

#ifdef DEBUG
    printf("[Flasher]: Write finished, %d pages should be writen, %x, with i = %d\n", nbrPage, tmp_flash_addr, i);
#endif

    for (i=0; i<sizeData; i++)
        L2_Buffer[i] = 0x0;


    // Read buffer burst
    tmp_flash_addr = flash_addr;
#ifdef DEBUG
    printf("GAP: read start, %d pages should be read, %x\n", nbrPage, tmp_flash_addr);
#endif
    for (i=0; i<nbrPage; i++){
        if (sizeLast && i == (nbrPage-1))
            read_page_from_flash(flash, &L2_Buffer[i*PAGE_SIZE], tmp_flash_addr, sizeLast);
        else
            read_page_from_flash(flash, &L2_Buffer[i*PAGE_SIZE], tmp_flash_addr, PAGE_SIZE);

        tmp_flash_addr += PAGE_SIZE ;
    }
#ifdef DEBUG
    printf("[Flasher] : %x %x %x %x\n", L2_Buffer[0], L2_Buffer[1], L2_Buffer[2], L2_Buffer[3]) ;
#endif

    checkSum_rx = calculCheckSum(L2_Buffer, sizeData);

    int error = 0;
    if (checkSum_tx !=checkSum_rx)
        error++;

#ifdef DEBUG
    printf("[Flasher]: Checksum Compare finished\n");
#endif


#ifdef DEBUG
    if (error) printf("[Flasher]: Test FAILED with %d errors: checkSum tx %X - checkSum rx %X\n", error, checkSum_tx, checkSum_rx);
    else printf("[Flasher]: Test SUCCESS: checkSum tx %X - checkSum rx %X \n", checkSum_tx, checkSum_rx);
#endif


    return error;
}

