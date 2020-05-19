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
#ifndef _GAP_QUIDDIKEY_H_
#define _GAP_QUIDDIKEY_H_

#include "gap_util.h"

/*!
 * @addtogroup quiddikey
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief QUIDDIKEY driver version 3.6.3. */
#define GAP_QUIDDIKEY_DRIVER_VERSION (MAKE_VERSION(3, 6, 3))
/*@}*/

/*! @brief QUIDDIKEY Active code length 996 bytes */
#define GAP_QUIDDIKEY_AC_BYTES 996

/*! @brief QUIDDIKEY Key code length table in bytes */
extern uint16_t key_code_length_0[];
extern uint16_t key_code_length_1[];

/*! @brief QUIDDIKEY Key code length in bytes */
#define GAP_QUIDDIKEY_KEY_CODE_LENGTH_BYTE(x)  ((x <= 1024) ?  key_code_length_0[(x >> 6)  - 1] : key_code_length_1[(x >> 10) - 2])

/*! @brief Device State */
typedef enum _quiddikey_state
{
    uQUIDDIKEY_OK        = 0U,   /*!< Device OK. */
    uQUIDDIKEY_Zeroized  = 1U,   /*!< Device zeroized, need to power-cycling. */
    uQUIDDIKEY_Lock      = 2U,   /*!< Device Lock, need to reset. */
} quiddikey_state_t;

/*! @brief Output Destination selection */
typedef enum _quiddikey_destination
{
    uQUIDDIKEY_DOR        = 0U,   /*!< Destination to data output. */
    uQUIDDIKEY_SO         = 1U,   /*!< Destination to key out interface. */
} quiddikey_destination_t;

/*! @brief Key operation scope in FSM type selection */
typedef enum _quiddikey_key_scope_type
{
    uQUIDDIKEY_Started   = 0U,   /*!< FSM now is start. */
    uQUIDDIKEY_Enrolled  = 1U,   /*!< FSM now is enroll. */
} quiddikey_key_scope_type_t;

/*! @brief Data Endianness selection */
typedef enum _quiddikey_endianness
{
    uQUIDDIKEY_LittleEndianness = 0U,   /*!< Little Endianness. */
    uQUIDDIKEY_BigEndianness = 1U,      /*!< Big Endianness. */
} quiddikey_endianness_t;

/*! @brief  QUIDDIKEY's interrupt type. */
typedef enum _quiddikey_interrupt_type
{
    uQUIDDIKEY_INT_Busy     = 0x0U,  /*!< Interrupt on falling edge. */
    uQUIDDIKEY_INT_OK       = 0x1U,  /*!< Interrupt on rising edge. */
    uQUIDDIKEY_INT_Error    = 0x2U,  /*!< Interrupt on rising edge or falling edge. */
    uQUIDDIKEY_INT_Zeroized = 0x3U,  /*!< Interrupt request is disabled. */
    uQUIDDIKEY_INT_Reject   = 0x4U,  /*!< Interrupt request is disabled. */
    uQUIDDIKEY_INT_DI_Request   = 0x5U,  /*!< Interrupt request is disabled. */
    uQUIDDIKEY_INT_DO_Request   = 0x6U,  /*!< Interrupt request is disabled. */
} quiddikey_interrupt_type_t;

/*! @brief QUIDDIKEY device context configuration */
typedef struct _quiddikey_context
{
    uint16_t data_length;   /*!< Length of the requested data in bit; the following lengths are supported:
                              - 64 .. 1024 bits in 64-bit increments
                              - 2048 bits
                              - 3072 bits
                              - 4096 bits. */

    uint16_t key_length;   /*!< Length of the key in bits; the following lengths are supported:
                             - 64 .. 1024 bits in 64-bit increments
                             - 2048 bits
                             - 3072 bits
                             - 4096 bits. */

    uint16_t key_scope_started;   /*!< Defines the allowed key destinations, when QuiddiKey
                                    is in the Started state:
                                    bit 8: Key can be made available via the
                                    QK_DOR register
                                    bit 9: Key can be made available via the SO in-
                                    terface
                                    Others: Reserved */
    uint8_t  key_scope_enrolled;  /*!< Defines the allowed key destinations, when QuiddiKey
                                    is in the Enrolled state:
                                    bit 0: Key can be made available via the
                                    QK_DOR register
                                    bit 1: Key can be made available via the SO in-
                                    terface
                                    Others: Reserved */
} quiddikey_context_t;

/*! @brief QUIDDIKEY device Configuration */
typedef struct _quiddikey_config
{
    uint8_t config;   /*!< . */
} quiddikey_config_t;

/*! @} */

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @addtogroup quiddikey_driver
 * @{
 */

/*! @name QUIDDIKEY Configuration */
/*@{*/

/*!
 * @brief Initializes QUIDDIKEY.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @param config QUIDDIKEY configuration pointer
 */
void QUIDDIKEY_Init(QUIDDIKEY_Type *base, const quiddikey_config_t *config);

/*!
 * @brief DeInitializes QUIDDIKEY.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 */
void QUIDDIKEY_DeInit(QUIDDIKEY_Type *base);

/*!
 * @brief QUIDDIKEY Enroll operation.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @param ac     Active code pointer
 * @retval status
 */
int QUIDDIKEY_Enroll(QUIDDIKEY_Type *base, uint32_t *ac);

/*!
 * @brief QUIDDIKEY Start operation.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @param ac     Active code pointer
 * @retval status
 */
int QUIDDIKEY_Start(QUIDDIKEY_Type *base, uint32_t *ac);

/*!
 * @brief QUIDDIKEY Stop operation.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval status
 */
int QUIDDIKEY_Stop(QUIDDIKEY_Type *base);

/*!
 * @brief QUIDDIKEY Get key operation.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @param key    Key pointer
 * @param context Context configuration pointer
 * @retval status
 */
int QUIDDIKEY_GetKey(QUIDDIKEY_Type *base, uint32_t *key, quiddikey_context_t *context);

/*!
 * @brief QUIDDIKEY Wrap Generate Random operation.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @param key_code    Key code output pointer
 * @param context Context configuration pointer
 * @retval status
 */
int QUIDDIKEY_WrapGeneratedRandom(QUIDDIKEY_Type *base, uint32_t *key_code, quiddikey_context_t *context);

/*!
 * @brief QUIDDIKEY Wrap Generate Random operation.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @param key    Key input pointer
 * @param key_code    Key code output pointer
 * @param context Context configuration pointer
 * @retval status
 */
int QUIDDIKEY_Wrap(QUIDDIKEY_Type *base, uint32_t *key, uint32_t *key_code, quiddikey_context_t *context);

/*!
 * @brief QUIDDIKEY Wrap Generate Random operation.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @param key_code  Key code input pointer
 * @param key       Key output pointer
 * @param context Context configuration pointer
 * @retval status
 */
int QUIDDIKEY_UnWrap(QUIDDIKEY_Type *base, uint32_t *key_code, uint32_t *key, quiddikey_context_t *context);

/*!
 * @brief QUIDDIKEY Generate random.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @param random Random data pointer
 * @param context Context configuration pointer
 * @retval status
 */
int QUIDDIKEY_GenerateRandom(QUIDDIKEY_Type *base, uint32_t *random_data, quiddikey_context_t *context);

/*!
 * @brief QUIDDIKEY Zeroize.
 *
 * After zeroize, a software reset must be set and clean
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval status
 */
int QUIDDIKEY_Zeroize(QUIDDIKEY_Type *base);

static inline int QUIDDIKEY_IsLock(QUIDDIKEY_Type *base)
{
    int sr = base->SR;

    /* Check qk_ok, qk_error, qk_zeroized flag*/
    if(!READ_QUIDDIKEY_SR_OK(sr) &&
       READ_QUIDDIKEY_SR_ERROR(sr) &&
       READ_QUIDDIKEY_SR_ZEROIZED(sr))
        return 0;
    else
        return -1;
}

static inline int QUIDDIKEY_IsZeroized(QUIDDIKEY_Type *base)
{
    int sr = base->SR;

    /* Check qk_ok, qk_error, qk_zeroized flag*/
    if(READ_QUIDDIKEY_SR_OK(sr) &&
       !READ_QUIDDIKEY_SR_ERROR(sr) &&
       READ_QUIDDIKEY_SR_ZEROIZED(sr))
        return 0;
    else
        return -1;
}

/*!
 * @brief Indicate QuiddiKey deive busy status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY busy 1 or 0 for not busy
 */
static inline int QUIDDIKEY_Busy(QUIDDIKEY_Type *base)
{
    return READ_QUIDDIKEY_SR_BUSY(base->SR);
}

/*!
 * @brief Indicate QuiddiKey deive ok status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY ok 1 or 0 for not ok
 */
static inline int QUIDDIKEY_Ok(QUIDDIKEY_Type *base)
{
    return READ_QUIDDIKEY_SR_OK(base->SR);
}

/*!
 * @brief Indicate QuiddiKey deive error status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY error 1 or 0 for not error
 */
static inline int QUIDDIKEY_Error(QUIDDIKEY_Type *base)
{
    return READ_QUIDDIKEY_SR_ERROR(base->SR);
}

/*!
 * @brief Indicate QuiddiKey deive zeroized status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY zeroized 1 or 0 for not zeroized
 */
static inline int QUIDDIKEY_Zeroized(QUIDDIKEY_Type *base)
{
    return READ_QUIDDIKEY_SR_ZEROIZED(base->SR);
}

/*!
 * @brief Indicate QuiddiKey deive rejected status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY rejected 1 or 0 for not rejected
 */
static inline int QUIDDIKEY_Rejected(QUIDDIKEY_Type *base)
{
    return READ_QUIDDIKEY_SR_REJECTED(base->SR);
}

/*!
 * @brief Indicate QuiddiKey deive di_request status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY di_request 1 or 0 for not di_request
 */
static inline int QUIDDIKEY_Di_Request(QUIDDIKEY_Type *base)
{
    return READ_QUIDDIKEY_SR_DI_REQUEST(base->SR);
}

/*!
 * @brief Indicate QuiddiKey deive do_request status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY do_request 1 or 0 for not do_request
 */
static inline int QUIDDIKEY_Do_Request(QUIDDIKEY_Type *base)
{
    return READ_QUIDDIKEY_SR_DO_REQUEST(base->SR);
}

/*!
 * @brief Set QuiddiKey data endianness.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @param endianness   Data endianness.
 */
static inline void QUIDDIKEY_SetEndianness(QUIDDIKEY_Type *base, quiddikey_endianness_t endianness)
{
    base->MISC = QUIDDIKEY_MISC_DATA_ENDIANNESS(endianness);
}

/*!
 * @brief Set the quiddikey interrupt global enable register.
 *
 * With the Interrupt Enable register all QuiddiKey interrupts can be enabled or disabled,
 * without changing the Interrupt Mask register.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @param value    QUIDDIKEY interrupt type
 */
static inline void QUIDDIKEY_SetInterruptEnable(QUIDDIKEY_Type *base, uint32_t value)
{
    base->IER = QUIDDIKEY_IER_INT_EN(value);
}

/*!
 * @brief Get the quiddikey interrupt mask register.
 *
 * Individual QuiddiKey interrupts can be enabled or disabled with the Interrupt Mask register.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval The QUIDDIKEY Interrupt mask register
 */
static inline uint32_t QUIDDIKEY_GetInterruptMask(QUIDDIKEY_Type *base)
{
    return base->IMR;
}

/*!
 * @brief Enable each quiddikey interrupt.
 *
 * Individual QuiddiKey interrupts can be enabled or disabled with the Interrupt Mask register.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @param irq    QUIDDIKEY interrupt type need to enabled or disable
 * @param value  QUIDDIKEY interrupt enable status
 */
static inline void QUIDDIKEY_SetInterruptMask(QUIDDIKEY_Type *base, uint32_t irq, uint32_t value)
{
    if(value)
        base->IMR |= (1 << irq);
    else
        base->IMR &= ~(1 << irq);
}

/*!
 * @brief Reads the QUIDDIKEY interrupt status register.
 *
 * The Interrupt Status register indicates which interrupt has occurred.
 * Multiple interrupts may occur at the same time (e.g. the busy interrupt occurs together with one of the
 * ok and error interrupts).
 *
 * Writing 1 to a bit, clears the corresponding bit.
 *
 * @param base QUIDDIKEY peripheral base pointer.
 * @retval The current QUIDDIKEY interrupt status register, for example, 0x00010001 means
 *         the uQUIDDIKEY_INT_Busy and uQUIDDIKEY_INT_Reject interrupt come.
 */
static inline uint32_t QUIDDIKEY_GetInterruptStatus(QUIDDIKEY_Type *base)
{
    return base->ISR;
}

/*!
 * @brief Clears QUIDDIKEY interrupt status register.
 *
 * The Interrupt Status register indicates which interrupt has occurred.
 * Multiple interrupts may occur at the same time (e.g. the busy interrupt occurs together with one of the
 * ok and error interrupts).
 *
 * Writing 1 to a bit, clears the corresponding bit.
 *
 * @param base QUIDDIKEY peripheral base pointer.
 * @param mask QUIDDIKEY clear interrupt type mask
 */
static inline void QUIDDIKEY_ClearInterruptStatus(QUIDDIKEY_Type *base, quiddikey_interrupt_type_t irq )
{
    base->ISR = (1 << irq);
}

/*!
 * @brief Binding QUIDDIKEY interrupt handler
 *
 * @param base QUIDDIKEY peripheral base pointer.
 * @param irq  QUIDDIKEY Interrupt handler pointer.
 */
void QUIDDIKEY_IRQHandlerBind(QUIDDIKEY_Type *base, uint32_t irq);

/*!
 * @brief QUIDDIKEY interrupt handler
 */
void QUIDDIKEY_IRQHandler(void);
/*@}*/


#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_QUIDDIKEY_H_*/
