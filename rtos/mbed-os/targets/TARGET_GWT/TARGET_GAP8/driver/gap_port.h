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
 * DISCLAIMED. IN NO EVENT SDRVL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _GAP_PORT_H_
#define _GAP_PORT_H_

#include <assert.h>

/*!
 * @addtogroup port
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! Version 1.0.0. */
#define GAP_PORT_DRIVER_VERSION (MAKE_VERSION(1, 0, 0))
/*@}*/

/*! @brief Internal resistor pull feature selection */
enum _port_pull
{
    uPORT_PullUpDisable = 0U, /*!< Internal pull-up resistor is disabled. */
    uPORT_PullUpEnable = 1U   /*!< Internal pull-up resistor is enabled. */
};

/*! @brief Configures the drive strength. */
enum _port_drive_strength
{
    uPORT_LowDriveStrength = 0U,  /*!< Low-drive strength is configured. */
    uPORT_HighDriveStrength = 1U, /*!< High-drive strength is configured. */
};

/*! @brief Pin mux selection */
typedef enum _port_mux
{
    uPORT_MuxAlt0 = 0U,           /*!< Default */
    uPORT_MuxAlt1 = 1U,           /*!< Corresponding pin is configured as GPIO. */
    uPORT_MuxGPIO = uPORT_MuxAlt1,/*!< Corresponding pin is configured as GPIO. */
    uPORT_MuxAlt2 = 2U,           /*!< Chip-specific */
    uPORT_MuxAlt3 = 3U,           /*!< Chip-specific */
} port_mux_t;

/*! @brief PORT pin configuration structure */
typedef struct _port_pin_config
{
    uint8_t pullSelect;    /*!< No-pull/pull-down/pull-up select */
    uint8_t driveStrength; /*!< Fast/slow drive strength configure */
    port_mux_t mux;         /*!< Pin mux Configure */
} port_pin_config_t;

/*******************************************************************************
* API
******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*! @name Configuration */
/*@{*/

static inline void PORT_SetPinMux(PORT_Type *base, uint32_t pin, port_mux_t mux);

/*!
 * @brief Sets the port PADCFG and PADFUN register.
 *
 * This is an example to define an input pin or output pin PADCFG and PADFUN configuration.
 * @code
 * // Define a digital input pin PADCFG and PADFUN configuration
 * port_pin_config_t config = {
 *      uPORT_PullUpEnable,
 *      uPORT_LowDriveStrength,
 *      uPORT_MuxAsGpio,
 * };
 * @endcode
 *
 * @param base   PORT peripheral base pointer.
 * @param pin    PORT pin number.
 * @param config PORT PADCFG and PADFUN register configuration structure.
 */
static inline void PORT_SetPinConfig(PORT_Type *base, uint32_t pin, const port_pin_config_t *config)
{
    assert(config);
    /* Set pin pull-up and drive strength*/
    int reg_num = pin >> 2;
    int pos = pin & 0x3;
    int val = base->PADCFG[reg_num];
    val &= ~((PORT_PADCFG_DRIVE_STRENGTH_MASK | PORT_PADCFG_PULL_EN_MASK) << (pos << 3));

    base->PADCFG[reg_num] = val | (uint32_t)((config->pullSelect |
                                              PORT_PADCFG_DRIVE_STRENGTH(config->driveStrength)) << (pos << 3));


    PORT_SetPinMux(base, pin, config->mux);
}

/*!
 * @brief Sets the port PADCFG and PADFUN register for multiple pins.
 *
 * This is an example to define input pins or output pins PADCFG and PADFUN configuration.
 * @code
 * // Define a digital input pin PADCFG and PADFUN configuration
 * port_pin_config_t config = {
 *      uPORT_PullUp ,
 *      uPORT_LowDriveStrength,
 *      uPORT_MuxAsGpio,
 * };
 * @endcode
 *
 * @param base   PORT peripheral base pointer.
 * @param mask   PORT pin number macro.
 * @param config PORT PADCFG and PADFUN register configuration structure.
 */
static inline void PORT_SetMultiplePinsConfig(PORT_Type *base, uint32_t mask, const port_pin_config_t *config)
{
    for (int pin = 0; pin < 32; pin++) {
        if(mask & (1 << pin))
            PORT_SetPinConfig(base, pin, config);
    }
}

/*!
 * @brief Configures the pin muxing.
 *
 * @param base  PORT peripheral base pointer.
 * @param pin   PORT pin number.
 * @param mux   pin muxing slot selection.
 *        - #uPORT_MuxAlt0            : Default.
 *        - #uPORT_MuxAlt1            : Set as GPIO.
 *        - #uPORT_MuxAlt2            : chip-specific.
 *        - #uPORT_MuxAlt3            : chip-specific.
 *        This function is recommended to use to reset the pin mux
 *
 */
static inline void PORT_SetPinMux(PORT_Type *base, uint32_t pin, port_mux_t mux)
{
    int reg_num = (pin >> 4) & 0x3;
    /* Positon in the target register */
    int pos = pin & 0xF;
    int val = base->PADFUN[reg_num];
    val &= ~(PORT_PADFUN_MUX_MASK << (pos << 1));
    base->PADFUN[reg_num] = val | (uint32_t)(mux << (pos << 1));
}

/*!
 * @brief Configures group pin muxing.
 *
 * @param base  PORT peripheral base pointer.
 * @param mux   pin muxing slot selection array for all pins.
 *        This function is recommended to use to reset the group pin mux
 *
 */
static inline void PORT_SetGroupPinMux(PORT_Type *base, uint32_t mux[])
{
    base->PADFUN[0] = mux[0];
    base->PADFUN[1] = mux[1];
    base->PADFUN[2] = mux[2];
    base->PADFUN[3] = mux[3];
}


/*@}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _GAP_PORT_H_ */
