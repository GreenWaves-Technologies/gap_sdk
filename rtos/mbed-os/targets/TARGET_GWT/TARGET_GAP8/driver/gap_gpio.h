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

#ifndef _GAP_GPIO_H_
#define _GAP_GPIO_H_

#include "gap_util.h"

/*!
 * @addtogroup gpio
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @name Driver version */
/*@{*/
/*! @brief GPIO driver version 1.0.0. */
#define GAP_GPIO_DRIVER_VERSION (MAKE_VERSION(1, 0, 0))
/*@}*/

/*! @brief Internal resistor pull feature selection */
typedef enum _gpio_pull
{
    uGPIO_PullUpDisable = 0U, /*!< Internal pull-up resistor is disabled. */
    uGPIO_PullUpEnable = 1U   /*!< Internal pull-up resistor is enabled. */
} gpio_pull_t;

/*! @brief Configures the drive strength. */
typedef enum _gpio_drive_strength
{
    uGPIO_LowDriveStrength = 0U,  /*!< Low-drive strength is configured. */
    uGPIO_HighDriveStrength = 1U, /*!< High-drive strength is configured. */
} gpio_driver_strength_t;

/*! @brief GPIO state definition */
typedef enum _gpio_pin_state
{
    uGPIO_LOW  = 0U,   /*!< Set current pin as 0 */
    uGPIO_HIGH = 1U,  /*!< Set current pin as 1 */
} gpio_pin_state_t;

/*! @brief GPIO direction definition */
typedef enum _gpio_pin_direction
{
    uGPIO_DigitalInput = 0U,  /*!< Set current pin as digital input*/
    uGPIO_DigitalOutput = 1U, /*!< Set current pin as digital output*/
} gpio_pin_direction_t;

/*! @brief Configures the interrupt generation condition. */
typedef enum _gpio_interrupt
{
    uGPIO_InterruptFallingEdge = 0x0U,  /*!< Interrupt on falling edge. */
    uGPIO_InterruptRisingEdge  = 0x1U,  /*!< Interrupt on rising edge. */
    uGPIO_InterruptEitherEdge  = 0x2U,  /*!< Interrupt on rising edge or falling edge. */
    uGPIO_InterruptDisabled    = 0x3U,  /*!< Interrupt request is disabled. */
} gpio_interrupt_t;

/*!
 * @brief The GPIO pin configuration structure.
 *
 * Each pin can only be configured as either an output pin or an input pin at a time.
 *
 * Note that in some use cases, the corresponding port property should be configured in advance
 *        with the PORT_SetPinConfig().
 */
typedef struct _gpio_pin_config
{
    gpio_pin_direction_t pinDirection; /*!< GPIO direction, input or output */
    /* Output configurations; ignore if configured as an input pin */
    uint8_t outputLogic; /*!< Set a default output logic, which has no use in input */
    uint8_t pullSelect;    /*!< No-pull/pull-down/pull-up select */
    uint8_t driveStrength; /*!< Fast/slow drive strength configure */
} gpio_pin_config_t;

/*! @} */

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @addtogroup gpio_driver
 * @{
 */

/*! @name GPIO Configuration */
/*@{*/

/*!
 * @brief Initializes a GPIO pin used by the board.
 *
 * To initialize the GPIO, define a pin configuration, as either input or output, in the user file.
 * Then, call the GPIO_PinInit() function.
 *
 * This is an example to define an input pin or an output pin configuration.
 * @code
 * // Define a digital input pin configuration,
 * gpio_pin_config_t config =
 * {
 *   uGPIO_DigitalInput,
 *   0,
 * }
 * //Define a digital output pin configuration,
 * gpio_pin_config_t config =
 * {
 *   uGPIO_DigitalOutput,
 *   0,
 * }
 * @endcode
 *
 * @param base   GPIO peripheral base pointer .
 * @param pin    GPIO port pin number
 * @param config GPIO pin configuration pointer
 */
void GPIO_PinInit(GPIO_Type *base, uint32_t pin, const gpio_pin_config_t *config);

/*@}*/

/*! @name GPIO Output Operations */
/*@{*/

/*!
 * @brief Sets the output level of the multiple GPIO pins to the logic 1 or 0.
 *
 * @param base    GPIO peripheral base pointer.
 * @param pin     GPIO pin number
 * @param output  GPIO pin output logic level.
 *        - 0: corresponding pin output low-logic level.
 *        - 1: corresponding pin output high-logic level.
 */
static inline void GPIO_WritePinOutput(GPIO_Type *base, uint32_t pin, uint8_t output)
{
    if (output == 0U)
    {
        base->OUT &= ~(1U << pin);
    }
    else
    {
        base->OUT |= (1U << pin);
    }
}

/*!
 * @brief Sets the output level of the multiple GPIO pins to the logic 1.
 *
 * @param base GPIO peripheral base pointer.
 * @param pin  GPIO pin number
 * @param direction GPIO pin direction
 */
static inline void GPIO_SetPinDirection(GPIO_Type *base, uint32_t pin, gpio_pin_direction_t direction)
{
    base->EN  |= (1U << pin);

    switch (direction) {
        case uGPIO_DigitalInput:
            base->DIR &= ~(1U << pin);
            break;
        case uGPIO_DigitalOutput:
            base->DIR |= (1U << pin);
            break;
    }
}

/*!
 * @brief Sets the gpio PADCFG register.
 *
 * This is an example to define an input pin or output pin PADCFG configuration.
 * @code
 * // Define a digital input pin PADCFG and PADFUN configuration
 * gpio_pin_config_t config = {
 *      uGPIO_DigitalInput,
 *      0,
 *      uGPIO_PullUpEnable,
 *      uGPIO_LowDriveStrength,
 * };
 * @endcode
 *
 * @param base   GPIO peripheral base pointer.
 * @param pin    GPIO pin number.
 * @param config GPIO PADCFG register configuration structure.
 */
static inline void GPIO_SetPinConfig(GPIO_Type *base, uint32_t pin, const gpio_pin_config_t *config)
{
    /* Set pin pull-up and drive strength*/
    int reg_num = pin >> 2;
    int pos = pin & 0x3;
    int val = base->PADCFG[reg_num];
    val &= ~((GPIO_PADCFG_DRIVE_STRENGTH_MASK | GPIO_PADCFG_PULL_EN_MASK) << (pos << 3));

    base->PADCFG[reg_num] = val | (uint32_t)((config->pullSelect |
                                              GPIO_PADCFG_DRIVE_STRENGTH(config->driveStrength)) << (pos << 3));
}

/*!
 * @brief Sets the output level of the multiple GPIO pins to the logic 1.
 *
 * @param base GPIO peripheral base pointer.
 * @param mask GPIO pin number macro
 */
static inline void GPIO_SetPinsOutput(GPIO_Type *base, uint32_t mask)
{
    base->OUT |= mask;
}

/*!
 * @brief Sets the output level of the multiple GPIO pins to the logic 0.
 *
 * @param base GPIO peripheral base pointer.
 * @param mask GPIO pin number macro
 */
static inline void GPIO_ClearPinsOutput(GPIO_Type *base, uint32_t mask)
{
    base->OUT &= ~mask;
}

/*!
 * @brief Reverses the current output logic of the multiple GPIO pins.
 *
 * @param base GPIO peripheral base pointer.
 * @param mask GPIO pin number macro
 */
static inline void GPIO_TogglePinsOutput(GPIO_Type *base, uint32_t mask)
{
    base->OUT ^= mask;
}
/*@}*/

/*! @name GPIO Input Operations */
/*@{*/

/*!
 * @brief Reads the current input value of the GPIO port.
 *
 * @param base GPIO peripheral base pointer.
 * @param pin     GPIO pin number
 * @retval GPIO port input value
 *        - 0: corresponding pin input low-logic level.
 *        - 1: corresponding pin input high-logic level.
 */
static inline uint32_t GPIO_ReadPinInput(GPIO_Type *base, uint32_t pin)
{
    if ((((base->DIR) >> pin) & 0x1) == uGPIO_DigitalOutput)
        return (((base->OUT) >> pin) & 0x01U);
    else
        return (((base->IN) >> pin) & 0x01U);
}
/*@}*/

/*! @name GPIO Interrupt */
/*@{*/

/*!
 * @brief Configures the gpio pin interrupt/DMA request.
 *
 * @param base    GPIO peripheral base pointer.
 * @param pin     GPIO pin number.
 * @param config  GPIO pin interrupt configuration.
 *        - #uGPIO_InterruptRisingEdge : Interrupt on rising edge.
 *        - #uGPIO_InterruptFallingEdge: Interrupt on falling edge.
 *        - #uGPIO_InterruptEitherEdge : Interrupt on rising or falling edge.
 */
static inline void GPIO_SetPinInterruptConfig(GPIO_Type *base, uint32_t pin, gpio_interrupt_t config)
{
    int int_type = config & GPIO_INTCFG_TYPE_BITS_WIDTH_MASK;

    if (int_type == uGPIO_InterruptDisabled)
    {
        base->INTEN &= ~(1 << pin);
    } else {
        base->INTEN |= (1 << pin);

        int reg_num = pin >> 4;
        /* Positon in the target register */
        int pos = pin & 0xF;
        int val = base->INTCFG[reg_num];
        val &= ~(GPIO_INTCFG_TYPE_MASK << (pos << 1));
        base->INTCFG[reg_num] = val | (uint32_t)(int_type << (pos << 1));
    }
}

/*!
 * @brief Reads the GPIO port interrupt status flag.
 *
 * @param base GPIO peripheral base pointer.
 * @retval The current GPIO port interrupt status flag, for example, 0x00010001 means the
 *         pin 0 and 17 have the interrupt.
 */
static inline uint32_t GPIO_GetPinsInterruptFlags(GPIO_Type *base)
{
    return base->INTSTATUS;
}

/*!
 * @brief Clears multiple GPIO pin interrupt status flags.
 *
 * @param base GPIO peripheral base pointer.
 * @param mask GPIO pin number macro
 */
static inline void GPIO_ClearPinsInterruptFlags(GPIO_Type *base, uint32_t mask)
{
    /* InterruptFlags will be all cleared once read*/
    GPIO_GetPinsInterruptFlags(base);
}

/*!
 * @brief Binding GPIO interrupt handler
 *
 * @param base GPIO peripheral base pointer.
 * @param irq  GPIO Interrupt handler pointer.
 */
void GPIO_IRQHandlerBind(GPIO_Type *base, uint32_t irq);

/*!
 * @brief GPIO interrupt handler
 */
void GPIO_IRQHandler(void);

/*@}*/


#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_GPIO_H_*/
