#pragma once

//#include "archi/chips/gap9_v2/quiddikey/quiddikey.h"

#include "archi/chips/gap9_v2/quiddikey/quiddikey.h"

#define quiddikey(device_id) ((quiddikey_t*)ARCHI_QUIDDIKEY_ADDR)
#define apb_soc_ctrl ((apb_soc_ctrl_t*)ARCHI_APB_SOC_CTRL_ADDR)

/*
 * HAL part
 */

/*! @name HW_RUC0 - QUIDDIKEY  register */
/*! @name Context Filed - 0 */
#define QUIDDIKEY_CONTEXT_DATA_LENGTH_MASK        (0x1FFFU)
#define QUIDDIKEY_CONTEXT_DATA_LENGTH_SHIFT       (0U)
#define QUIDDIKEY_CONTEXT_DATA_LENGTH(x)          (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_CONTEXT_DATA_LENGTH_SHIFT */)) & QUIDDIKEY_CONTEXT_DATA_LENGTH_MASK)
#define READ_QUIDDIKEY_CONTEXT_DATA_LENGTH(x)     (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_CONTEXT_DATA_LENGTH_MASK)) /* >> QUIDDIKEY_CONTEXT_DATA_LENGTH_SHIFT */)
#define QUIDDIKEY_CONTEXT_KEY_LENGTH_MASK         (0x1FFFU)
#define QUIDDIKEY_CONTEXT_KEY_LENGTH_SHIFT        (0U)
#define QUIDDIKEY_CONTEXT_KEY_LENGTH(x)           (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_CONTEXT_KEY_LENGTH_SHIFT */)) & QUIDDIKEY_CONTEXT_KEY_LENGTH_MASK)
#define READ_QUIDDIKEY_CONTEXT_KEY_LENGTH(x)      (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_CONTEXT_KEY_LENGTH_MASK)) /* >> QUIDDIKEY_CONTEXT_KEY_LENGTH_SHIFT */)
#define QUIDDIKEY_CONTEXT_TYPE_MASK               (0xFFFF0000U)
#define QUIDDIKEY_CONTEXT_TYPE_SHIFT              (16U)
#define QUIDDIKEY_CONTEXT_TYPE(x)                 (((uint32_t)(((uint32_t)(x))  << QUIDDIKEY_CONTEXT_TYPE_SHIFT)) & QUIDDIKEY_CONTEXT_TYPE_MASK)
#define READ_QUIDDIKEY_CONTEXT_TYPE(x)            (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_CONTEXT_TYPE_MASK))>> QUIDDIKEY_CONTEXT_TYPE_SHIFT)


/*! @name HW_RUC1 - QUIDDIKEY  register */
/*! @name Context Filed - 1 */
#define QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED_MASK        (0xFF)
#define QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED_SHIFT       (0U)
#define QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED(x)          (((uint32_t)(((uint32_t)(x)) /* << QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED_SHIFT */)) & QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED_MASK)
#define READ_QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED(x)     (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED_MASK)) /* >> QUIDDIKEY_CONTEXT_KEY_SCOPE_ENROLLED_SHIFT*/)
#define QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED_MASK         (0xFF00)
#define QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED_SHIFT        (8U)
#define QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED(x)           (((uint32_t)(((uint32_t)(x))  << QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED_SHIFT)) & QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED_MASK)
#define READ_QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED(x)      (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED_MASK))>> QUIDDIKEY_CONTEXT_KEY_SCOPE_STARTED_SHIFT)
#define QUIDDIKEY_CONTEXT_KEY_TYPE_MASK                  (0xFFFF0000U)
#define QUIDDIKEY_CONTEXT_KEY_TYPE_SHIFT                 (16U)
#define QUIDDIKEY_CONTEXT_KEY_TYPE(x)                    (((uint32_t)(((uint32_t)(0x0000))  << QUIDDIKEY_CONTEXT_KEY_TYPE_SHIFT)) & QUIDDIKEY_CONTEXT_KEY_TYPE_MASK)
#define READ_QUIDDIKEY_CONTEXT_KEY_TYPE(x)               (((uint32_t)(((uint32_t)(x)) & QUIDDIKEY_CONTEXT_KEY_TYPE_MASK))>> QUIDDIKEY_CONTEXT_KEY_TYPE_SHIFT)



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

/*! @brief QUIDDIKEY Active code length 996 bytes */
#define GAP_QUIDDIKEY_AC_BYTES 996


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



static inline uint32_t hal_quiddikey_dor_get(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    return base->dor;
}

static inline void hal_quiddikey_dir_set(uint32_t device_id, uint32_t value)
{
    quiddikey_t *base = quiddikey(device_id);
    base->dir = value;
}

static inline int hal_quiddikey_enroll_allowed(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int ar = base->ar;
    return QUIDDIKEY_AR_ALLOW_ENROLL_GET(ar);
}

static inline int hal_quiddikey_start_allowed(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int ar = base->ar;
    return QUIDDIKEY_AR_ALLOW_START_GET(ar);
}

static inline int hal_quiddikey_stop_allowed(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int ar = base->ar;
    return QUIDDIKEY_AR_ALLOW_STOP_GET(ar);
}

static inline int hal_quiddikey_get_key_allowed(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int ar = base->ar;
    return QUIDDIKEY_AR_ALLOW_GET_KEY_GET(ar);
}

static inline int hal_quiddikey_wrap_random_allowed(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int ar = base->ar;
    return QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_GET(ar);
}

static inline int hal_quiddikey_wrap_key_allowed(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int ar = base->ar;
    return QUIDDIKEY_AR_ALLOW_WRAP_GET(ar);
}

static inline int hal_quiddikey_unwrap_allowed(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int ar = base->ar;
    return QUIDDIKEY_AR_ALLOW_UNWRAP_GET(ar);
}

static inline int hal_quiddikey_gen_random_allowed(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int ar = base->ar;
    return QUIDDIKEY_AR_ALLOW_GEN_RND_GET(ar);
}

static inline void hal_quiddikey_enroll_cmd_send(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    base->cr = QUIDDIKEY_CR_ENROLL(1);
}

static inline void hal_quiddikey_start_cmd_send(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    base->cr = QUIDDIKEY_CR_START(1);
}

static inline void hal_quiddikey_stop_cmd_send(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    base->cr = QUIDDIKEY_CR_STOP(1);
}

static inline void hal_quiddikey_get_key_cmd_send(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    base->cr = QUIDDIKEY_CR_GET_KEY(1);
}

static inline void hal_quiddikey_wrap_random_cmd_send(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    base->cr = QUIDDIKEY_CR_WRAP_GENERATED_RANDOM(1);
}

static inline void hal_quiddikey_wrap_cmd_send(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    base->cr = QUIDDIKEY_CR_WRAP(1);
}

static inline void hal_quiddikey_unwrap_cmd_send(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    base->cr = QUIDDIKEY_CR_UNWRAP(1);
}

static inline void hal_quiddikey_gen_random_cmd_send(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    base->cr = QUIDDIKEY_CR_GENERATE_RANDOM(1);
}

static inline void hal_quiddikey_zeroize_cmd_send(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    base->cr = QUIDDIKEY_CR_ZEROIZE(1);
}

static inline void hal_quiddikey_data_dest_set(uint32_t device_id, uint32_t dest,
        uint32_t value)
{
    quiddikey_t *base = quiddikey(device_id);
    uint32_t data_dest = (dest == uQUIDDIKEY_DOR) ? 
        QUIDDIKEY_DATA_DEST_DEST_DOR(value)
        : QUIDDIKEY_DATA_DEST_DEST_SO(value);
    base->data_dest = data_dest;
}

static inline int hal_quiddikey_is_lock(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int sr = base->sr;

    /* Check qk_ok, qk_error, qk_zeroized flag*/
    if(!QUIDDIKEY_SR_OK_GET(sr) &&
       QUIDDIKEY_SR_ERROR_GET(sr) &&
       QUIDDIKEY_SR_ZEROIZED_GET(sr))
        return 0;
    else
        return -1;
}

static inline int hal_quiddikey_is_zeroized(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int sr = base->sr;

    /* Check qk_ok, qk_error, qk_zeroized flag*/
    if(QUIDDIKEY_SR_OK_GET(sr) &&
       !QUIDDIKEY_SR_ERROR_GET(sr) &&
       QUIDDIKEY_SR_ZEROIZED_GET(sr))
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
static inline int hal_quiddikey_is_busy(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int sr = base->sr;
    return QUIDDIKEY_SR_BUSY_GET(sr);
}

/*!
 * @brief Indicate QuiddiKey deive ok status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY ok 1 or 0 for not ok
 */
static inline int hal_quiddikey_is_ok(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int sr = base->sr;
    return QUIDDIKEY_SR_OK_GET(sr);
}

/*!
 * @brief Indicate QuiddiKey deive error status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY error 1 or 0 for not error
 */
static inline int hal_quiddikey_is_error(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int sr = base->sr;
    return QUIDDIKEY_SR_ERROR_GET(sr);
}

/*!
 * @brief Indicate QuiddiKey deive zeroized status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY zeroized 1 or 0 for not zeroized
 */
static inline int hal_quiddikey_zeroized(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int sr = base->sr;
    return QUIDDIKEY_SR_ZEROIZED_GET(sr);
}

/*!
 * @brief Indicate QuiddiKey deive rejected status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY rejected 1 or 0 for not rejected
 */
static inline int hal_quiddikey_rejected(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int sr = base->sr;
    return QUIDDIKEY_SR_REJECTED_GET(sr);
}

/*!
 * @brief Indicate QuiddiKey deive di_request status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY di_request 1 or 0 for not di_request
 */
static inline int hal_quiddikey_di_request(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int sr = base->sr;
    return QUIDDIKEY_SR_DI_REQUEST_GET(sr);
}

/*!
 * @brief Indicate QuiddiKey deive do_request status.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval QUIDDIKEY do_request 1 or 0 for not do_request
 */
static inline int hal_quiddikey_do_request(uint32_t device)
{
    quiddikey_t *base = quiddikey(device_id);
    int sr = base->sr;
    return QUIDDIKEY_SR_DO_REQUEST_GET(sr);
}

/*!
 * @brief Set QuiddiKey data endianness.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @param endianness   Data endianness.
 */
static inline void hal_quiddikey_set_endianness(uint32_t device_id,
        quiddikey_endianness_t endianness)
{
    quiddikey_t *base = quiddikey(device_id);
    base->misc = QUIDDIKEY_MISC_ENDIANNESS_SET(base->misc,
            QUIDDIKEY_MISC_ENDIANNESS(endianness));
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
static inline void hal_quiddikey_interrupt_enable_set(uint32_t device_id, uint32_t value)
{
    quiddikey_t *base = quiddikey(device_id);
    base->ier = QUIDDIKEY_IER_INT_EN_SET(QUIDDIKEY_IER_INT_EN_GET(base->ier),value);
}

/*!
 * @brief Get the quiddikey interrupt mask register.
 *
 * Individual QuiddiKey interrupts can be enabled or disabled with the Interrupt Mask register.
 *
 * @param base   QUIDDIKEY peripheral base pointer.
 * @retval The QUIDDIKEY Interrupt mask register
 */
static inline uint32_t hal_quiddikey_get_interrupt_mask(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    return base->imr;
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
static inline void hal_quiddikey_set_interrupt_mask(uint32_t device_id, uint32_t irq, uint32_t value)
{
    quiddikey_t *base = quiddikey(device_id);
    if(value)
        base->imr |= (1 << irq);
    else
        base->imr &= ~(1 << irq);
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
static inline uint32_t hal_quiddikey_get_interrupt_status(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    return base->isr;
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
static inline void hal_quiddikey_clear_interrupt_status(uint32_t device_id, quiddikey_interrupt_type_t irq )
{
    quiddikey_t *base = quiddikey(device_id);
    base->isr = (1 << irq);
}

/*******************************************************************************
 * Helpers
 * ****************************************************************************/

static inline int hal_quiddikey_wait_do_request_ready(uint32_t device_id)
{

    while (!hal_quiddikey_do_request(device_id)) {
        hal_itc_wait_for_event_noirq(1<<ARCHI_FC_EVT_QUIDDIKEY_IRQ);
    }

    int ret = 0;

    /* Write 1 to clear IRQ */
    hal_quiddikey_clear_interrupt_status(device_id, uQUIDDIKEY_INT_DO_Request);
    ret |= 0x10;

    return ret;
}

static inline int hal_quiddikey_wait_di_request_ready(uint32_t device_id)
{
    while (!hal_quiddikey_di_request(device_id)) {
        hal_itc_wait_for_event_noirq(1<<ARCHI_FC_EVT_QUIDDIKEY_IRQ);
    }

    int ret = 0;
    hal_quiddikey_clear_interrupt_status(device_id, uQUIDDIKEY_INT_DI_Request);
    ret |= 0x01;
    return ret;
}

static inline int hal_quiddikey_wait_interleaved_request_ready(uint32_t device_id)
{
    int di_request = hal_quiddikey_di_request(device_id);
    int do_request = hal_quiddikey_do_request(device_id);

    while (!di_request && !do_request) {
        hal_itc_wait_for_event_noirq(1<<ARCHI_FC_EVT_QUIDDIKEY_IRQ);

        di_request = hal_quiddikey_di_request(device_id);
        do_request = hal_quiddikey_do_request(device_id);
    }

    int ret = 0;

    /* Write 1 to clear IRQ */
    if(di_request) {
        hal_quiddikey_clear_interrupt_status(device_id, uQUIDDIKEY_INT_DI_Request);
        ret |= 0x01;
    }

    if(do_request) {
        hal_quiddikey_clear_interrupt_status(device_id, uQUIDDIKEY_INT_DO_Request);
        ret |= 0x10;
    }

    return ret;
}

static inline void hal_quiddikey_wait_busy(uint32_t device_id)
{

    while (hal_quiddikey_is_busy(device_id))
    {
        hal_itc_wait_for_event_noirq(1<<ARCHI_FC_EVT_QUIDDIKEY_IRQ);
    }

    /* Write 1 to clear IRQ */
    hal_quiddikey_clear_interrupt_status(device_id, uQUIDDIKEY_INT_Busy);
}

static inline void hal_quiddikey_wait_di(uint32_t device_id)
{
        while (!hal_quiddikey_di_request(device_id)) {
            hal_itc_wait_for_event_noirq(1<<ARCHI_FC_EVT_QUIDDIKEY_IRQ);
        }

        /* Write 1 to clear IRQ */
        hal_quiddikey_clear_interrupt_status(device_id, uQUIDDIKEY_INT_DI_Request);
}



static inline int hal_quiddikey_evaluate_result(uint32_t device_id)
{
    quiddikey_t *base = quiddikey(device_id);
    int sr = base->sr;

    /* Check qk_ok, qk_error, qk_zeroized flag*/
    if(QUIDDIKEY_SR_OK_GET(sr) && !QUIDDIKEY_SR_ERROR_GET(sr))
        return 0;
    else
        return -1;
}

/*
 * Driver part
 */

int quiddikey_init(uint32_t *ac)
{
    /* De-Clock Gating */
    apb_soc_ctrl->clk_en_quiddikey |= 0x1 << 0;

    /* De-Reset IP */
    apb_soc_ctrl->clk_en_quiddikey |= 0x1 << 1;



    /* Wait initialized - with timeout */
    while (!hal_quiddikey_start_allowed(0))
    {
    }

    /* Use little endianness*/
    hal_quiddikey_set_endianness(0, uQUIDDIKEY_BigEndianness);

    /* Enable Quiddikey global IRQ */
    hal_quiddikey_interrupt_enable_set(0, 1);

    /* Enable Busy and DI REQUEST IRQ */
    hal_quiddikey_set_interrupt_mask(0, uQUIDDIKEY_INT_Busy, 1);
    hal_quiddikey_set_interrupt_mask(0, uQUIDDIKEY_INT_DI_Request, 1);
    /* 1. Send Start command */
    hal_quiddikey_start_cmd_send(0);

    /* Write Active code twice, 32 bits write */
    for (int times = 0; times <  2; times++) {
        for (int i = 0; i <  (GAP_QUIDDIKEY_AC_BYTES / 4); i++) {
            hal_quiddikey_wait_di_request_ready(0);

            /* 2. Provide the context, Write QK_DIR register*/
            hal_quiddikey_dir_set(0,ac[i]);
        }
    }

    /* Check Write */
    if(hal_quiddikey_is_error(0))
        return -2;

    /* 3. Wait Busy to zero */
    hal_quiddikey_wait_busy(0);

    /* 4. Evalute result QK_SR */
    return hal_quiddikey_evaluate_result(0);
}

int quiddikey_unwrap(uint32_t *key_code, uint32_t *key, uint16_t key_length, uint16_t key_code_length,
        quiddikey_destination_t dest)
{
    /* Enable Busy, DI REQUEST IRQ and DO REQUEST IRQ */
    hal_quiddikey_set_interrupt_mask(0, uQUIDDIKEY_INT_Busy, 1);
    hal_quiddikey_set_interrupt_mask(0, uQUIDDIKEY_INT_DO_Request, 1);
    hal_quiddikey_set_interrupt_mask(0, uQUIDDIKEY_INT_DI_Request, 1);

    /* 1. Set QK_DATA_DEST.qk_dest_dor = 1 */
    hal_quiddikey_data_dest_set(0, dest, 1);

    /* Allow Unwrap command */
    uint32_t allow_unwrap = hal_quiddikey_unwrap_allowed(0);

    if (allow_unwrap) {
        /* 2. Send Unwrap command */
        hal_quiddikey_unwrap_cmd_send(0);

        /*
         *          * No nedd to to provide the context, provide the key code,
         *                   * Document QUIDDIKEY 3.3, Doc version 1.0, Page 36 ERROR
         *                            */
        int key_32      = (key_length / 32);
        int key_code_32 = (key_code_length / 4);

        int key_32_length      = 0;
        int key_code_32_length = 0;

        /* 3. Interleaves Write and Read */
        while ((key_32_length < key_32) || (key_code_32_length < key_code_32)) {
            if(hal_quiddikey_is_error(0)) // always check if there is an error
                return -1;
            int ret = hal_quiddikey_wait_interleaved_request_ready(0);

            if (ret & 0x01) {
                /* 3.1 Write QK_DIR register with Key Code */
                hal_quiddikey_dir_set(0,key_code[key_code_32_length++]);
            }

            if (dest == uQUIDDIKEY_DOR) {
                if (ret & 0x10) {
                    /* 3.2 Read QK_DOR register*/
                    key[key_32_length++] = hal_quiddikey_dor_get(0);
                }
            } else {
                key_32_length = key_32;
            }
        }
    } else {
        return -1;
    }

    /* Check interleaves */
    if(hal_quiddikey_is_error(0))
        return -1;

    /* 4. Wait Busy to zero */
    hal_quiddikey_wait_busy(0);

    /* 5. Evalute result QK_SR */
    return hal_quiddikey_evaluate_result(0);
}

int quiddikey_deinit()
{
    /* Reset IP */
    apb_soc_ctrl->clk_en_quiddikey &= ~(0x1 << 1);
    /* Clock Gating */
    apb_soc_ctrl->clk_en_quiddikey &= ~(0x1 << 0);
}
