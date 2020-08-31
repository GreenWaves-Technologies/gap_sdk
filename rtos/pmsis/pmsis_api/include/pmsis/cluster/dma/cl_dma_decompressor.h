/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __CL_DMA_DECOMPRESSOR_H__
#define __CL_DMA_DECOMPRESSOR_H__

/**
 * \brief Decompression/compression mode
 * TODO explain compression/decompression modes
 */
typedef enum
{
    PI_CL_DMA_DECOMPRESSOR_MODE_T1,
    PI_CL_DMA_DECOMPRESSOR_MODE_T2,
    PI_CL_DMA_DECOMPRESSOR_MODE_T3,
} pi_cl_dma_decompressor_mode_e;

/**
 * \brief type of the decompressed data
 */
typedef enum
{
    PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_8,
    PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_16,
    PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_32,
} pi_cl_dma_decompressor_ext_type_e;

/**
 * \brief signedness of decompressed data
 */
typedef enum
{
    PI_CL_DMA_DECOMPRESSOR_EXT_SIGN_UNSIGNED,
    PI_CL_DMA_DECOMPRESSOR_EXT_SIGN_SIGNED,
} pi_cl_dma_decompressor_ext_sign_e;

/**
 * \brief decompressor direction
 */
typedef enum
{
    PI_CL_DMA_DECOMPRESSOR_DIR_EXT2LOC, /*!< Decompress data from L2 to TCDM */
    PI_CL_DMA_DECOMPRESSOR_DIR_LOC2EXT, /*!< Compress data from TCDM to L2 (only T1 mode) */
} pi_cl_dma_decompressor_dir_e;

typedef enum
{
    PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_LIN_LOC_LIN,
    PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_2D_LOC_LIN,
    PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_LIN_LOC_2D,
    PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_2D_LOC_2D,
} pi_cl_dma_decompressor_transfer_type_e;

typedef struct pi_cl_dma_decompressor_2d_transfer
{
    uint16_t count;  /*!< number of items in a row */
    uint16_t stride; /*!< number of items from the start of a row to the next one*/
} pi_cl_dma_decompressor_2d_transfer_t;

/**
 * \brief structure containing command information for decompression/compression
 */
typedef struct pi_cl_dma_decompressor_cmd_s
{
    uint32_t ext;                                       /*!< L2 address */
    uint32_t loc;                                       /*!< cluster TCDM address */
    uint32_t size;                                      /*!< size of the transfer in number of items */
    uint32_t special_symbol;                            /*!< special symbol (only used in T3 mode) */
    uint16_t *lut;                                      /*!< array containing LUT elements */
    uint8_t lut_size;                                   /*!< number of elements in the LUT (max 256) */
    uint8_t item_bit_width;                             /*!< length in bits of the item in l2 (ranging from 1 to 32) */
    uint8_t bit_offset;                                 /*!< bit offset (from 0 to 7) */
    uint8_t start_byte;                                 /*!< start byte, also called byte offset (from 0 to 3) */
    pi_cl_dma_decompressor_mode_e mode;                 /*!< T1, T2, T3 */
    pi_cl_dma_decompressor_dir_e direction;             /*!< L2->TCDM (decompression), TCDM->L2 (compression) */
    pi_cl_dma_decompressor_transfer_type_e transf_type; /*!< L2->TCDM (decompression), TCDM->L2 (compression) */
    pi_cl_dma_decompressor_ext_type_e extension_type;   /*!< extension type in TCDM - 8, 16 or 32 bits */
    pi_cl_dma_decompressor_ext_sign_e extension_sign;   /*!< signed/unsigned */
    pi_cl_dma_decompressor_2d_transfer_t l2_2d;         /*!< parameters for the L2 2D transfer */
    pi_cl_dma_decompressor_2d_transfer_t tcdm_2d;       /*!< parameters for the TCDM 2 transfer */
    uint8_t done;                                       /*!< done flag, set by the internal driver to 1 when transfer is done */
} pi_cl_dma_decompressor_cmd_t;

/**
 * \brief enqueue a decompression/compression
 *
 * \param cmd    A pointer to the command structure.
 *               This structure should stay valid during the entirety of the transfer
 */
void pi_cl_dma_decompressor_cmd(pi_cl_dma_decompressor_cmd_t* cmd);

/**
 * \brief blocks until the specified transfer ends
 *
 * \param cmd    A pointer to the structure for the copy.
 */
void pi_cl_dma_decompressor_wait(pi_cl_dma_decompressor_cmd_t* cmd);

#endif
