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

#ifndef _GAP_CPI_H_
#define _GAP_CPI_H_

#include "gap_common.h"
#include "PinNames.h"
#include "gap_udma.h"

/*!
 * @addtogroup cpi
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct cpi_s cpi_t;

/*! @name Driver version */
/*@{*/
#define GAP_CPI_DRIVER_VERSION (MAKE_VERSION(1, 0, 0)) /*!< Version 1.0.0 */
/*@}*/

/*! @brief CPI transfer state. */
enum _cpi_status
{
    uStatus_CPI_Busy = MAKE_STATUS(uStatusGroup_CPI, 0),            /*!< CPI is busy with current transfer. */
    uStatus_CPI_Idle = MAKE_STATUS(uStatusGroup_CPI, 1),            /*!< Bus is Idle. */
    uStatus_CPI_Error = MAKE_STATUS(uStatusGroup_CPI, 2)            /*!< Error received during transfer. */
};

/** \enum cpi_resolution_e
 * \brief Resolution of image.
 *
 * This enum defines the resolution of image sensor.
 */
typedef enum
{
    VGA = 0,
    QVGA,
    QQVGA
} cpi_resolution_e;

/** \enum cpi_format_e
 * \brief Format of image.
 *
 * This enum defines the format of image sensor.
 */
typedef enum
{
    RGB565 = 0,       /*!< Data format RGB565, 5bits for R, 6bits for G and 5bits for B. */
    RGB555,           /*!< Data format RGB555, 5bits for R, 5bits for G and 5bits for B. */
    RGB444,           /*!< Data format RGB444, 4bits for R, 4bits for G and 4bits for B. */
    YUV422 =4,        /*!< Data format YUV442, 4bits for Y, 2bits for U and 2bits for V. */
    BYPASS_LITEND =4, /*!< Data format BYPASS in Little Endian */
    BYPASS_BIGEND,    /*!< Data format BYPASS in Big Endian */
} cpi_format_e;

/** \struct image_coordinate_t
 * \brief Coordinate of a pixel.
 *
 * This structure is used to describe the coordinates of a pixel in an image.
 */
typedef struct
{
  unsigned int x:16;
  unsigned int y:16;
} image_coordinate_t;


/** \struct image_slice_t
 * \brief CPI interface parameter.
 *
 * This structure is used to define an area of interest in an image.
 */
typedef struct
{
  image_coordinate_t   slice_ll;   /*!< Structure contains the coordinates of lower-left corner. */
  image_coordinate_t   slice_ur;   /*!< Structure contains the coordinates of upper-right corner. */
} image_slice_t;

/** \struct image_filter_t
 * \brief CPI interface parameter .
 *
 * This structure is used to define the HW filter coefficients (red, green, blue).
 * These coefficients are applied as follows:
 *          pixel = red*r_coeff + green*g_coeff + blue*b_coeff
 *
 * For example, with the RGB565 format, each pixel is coded in 2 bytes:
 *          red = pixel[15:11]
 *          green  = pixel[10:5]
 *          blue = pixel[4:0]
 * In the rx buffer, each pixel received would be the result of the above formula.
 */
typedef struct
{
  unsigned char r_coeff;    /*!< Value of red coefficient */
  unsigned char g_coeff;    /*!< Value of green coefficient */
  unsigned char b_coeff;    /*!< Value of blue coefficient */
} image_filter_t;


/** \struct cpi_config_t
 * \brief CPI interface configuration structure.
 *
 * This structure is used to pass the desired CPI configuration.
 */
typedef struct{
  unsigned int     resolution;            /*!< Resolution of the sensor. Please check the available values in enum rt_cam_resol_e */
  unsigned int     row_len;               /*!< Row lenght of image */
  unsigned int     format;                /*!< Image format of the sensor. Please check the available values in enum rt_ov7670_format_e or rt_himax_format_e */
  unsigned int     fps;                   /*!< Frames per second of the sensor. Please check the available value in enum rt_cam_fps_e */
  unsigned int     slice_en;              /*!< Enables slicing mode */
  unsigned int     shift;                 /*!< Number of bits to shift pixels. For example, shift = 2, pixel received = (pixel >> 2) */
  unsigned int     frameDrop_en;          /*!< Enables frame dropping */
  unsigned int     frameDrop_value;       /*!< Define how many frames need to be dropped */
  unsigned int     wordWidth;                /*!< Width of CPI interface*/
}cpi_config_t;

/*! @brief CPI transfer structure.*/
typedef struct _cpi_transfer
{
    uint8_t *data;
    size_t dataSize;
    uint8_t configFlags;
} cpi_transfer_t;

/*! @brief CPI transfer request type definition.*/
#define  cpi_req_t udma_req_t

/*!
 * @brief Forward declaration of the _cpi_handle typedefs.
 */
typedef struct _cpi_handle cpi_handle_t;

/*! @brief CPI transfer callback typedef. */
typedef void (*cpi_transfer_callback_t)(CPI_Type *base,
                                        cpi_handle_t *handle,
                                        status_t status,
                                        void *userData);

struct _cpi_handle
{
    status_t state;
    cpi_transfer_callback_t callback;
    void *userData;
};

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief Initializes the CPI device, including pin mapping.
 * @param base      CPI peripheral address.
 * @param pclk      Pin PCLK
 * @param hync      Pin HYNC
 * @param vsync     Pin VSYNC
 * @param data0     Pin DATA0
 * @param data1     Pin DATA1
 * @param data2     Pin DATA2
 * @param data3     Pin DATA3
 * @param data4     Pin DATA4
 * @param data5     Pin DATA5
 * @param data6     Pin DATA6
 * @param data7     Pin DATA7
 *
 */
void CPI_Init(CPI_Type *base, PinName pclk, PinName hync, PinName vsync, PinName data0, PinName data1, PinName data2, PinName data3, PinName data4, PinName data5, PinName data6, PinName data7);

/*!
 * @brief De-initializes the CPI peripheral, Clock Gating. Call this API to disable the CPI clock.
 * @param base CPI peripheral address.
 */
void CPI_Deinit(CPI_Type *base);

/*!
 * @brief Sets the cpi_config_t structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for the CPI_Init().
 * Users may use the initialized structure unchanged in the CPI_Init() or modify the structure
 * before calling the CPI_Init().
 * Example:
 * @code
 *  cpi_config_t  masterConfig;
 *  CPI_GetDefaultConfig(&masterConfig);
 * @endcode
 * @param masterConfig pointer to cpi_config_t structure
 */
void CPI_GetDefaultConfig(cpi_config_t *masterConfig);

/*!
 * @brief Configure CPI drop frame
 *
 * @param masterConfig pointer to cpi_config_t structure
 * @param arg transfer dropframe argument
 *
 */
static inline void CPI_DropFrame(cpi_config_t *masterConfig, uint16_t arg) {
    masterConfig->frameDrop_en = 1;
    masterConfig->frameDrop_value = arg;
}

/*!
 * @brief CPI normilization
 *
 * @param masterConfig pointer to cpi_config_t structure
 * @param arg transfer Nomilization argument
 *
 */
static inline void CPI_Normalization(cpi_config_t *masterConfig, uint16_t arg) {
    masterConfig->shift = arg;
}

/*!
 * @brief CPI Filter
 *
 * @param base CPI peripheral address.
 * @param filter Pointer to image filter structure
 *
 */
static inline void CPI_Filter(CPI_Type *base, image_filter_t *filter) {
    base->CFG_FILTER = (CPI_CFG_FILTER_R_COEFF(filter->r_coeff)|
                        CPI_CFG_FILTER_G_COEFF(filter->g_coeff)|
                        CPI_CFG_FILTER_B_COEFF(filter->b_coeff));
}

/*!
 * @brief CPI Image Extract
 *
 * @param base CPI peripheral address.
 * @param masterConfig pointer to cpi_config_t structure
 * @param slicer Pointer to image slice structure
 *
 */
static inline void CPI_ImageExtract(CPI_Type *base, cpi_config_t *masterConfig, image_slice_t *slicer) {
    base->CFG_LL = (CPI_CFG_LL_FRAMESLICE_LLX(slicer->slice_ll.x ) | CPI_CFG_LL_FRAMESLICE_LLY(slicer->slice_ll.y ));
    base->CFG_UR = (CPI_CFG_UR_FRAMESLICE_URX((slicer->slice_ur.x-1) ) | CPI_CFG_UR_FRAMESLICE_URY((slicer->slice_ur.y-1) ));

    masterConfig->slice_en = 1;
}

/*!
 * @brief CPI Enable
 *
 * @param base CPI peripheral address.
 * @param masterConfig pointer to cpi_config_t structure
 *
 */
static inline void CPI_Enable(CPI_Type *base, cpi_config_t *masterConfig) {
    base->CFG_SIZE = CPI_CFG_SIZE(masterConfig->row_len - 1);
    base->CFG_GLOB = (CPI_CFG_GLOB_FRAMEDROP_EN(masterConfig->frameDrop_en)     |
                      CPI_CFG_GLOB_FRAMEDROP_VAL(masterConfig->frameDrop_value) |
                      CPI_CFG_GLOB_FRAMESLICE_EN(masterConfig->slice_en)        |
                      CPI_CFG_GLOB_FORMAT(masterConfig->format)                 |
                      CPI_CFG_GLOB_SHIFT(masterConfig->shift)                   |
                      CPI_CFG_GLOB_EN(1));
}

/*!
 * @brief CPI Disable
 *
 * @param base CPI peripheral address.
 *
 */
static inline void CPI_Disable(CPI_Type *base) {
    base->CFG_GLOB &= ~(CPI_CFG_GLOB_EN(0));
}

/*!
 * @brief Writes data into the data buffer master mode and waits till complete to return.
 *
 * In master mode, the 16-bit data is appended to the 16-bit command info. The command portion
 *
 * @param base CPI peripheral address.
 * @param transfer Pointer to the cpi_transfer_t structure.
 * @return status of status_t.
 *
 */
status_t CPI_ReceptionBlocking(CPI_Type *base, cpi_transfer_t *transfer);

/*!
 * @brief CPI master transfer data using interrupts.
 *
 * This function transfers data using interrupts. This is a non-blocking function, which returns right away. When all
 * data is transferred, the callback function is called.

 * @param base CPI peripheral base address.
 * @param handle Pointer to the cpi_handle_t structure which stores the transfer state.
 * @param transfer Pointer to the cpi_transfer_t structure.
 * @return status of status_t.
 */
status_t CPI_ReceptionNonBlocking(CPI_Type *base, cpi_handle_t *handle, cpi_transfer_t * transfer);

/*!
 * @brief Initializes the CPI master handle.
 *
 * This function initializes the CPI handle, which can be used for other CPI transactional APIs.  Usually, for a
 * specified CPI instance,  call this API once to get the initialized handle.
 *
 * @param base CPI peripheral base address.
 * @param handle CPI handle pointer to cpi_handle_t.
 * @param callback CPI callback.
 * @param userData Callback function parameter.
 */
void CPI_ReceptionCreateHandle(CPI_Type *base, cpi_handle_t *handle, cpi_transfer_callback_t callback, void *userData);

/*!
 * @brief CPI IRQ handler function.
 *
 * This function processes the CPI transmit and receive IRQ.

 * @param base CPI peripheral base address.
 * @param handle Pointer to the cpi_handle_t structure which stores the transfer state.
 */
void CPI_ReceptionHandleIRQ(CPI_Type *base, cpi_handle_t *handle);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/**
 * @}
 */

#endif
