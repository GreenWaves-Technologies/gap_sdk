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

#ifndef __CHIPS_GAP8_DRIVERS_PERF_H__
#define __CHIPS_GAP8_DRIVERS_PERF_H__

/**        
 * @ingroup groupChips
 */

/**        
 * @defgroup GAP8 GAP8
 *
 * This section gives information about GAP8 specific features.
 */

/**
 * @addtogroup GAP8
 * @{
 */

/**        
 * @defgroup GAP8_INFO GAP8 information
 *
 * The closest memory for the FC is the FC TCDM and is referred in the API as
 * the FC L1 memory.
 *  
 * All functions transfering data between an external device and a chip memory
 * must use the L2 memory for the chip memory.
 *
 * The cluster controller is core 0 of the cluster. Core 1 to 7 are the slave
 * cores.
 *
 * Up to 8 cluster DMA counters can be allocated at the same time. Trying to
 * allocate one more will stall the core.
 *
 * With the I2S driver, When using 2 RX channels on the same interface (left and
 * right), the buffer contains the samples for both channels interleaved one by
 * one.
 * 
 * I2S buffers size must be a most 64KB - 4.
 * 
 * PI_SPI_WORDSIZE_16 is not supported.
 */

/**
 * @}
 */

/**
 * @addtogroup GAP8
 * @{
 */

/**        
 * @defgroup GAP8_PERF GAP8 performance counters
 *
 */

/**
 * @}
 */

/**        
 * @addtogroup GAP8_PERF
 * @{        
 */

/**@{*/

/** \enum pi_perf_event_e
 * \brief Performance event identifiers.
 *
 * This can be used to describe which performance event to monitor (cycles, 
 * cache miss, etc).
 */
typedef enum {
  PI_PERF_CYCLES        = 16, /*!< Total number of cycles (also includes the
    cycles where the core is sleeping). Be careful that this event is using a
    timer shared within the cluster, so resetting, starting or stopping it on
    one core will impact other cores of the same cluster. */
  PI_PERF_ACTIVE_CYCLES = 0,  /*!< Counts the number of cycles the core was
    active (not sleeping). */
  PI_PERF_INSTR         = 1,  /*!< Counts the number of instructions executed.
  */
  PI_PERF_LD_STALL      = 2,  /*!< Number of load data hazards. */  
  PI_PERF_JR_STALL      = 3,  /*!< Number of jump register data hazards. */
  PI_PERF_IMISS         = 4,  /*!< Cycles waiting for instruction fetches, i.e.
    number of instructions wasted due to non-ideal caching. */
  PI_PERF_LD            = 5,  /*!< Number of data memory loads executed.
    Misaligned accesses are counted twice. */
  PI_PERF_ST            = 6,  /*!< Number of data memory stores executed.
    Misaligned accesses are counted twice. */
  PI_PERF_JUMP          = 7,  /*!< Number of unconditional jumps (j, jal, jr,
    jalr). */
  PI_PERF_BRANCH        = 8,  /*!< Number of branches. Counts both taken and
    not taken branches. */
  PI_PERF_BTAKEN        = 9, /*!< Number of taken branches. */
  PI_PERF_RVC           = 10, /*!< Number of compressed instructions
    executed. */
  PI_PERF_LD_EXT        = 11, /*!< Number of memory loads to EXT executed.
    Misaligned accesses are counted twice. Every non-TCDM access is considered
    external (cluster only). */
  PI_PERF_ST_EXT        = 12, /*!< Number of memory stores to EXT executed.
    Misaligned accesses are counted twice. Every non-TCDM access is considered
    external (cluster only). */
  PI_PERF_LD_EXT_CYC    = 13, /*!< Cycles used for memory loads to EXT.
  Every non-TCDM access is considered external (cluster only). */
  PI_PERF_ST_EXT_CYC    = 14, /*!< Cycles used for memory stores to EXT.
  Every non-TCDM access is considered external (cluster only). */
  PI_PERF_TCDM_CONT     = 15, /*!< Cycles wasted due to TCDM/log-interconnect
  contention (cluster only). */
} pi_perf_event_e;

//!@}

/**
 * @}
 */

#endif