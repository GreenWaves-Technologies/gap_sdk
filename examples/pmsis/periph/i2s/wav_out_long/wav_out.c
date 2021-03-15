/*
 * Copyright (C) 2018 GreenWaves Technologies
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
#include <string.h>
#include <pmsis.h>
#include <bsp/fs.h>
#include <bsp/flash/hyperflash.h>
#include "wav_out.h"
#include "bsp/fs/hostfs.h"
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

#define INTER_BUFF_SIZE     (1000*2)

static PI_L2 uint8_t header_buffer[WAV_HEADER_SIZE];
static uint8_t *_tmp_buffer;

static int fs_write_from_L3(void *file, void *data, int size_total, struct pi_device *ram)
{
    int l3_index = 0;
    int size = 0;
    int rest_size = size_total;
    uint32_t _l3_buff = (uint32_t) (data);

    printf("l3_buff: %x, size_total: %d\n", _l3_buff, size_total);

    _tmp_buffer = (uint8_t *) pmsis_l2_malloc((uint32_t) INTER_BUFF_SIZE);
    if (_tmp_buffer == NULL)
    {
        printf("TMP BUFFER malloc failed\n");
        return -1;
    }

    do
    {
        if(rest_size >= INTER_BUFF_SIZE)
            size = INTER_BUFF_SIZE;
        else
            size = INTER_BUFF_SIZE - rest_size;

        pi_ram_read(ram, (_l3_buff+l3_index), _tmp_buffer, (uint32_t) size);
        pi_fs_write(file, _tmp_buffer, size);

        l3_index += size;
        rest_size = rest_size - size;
    } while (rest_size);

    return 0;
}

void dump_wav(char *filename, int width, int sampling_rate, int nb_channels, void *data, int size, int fromL3, struct pi_device *ram)
{
    unsigned int idx = 0;
    unsigned int sz = WAV_HEADER_SIZE + size;

    // 4 bytes "RIFF"
    header_buffer[idx++] = 'R';
    header_buffer[idx++] = 'I';
    header_buffer[idx++] = 'F';
    header_buffer[idx++] = 'F';

    // 4 bytes File size - 8bytes 32kS 0x10024 - 65408S 0x1ff24
    //header_buffer[idx++] = 0x24;
    //header_buffer[idx++] = 0xff;
    //header_buffer[idx++] = 0x01;
    //header_buffer[idx++] = 0x00;
    header_buffer[idx++] = (unsigned char) (sz & 0x000000ff);
    header_buffer[idx++] = (unsigned char)((sz & 0x0000ff00) >> 8);
    header_buffer[idx++] = (unsigned char)((sz & 0x00ff0000) >> 16);
    header_buffer[idx++] = (unsigned char)((sz & 0xff000000) >> 24);

    // 4 bytes file type: "WAVE"
    header_buffer[idx++] = 'W';
    header_buffer[idx++] = 'A';
    header_buffer[idx++] = 'V';
    header_buffer[idx++] = 'E';

    // 4 bytes format chunk: "fmt " last char is trailing NULL
    header_buffer[idx++] = 'f';
    header_buffer[idx++] = 'm';
    header_buffer[idx++] = 't';
    header_buffer[idx++] = ' ';

    // 4 bytes length of format data below, until data part
    header_buffer[idx++] = 0x10;
    header_buffer[idx++] = 0x00;
    header_buffer[idx++] = 0x00;
    header_buffer[idx++] = 0x00;

    // 2 bytes type of format: 1 (PCM)
    header_buffer[idx++] = 0x01;
    header_buffer[idx++] = 0x00;

    // 2 bytes nb of channels: 1 or 2
    //header_buffer[idx++] = 0x02;
    //header_buffer[idx++] = 0x01;
    header_buffer[idx++] = nb_channels;
    header_buffer[idx++] = 0x00;

    // 4 bytes sample rate in Hz:
    header_buffer[idx++] = (sampling_rate >> 0) & 0xff;
    header_buffer[idx++] = (sampling_rate >> 8) & 0xff;
    header_buffer[idx++] = (sampling_rate >> 16) & 0xff;
    header_buffer[idx++] = (sampling_rate >> 24) & 0xff;

    // 4 bytes (Sample Rate * BitsPerSample * Channels) / 8:
    // (8000*16*1)/8=0x3e80 * 2
    // (16000*16*1)/8=32000 or 0x6F00
    // (22050*16*1)/8=0xac44
    // (22050*16*2)/8=0x15888
    int rate = (sampling_rate * width * nb_channels) / 8;
    header_buffer[idx++] = (rate >> 0) & 0xff;
    header_buffer[idx++] = (rate >> 8) & 0xff;
    header_buffer[idx++] = (rate >> 16) & 0xff;
    header_buffer[idx++] = (rate >> 24) & 0xff;

    // 2 bytes (BitsPerSample * Channels) / 8:
    // 16*1/8=2 - 16b mono
    // 16*2/8=4 - 16b stereo
    rate = (width * nb_channels) / 8;
    header_buffer[idx++] = (rate >> 0) & 0xff;
    header_buffer[idx++] = (rate >> 8) & 0xff;

    // 2 bytes bit per sample:
    header_buffer[idx++] = width;
    header_buffer[idx++] = 0x00;

    // 4 bytes "data" chunk
    header_buffer[idx++] = 'd';
    header_buffer[idx++] = 'a';
    header_buffer[idx++] = 't';
    header_buffer[idx++] = 'a';

    // 4 bytes size of data section in bytes:
    header_buffer[idx++] = (unsigned char) (size & 0x000000ff);
    header_buffer[idx++] = (unsigned char)((size & 0x0000ff00) >> 8);
    header_buffer[idx++] = (unsigned char)((size & 0x00ff0000) >> 16);
    header_buffer[idx++] = (unsigned char)((size & 0xff000000) >> 24);

    struct pi_hostfs_conf conf;
    pi_hostfs_conf_init(&conf);
    struct pi_device fs;

    pi_open_from_conf(&fs, &conf);

    if (pi_fs_mount(&fs))
     return;

    void *file = pi_fs_open(&fs, filename, PI_FS_FLAGS_WRITE);
    if (file == 0)
    {
        printf("Failed to open file, %s\n", filename);
        return;
    }

    pi_fs_write(file, header_buffer, WAV_HEADER_SIZE);
    if(fromL3)
        fs_write_from_L3(file, data, size, ram);
    else
        pi_fs_write(file, data, size);

    pi_fs_close(file);

    pi_fs_unmount(&fs);
}
