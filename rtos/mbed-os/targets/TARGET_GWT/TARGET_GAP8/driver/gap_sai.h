/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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

#ifndef _GAP_SAI_H_
#define _GAP_SAI_H_

#include "gap_common.h"
#include "PinNames.h"
#include "gap_udma.h"

/*!
 * @addtogroup sai
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct i2s_s i2s_t;

/*! @name Driver version */
/*@{*/
#define GAP_SAI_DRIVER_VERSION (MAKE_VERSION(1, 0, 0)) /*!< Version 1.0.0 */
/*@}*/

/*! @brief Status for the SAI driver.*/
enum _sai_status_t
{
    uStatus_SAI_Error_RX = MAKE_STATUS(uStatusGroup_SAI, 0),
    uStatus_SAI_Idle_RX  = MAKE_STATUS(uStatusGroup_SAI, 1),
    uStatus_SAI_Busy_RX  = MAKE_STATUS(uStatusGroup_SAI, 2),
    uStatus_SAI_Done_RX  = MAKE_STATUS(uStatusGroup_SAI, 3)
};

/*! @brief Status for transfer, may be used for polling.*/
enum _sai_transfer_status
{
    uSAI_Error = 0,
    uSAI_Idle = 1,
    uSAI_Busy = 2,
    uSAI_Done = 3
};

/*! @brief SAI Standard.*/
typedef enum _sai_protocol
{
    uSAI_SAI = 0x1U,
    uSAI_LSB = 0x10U,
    uSAI_PDM_FILTER = 0x100U,
    uSAI_PDM_EN = 0x1000U,
    uSAI_USEDDR = 0x10000U,
    uSAI_MODE = 0x3000000U
} sai_protocol_t;

/*! @brief SAI clock config.*/
typedef enum _sai_clk_mode
{
    uSAI_CLK0_INT_WS = 0,
    uSAI_CLK1_INT_WS,
    uSAI_EXT_CLK_INT_WS,
    uSAI_EXT_CLK_EXT_WS
} sai_clk_mode_t;

/*! @brief SAI Chip Select (csn) configuration (which csn to configure).*/
typedef enum _sai_chan
{
    uSAI_Channel0 = 0U,
    uSAI_Channel1 = 1U
} sai_chan_t;

/*! @brief Audio sample rate.*/
typedef enum _sai_sample_rate
{
    uSAI_Freq_8 = 8000,
    uSAI_Freq_11 = 11025,
    uSAI_Freq_16 = 16000,
    uSAI_Freq_22 = 22050,
    uSAI_Freq_32 = 32000,
    uSAI_Freq_44 = 44100,
    uSAI_Freq_48 = 48000,
    uSAI_Freq_96 = 96000,
    uSAI_Freq_192 = 192000
} sai_sample_rate_t;

/*! @brief Audio word width.*/
typedef enum _sai_word_width
{
    uSAI_Word_8 = 8,
    uSAI_Word_16 = 16,
    uSAI_Word_24 = 24,
    uSAI_Word_32 = 32
} sai_word_width_t;

/*! @brief Sai channel config.*/
typedef struct _sai_chan_mode
{
    char ext_clk;
    uint8_t clk_en;
    sai_sample_rate_t sampleRate_Hz;
    sai_word_width_t dataWidth;
    sai_protocol_t protocol;
} sai_chan_mode_t;

/*! @brief Sai filter config.*/
typedef struct _sai_filter
{
    uint16_t decimation;
    uint16_t shift;
} sai_filter_t;

/*! @brief SAI config structure.*/
typedef struct _sai_config
{
    sai_chan_mode_t chan_mode;
    sai_filter_t filter;
} sai_config_t;

/*! @brief SAI transfer structure.*/
typedef struct _sai_transfer
{
    uint8_t *data;
    size_t dataSize;
    uint8_t configFlags;
    sai_chan_t channel;
} sai_transfer_t;

/*! @brief SAI transfer request type definition.*/
#define  sai_req_t udma_req_t

typedef struct _sai_handle_t sai_handle_t;

/*! @brief SAI master transfer callback typedef. */
typedef void (*sai_transfer_callback_t)(I2S_Type *base,
                                        sai_handle_t *handle,
                                        status_t status,
                                        void *userData);

struct _sai_handle_t
{
    uint8_t state;
    sai_transfer_callback_t callback;
    void *userData;
};


/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*__cplusplus*/

/*!
 * @brief  SAI audio device initialization
 *
 * @param base SAI base pointer.
 * @param sdi  I2S serial date pin num.
 * @param ws   I2S word select pin num.
 * @param sck  I2S serial clock pin num.
 */
void SAI_Init(I2S_Type *base, PinName sdi, PinName ws, PinName sck);

/*!
 * @brief  SAI audio device initialization
 *
 * @param base SAI base pointer.
 */
void SAI_Deinit(I2S_Type *base);

/*!
 * @brief  SAI audio device filter initilization
 *
 * @param base       SAI base pointer.
 * @param ch_id      Channel ID.
 * @param decimation Filter decimation.
 * @param shift      Filter shift.
 */
void SAI_FilterConfig(I2S_Type *base,
                      char ch_id,
                      uint16_t decimation,
                      uint16_t shift);

/*!
 * @brief  SAI audio device external bits word configuration
 *
 * @param base              SAI base pointer.
 * @param bits              When using EXT Clock and internal WS, selects after how many bits the WS is toggled.
 *                          The value written here is (num bits – 1).
 */
void SAI_ExternalBitsWordConfig(I2S_Type *base, uint8_t bits);

/*!
 * @brief  SAI audio device clock (CLK) configuration
 *
 * @param base              SAI base pointer.
 * @param ch_id             Channel ID.
 * @param div               Frequency divider.
 * @param bits              Number of bits to sample.
 * @param en                Enable clock
 */
void SAI_ClockConfig(I2S_Type *base,
                     char ch_id,
                     uint8_t bits,
                     uint8_t en,
                     uint16_t div);

/*!
 * @brief  SAI audio device mode initilization
 *
 * @param base              SAI base pointer.
 * @param ch_id             Channel ID.
 * @param lsb_first         LSB or MSB
 * @param pdm_filt_en       Enable PDM Filter
 * @param pdm_en            Enable PDM
 * @param use_ddr           Double Rate
 * @param clk_mode          Channel clock modes
 */
void SAI_ModeConfig(I2S_Type *base,
                    char ch_id,
                    uint8_t lsb_first,
                    uint8_t pdm_filt_en,
                    uint8_t pdm_en,
                    uint8_t use_ddr,
                    uint8_t clk_mode);


/*!
 * @brief Performs an blocking receive transfer on SAI.
 *
 * @note This API returns immediately after the transfer initiates.
 * Use event to check whether the transfer is finished.
 * If the return status is not ustatus_SAI_Busy, the transfer
 * is finished.
 *
 * @param base SAI base pointer
 * @param xfer Pointer to the sai_transfer_t structure.
 * @retval ustatus_Success Successfully started the data receive.
 */
status_t SAI_TransferReceiveBlocking(I2S_Type *base, sai_transfer_t *xfer);

/*!
 * @brief Initializes the SAI Rx handle.
 *
 * This function initializes the Rx handle for the SAI Rx transactional APIs. Call
 * this function once to get the handle initialized.
 *
 * @param base SAI base pointer.
 * @param handle SAI handle pointer.
 * @param callback Pointer to the user callback function.
 * @param userData User parameter passed to the callback function.
 */
void SAI_TransferRxCreateHandle(I2S_Type *base,
                                sai_handle_t *handle,
                                sai_transfer_callback_t callback,
                                void *userData);

/*!
 * @brief Performs an interrupt non-blocking receive transfer on SAI.
 *
 * @note This API returns immediately after the transfer initiates.
 * Call the SAI_RxGetTransferStatusIRQ to poll the transfer status and check whether
 * the transfer is finished. If the return status is not ustatus_SAI_Busy, the transfer
 * is finished.
 *
 * @param base SAI base pointer
 * @param handle Pointer to the sai_handle_t structure which stores the transfer state.
 * @param xfer Pointer to the sai_transfer_t structure.
 * @retval ustatus_Success Successfully started the data receive.
 * @retval ustatus_SAI_RxBusy Previous receive still not finished.
 */
status_t SAI_TransferReceiveNonBlocking(I2S_Type *base, sai_handle_t *handle, sai_transfer_t *xfer);

/*!
 * @brief Rx interrupt handler.
 *
 * @param base SAI base pointer.
 * @param handle Pointer to the sai_handle_t structure.
 */
void SAI_TransferRxHandleIRQ(I2S_Type *base, sai_handle_t *handle);

/*!
 * @brief Rx interrupt handler for channel0.
 * @param handle Pointer to user handler
 *
 */
void SAI_IRQHandler_CH0(void *handle);

/*!
 * @brief Rx interrupt handler for channel1.
 * @param handle Pointer to user handler
 *
 */
void SAI_IRQHandler_CH1(void *handle);



/*! @} */

#if defined(__cplusplus)
}
#endif /*_cplusplus*/

/*! @} */

#endif /* _GAP_SAI_H_ */
