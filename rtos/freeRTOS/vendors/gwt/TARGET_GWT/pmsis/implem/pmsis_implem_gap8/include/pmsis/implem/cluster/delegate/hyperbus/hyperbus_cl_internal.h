/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __HYPERBUS_CL_INTERNAL_H__
#define __HYPERBUS_CL_INTERNAL_H__

#if defined(FEATURE_CLUSTER)

#include "pmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

struct pi_cl_hyper_req_s
{
    struct pi_device *device;
    struct hyper_transfer_s transfer;
    struct pi_cl_hyper_req_s *next;
    uint8_t done;
    pi_task_t cb;
    uint8_t cid;
    uint8_t is_2d;
};

struct pi_cl_hyperram_alloc_req_s
{
    struct pi_device *device;   /*!< Hyper device used. */
    uint32_t chunk;             /*!< Address of the allocated memory bloc. */
    uint32_t size;              /*!< Size of the allocated memory bloc. */
    uint8_t done;               /*!< Used to check req compltetion.  */
    pi_task_t cb;               /*!< Callback associated to req. */
    uint8_t cid;                /*!< Cluster ID. */
};

struct pi_cl_hyperram_free_req_s
{
    struct pi_device *device;   /*!< Hyper device used. */
    uint32_t chunk;             /*!< Address of the allocated memory bloc. */
    uint32_t size;              /*!< Size of the allocated memory bloc. */
    uint8_t done;               /*!< Used to check req compltetion.  */
    pi_task_t cb;               /*!< Callback associated to req. */
    uint8_t cid;                /*!< Cluster ID. */
};

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

/* Actual copy functions, setting transfer struct and calling FC. */
void __pi_cl_hyper_copy(struct pi_device *device, uint32_t hyper_addr, void *buffer,
                        uint32_t size, udma_channel_e read, pi_cl_hyper_req_t *req);
void __pi_cl_hyper_copy_2d(struct pi_device *device, uint32_t hyper_addr,
                           void *buffer, uint32_t size, uint32_t stride,
                           uint32_t length, udma_channel_e read, pi_cl_hyper_req_t *req);

/* Hyper copy waiting functions, cluster is asleep. */
void __pi_cl_hyper_copy_task_wait(void *arg);

/* Hyper memory alloc & free functions, cluster is asleep. */
void __pi_cl_hyper_alloc_task_wait(void *arg);

void __pi_cl_hyper_free_task_wait(void *arg);

/*******************************************************************************
 * Function implementation
 ******************************************************************************/

static inline void pi_cl_hyper_read(struct pi_device *device, uint32_t hyper_addr,
                                    void *addr, uint32_t size, pi_cl_hyper_req_t *req)
{
    __pi_cl_hyper_copy(device, hyper_addr, addr, size, RX_CHANNEL, req);
    hal_compiler_barrier();
}

static inline void pi_cl_hyper_read_2d(struct pi_device *device, uint32_t hyper_addr,
                                       void *addr, uint32_t size,
                                       uint32_t stride, uint32_t length, pi_cl_hyper_req_t *req)
{
    __pi_cl_hyper_copy_2d(device, hyper_addr, addr, size, stride, length, RX_CHANNEL, req);
    hal_compiler_barrier();
}

static inline void pi_cl_hyper_read_wait(pi_cl_hyper_req_t *req)
{
    __pi_cl_hyper_copy_task_wait(req);
    hal_compiler_barrier();
}

static inline void pi_cl_hyper_write(struct pi_device *device, uint32_t hyper_addr,
                                     void *addr, uint32_t size, pi_cl_hyper_req_t *req)
{
    __pi_cl_hyper_copy(device, hyper_addr, addr, size, TX_CHANNEL, req);
    hal_compiler_barrier();
}

static inline void pi_cl_hyper_write_2d(struct pi_device *device, uint32_t hyper_addr,
                                        void *addr, uint32_t size,
                                        uint32_t stride, uint32_t length, pi_cl_hyper_req_t *req)
{
    __pi_cl_hyper_copy_2d(device, hyper_addr, addr, size, stride, length, TX_CHANNEL, req);
    hal_compiler_barrier();
}

static inline void pi_cl_hyper_write_wait(pi_cl_hyper_req_t *req)
{
    __pi_cl_hyper_copy_task_wait(req);
    hal_compiler_barrier();
}

static inline void pi_cl_hyper_copy(struct pi_device *device, uint32_t hyper_addr,
                                    void *addr, uint32_t size,
                                    int ext2loc, pi_cl_hyper_req_t *req)
{
    __pi_cl_hyper_copy(device, hyper_addr, addr, size, !ext2loc, req);
    hal_compiler_barrier();
}

static inline void pi_cl_hyper_copy_2d(struct pi_device *device, uint32_t hyper_addr,
                                       void *addr, uint32_t size,
                                       uint32_t stride, uint32_t length,
                                       int ext2loc, pi_cl_hyper_req_t *req)
{
    __pi_cl_hyper_copy_2d(device, hyper_addr, addr, size, stride, length, !ext2loc, req);
}

static inline uint32_t pi_cl_hyperram_alloc_wait(pi_cl_hyperram_alloc_req_t *req)
{
    __pi_cl_hyper_alloc_task_wait(req);
    hal_compiler_barrier();
    return req->chunk;
}

static inline void pi_cl_hyperram_free_wait(pi_cl_hyperram_free_req_t *req)
{
    __pi_cl_hyper_free_task_wait(req);
    hal_compiler_barrier();
}

#endif  /* FEATURE_CLUSTER */

#endif  /* __HYPERBUS_CL_INTERNAL_H__ */
