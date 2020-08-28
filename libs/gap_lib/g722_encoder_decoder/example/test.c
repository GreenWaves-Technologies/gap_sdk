/*
 * Copyright (c) 2014-2016 Sippy Software, Inc., http://www.sippysoft.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "pmsis.h"
#include "bsp/fs.h"
#include "bsp/flash/hyperflash.h"

#include "g722_encoder.h"
#include "g722_decoder.h"

#define INPUT_FILE "file.wav"
#define OUTPUT_FILE "../../../encoded_file"
#define WAV_HEADER_SIZE     44
#define CHUNK_SIZE 264
#define ENCODED_BUF_SIZE    66

int8_t ibuf[CHUNK_SIZE];
int8_t obuf[CHUNK_SIZE];
int8_t *input_l1;
uint8_t *output_l1;

void test_encoder()
{
    G722_ENC_CTX *g722_ectx;
    int i, srate;
    int bytes_read = 1;
    int encoded_size = 0;

    pi_fs_file_t *fi;
    pi_fs_file_t *fo;
    struct pi_device fs;
    struct pi_device hyperflash;
    struct pi_hyperflash_conf flash_conf;
    struct pi_fs_conf   fs_conf;

    pi_freq_set(PI_FREQ_DOMAIN_FC, 250000000);

    input_l1 = (int8_t *) pi_fc_l1_malloc(CHUNK_SIZE);
    if (input_l1 == NULL)
    {
        printf("Buff alloc failed !\n");
        pmsis_exit(-1);
    }

    output_l1 = (uint8_t *) pi_fc_l1_malloc(ENCODED_BUF_SIZE);
    if (output_l1 == NULL)
    {
        printf("Buff alloc failed !\n");
        pmsis_exit(-1);
    }

    /* Init & open flash. */
    pi_hyperflash_conf_init(&flash_conf);
    pi_open_from_conf(&hyperflash, &flash_conf);
    if (pi_flash_open(&hyperflash))
    {
        printf("Error flash open !\n");
        pmsis_exit(-2);
    }

    /* Init & open read filesystem and fi. */
    pi_fs_conf_init(&fs_conf);

    fs_conf.type = PI_FS_READ_ONLY;
    fs_conf.flash = &hyperflash;
    pi_open_from_conf(&fs, &fs_conf);
    if (pi_fs_mount(&fs))
    {
        printf("Error FS mounting !\n");
        pmsis_exit(-2);
    }
    fi = pi_fs_open(&fs, INPUT_FILE, 0);
    if (fi == NULL) {
        printf("cannot open %s\n", INPUT_FILE);
        pmsis_exit(-2);
    }
    printf("%s opened\n", INPUT_FILE);


    fs_conf.type = PI_FS_HOST;
    pi_open_from_conf(&fs, &fs_conf);
    if (pi_fs_mount(&fs))
    {
        printf("Error FS mounting !\n");
        pmsis_exit(-3);
    }
    fo = pi_fs_open(&fs, INPUT_FILE, 1);
    if (fo == NULL) {
        printf("cannot open %s\n", OUTPUT_FILE);
        pmsis_exit(-3);
    }
    printf("%s opened\n", OUTPUT_FILE);


    g722_ectx = g722_encoder_new(64000, 0);
    if (g722_ectx == NULL) {
        printf("g722_encoder_new() failed\n");
        pmsis_exit(-3);
    }
    printf("encoder new allocated\n");

	pi_fs_seek(fi, 6);

    /* Init & open dmacpy. */
    struct pi_device dmacpy;
    struct pi_dmacpy_conf dmacpy_conf = {0};
    pi_dmacpy_conf_init(&dmacpy_conf);
    pi_open_from_conf(&dmacpy, &dmacpy_conf);
    if (pi_dmacpy_open(&dmacpy))
    {
        printf("Error dmacpy open !\n");
        pmsis_exit(-4);
    }

    pi_perf_conf(1 << PI_PERF_CYCLES | 1 << PI_PERF_ACTIVE_CYCLES);
    pi_perf_start();
    uint32_t start, end;

    while (bytes_read > 0) {
        bytes_read = pi_fs_read(fi, ibuf, CHUNK_SIZE);
        if (bytes_read){
            for(i=0; i<bytes_read; i++)
                printf("%d ", ibuf[i]);
            printf("\n\n");

            pi_perf_start();
            start = pi_perf_read(PI_PERF_ACTIVE_CYCLES);

            if(pi_dmacpy_copy(&dmacpy, (void *) ibuf, (void *) input_l1, (bytes_read+3)&0xfffc, PI_DMACPY_L2_FC_L1)) printf("copy from L2 to L1 failed\n");
            encoded_size = g722_encode(g722_ectx, (int16_t *) input_l1, bytes_read/ sizeof(int16_t), output_l1);
            if(pi_dmacpy_copy(&dmacpy, (void *) output_l1, (void *) obuf, (encoded_size+3)&0xfffc, PI_DMACPY_FC_L1_L2))
                printf("copy from L1 to L2 failed\n");
            pi_perf_stop();
            end = pi_perf_read(PI_PERF_ACTIVE_CYCLES);

            printf("encode 1 frame takes: %d cycles \n", (end - start));

            for(i=0; i<encoded_size; i++)
                printf("%d ", obuf[i]);
            printf("\n");

            pmsis_exit(0);

            pi_fs_write(fo, obuf, encoded_size);
        }
    }
    printf("encode done\n");

    pi_fs_close(fo);
    pi_fs_close(fi);
    pi_fs_unmount(&fs);
    pi_flash_close(&hyperflash);

    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS G722 encoder Test ***\n\n");
    return pmsis_kickoff((void *) test_encoder);
}
