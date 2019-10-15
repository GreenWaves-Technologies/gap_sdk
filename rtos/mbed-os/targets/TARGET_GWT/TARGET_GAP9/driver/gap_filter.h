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

#ifndef _GAP_FILTER_H_
#define _GAP_FILTER_H_

#include "gap_udma.h"

/*!
 * @addtogroup filter_driver
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief  FILTER status return codes. */
enum _filter_status
{
    uStatus_FILTER_Busy = MAKE_STATUS(uStatusGroup_FILTER, 0),            /*!< FILTER is busy with current transfer. */
    uStatus_FILTER_Idle = MAKE_STATUS(uStatusGroup_FILTER, 1),            /*!< Bus is Idle. */
    uStatus_FILTER_Error = MAKE_STATUS(uStatusGroup_FILTER, 2)            /*!< Error received during transfer. */
};

/*! @brief FILTER channels data format. */
typedef enum _filter_channel_date_format
{
    uFILTER_8_Bits   = 0,    /*!< FILTER channel data format 8-bit. */
    uFILTER_16_Bits  = 1,    /*!< FILTER channel data format 16-bit. */
    uFILTER_32_Bits  = 2     /*!< FILTER channel data format 32-bit. */
} filter_channel_date_format_t;

/*! @brief FILTER TX channel data input mode. */
typedef enum _filter_channel_in_mode
{
    uFILTER_In_Linear    = 0,      /*!< FILTER channel data mode in linear. */
    uFILTER_In_Sliding   = 1,     /*!< FILTER channel data mode in slide window. */
    uFILTER_In_Circular  = 2,     /*!< FILTER channel data mode in circular. */
    uFILTER_In_2D        = 3       /*!< FILTER channel data mode in 2D. */
} filter_channel_in_mode_t;

/*! @brief FILTER RX channel data output mode. */
typedef enum _filter_channel_out_mode
{
    uFILTER_Out_Linear  = 0,     /*!< FILTER channel data mode out linear. */
    uFILTER_Out_2D_ROW  = 1,     /*!< FILTER channel data mode out 2D row. */
    uFILTER_Out_2D_COL  = 2      /*!< FILTER channel data mode out 2D colume. */
} filter_channel_out_mode_t;

/*! @brief FILTER arithmetic unit operation mode. */
typedef enum _filter_au_mode
{
    uFILTER_AU_AxB     = 0, /*!< FILTER arithmetic unit operation mode AxB. */
    uFILTER_AU_AxBpREG0,    /*!< FILTER arithmetic unit operation mode AxB+REG0. */
    uFILTER_AU_accAxB,      /*!< FILTER arithmetic unit operation mode accumulate AxB. */
    uFILTER_AU_AxA,         /*!< FILTER arithmetic unit operation mode AxA. */
    uFILTER_AU_AxApB,       /*!< FILTER arithmetic unit operation mode AxA+B. */
    uFILTER_AU_AxAmB,       /*!< FILTER arithmetic unit operation mode AxA-B. */
    uFILTER_AU_accAxA,      /*!< FILTER arithmetic unit operation mode accumulate AxA. */
    uFILTER_AU_AxApREG,     /*!< FILTER arithmetic unit operation mode AxA+REG0. */
    uFILTER_AU_AxREG1,      /*!< FILTER arithmetic unit operation mode AxREG1. */
    uFILTER_AU_AxREG1pB,    /*!< FILTER arithmetic unit operation mode AxREG1+B. */
    uFILTER_AU_AxREG1mB,    /*!< FILTER arithmetic unit operation mode AxREG1-B. */
    uFILTER_AU_AxREG1pREG0, /*!< FILTER arithmetic unit operation mode AxREG1+REG0. */
    uFILTER_AU_accAxREG1,   /*!< FILTER arithmetic unit operation mode accumulate AxREG1. */
    uFILTER_AU_ApB,         /*!< FILTER arithmetic unit operation mode A+B. */
    uFILTER_AU_AmB,         /*!< FILTER arithmetic unit operation mode A-B. */
    uFILTER_AU_ApREG0       /*!< FILTER arithmetic unit operation mode A+REG0. */
} filter_au_mode_t;

/*! @brief FILTER OperandA OperandB Output TBUnit. */
typedef enum _filter_control
{
    uFILTER_L2_L2_ON_OFF   =  0,  //  L2       L2      ON     OFF
    uFILTER_ST_L2_ON_OFF   =  1,  // STREAM    L2      ON     OFF
    uFILTER_L2_OFF_ON_OFF  =  2,  //  L2      OFF      ON     OFF
    uFILTER_ST_OFF_ON_OFF  =  3,  // STREAM   OFF      ON     OFF
    uFILTER_L2_L2_OFF_ON   =  4,  //  L2       L2      OFF    ON
    uFILTER_ST_L2_OFF_ON   =  5,  // STREAM    L2      OFF    ON
    uFILTER_L2_OFF_OFF_ON  =  6,  //  L2      OFF      OFF    ON
    uFILTER_ST_OFF_OFF_ON  =  7,  // STREAM   OFF      OFF    ON
    uFILTER_L2_L2_ON_ON    =  8,  //  L2       L2      ON     ON
    uFILTER_ST_L2_ON_ON    =  9,  // STREAM    L2      ON     ON
    uFILTER_L2_OFF_ON_ON   = 10,  //  L2      OFF      ON     ON
    uFILTER_ST_OFF_ON_ON   = 11,  // STREAM   OFF      ON     ON
    uFILTER_OFF_OFF_OFF_ON = 12,  // OFF      OFF      OFF    ON
    uFILTER_OFF_OFF_ON_ON  = 13   // OFF      OFF      ON     ON
} filter_control_t;

/*! @brief FILTER threshold operation configuration structure.*/
typedef struct _filter_threshold_config
{
    filter_channel_date_format_t in_format;  /*!< FILTER threshold input date format. */
    filter_channel_in_mode_t mode;        /*!< FILTER threshold input date mode. */
    filter_au_mode_t operation;  /*!< FILTER threshold operation mode. */
    filter_control_t control;    /*!< FILTER threshold control. */
    void *input;        /*!< FILTER threshold input date pointer. */
    int size;           /*!< FILTER threshold input date size. */
    int window_size;    /*!< FILTER threshold window size. */
    uint32_t threshold; /*!< FILTER threshold value. */
    filter_channel_date_format_t out_format;  /*!< FILTER threshold output date format. */
} filter_threshold_config_t;

/*! @brief FILTER Binarization operation configuration structure.*/
typedef struct _filter_binarization_config
{
    filter_channel_date_format_t in_format;  /*!< FILTER binarization input date format. */
    filter_channel_in_mode_t in_mode;        /*!< FILTER binarization input date mode. */
    filter_control_t control;    /*!< FILTER binarization control. */
    void *input;        /*!< FILTER binarization input date pointer. */
    int size;           /*!< FILTER binarization input date size. */
    filter_channel_out_mode_t out_mode;        /*!< FILTER binarization output date mode. */
    void *output;       /*!< FILTER binarization output date pointer. */
    uint32_t threshold; /*!< FILTER binarization value. */
    filter_channel_date_format_t out_format;  /*!< FILTER binarization output date format. */
} filter_binarization_config_t;

/*! @brief FILTER memory copy operation configuration structure.*/
typedef struct _filter_memcpy_config
{
    filter_channel_date_format_t in_format;  /*!< FILTER memery copy input date format. */
    filter_channel_in_mode_t in_mode;        /*!< FILTER memery copy input date mode. */
    filter_control_t control;    /*!< FILTER memery copy control. */
    void *input;        /*!< FILTER memery copy input date pointer. */
    int size;           /*!< FILTER memery copy input date size. */
    filter_channel_out_mode_t out_mode;        /*!< FILTER memery copy output date mode. */
    void *output;       /*!< FILTER memery copy output date pointer. */
} filter_memcpy_config_t;

/*! @brief FILTER transpose operation configuration structure.*/
typedef struct _filter_transpose_config
{
    filter_channel_date_format_t in_format;  /*!< FILTER transpose input date format. */
    filter_channel_in_mode_t in_mode;        /*!< FILTER transpose input date mode. */
    filter_control_t control;    /*!< FILTER transpose control. */
    int col_num;           /*!< FILTER transpose date colume number. */
    int row_num;           /*!< FILTER transpose data row number. */
    int in_stride;         /*!< FILTER transpose input stride. */
    int out_stride;        /*!< FILTER transpose output stride. */
    void *input;        /*!< FILTER transpose input date pointer. */
    filter_channel_out_mode_t out_mode;        /*!< FILTER transpose output date mode. */
    void *output;       /*!< FILTER transpose output date pointer. */
} filter_transpose_config_t;

/*! @brief FILTER 1D convolution operation configuration structure.*/
typedef struct _filter_convolution_config
{
    filter_channel_date_format_t in_format;  /*!< FILTER convolution input date format. */
    filter_channel_in_mode_t in_mode0;       /*!< FILTER convolution TX Channel 0 input date mode. */
    filter_channel_in_mode_t in_mode1;       /*!< FILTER convolution TX Channel 1 input date mode. */
    filter_au_mode_t operation;  /*!< FILTER convolution operation mode. */
    filter_control_t control;    /*!< FILTER convolution control. */
    void *input;        /*!< FILTER convolution input date pointer. */
    int size;           /*!< FILTER convolution input date size. */
    void *kernel;       /*!< FILTER convolution input kernel pointer. */
    int kernel_size;    /*!< FILTER convolution kernel size. */
    filter_channel_out_mode_t out_mode;        /*!< FILTER convolution output date mode. */
    void *output;       /*!< FILTER convolution output data pointer. */
} filter_convolution_config_t;

/*! @brief FILTER vector operation configuration structure.*/
typedef struct _filter_vector_config
{
    filter_channel_date_format_t in_format;  /*!< FILTER vector input date format. */
    filter_channel_in_mode_t in_mode0;       /*!< FILTER vector TX Channel 0 input date mode. */
    filter_channel_in_mode_t in_mode1;       /*!< FILTER vector TX Channel 1 input date mode. */
    filter_au_mode_t operation;  /*!< FILTER vector operation mode. */
    filter_control_t control;    /*!< FILTER vector control. */
    void *opa;        /*!< FILTER vector input date operation A pointer. */
    void *opb;        /*!< FILTER vector input date operation B pointer. */
    int size;         /*!< FILTER vector input date size. */
    int  reg0;        /*!< FILTER vector input date constant register0 value. */
    int  reg1;        /*!< FILTER vector input date constant register1 value. */
    filter_channel_out_mode_t out_mode;        /*!< FILTER vector output date mode. */
    void *output;     /*!< FILTER vector output data pointer. */
} filter_vector_config_t;

/*! @brief FILTER configuration structure.*/
typedef struct _filter_config
{
    uint32_t frequency;     /*!< Frequency configuration of FILTER peripheral. */
} filter_config_t;

/*! @brief FILTER transfer request type definition.*/
#define  filter_req_t udma_req_t

/*! @brief FILTER  handle typedef. */
typedef struct _filter_handle filter_handle_t;

/*! @brief FILTER  transfer callback typedef. */
typedef void (*filter_transfer_callback_t)(FILTER_Type *base,
                                               filter_handle_t *handle,
                                               status_t status,
                                               void *userData);

/*! @brief FILTER  transfer structure. */
typedef struct _filter_transfer
{
    uint8_t *txData;          /*!< Send buffer. */
    uint8_t *rxData;          /*!< Receive buffer. */
    volatile size_t txDataSize; /*!< TX Transfer bytes. */
    volatile size_t rxDataSize; /*!< RX Transfer bytes. */

    uint32_t configFlags; /*!< Transfer transfer configuration flags; is used for slave.*/
} filter_transfer_t;


/*! @brief FILTER  handle structure. */
struct _filter_handle
{
    filter_transfer_t transfer;                    /*!< FILTER  transfer copy. */
    size_t transferSize;                               /*!< Total bytes to be transferred. */
    uint8_t state;                                     /*!< A transfer state maintained during transfer. */
    filter_transfer_callback_t callback; /*!< A callback function called when the transfer is finished. */
    void *userData;                                    /*!< A callback parameter passed to the callback function. */
};

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name Initialization and deinitialization
 * @{
 */
/*!
 * @brief Initializes the FILTER .
 *
 * This function initializes the FILTER configuration. This is an example use case.
 *  @code
 *   filter_config_t  Config;
 *   Config.frequency = 50000000;
 *   FILTER_Init(base, &Config, srcClock_Hz);
 *  @endcode
 *
 * @param base FILTER peripheral address.
 * @param Config Pointer to the structure filter_config_t.
 * @param srcClock_Hz Module source input clock in Hertz.
 */
void FILTER_Init(FILTER_Type *base, const filter_config_t *Config, uint32_t srcClock_Hz);

/*!
 * @brief De-initializes the FILTER peripheral. Call this API to gate the FILTER clock.
 * The FILTER module can't work unless the FILTER_Init is called.
 * @param base FILTER base pointer
 */
void FILTER_Deinit(FILTER_Type *base);

/*!
 * @brief  Sets the FILTER configuration structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for use in the FILTER_Configure().
 * Use the initialized structure unchanged in the FILTER_Configure() or modify
 * the structure before calling the FILTER_Configure().
 * This is an example.
 * @code
 * filter_config_t config;
 * FILTER_GetDefaultConfig(&config);
 * @endcode
 * @param Config A pointer to the  configuration structure.
*/
void FILTER_GetDefaultConfig(filter_config_t *Config);


/* @} */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Sets the FILTER transfer frequency.
 *
 * @param base FILTER base pointer
 * @param frequency Frequency value in Hz
 * @param srcClock_Hz Source clock
 */
void FILTER_SetFrequency(FILTER_Type *base, uint32_t frequency, uint32_t srcClock_Hz);

/*!
 * @brief FILTER Threshold operation.
 *
 * @param base        FILTER base pointer
 * @param config      FILTER structure filter_threshold_config_t pointer
 * @retval     Pass threshold count
 */
int FILTER_Threshold(FILTER_Type *base, filter_threshold_config_t *config);

/*!
 * @brief FILTER Binarization operation.
 *
 * @param base        FILTER base pointer
 * @param config      FILTER structure filter_binarization_config_t pointer
 * @retval     Pass transpose count
 */
int FILTER_Binarization(FILTER_Type *base, filter_binarization_config_t *config);

/*!
 * @brief FILTER Transpose operation.
 *
 * @param base        FILTER base pointer
 * @param config      FILTER structure filter_transpose_config_t pointer
 * @retval     Success or not
 */
int FILTER_Transpose(FILTER_Type *base, filter_transpose_config_t *config);

/*!
 * @brief FILTER Memcpy operation.
 *
 * @param base        FILTER base pointer
 * @param config      FILTER structure filter_memcpy_config_t pointer
 * @retval     Success or not
 */
int FILTER_Memcpy(FILTER_Type *base, filter_memcpy_config_t *config);

/*!
 * @brief FILTER Convolution operation.
 *
 * @param base        FILTER base pointer
 * @param config      FILTER structure filter_convolution_config_t pointer
 * @retval     Success or not
 */
int FILTER_Convolution(FILTER_Type *base, filter_convolution_config_t *config);

/*!
 * @brief FILTER Vector operation.
 *
 * @param base        FILTER base pointer
 * @param config      FILTER structure filter_vector_config_t pointer
 * @retval     Success or not
 */
int FILTER_Vector(FILTER_Type *base, filter_vector_config_t *config);

/*!
 * @brief Performs a transfer on the FILTER bus.
 *
 * @note The API does not return until the transfer succeeds or fails due
 * to arbitration lost or receiving a NAK.
 *
 * @param base FILTER peripheral base address.
 * @param tx_buffer Udma tx buffer base address.
 * @param tx_length Udma tx buffer size.
 * @param rx_buffer Udma rx buffer base address.
 * @param rx_length Udma rx buffer size.
 */
status_t FILTER_TransferBlocking(FILTER_Type *base, const void *tx_buffer, int tx_length, void *rx_buffer, int rx_length);


/*!
 * @brief Performs a interrupt non-blocking transfer on the FILTER bus.
 *
 * @note Calling the API returns immediately after transfer initiates. When all
 * data is transferred, the callback function is called.
 *
 * @param base FILTER base pointer.
 * @param handle pointer to filter_handle_t structure which stores the transfer state.
 * @param transfer pointer to filter_transfer_t structure.
 * @retval uStatus_Success Successfully start the data transmission.
 * @retval uStatus_FILTER_Busy Previous transmission still not finished.
 * @retval uStatus_FILTER_Error Transfer error, wait signal timeout.
 */
status_t FILTER_TransferNonBlocking(FILTER_Type *base, filter_handle_t *handle, filter_transfer_t *transfer);

/*!
 * @brief  interrupt handler.
 *
 * @param base FILTER base pointer.
 * @param handle pointer to filter_handle_t structure.
 */
void FILTER_TransferHandleIRQ(FILTER_Type *base, filter_handle_t *handle);

/*!
 * @brief Initializes the FILTER handle which is used in transactional functions.
 *
 * @param base FILTER base pointer.
 * @param handle pointer to filter_handle_t structure to store the transfer state.
 * @param callback pointer to user callback function.
 * @param userData user parameter passed to the callback function.
 */
void FILTER_TransferCreateHandle(FILTER_Type *base,
                                    filter_handle_t *handle,
                                    filter_transfer_callback_t callback,
                                    void *userData);

/*!
 * @brief FILTER IRQ handler.
 *
 * This function is called to handle IRQ coming from filter end event.
 */
void FILTER_IRQHandler(void);

/*!
 * @brief FILTER Binarization count IRQ handler.
 *
 * This function is called to handle IRQ coming from binarization count event.
 */
void FILTER_BINCNT_IRQHandler(void);


/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_FILTER_H_*/
