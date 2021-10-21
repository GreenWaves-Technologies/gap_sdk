#pragma once

#include "pmsis/pmsis_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PI_INLINE_QK0 static inline

#ifndef PI_INLINE_QK0
#define PI_INLINE_QK0
#endif

typedef struct pi_quiddikey_conf_s {
    uint32_t id;
    uint32_t enroll;
    uint8_t *ac;
} pi_quiddikey_conf_t;

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
    uint8_t  qk_so_index;  /*!< User context for the key derivation; available bits are
                             restricted by the qk_restrict_user_context_0 bus
                             bit 3..0: Index; passed to qk_so_index when key is
                             output to the SO interface */

} pi_quiddikey_context_t;

/**
 * Create a quididkey context for data operations ( random generation )
 */
PI_INLINE_QK0 void pi_quiddikey_set_data_context(pi_quiddikey_context_t *ctxt,
        quiddikey_key_scope_type_t scope, uint32_t length, uint8_t dor_allowed, 
        uint8_t so_allowed, uint8_t so_ctxt);

/**
 * Create a quididkey context for key operations ( get_key, wrap/unwrap )
 */
PI_INLINE_QK0 void pi_quiddikey_set_key_context(pi_quiddikey_context_t *ctxt,
        quiddikey_key_scope_type_t scope, uint32_t length, uint8_t dor_allowed, 
        uint8_t so_allowed, uint8_t so_ctxt);

PI_INLINE_QK0 int pi_quiddikey_get_key(pi_device_t *device,  uint32_t *key,
        pi_quiddikey_context_t *context);

PI_INLINE_QK0 int pi_quiddikey_wrap(pi_device_t *device, uint32_t *key,
        uint32_t *key_code, pi_quiddikey_context_t *context);

PI_INLINE_QK0 int pi_quiddikey_unwrap(pi_device_t *device, uint32_t *key_code,
        uint32_t *key, uint16_t key_length);

PI_INLINE_QK0 int pi_quiddikey_generate_random(pi_device_t *device,
        uint32_t *random_data, pi_quiddikey_context_t *context);

PI_INLINE_QK0 int pi_quiddikey_open(pi_device_t *device);

PI_INLINE_QK0 void pi_quiddikey_close(pi_device_t *device);

#ifdef __cplusplus
}
#endif
