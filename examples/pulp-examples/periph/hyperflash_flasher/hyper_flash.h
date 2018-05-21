/*
 * Copyright (C) 2015 ETH Zurich and University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou (germain.haugou@gmail.com)
 */

#ifndef __RT__RT_API_HYPER_H__
#define __RT__RT_API_HYPER_H__

#define CHANNEL_RX  0
#define CHANNEL_TX  1

#define PLP_PAD_HYPER_CKN        36
#define PLP_PAD_HYPER_CK         37
#define PLP_PAD_HYPER_DQ0        40
#define PLP_PAD_HYPER_DQ1        41
#define PLP_PAD_HYPER_DQ2        42
#define PLP_PAD_HYPER_DQ3        43
#define PLP_PAD_HYPER_DQ4        44
#define PLP_PAD_HYPER_DQ5        45
#define PLP_PAD_HYPER_DQ6        46
#define PLP_PAD_HYPER_DQ7        47
#define PLP_PAD_HYPER_CSN0       48
#define PLP_PAD_HYPER_CSN1       49
#define PLP_PAD_HYPER_RWDS       50

#define PLP_PAD_GPIO_DEFAULT     0
#define PLP_PAD_GPIO_ALTERNATE1  1
#define PLP_PAD_GPIO_ALTERNATE2  2
#define PLP_PAD_GPIO_ALTERNATE3  3

#define FLASHER_BUFFER_MAX_SIZE                   0x8000

// This structure can be used to interact with the flasher, used to copy binary from host to the embedded flash
typedef struct {
    char buffer[FLASHER_BUFFER_MAX_SIZE];
    int buffer_pulp_status;
    int buffer_ext_status;
    int buffer_size;
    unsigned int buffer_dest;
    int buffer_start;
} flasherStruct_t;

void hyper_pad_init();
int hyper_init();
void hyper_flash_write(rt_flash_t *flash, unsigned int l2Addr, unsigned int hyperFlashAddr, rt_event_t *event);
void hyper_flash_burst_write(rt_flash_t *flash, unsigned int l2Addr, unsigned int hyperFlashAddr, unsigned int size, rt_event_t *event);
void hyper_flash_read(rt_flash_t *flash, unsigned int l2Addr, unsigned int hyperFlashAddr, unsigned int size, rt_event_t *event);
void hyper_close();
int rt_hyper_init();
void rt_hyper_deinit();

#endif
