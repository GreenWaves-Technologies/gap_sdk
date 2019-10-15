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

#include "gap_quiddikey.h"
#include "gap_itc.h"
#include "gap_handler_wrapper.h"
#include <assert.h>

/*******************************************************************************
 * Variables
 ******************************************************************************/
typedef void (*irq_handler)();

static irq_handler quiddikey_irq_handler;

/*! @brief QUIDDIKEY Key Code length table in bytes */
uint16_t key_code_length_0[] = {56,  64,  72,  80,
                                88,  96,  120, 128,
                                136, 144, 152, 160,
                                184, 192, 200, 208};

uint16_t key_code_length_1[] = {384, 544, 720};

/*******************************************************************************
* Prototypes
******************************************************************************/
/* handler wrapper  */
Handler_Wrapper(QUIDDIKEY_IRQHandler);

/*******************************************************************************
 * Code
 ******************************************************************************/
void QUIDDIKEY_Init(QUIDDIKEY_Type *base, const quiddikey_config_t *config)
{
    assert(config);

    /* Activate interrupt handler for FC when cluster want to push a task to FC */
    NVIC_SetVector(FC_SOC_QUIDDIKEY_IRQn, (uint32_t)__handler_wrapper_QUIDDIKEY_IRQHandler);

    /* Enable QuiddiKey Interrupt */
    NVIC_EnableIRQ(FC_SOC_QUIDDIKEY_IRQn);

    /* Use little endianness*/
    QUIDDIKEY_SetEndianness(base, uQUIDDIKEY_BigEndianness);

    /* Enable Quiddikey global IRQ */
    QUIDDIKEY_SetInterruptEnable(base, 1);
}

void QUIDDIKEY_DeInit(QUIDDIKEY_Type *base)
{
    /* Disable QuiddiKey Interrupt */
    NVIC_DisableIRQ(FC_SOC_QUIDDIKEY_IRQn);
}

static inline int QUIDDIKEY_WaitInterleaveRequestReady(QUIDDIKEY_Type *base)
{
    int di_request = QUIDDIKEY_Di_Request(base);
    int do_request = QUIDDIKEY_Do_Request(base);

    while (!di_request && !do_request) {
        ITC_WaitEvent_NOIRQ(FC_SOC_QUIDDIKEY_IRQn);

        di_request = QUIDDIKEY_Di_Request(base);
        do_request = QUIDDIKEY_Do_Request(base);
    }

    int ret = 0;

    /* Write 1 to clear IRQ */
    if(di_request) {
        QUIDDIKEY_ClearInterruptStatus(QUIDDIKEY, uQUIDDIKEY_INT_DI_Request);
        ret |= 0x01;
    }

    if(do_request) {
        QUIDDIKEY_ClearInterruptStatus(QUIDDIKEY, uQUIDDIKEY_INT_DO_Request);
        ret |= 0x10;
    }

    return ret;
}

static inline void QUIDDIKEY_WaitDiRequestReady(QUIDDIKEY_Type *base)
{
    /*
     * Polling qk_di_request (or use IRQ)
     * Request for data in transfer via the QK_DIR register
     */

    /* while(!QUIDDIKEY_Di_Request(base)); */

    while (!QUIDDIKEY_Di_Request(base)) {
        ITC_WaitEvent_NOIRQ(FC_SOC_QUIDDIKEY_IRQn);
    }

    /* Write 1 to clear IRQ */
    QUIDDIKEY_ClearInterruptStatus(QUIDDIKEY, uQUIDDIKEY_INT_DI_Request);
}

static inline void QUIDDIKEY_WaitDoRequestReady(QUIDDIKEY_Type *base)
{
    /*
     * Polling qk_do_request (or use IRQ)
     * Request for data out transfer via the QK_DOR register
     */

    /* while(!QUIDDIKEY_Do_Request(base)); */

    while (!QUIDDIKEY_Do_Request(base)) {
        ITC_WaitEvent_NOIRQ(FC_SOC_QUIDDIKEY_IRQn);
    }

    /* Write 1 to clear IRQ */
    QUIDDIKEY_ClearInterruptStatus(QUIDDIKEY, uQUIDDIKEY_INT_DO_Request);
}

static inline void QUIDDIKEY_WaitBusyFinish(QUIDDIKEY_Type *base)
{
    /* while (QUIDDIKEY_Busy(base)); */

    while (QUIDDIKEY_Busy(base)) {
        ITC_WaitEvent_NOIRQ(FC_SOC_QUIDDIKEY_IRQn);
    }

    /* Write 1 to clear IRQ */
    QUIDDIKEY_ClearInterruptStatus(QUIDDIKEY, uQUIDDIKEY_INT_Busy);
}

static inline int QUIDDIKEY_EvaluateResult(QUIDDIKEY_Type *base)
{
    int sr = base->SR;

    /* Check qk_ok, qk_error, qk_zeroized flag*/
    if(READ_QUIDDIKEY_SR_OK(sr) &&
       !READ_QUIDDIKEY_SR_ERROR(sr))
        return 0;
    else
        return -1;
}

static inline int QUIDDIKEY_EvaluateZeroized(QUIDDIKEY_Type *base)
{
    /* Check qk_zeroized flag*/
    if(READ_QUIDDIKEY_SR_ZEROIZED(base->SR))
        return 0;
    else
        return -1;
}

int QUIDDIKEY_Enroll(QUIDDIKEY_Type *base, uint32_t *ac)
{
    /* Enable Busy and DO REQUEST IRQ */
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_Busy, 1);
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_DO_Request, 1);

    /* Allow Enroll command */
    uint32_t allow_enroll = READ_QUIDDIKEY_AR_ALLOW_ENROLL(base->AR);

    if (allow_enroll) {

        /* 1. Send Enroll command */
        base->CR = QUIDDIKEY_CR_ENROLL(1);

        /* Read Active code, 32 bits read */
        for (int i = 0; i <  (GAP_QUIDDIKEY_AC_BYTES / 4); i++) {

            QUIDDIKEY_WaitDoRequestReady(base);

            /* 2. Read QK_DOR register*/
            ac[i] = base->DOR;
        }
    } else {
        return -1;
    }

    /* 3. Wait Busy to zero */
    QUIDDIKEY_WaitBusyFinish(base);

    /* 4. Evalute result QK_SR */
    return QUIDDIKEY_EvaluateResult(base);
}

int QUIDDIKEY_Start(QUIDDIKEY_Type *base, uint32_t *ac)
{
    /* Enable Busy and DI REQUEST IRQ */
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_Busy, 1);
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_DI_Request, 1);

    /* Allow Start command */
    uint32_t allow_start = READ_QUIDDIKEY_AR_ALLOW_START(base->AR);

    if (allow_start) {

        /* 1. Send Start command */
        base->CR = QUIDDIKEY_CR_START(1);

        /* Write Active code twice, 32 bits write */
        for (int times = 0; times <  2; times++) {
            for (int i = 0; i <  (GAP_QUIDDIKEY_AC_BYTES / 4); i++) {

                QUIDDIKEY_WaitDiRequestReady(base);

                /* 2. Provide the context, Write QK_DIR register*/
                base->DIR = ac[i];
            }
        }
    } else {
        return -1;
    }

    /* Check Write */
    if(READ_QUIDDIKEY_SR_ERROR(base->SR))
        return -1;

    /* 3. Wait Busy to zero */
    QUIDDIKEY_WaitBusyFinish(base);

    /* 4. Evalute result QK_SR */
    return QUIDDIKEY_EvaluateResult(base);
}

int QUIDDIKEY_Stop(QUIDDIKEY_Type *base)
{
    /* Enable Busy IRQ */
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_Busy, 1);

    /* Allow Stop command */
    uint32_t allow_stop = READ_QUIDDIKEY_AR_ALLOW_STOP(base->AR);

    if (allow_stop) {

        /* 1. Send Stop command */
        base->CR = QUIDDIKEY_CR_STOP(1);
    } else {
        return -1;
    }

    /* 2. Wait Busy to zero */
    QUIDDIKEY_WaitBusyFinish(base);

    /* 3. Evalute result QK_SR */
    return QUIDDIKEY_EvaluateResult(base);
}

int QUIDDIKEY_GetKey(QUIDDIKEY_Type *base, uint32_t *key, quiddikey_context_t *context)
{
    /* Enable Busy, DI REQUEST IRQ and DO REQUEST IRQ */
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_Busy, 1);
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_DO_Request, 1);
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_DI_Request, 1);

    uint32_t key_context[4] = {
        QUIDDIKEY_CONTEXT_TYPE(0x0010) |
        QUIDDIKEY_CONTEXT_DATA_LENGTH(context->key_length),

        QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED(context->key_scope_started) |
        QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED(context->key_scope_enrolled),
        0,
        0
    };

    /* 1. Set QK_KEY_DEST.qk_dest_reg = 1 */
    base->KEY_DEST = QUIDDIKEY_KEY_DEST_REG(1);

    /* Allow Get Key command */
    uint32_t allow_get_key = READ_QUIDDIKEY_AR_ALLOW_GET_KEY(base->AR);

    if (allow_get_key) {
        /* 2. Send Get Key command */
        base->CR = QUIDDIKEY_CR_GET_KEY(1);

        for (int i = 0; i < 4; i++) {
            QUIDDIKEY_WaitDiRequestReady(base);
            /* 3. Write QK_DIR register*/
            base->DIR = key_context[i];
        }

        /* Check context set */
        if(READ_QUIDDIKEY_SR_ERROR(base->SR))
            return -1;

        /* Read key, 32 bits read */
        for (int i = 0; i < (context->key_length / 32); i++) {

            QUIDDIKEY_WaitDoRequestReady(base);

            /* 4. Read QK_DOR register*/
            key[i] = base->DOR;
        }
    } else {
        return -1;
    }

    /* 5. Wait Busy to zero */
    QUIDDIKEY_WaitBusyFinish(base);

    base->KEY_DEST = QUIDDIKEY_KEY_DEST_REG(0);

    /* 6. Evalute result QK_SR */
    return QUIDDIKEY_EvaluateResult(base);
}

int QUIDDIKEY_WrapGeneratedRandom(QUIDDIKEY_Type *base, uint32_t *key_code, quiddikey_context_t *context)
{
    assert(((context->key_length >= 64) && (context->key_length <=4096)));

    /* Enable Busy, DI REQUEST IRQ and DO REQUEST IRQ */
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_Busy, 1);
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_DO_Request, 1);
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_DI_Request, 1);

    uint32_t key_context[4] = {
        QUIDDIKEY_CONTEXT_TYPE(0x0010) |
        QUIDDIKEY_CONTEXT_DATA_LENGTH(context->key_length),

        QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED(context->key_scope_started) |
        QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED(context->key_scope_enrolled),
        0,
        0
    };

    /* Allow Generate Random command */
    uint32_t allow_wrap_generate_random = READ_QUIDDIKEY_AR_ALLOW_GENERATED_RANDOM(base->AR);

    if (allow_wrap_generate_random) {

        /* 1. Send Generate Random command */
        base->CR = QUIDDIKEY_CR_WRAP_GENERATED_RANDOM(1);

        QUIDDIKEY_WaitDiRequestReady(base);

        for (int i = 0; i < 4; i++) {
            QUIDDIKEY_WaitDiRequestReady(base);
            /* 2. Provide the context, Write QK_DIR register*/
            base->DIR = key_context[i];
        }

        /* Check context set */
        if(READ_QUIDDIKEY_SR_ERROR(base->SR))
            return -1;

        /* Read random data code, 32 bits read */
        for (int i = 0; i < (GAP_QUIDDIKEY_KEY_CODE_LENGTH_BYTE(context->key_length) / 4); i++) {

            QUIDDIKEY_WaitDoRequestReady(base);

            /* 3. Read QK_DOR register*/
            key_code[i] = base->DOR;
        }
    } else {
        return -1;
    }

    /* 4. Wait Busy to zero */
    QUIDDIKEY_WaitBusyFinish(base);

    /* 5. Evalute result QK_SR */
    return QUIDDIKEY_EvaluateResult(base);
}

int QUIDDIKEY_Wrap(QUIDDIKEY_Type *base, uint32_t *key, uint32_t *key_code, quiddikey_context_t *context)
{
    /* Enable Busy, DI REQUEST IRQ and DO REQUEST IRQ */
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_Busy, 1);
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_DO_Request, 1);
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_DI_Request, 1);

    uint32_t key_context[4] = {
        QUIDDIKEY_CONTEXT_TYPE(0x0010) |
        QUIDDIKEY_CONTEXT_DATA_LENGTH(context->key_length),

        QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED(context->key_scope_started) |
        QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED(context->key_scope_enrolled),
        0,
        0
    };

    /* Allow Wrap command */
    uint32_t allow_wrap = READ_QUIDDIKEY_AR_ALLOW_WRAP(base->AR);

    if (allow_wrap) {
        /* 1. Send Wrap command */
        base->CR = QUIDDIKEY_CR_WRAP(1);

        for (int i = 0; i < 4; i++) {
            QUIDDIKEY_WaitDiRequestReady(base);
            /* 2. Write QK_DIR register*/
            base->DIR = key_context[i];
        }

        /* Check context set */
        if(READ_QUIDDIKEY_SR_ERROR(base->SR))
            return -1;

        int key_32      =  (context->key_length / 32);
        int key_code_32 = (GAP_QUIDDIKEY_KEY_CODE_LENGTH_BYTE(context->key_length) / 4);

        int key_32_length      = 0;
        int key_code_32_length = 0;

        /* 3. Interleaves Write and Read */
        while ((key_32_length < key_32) || (key_code_32_length < key_code_32)) {
            int ret = QUIDDIKEY_WaitInterleaveRequestReady(base);

            if (ret & 0x01) {
                /* 3.1 Write QK_DIR register with Key */
                base->DIR = key[key_32_length++];
            }

            if (ret & 0x10) {
                /* 3.2 Read QK_DOR register*/
                key_code[key_code_32_length++] = base->DOR;
            }
        }
    } else {
        return -1;
    }

    /* Check interleaves */
    if(READ_QUIDDIKEY_SR_ERROR(base->SR))
        return -1;

    /* 4. Wait Busy to zero */
    QUIDDIKEY_WaitBusyFinish(base);

    /* 5. Evalute result QK_SR */
    return QUIDDIKEY_EvaluateResult(base);
}

int QUIDDIKEY_UnWrap(QUIDDIKEY_Type *base, uint32_t *key_code, uint32_t *key, quiddikey_context_t *context)
{
    /* Enable Busy, DI REQUEST IRQ and DO REQUEST IRQ */
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_Busy, 1);
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_DO_Request, 1);
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_DI_Request, 1);

    uint32_t key_context[4] = {
        QUIDDIKEY_CONTEXT_TYPE(0x0010) |
        QUIDDIKEY_CONTEXT_DATA_LENGTH(context->key_length),

        QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED(context->key_scope_started) |
        QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED(context->key_scope_enrolled),
        0,
        0
    };

    /* 1. Set QK_KEY_DEST.qk_dest_reg = 1 */
    base->KEY_DEST = QUIDDIKEY_KEY_DEST_REG(1);

    /* Allow Unwrap command */
    uint32_t allow_unwrap = READ_QUIDDIKEY_AR_ALLOW_UNWRAP(base->AR);

    if (allow_unwrap) {
        /* 2. Send Unwrap command */
        base->CR = QUIDDIKEY_CR_UNWRAP(1);

        /*
         * No nedd to to provide the context, provide the key code,
         * Document QUIDDIKEY 3.3, Doc version 1.0, Page 36 ERROR
         */
        int key_32      =  (context->key_length / 32);
        int key_code_32 = (GAP_QUIDDIKEY_KEY_CODE_LENGTH_BYTE(context->key_length) / 4);

        int key_32_length      = 0;
        int key_code_32_length = 0;

        /* 3. Interleaves Write and Read */
        while ((key_32_length < key_32) || (key_code_32_length < key_code_32)) {
            int ret = QUIDDIKEY_WaitInterleaveRequestReady(base);

            if (ret & 0x01) {
                /* 3.1 Write QK_DIR register with Key Code */
                base->DIR = key_code[key_code_32_length++];
            }

            if (ret & 0x10) {
                /* 3.2 Read QK_DOR register*/
                key[key_32_length++] = base->DOR;
            }
        }
    } else {
        return -1;
    }

    /* Check interleaves */
    if(READ_QUIDDIKEY_SR_ERROR(base->SR))
        return -1;

    /* 4. Wait Busy to zero */
    QUIDDIKEY_WaitBusyFinish(base);

    base->KEY_DEST = QUIDDIKEY_KEY_DEST_REG(0);

    /* 5. Evalute result QK_SR */
    return QUIDDIKEY_EvaluateResult(base);
}

int QUIDDIKEY_GenerateRandom(QUIDDIKEY_Type *base, uint32_t *random_data, quiddikey_context_t *context)
{
    /* Enable Busy, DI REQUEST IRQ and DO REQUEST IRQ */
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_Busy, 1);
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_DO_Request, 1);
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_DI_Request, 1);

    uint32_t data_context =
        QUIDDIKEY_CONTEXT_TYPE(0x0000) |
        QUIDDIKEY_CONTEXT_DATA_LENGTH(context->data_length);


    /* Allow Generate Random command */
    uint32_t allow_generate_random = READ_QUIDDIKEY_AR_ALLOW_GENERATED_RANDOM(base->AR);

    if (allow_generate_random) {

        /* 1. Send Generate Random command */
        base->CR = QUIDDIKEY_CR_GENERATED_RANDOM(1);

        QUIDDIKEY_WaitDiRequestReady(base);
        /* 2. Provide the context, Write QK_DIR register*/
        base->DIR = data_context;

        /* Check context set */
        if(READ_QUIDDIKEY_SR_ERROR(base->SR))
            return -1;

        /* Read random data code, 32 bits read */
        for (int i = 0; i < (context->data_length / 32); i++) {

            QUIDDIKEY_WaitDoRequestReady(base);

            /* 3. Read QK_DOR register*/
            random_data[i] = base->DOR;
        }
    } else {
        return -1;
    }

    /* 4. Wait Busy to zero */
    QUIDDIKEY_WaitBusyFinish(base);

    /* 5. Evalute result QK_SR */
    return QUIDDIKEY_EvaluateResult(base);
}

int QUIDDIKEY_Zeroize(QUIDDIKEY_Type *base)
{
    /* Enable Busy */
    QUIDDIKEY_SetInterruptMask(base, uQUIDDIKEY_INT_Busy, 1);

    /* 1. Send Zeroize command */
    base->CR = QUIDDIKEY_CR_ZEROIZE(1);

    /* 2. Wait Busy to zero */
    QUIDDIKEY_WaitBusyFinish(base);

    /* 3. Evalute result QK_Zeroized */
    return QUIDDIKEY_EvaluateZeroized(base);
}

void QUIDDIKEY_IRQHandlerBind(QUIDDIKEY_Type *base, uint32_t irq)
{
    quiddikey_irq_handler = (irq_handler)irq;
}

void QUIDDIKEY_IRQHandler(void) {

    if(quiddikey_irq_handler) {
        quiddikey_irq_handler();
    } else {
        int isr = QUIDDIKEY_GetInterruptStatus(QUIDDIKEY);

        if (READ_QUIDDIKEY_ISR_INT_BUSY(isr)) {
            /* Write 1 to clear IRQ */
            QUIDDIKEY_ClearInterruptStatus(QUIDDIKEY, uQUIDDIKEY_INT_Busy);
        } else if (READ_QUIDDIKEY_ISR_INT_OK(isr)) {
            /* Write 1 to clear */
            QUIDDIKEY_ClearInterruptStatus(QUIDDIKEY, uQUIDDIKEY_INT_OK);

        } else if (READ_QUIDDIKEY_ISR_INT_ERROR(isr)) {
            /* Write 1 to clear */
            QUIDDIKEY_ClearInterruptStatus(QUIDDIKEY, uQUIDDIKEY_INT_Error);

        } else if (READ_QUIDDIKEY_ISR_INT_ZEROIZED(isr)) {
            /* Write 1 to clear */
            QUIDDIKEY_ClearInterruptStatus(QUIDDIKEY, uQUIDDIKEY_INT_Zeroized);

        } else if (READ_QUIDDIKEY_ISR_INT_REJECTED(isr)) {
            /* Write 1 to clear */
            QUIDDIKEY_ClearInterruptStatus(QUIDDIKEY, uQUIDDIKEY_INT_Reject);

        } else if (READ_QUIDDIKEY_ISR_INT_DI_REQUEST(isr)) {
            /* Write 1 to clear */
            QUIDDIKEY_ClearInterruptStatus(QUIDDIKEY, uQUIDDIKEY_INT_DI_Request);

        } else if (READ_QUIDDIKEY_ISR_INT_DO_REQUEST(isr)) {
        }
    }

}
