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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG_ASSERT
#include "assert.h"
#endif


//*************************************************************************************************
// Decompressor CONF_REG register definition
//*************************************************************************************************

/**
 * \brief Decompression/compression mode
 * 
 * T1
 * The decompression scheme T1 consists in data expansion (signed/unsigned) to 8
 * 16 or 32 bits, within linear or 2D transfers from L2 to L1. Compressed data can
 * be any size ranging from 1 to 31 bits.
 * 
 * T2
 * The decompression scheme T2 consists in data transformation and expansion
 * (signed/unsigned) to 8, 16 or 32 bits. Only linear transfers from L2 to L1 are
 * supported. The transformation is performed via a look-up table (LUT), where the
 * compressed data is interpreted as the LUT address, and the content of the LUT
 * is the corresponding decompressed data. Compressed data (LUT addresses) size is
 * ranging from 1 to 8 bits (i.e. maximum of 256 LUT entries).
 * 
 * T3
 * The decompression scheme T3 consists in data transformation and expansion
 * (signed/unsigned) to 8, 16 or 32 bits. Only linear transfers from L2 to L1 are
 * supported. The T3 scheme introduces a special symbol in the compressed data,
 * which is encoded with a single bit (0). The T3 engine starts decompressing data
 * and discriminates between occurancies of the special symbol (encoded with 0) and
 * other data, which are compressed data using the LUT like in T2. Non special
 * symbols are therefore encoded with a bit set to 1 (to discriminate from the special
 * symbol) followed by the 1- to 8-bit LUT address.
 */
typedef enum
{
    PI_CL_DMA_DECOMPRESSOR_MODE_T1 = ((uint32_t)0), // T1 mode
    PI_CL_DMA_DECOMPRESSOR_MODE_T2 = ((uint32_t)1), // T2 mode
    PI_CL_DMA_DECOMPRESSOR_MODE_T3 = ((uint32_t)2), // T3 mode
} pi_cl_dma_decompressor_mode_e;

/**
 * \brief type of the decompressed data
 */
typedef enum
{
    PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_8  = ((uint32_t)0),
    PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_16 = ((uint32_t)1),
    PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_32 = ((uint32_t)3),
} pi_cl_dma_decompressor_ext_type_e;

/**
 * \brief signedness of decompressed data
 */
typedef enum
{
    PI_CL_DMA_DECOMPRESSOR_EXT_SIGN_UNSIGNED = ((uint32_t)0),
    PI_CL_DMA_DECOMPRESSOR_EXT_SIGN_SIGNED   = ((uint32_t)1),
} pi_cl_dma_decompressor_ext_sign_e;

/**
 * \brief decompressor direction
 */
typedef enum
{
    PI_CL_DMA_DECOMPRESSOR_DIR_EXT2LOC = ((uint32_t)0), /*!< Decompress data from L2 to TCDM */
    PI_CL_DMA_DECOMPRESSOR_DIR_LOC2EXT = ((uint32_t)1), /*!< Compress data from TCDM to L2 (only T1 mode) */
} pi_cl_dma_decompressor_dir_e;

// Used in assertion mechanism. Check if the decompression mode is a valid one 
#define DECOMPRESSOR_IS_VALID_CONF_MODE(decompr_mode)  (((decompr_mode) == PI_CL_DMA_DECOMPRESSOR_MODE_T1) || \
                                                        ((decompr_mode) == PI_CL_DMA_DECOMPRESSOR_MODE_T2) || \
                                                        ((decompr_mode) == PI_CL_DMA_DECOMPRESSOR_MODE_T3))

// Used in assertion mechanism. Check if the extension type is a valid one 
#define DECOMPRESSOR_IS_VALID_CONF_EXT_TYPE(extension_type) (((extension_type) == PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_8)  || \
                                                             ((extension_type) == PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_16) || \
                                                             ((extension_type) == PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_32))

// Used in assertion mechanism. Check if the item bit width is a valid one 
#define DECOMPRESSOR_IS_VALID_CONF_ITEM_BIT_WIDTH(item_bit_width,decompr_mode)  ((((decompr_mode)  == PI_CL_DMA_DECOMPRESSOR_MODE_T1) && ((item_bit_width) >= 1) && ((item_bit_width) <= 31)) || \
                                                                                 ((((decompr_mode) == PI_CL_DMA_DECOMPRESSOR_MODE_T2) || ((decompr_mode) == PI_CL_DMA_DECOMPRESSOR_MODE_T3)) && ((item_bit_width) >= 1) && ((item_bit_width) <= 15)))   

// Used in assertion mechanism. Check if the sign extension is a valid one 
#define DECOMPRESSOR_IS_VALID_CONF_SIGN_EXT(sign_extension) (((sign_extension) == PI_CL_DMA_DECOMPRESSOR_EXT_SIGN_UNSIGNED) || \
                                                             ((sign_extension) == PI_CL_DMA_DECOMPRESSOR_EXT_SIGN_SIGNED))

// Used in assertion mechanism. Check if the decompression direction is a valid one 
#define DECOMPRESSOR_IS_VALID_CONF_DIRECTION(decompr_direction) (((decompr_direction) == PI_CL_DMA_DECOMPRESSOR_DIR_EXT2LOC) || \
                                                                 ((decompr_direction) == PI_CL_DMA_DECOMPRESSOR_DIR_LOC2EXT))


// Used in assertion mechanism. Verify that the configuration (related to CONF_REG register) is valid
#define DECOMPRESSOR_IS_VALID_CONF(conf)    (DECOMPRESSOR_IS_VALID_CONF_MODE(conf.decompr_mode) && \
                                             DECOMPRESSOR_IS_VALID_CONF_EXT_TYPE(conf.extension_type) && \
                                             DECOMPRESSOR_IS_VALID_CONF_ITEM_BIT_WIDTH(conf.item_bit_width, conf.decompr_mode) && \
                                             DECOMPRESSOR_IS_VALID_CONF_SIGN_EXT(conf.sign_extension) && \
                                             DECOMPRESSOR_IS_VALID_CONF_DIRECTION(conf.decompr_direction))

// Used in assertion mechanism. Check if the command is well parametered for using the LUT. 
#define DECOMPRESSOR_IS_VALID_LUT_CONFIG(flag, lut, size, mode) ( (!(flag)) || \
                                                                  ((flag) && ((lut) != NULL) && (size > 0) && (mode != PI_CL_DMA_DECOMPRESSOR_MODE_T1)))
 
/**
 * @brief Transfer mode (linear or 2D) for source and destination
 */
typedef enum
{
    PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_LIN_LOC_LIN = ((uint8_t)0), // linear L2/linear TCDM
    PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_2D_LOC_LIN  = ((uint8_t)1), // 2D L2/linear TCDM
    PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_LIN_LOC_2D  = ((uint8_t)2), // linear L2/2D TCDM
} pi_cl_dma_decompressor_transfer_type_e;

// Used in assertion mechanism. Verify that the transfer type is a valid one
#define DECOMPRESSOR_IS_VALID_TRANSFER_TYPE(type) ( ((type) == PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_LIN_LOC_LIN) || \
                                                    ((type) == PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_LIN_LOC_LIN) || \
                                                    ((type) == PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_LIN_LOC_LIN))

/**
 * @brief 2D transfer options structure. 
 */
typedef struct pi_cl_dma_decompressor_2d_transfer
{
    uint16_t count;  /*!< number of items in a row */
    uint16_t stride; /*!< number of items from the start of a row to the next one*/
} pi_cl_dma_decompressor_2d_transfer_t;

/**
 * @brief Decompressor's CONF_REG register description.
 */
typedef union 
{
    struct 
    {
        uint32_t decompr_mode       : 2;  //!< Decompression Mode: b00: T1; b01: T2; b11:T3; b10: reserved.
        uint32_t extension_type     : 2;  //!< Extension Type: b00: 8-bit; b01: 16-bit; b11: 32bit; b10: reserved.
        uint32_t item_bit_width     : 5;  //!< Size of compressed data item in L2. Allowed sizes are 1 to 31 for T1, 1 to 15 for T2 and T3. Warning: An illegal configuration will be handled like a 1-bit width configuration
        uint32_t sign_extension     : 1;  //!< Sign extension during decompression: 0: Unsigned; 1: Signed.
        uint32_t start_bit          : 3;  //!< Bit offset (in the byte) for L2 start address during decompression or compression.
        uint32_t decompr_direction  : 1;  //!< Transfer direction: 0: TX (decompression); 1: RX (compression T1).
        uint32_t start_byte         : 2;  //!< Byte offset (in a 32-bit word) in L2 start address during decompression or compression.
        uint32_t items_to_transfer  : 16; //!< Total items to compress/decompress during a transfer.
    };
    uint32_t raw;                         //!< Register content raw version 
}__attribute__((packed))pi_cl_dma_decompressor_conf_reg_t;

/**
 * @brief   Bitfield structure that gather all decompressor's flags.
 *          These flags are used to write additionnal decompressor 
 *          registers from the @ref pi_cl_dma_decompressor_cmd_t. 
 */
typedef union
{
    struct
    {
        uint8_t update_lut      : 1; //!< Trigger a LUT write process during the Exec command
        uint8_t update_2d_ext   : 1; //!< Allows to write L2 2D transfer registers during the Exec command
        uint8_t update_2d_loc   : 1; //!< Allows to write TCDM 2D transfer registers during the Exec command
        uint8_t unused          : 5; //!< Not used
    };
    uint8_t all; //!< Used to check if at least one flag is set. 
}pi_cl_decompressor_flag_t;


// The L2 address set in L2_ADDR_REG can only be a multiple of "4". This define filter the input value
// given by the user and set the "start_byte" bits in CONF_REG register to get the right address value.
#define DECOMPRESSOR_L2_ADDRESS_FILTER 0x03

#define DECOMPRESSOR_L2_START_BYTE_MASK 0x0000C000
/**
 * \brief structure containing command information for decompression/compression
 */
typedef struct pi_cl_dma_decompressor_cmd_s
{
    uint32_t                                loc;                //!< Cluster TCDM address
    uint32_t                                ext;                //!< L2 address
    pi_cl_dma_decompressor_conf_reg_t       conf;               //!< CONF_REG register content
    pi_cl_dma_decompressor_transfer_type_e  transf_type;        //!< L2->TCDM (decompression), TCDM->L2 (compression)
    pi_cl_dma_decompressor_2d_transfer_t    l2_2d;              //!< Parameters for the L2 2D transfer
    pi_cl_dma_decompressor_2d_transfer_t    tcdm_2d;            //!< Parameters for the TCDM 2 transfer
    uint32_t                                special_symbol;     //!< special symbol (only used in T3 mode)   
    void                                    *lut;               //!< array containing LUT elements. Can be a uin8_t or uint16_t depending item_bit_width
    uint8_t                                 lut_size;           //!< number of elements in the LUT (max 256)
    uint8_t                                 done;               //!< done flag, set by the internal driver to 1 when transfer is done 
    struct pi_cl_dma_decompressor_cmd_s*    next_cmd;           //!< Pointer to the next command. Reserved for runtime usage
    pi_cl_decompressor_flag_t               flag;               //!< Bitfield structure to gather all decompressor's flags.
}pi_cl_dma_decompressor_cmd_t;

/**
 * @brief Initialize the decompressor. Enable its interrupt. 
 */
void pi_cl_dma_decompressor_init(void);


/**
 * @brief Write decompressor register from a @ref pi_cl_dma_decompressor_cmd_t object type and initiate a transaction. 
 * 
 * @param cmd               Pointer to the command to write/execute
 *                          This structure should stay valid during the entirety of the transfer. 
 *                          Following parameter comes from this cmd object. They are exposed for optimization purpose.
 * @param ext               L2 address
 * @param loc               L1 address
 * @param conf              CONF_REG register structure. It appears here for optimization purpose
 * @param transfer_type     MODE_REG register content. It appears here for optimization purpose 
 * @param lut_size          Look up table size. It appers here for optimization purpose
 */
void pi_cl_dma_decompressor_cmd(pi_cl_dma_decompressor_cmd_t* cmd,
                                uint32_t ext,
                                uint32_t loc,
                                pi_cl_dma_decompressor_conf_reg_t conf,
                                pi_cl_dma_decompressor_transfer_type_e transfer_type,
                                uint8_t lut_size);
/**
 * \brief blocks until the specified transfer ends
 *
 * \param cmd    A pointer to the structure that represents the transfer
 */
static inline void pi_cl_dma_decompressor_wait(pi_cl_dma_decompressor_cmd_t* cmd)
{
    while((*(volatile uint8_t *)&cmd->done) == 0)
    {
        hal_cl_eu_event_mask_wait_and_clear(1 << CL_DECOMP_DONE_NOTIFY_EVENT);
    }
}


#ifdef __cplusplus
}
#endif
#endif
