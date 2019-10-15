/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <arm_cmse.h>
#include "tfm_secure_api.h"
#include "tfm_nspm.h"
#include "secure_utilities.h"
#include "secure_fw/spm/spm_api.h"
#include "region_defs.h"
#include "tfm_api.h"

#define EXC_RETURN_SECURE_FUNCTION 0xFFFFFFFD

#ifndef TFM_LVL
#error TFM_LVL is not defined!
#endif

/* Macros to pick linker symbols and allow references to sections */
#define REGION(a, b, c) a##b##c
#define REGION_NAME(a, b, c) REGION(a, b, c)
#define REGION_DECLARE(a, b, c) extern uint32_t REGION_NAME(a, b, c)

REGION_DECLARE(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Limit);

#if TFM_LVL == 1
REGION_DECLARE(Image$$, TFM_SECURE_STACK, $$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SECURE_STACK, $$ZI$$Limit);
#endif

/* This is the "Big Lock" on the secure side, to guarantee single entry
 * to SPE
 */
int32_t tfm_secure_lock;
static int32_t tfm_secure_api_initializing = 1;

static int32_t is_iovec_api_call(void)
{
    uint32_t current_partition_idx =
            tfm_spm_partition_get_running_partition_idx();
    const struct spm_partition_runtime_data_t *curr_part_data =
            tfm_spm_partition_get_runtime_data(current_partition_idx);
    return curr_part_data->iovec_api;
}

static uint32_t *prepare_partition_ctx(
            const struct tfm_exc_stack_t *svc_ctx,
            const struct tfm_sfn_req_s *desc_ptr,
            uint32_t *dst)
{
    /* XPSR  = as was when called, but make sure it's thread mode */
    *(--dst) = svc_ctx->XPSR & 0xFFFFFE00U;
    /* ReturnAddress = resume veneer in new context */
    *(--dst) = svc_ctx->RetAddr;
    /* LR = sfn address */
    *(--dst) = (uint32_t)desc_ptr->sfn;
    /* R12 = don't care */
    *(--dst) = 0;

    /* R0-R3 = sfn arguments */
    int32_t i = 4;

    while (i > 0) {
        i--;
        *(--dst) = (uint32_t)desc_ptr->args[i];
    }
    return dst;
}

static uint32_t *prepare_partition_iovec_ctx(
                             const struct tfm_exc_stack_t *svc_ctx,
                             const struct tfm_sfn_req_s *desc_ptr,
                             const struct iovec_args_t *iovec_args,
                             uint32_t *dst)
{
    /* XPSR  = as was when called, but make sure it's thread mode */
    *(--dst) = svc_ctx->XPSR & 0xFFFFFE00U;
    /* ReturnAddress = resume veneer in new context */
    *(--dst) = svc_ctx->RetAddr;
    /* LR = sfn address */
    *(--dst) = (uint32_t)desc_ptr->sfn;
    /* R12 = don't care */
    *(--dst) = 0U;

    /* R0-R3 = sfn arguments */
    *(--dst) = iovec_args->out_len;
    *(--dst) = (uint32_t)iovec_args->out_vec;
    *(--dst) = iovec_args->in_len;
    *(--dst) = (uint32_t)iovec_args->in_vec;

    return dst;
}

static void restore_caller_ctx(
            const struct tfm_exc_stack_t *svc_ctx,
            struct tfm_exc_stack_t *target_ctx)
{
    /* ReturnAddress = resume veneer after second SVC */
    target_ctx->RetAddr = svc_ctx->RetAddr;

    /* R0 = function return value */
    target_ctx->R0 = svc_ctx->R0;

    return;
}

/**
 * \brief Check whether a memory range is inside a memory region.
 *
 * \param[in] p             The start address of the range to check
 * \param[in] s             The size of the range to check
 * \param[in] region_start  The start address of the region, which should
 *                          contain the range
 * \param[in] region_len    The size of the region, which should contain the
 *                          range
 *
 * \return 1 if the region contains the range, 0 otherwise.
 */
static int32_t check_address_range(const void *p, size_t s,
                                   uintptr_t region_start, uint32_t region_len)
{
    int32_t range_in_region = 0;

    /* Check for overflow in the range parameters */
    if ((uintptr_t)p > UINTPTR_MAX-s) {
        return 0;
    }

    /* We trust the region parameters, and don't check for overflow */

    /* Calculate the result */
    range_in_region = ((uintptr_t)p >= region_start) &&
                      ((uintptr_t)p+s <= region_start+region_len);

    return range_in_region;
}

/**
 * \brief Check whether the current partition has access to a memory range
 *
 * This function assumes, that the current MPU configuration is set for the
 * partition to be checked. The flags should contain information of the
 * execution mode of the partition code (priv/unpriv), and access type
 * (read/write) as specified in "ARMv8-M Security Extensions: Requirements on
 * Development Tools" chapter "Address range check intrinsic"
 *
 * \param[in] p      The start address of the range to check
 * \param[in] s      The size of the range to check
 * \param[in] flags  The flags to pass to the cmse_check_address_range func
 *
 * \return 1 if the partition has access to the memory range, 0 otherwise.
 */
static int32_t has_access_to_region(const void *p, size_t s, int flags)
{
    int32_t range_access_allowed_by_mpu;

    uint32_t scratch_base =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Base);
    uint32_t scratch_limit =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Limit);

    /* Use the TT instruction to check access to the partition's regions*/
    range_access_allowed_by_mpu =
                          cmse_check_address_range((void *)p, s, flags) != NULL;

    if (range_access_allowed_by_mpu) {
        return 1;
    }

    /* If the check for the current MPU settings fails, check for the share
     * region, only if the partition is secure
     */
    if ((flags & CMSE_NONSECURE) == 0) {
        if (check_address_range(p, s, scratch_base,
                                scratch_limit+1-scratch_base)) {
            return 1;
        }
    }

    /* If all else fails, check whether the region is in the non-secure
     * memory
     */
    return
      check_address_range(p, s, NS_CODE_START, NS_CODE_LIMIT+1-NS_CODE_START) ||
      check_address_range(p, s, NS_DATA_START, NS_DATA_LIMIT+1-NS_DATA_START);
}

int32_t tfm_core_has_read_access_to_region(const void *p, size_t s,
                                           uint32_t ns_caller)
{
    int flags = CMSE_MPU_UNPRIV|CMSE_MPU_READ;

    if (ns_caller) {
        flags |= CMSE_NONSECURE;
    }

    return has_access_to_region(p, s, flags);
}

int32_t tfm_core_has_write_access_to_region(void *p, size_t s,
                                            uint32_t ns_caller)
{
    int flags = CMSE_MPU_UNPRIV|CMSE_MPU_READWRITE;

    if (ns_caller) {
        flags |= CMSE_NONSECURE;
    }

    return has_access_to_region(p, s, flags);
}

/** \brief Check whether the iovec parameters are valid, and the memory ranges
 *         are in the posession of the calling partition
 *
 * \param[in] desc_ptr  The secure function request descriptor
 *
 * \return Return /ref TFM_SUCCESS if the iovec parameters are valid, error code
 *         otherwise as in /ref tfm_status_e
 */
static int32_t tfm_core_check_sfn_parameters(
                                           const struct tfm_sfn_req_s *desc_ptr)
{
    struct psa_invec *in_vec = (psa_invec *)desc_ptr->args[0];
    size_t in_len;
    struct psa_outvec *out_vec = (psa_outvec *)desc_ptr->args[2];
    size_t out_len;

    uint32_t i;

    if ((desc_ptr->args[1] < 0) || (desc_ptr->args[3] < 0)) {
        return TFM_ERROR_INVALID_PARAMETER;
    }

    in_len = (size_t)(desc_ptr->args[1]);
    out_len = (size_t)(desc_ptr->args[3]);

    /* The number of vectors are within range. Extra checks to avoid overflow */
    if ((in_len > PSA_MAX_IOVEC) || (out_len > PSA_MAX_IOVEC) ||
        (in_len + out_len > PSA_MAX_IOVEC)) {
        return TFM_ERROR_INVALID_PARAMETER;
    }

    /* Check whether the caller partition has at write access to the iovec
     * structures themselves. Use the TT instruction for this.
     */
    if (in_len > 0) {
        if ((in_vec == NULL) ||
            (tfm_core_has_write_access_to_region(in_vec,
                                                 sizeof(psa_invec)*in_len,
                                                 desc_ptr->ns_caller) != 1)) {
            return TFM_ERROR_INVALID_PARAMETER;
        }
    } else {
        if (in_vec != NULL) {
            return TFM_ERROR_INVALID_PARAMETER;
        }
    }
    if (out_len > 0) {
        if ((out_vec == NULL) ||
            (tfm_core_has_write_access_to_region(out_vec,
                                                 sizeof(psa_outvec)*out_len,
                                                 desc_ptr->ns_caller) != 1)) {
            return TFM_ERROR_INVALID_PARAMETER;
        }
    } else {
        if (out_vec != NULL) {
            return TFM_ERROR_INVALID_PARAMETER;
        }
    }

    /* Check whether the caller partition has access to the data inside the
     * iovecs
     */
    for (i = 0; i < in_len; ++i) {
        if (in_vec[i].len > 0) {
            if ((in_vec[i].base == NULL) ||
                (tfm_core_has_read_access_to_region(in_vec[i].base,
                                                  in_vec[i].len,
                                                  desc_ptr->ns_caller) != 1)) {
                return TFM_ERROR_INVALID_PARAMETER;
            }
        }
    }
    for (i = 0; i < out_len; ++i) {
        if (out_vec[i].len > 0) {
            if ((out_vec[i].base == NULL) ||
                (tfm_core_has_write_access_to_region(out_vec[i].base,
                                                  out_vec[i].len,
                                                  desc_ptr->ns_caller) != 1)) {
                return TFM_ERROR_INVALID_PARAMETER;
            }
        }
    }

    return TFM_SUCCESS;
}

static void tfm_copy_iovec_parameters(struct iovec_args_t *target,
                               const struct iovec_args_t *source)
{
    size_t i;

    target->in_len = source->in_len;
    for (i = 0; i < source->in_len; ++i) {
        target->in_vec[i].base = source->in_vec[i].base;
        target->in_vec[i].len = source->in_vec[i].len;
    }
    target->out_len = source->out_len;
    for (i = 0; i < source->out_len; ++i) {
        target->out_vec[i].base = source->out_vec[i].base;
        target->out_vec[i].len = source->out_vec[i].len;
    }
}

static void tfm_clear_iovec_parameters(struct iovec_args_t *args)
{
    int i;

    args->in_len = 0;
    for (i = 0; i < PSA_MAX_IOVEC; ++i) {
        args->in_vec[i].base = NULL;
        args->in_vec[i].len = 0;
    }
    args->out_len = 0;
    for (i = 0; i < PSA_MAX_IOVEC; ++i) {
        args->out_vec[i].base = NULL;
        args->out_vec[i].len = 0;
    }
}

static int32_t tfm_start_partition(const struct tfm_sfn_req_s *desc_ptr,
                                                             uint32_t excReturn)
{
    uint32_t caller_partition_idx = desc_ptr->caller_part_idx;
    const struct spm_partition_runtime_data_t *curr_part_data;
    uint32_t caller_flags;
    register uint32_t partition_idx;
    uint32_t psp = __get_PSP();
    uint32_t partition_psp, partition_psplim;
    uint32_t partition_state;
    uint32_t partition_flags;
    struct tfm_exc_stack_t *svc_ctx = (struct tfm_exc_stack_t *)psp;
    uint32_t caller_partition_id;
    int32_t client_id;
    struct iovec_args_t *iovec_args;

    caller_flags = tfm_spm_partition_get_flags(caller_partition_idx);

    /* Check partition state consistency */
    if (((caller_flags & SPM_PART_FLAG_APP_ROT) != 0)
        != (!desc_ptr->ns_caller)) {
        /* Partition state inconsistency detected */
        return TFM_SECURE_LOCK_FAILED;
    }

    if((caller_flags & SPM_PART_FLAG_APP_ROT) == 0) {
        /* Disable NS exception handling while secure service is running.
         * FixMe:
         * This restriction is applied to limit the number of possible attack
         * vectors.
         * To be removed when pre-emption and context management issues have
         * been analysed and resolved.
         */
        TFM_NS_EXC_DISABLE();
    }

    partition_idx = get_partition_idx(desc_ptr->sp_id);

    curr_part_data = tfm_spm_partition_get_runtime_data(partition_idx);
    partition_state = curr_part_data->partition_state;
    partition_flags = tfm_spm_partition_get_flags(partition_idx);
    caller_partition_id = tfm_spm_partition_get_partition_id(
                                                          caller_partition_idx);

    if (tfm_secure_api_initializing) {
#if TFM_LVL != 1
        /* Make thread mode unprivileged while untrusted partition init is
         * executed
         */
        if ((partition_flags & SPM_PART_FLAG_PSA_ROT) == 0) {
            CONTROL_Type ctrl;

            ctrl.w = __get_CONTROL();
            ctrl.b.nPRIV = 1;
            __set_CONTROL(ctrl.w);
            __DSB();
            __ISB();
        }
#endif
    } else if (partition_state == SPM_PARTITION_STATE_RUNNING ||
        partition_state == SPM_PARTITION_STATE_SUSPENDED ||
        partition_state == SPM_PARTITION_STATE_BLOCKED) {
        /* Recursion is not permitted! */
        return TFM_ERROR_PARTITION_NON_REENTRANT;
    } else if (partition_state != SPM_PARTITION_STATE_IDLE) {
        /* The partition to be called is not in a proper state */
        return TFM_SECURE_LOCK_FAILED;
    }

#if TFM_LVL == 1
    /* Prepare switch to shared secure partition stack */
    /* In case the call is coming from the non-secure world, we save the iovecs
     * on the stop of the stack. So the memory area, that can actually be used
     * as stack by the partitions starts at a lower address
     */
    partition_psp =
        (uint32_t)&REGION_NAME(Image$$, TFM_SECURE_STACK, $$ZI$$Limit)-
        sizeof(struct iovec_args_t);
    partition_psplim =
        (uint32_t)&REGION_NAME(Image$$, TFM_SECURE_STACK, $$ZI$$Base);
#else
    partition_psp = curr_part_data->stack_ptr;
    partition_psplim = tfm_spm_partition_get_stack_bottom(partition_idx);
#endif
    /* Store the context for the partition call */
    tfm_spm_partition_set_caller_partition_idx(partition_idx,
                                               caller_partition_idx);
    tfm_spm_partition_store_context(caller_partition_idx, psp, excReturn);

    if ((caller_flags & SPM_PART_FLAG_APP_ROT)) {
        tfm_spm_partition_set_caller_client_id(partition_idx,
                                               caller_partition_id);
    } else {
        client_id = tfm_nspm_get_current_client_id();
        if (client_id >= 0)
        {
            return TFM_SECURE_LOCK_FAILED;
        }
        tfm_spm_partition_set_caller_client_id(partition_idx, client_id);
    }

#if (TFM_LVL != 1) && (TFM_LVL != 2)
    /* Dynamic partitioning is only done is TFM level 3 */
    tfm_spm_partition_sandbox_deconfig(caller_partition_idx);

    /* Configure partition execution environment */
    if (tfm_spm_partition_sandbox_config(partition_idx) != SPM_ERR_OK) {
        ERROR_MSG("Failed to configure sandbox for partition!");
        tfm_secure_api_error_handler();
    }
#endif

    /* Default share to scratch area in case of partition to partition calls
     * this way partitions always get default access to input buffers
     */
    /* FixMe: return value/error handling TBD */
    tfm_spm_partition_set_share(partition_idx, desc_ptr->ns_caller ?
        TFM_BUFFER_SHARE_NS_CODE : TFM_BUFFER_SHARE_SCRATCH);

#if TFM_LVL == 1
    /* In level one, only switch context and return from exception if in
     * handler mode
     */
    if ((desc_ptr->ns_caller) || (tfm_secure_api_initializing)) {
        if (desc_ptr->iovec_api == TFM_SFN_API_IOVEC) {
            /* Save the iovecs on the common stack. The vectors had been sanity
             * checked already, and since then the interrupts have been kept
             * disabled. So we can be sure that the vectors haven't been
             * tampered with since the check.
             */
            iovec_args = (struct iovec_args_t *)
                    ((uint32_t)&REGION_NAME(Image$$, TFM_SECURE_STACK, $$ZI$$Limit)-
                     sizeof(struct iovec_args_t));
            if (tfm_spm_partition_set_iovec(partition_idx, desc_ptr->args) !=
                SPM_ERR_OK) {
                return TFM_ERROR_GENERIC;
            }
            tfm_copy_iovec_parameters(iovec_args,
                                      &(curr_part_data->iovec_args));

            /* Prepare the partition context, update stack ptr */
            psp = (uint32_t)prepare_partition_iovec_ctx(svc_ctx, desc_ptr,
                                                        iovec_args,
                                                     (uint32_t *)partition_psp);
        } else {
            /* Prepare the partition context, update stack ptr */
            psp = (uint32_t)prepare_partition_ctx(svc_ctx, desc_ptr,
                                                  (uint32_t *)partition_psp);
        }
        __set_PSP(psp);
        __set_PSPLIM(partition_psplim);
    }
#else
    if (desc_ptr->iovec_api == TFM_SFN_API_IOVEC) {
        /* Save the iovecs on the stack of the partition. The vectors had been
         * sanity checked already, and since then the interrupts have been kept
         * disabled. So we can be sure that the vectors haven't been tampered
         * with since the check.
         */
        iovec_args =
        (struct iovec_args_t *)(tfm_spm_partition_get_stack_top(partition_idx) -
        sizeof(struct iovec_args_t));
        if (tfm_spm_partition_set_iovec(partition_idx, desc_ptr->args) !=
            SPM_ERR_OK) {
            return TFM_ERROR_GENERIC;
        }
        tfm_copy_iovec_parameters(iovec_args, &(curr_part_data->iovec_args));

        /* Prepare the partition context, update stack ptr */
        psp = (uint32_t)prepare_partition_iovec_ctx(svc_ctx, desc_ptr,
                                                    iovec_args,
                                                    (uint32_t *)partition_psp);
    } else {
        /* Prepare the partition context, update stack ptr */
        psp = (uint32_t)prepare_partition_ctx(svc_ctx, desc_ptr,
                                              (uint32_t *)partition_psp);
    }
    __set_PSP(psp);
    __set_PSPLIM(partition_psplim);
#endif

    tfm_spm_partition_set_state(caller_partition_idx,
                                SPM_PARTITION_STATE_BLOCKED);
    tfm_spm_partition_set_state(partition_idx, SPM_PARTITION_STATE_RUNNING);
    tfm_secure_lock++;

    return TFM_SUCCESS;
}

static int32_t tfm_return_from_partition(uint32_t *excReturn)
{
    uint32_t current_partition_idx =
            tfm_spm_partition_get_running_partition_idx();
    const struct spm_partition_runtime_data_t *curr_part_data, *ret_part_data;
    uint32_t current_partition_flags;
    uint32_t return_partition_idx;
    uint32_t return_partition_flags;
    uint32_t psp = __get_PSP();
    size_t i;
    struct tfm_exc_stack_t *svc_ctx = (struct tfm_exc_stack_t *)psp;
    struct iovec_args_t *iovec_args;

    if (current_partition_idx == SPM_INVALID_PARTITION_IDX) {
        return TFM_SECURE_UNLOCK_FAILED;
    }

    curr_part_data = tfm_spm_partition_get_runtime_data(current_partition_idx);
    return_partition_idx = curr_part_data->caller_partition_idx;

    if (return_partition_idx == SPM_INVALID_PARTITION_IDX) {
        return TFM_SECURE_UNLOCK_FAILED;
    }

    ret_part_data = tfm_spm_partition_get_runtime_data(return_partition_idx);

    return_partition_flags = tfm_spm_partition_get_flags(return_partition_idx);
    current_partition_flags = tfm_spm_partition_get_flags(
            current_partition_idx);

    tfm_secure_lock--;

    if((return_partition_flags & SPM_PART_FLAG_APP_ROT) == 0) {
        /* Re-enable NS exceptions when secure service returns to NS client.
         * FixMe:
         * To be removed when pre-emption and context management issues have
         * been analysed and resolved.
         */
        TFM_NS_EXC_ENABLE();
    }

#if (TFM_LVL != 1) && (TFM_LVL != 2)
    /* Deconfigure completed partition environment */
    tfm_spm_partition_sandbox_deconfig(current_partition_idx);
    if (tfm_secure_api_initializing) {
        /* Restore privilege for thread mode during TF-M init. This is only
         * have to be done if the partition is not trusted.
         */
        if ((current_partition_flags & SPM_PART_FLAG_PSA_ROT) == 0) {
            CONTROL_Type ctrl;

            ctrl.w = __get_CONTROL();
            ctrl.b.nPRIV = 0;
            __set_CONTROL(ctrl.w);
            __DSB();
            __ISB();
        }
    } else {
        /* Configure the caller partition environment in case this was a
         * partition to partition call and returning to untrusted partition
         */
        if (tfm_spm_partition_sandbox_config(return_partition_idx)
            != SPM_ERR_OK) {
            ERROR_MSG("Failed to configure sandbox for partition!");
            tfm_secure_api_error_handler();
        }
        if (return_partition_flags & SPM_PART_FLAG_APP_ROT) {
            /* Restore share status */
            tfm_spm_partition_set_share(
                return_partition_idx,
                tfm_spm_partition_get_runtime_data(
                    return_partition_idx)->share);
        }
    }
#endif

#if TFM_LVL == 1
    if (!(return_partition_flags & SPM_PART_FLAG_APP_ROT) ||
        (tfm_secure_api_initializing)) {
        /* In TFM level 1 context restore is only done when
         * returning to NS or after initialization
         */
        /* Restore caller context */
        restore_caller_ctx(svc_ctx,
            (struct tfm_exc_stack_t *)ret_part_data->stack_ptr);
        *excReturn = ret_part_data->lr;
        __set_PSP(ret_part_data->stack_ptr);
        extern uint32_t Image$$ARM_LIB_STACK$$ZI$$Base[];
        uint32_t psp_stack_bottom = (uint32_t)Image$$ARM_LIB_STACK$$ZI$$Base;
       __set_PSPLIM(psp_stack_bottom);

        /* FIXME: The condition should be removed once all the secure service
         *        calls are done via the iovec veneers */
        if (curr_part_data->iovec_api) {
            iovec_args = (struct iovec_args_t *)
                         ((uint32_t)&REGION_NAME(Image$$, TFM_SECURE_STACK, $$ZI$$Limit)-
                         sizeof(struct iovec_args_t));

            for (i = 0; i < curr_part_data->iovec_args.out_len; ++i) {
                curr_part_data->orig_outvec[i].len = iovec_args->out_vec[i].len;
            }
            tfm_clear_iovec_parameters(iovec_args);
        }
    }
#else
    /* Restore caller context */
    restore_caller_ctx(svc_ctx,
        (struct tfm_exc_stack_t *)ret_part_data->stack_ptr);
    *excReturn = ret_part_data->lr;
    __set_PSP(ret_part_data->stack_ptr);
    __set_PSPLIM(tfm_spm_partition_get_stack_bottom(return_partition_idx));
    /* Clear the context entry before returning */
    tfm_spm_partition_set_stack(
                current_partition_idx, psp + sizeof(struct tfm_exc_stack_t));

    /* FIXME: The condition should be removed once all the secure service
     *        calls are done via the iovec veneers */
    if (curr_part_data->iovec_api) {
        iovec_args = (struct iovec_args_t *)
                     (tfm_spm_partition_get_stack_top(current_partition_idx) -
                     sizeof(struct iovec_args_t));

        for (i = 0; i < curr_part_data->iovec_args.out_len; ++i) {
            curr_part_data->orig_outvec[i].len = iovec_args->out_vec[i].len;
        }
        tfm_clear_iovec_parameters(iovec_args);
    }
#endif

    tfm_spm_partition_cleanup_context(current_partition_idx);

    tfm_spm_partition_set_state(current_partition_idx,
                                SPM_PARTITION_STATE_IDLE);
    tfm_spm_partition_set_state(return_partition_idx,
                                SPM_PARTITION_STATE_RUNNING);

    return TFM_SUCCESS;
}

void tfm_secure_api_error_handler(void)
{
    ERROR_MSG("Security violation when calling secure API");
    while (1) {
        ;
    }
}

static int32_t tfm_check_sfn_req_integrity(const struct tfm_sfn_req_s *desc_ptr)
{
    if ((desc_ptr == NULL) ||
        (desc_ptr->sp_id == 0) ||
        (desc_ptr->sfn == NULL)) {
        /* invalid parameter */
        return TFM_ERROR_INVALID_PARAMETER;
    }
    return TFM_SUCCESS;
}

static int32_t tfm_core_check_sfn_req_rules(
        const struct tfm_sfn_req_s *desc_ptr)
{
    /* Check partition idx validity */
    if (desc_ptr->caller_part_idx == SPM_INVALID_PARTITION_IDX) {
        return TFM_ERROR_NO_ACTIVE_PARTITION;
    }

    if ((desc_ptr->ns_caller) && (tfm_secure_lock != 0)) {
        /* Secure domain is already locked!
         * This should only happen if caller is secure partition!
         * FixMe: This scenario is a potential security breach
         * Take appropriate action!
         */
        return TFM_ERROR_SECURE_DOMAIN_LOCKED;
    }

    if (tfm_secure_api_initializing) {
        int32_t id =
            tfm_spm_partition_get_partition_id(desc_ptr->caller_part_idx);

        if ((id != TFM_SP_CORE_ID) || (tfm_secure_lock != 0)) {
            /* Invalid request during system initialization */
            ERROR_MSG("Invalid service request during initialization!");
            return TFM_ERROR_NOT_INITIALIZED;
        }
    }

    return TFM_SUCCESS;
}

void tfm_secure_api_init_done(void)
{
    tfm_secure_api_initializing = 0;
#if TFM_LVL != 1
    if (tfm_spm_partition_sandbox_config(TFM_SP_NON_SECURE_ID) != SPM_ERR_OK) {
        ERROR_MSG("Failed to configure sandbox for partition!");
        tfm_secure_api_error_handler();
    }
#endif
}

int32_t tfm_core_sfn_request_handler(
                             struct tfm_sfn_req_s *desc_ptr, uint32_t excReturn)
{
    int32_t res;

    res = tfm_check_sfn_req_integrity(desc_ptr);
    if (res != TFM_SUCCESS) {
        ERROR_MSG("Invalid service request!");
        tfm_secure_api_error_handler();
    }

    __disable_irq();

    desc_ptr->caller_part_idx = tfm_spm_partition_get_running_partition_idx();

    if (desc_ptr->iovec_api == TFM_SFN_API_IOVEC) {
        res = tfm_core_check_sfn_parameters(desc_ptr);
        if (res != TFM_SUCCESS) {
            /* The sanity check of iovecs failed. */
            __enable_irq();
            tfm_secure_api_error_handler();
        }
    }

    res = tfm_core_check_sfn_req_rules(desc_ptr);
    if (res != TFM_SUCCESS) {
        /* FixMe: error compartmentalization TBD */
        tfm_spm_partition_set_state(
            desc_ptr->caller_part_idx, SPM_PARTITION_STATE_CLOSED);
        __enable_irq();
        ERROR_MSG("Unauthorized service request!");
        tfm_secure_api_error_handler();
    }

    res = tfm_start_partition(desc_ptr, excReturn);
    if (res != TFM_SUCCESS) {
        /* FixMe: consider possible fault scenarios */
        __enable_irq();
        ERROR_MSG("Failed to process service request!");
        tfm_secure_api_error_handler();
    }

    __enable_irq();

    return res;
}

#if TFM_LVL == 1
int32_t tfm_core_sfn_request_thread_mode(struct tfm_sfn_req_s *desc_ptr)
{
    int32_t res;
    int32_t *args;
    int32_t retVal;

    if (desc_ptr->iovec_api == TFM_SFN_API_IOVEC) {
        res = tfm_core_check_sfn_parameters(desc_ptr);
        if (res != TFM_SUCCESS) {
            /* The sanity check of iovecs failed. */
            return res;
        }
    }

    /* No excReturn value is needed as no exception handling is used */
    res = tfm_core_sfn_request_handler(desc_ptr, 0);

    if (res != TFM_SUCCESS) {
        tfm_secure_api_error_handler();
    }

    /* Secure partition to secure partition call in TFM level 1 */
    args = desc_ptr->args;
    retVal = desc_ptr->sfn(args[0], args[1], args[2], args[3]);

    /* return handler should restore original exc_return value... */
    res = tfm_return_from_partition(NULL);
    if (res == TFM_SUCCESS) {
        /* If unlock successful, pass SS return value to caller */
        res = retVal;
    } else {
        /* Unlock errors indicate ctx database corruption or unknown
         * anomalies. Halt execution
         */
        ERROR_MSG("Secure API error during unlock!");
        tfm_secure_api_error_handler();
    }
    return res;
}
#endif

void tfm_core_validate_secure_caller_handler(uint32_t *svc_args)
{

    int32_t res = TFM_ERROR_GENERIC;
    uint32_t running_partition_idx =
            tfm_spm_partition_get_running_partition_idx();
    const struct spm_partition_runtime_data_t *curr_part_data =
            tfm_spm_partition_get_runtime_data(running_partition_idx);
    uint32_t running_partition_flags =
            tfm_spm_partition_get_flags(running_partition_idx);
    uint32_t caller_partition_flags =
            tfm_spm_partition_get_flags(curr_part_data->caller_partition_idx);

    if (!(running_partition_flags & SPM_PART_FLAG_APP_ROT))  {
        /* This handler shouldn't be called from outside partition context.
         * Partitions are only allowed to run while S domain is locked.
         */
        svc_args[0] = TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    /* Store return value in r0 */
    if (caller_partition_flags & SPM_PART_FLAG_APP_ROT) {
        res = TFM_SUCCESS;
    }
    svc_args[0] = res;
}

int32_t tfm_core_check_buffer_access(uint32_t  partition_idx,
                                     void     *start_addr,
                                     size_t    len,
                                     uint32_t  alignment)
{
    uintptr_t start_addr_value = (uintptr_t)start_addr;
    uintptr_t end_addr_value = (uintptr_t)start_addr + len;
    uintptr_t alignment_mask;

    alignment_mask = (((uintptr_t)1) << alignment) - 1;

    /* Check that the pointer is aligned properly */
    if (start_addr_value & alignment_mask) {
        /* not aligned, return error */
        return 0;
    }

    /* Protect against overflow (and zero len) */
    if (end_addr_value <= start_addr_value)
    {
        return 0;
    }

#if TFM_LVL == 1
    /* For privileged partition execution, all secure data memory and stack
     * is accessible
     */
    if (start_addr_value >= S_DATA_START &&
        end_addr_value <= (S_DATA_START + S_DATA_SIZE)) {
        return 1;
    }
#else
    /* For non-privileged execution the partition's data and stack is
     * accessible
     */
    if (start_addr_value >=
            tfm_spm_partition_get_stack_bottom(partition_idx) &&
        end_addr_value <=
            tfm_spm_partition_get_stack_top(partition_idx)) {
        return 1;
    }
    if (start_addr_value >=
           tfm_spm_partition_get_rw_start(partition_idx) &&
        end_addr_value <=
           tfm_spm_partition_get_rw_limit(partition_idx)) {
        return 1;
    }
    if (start_addr_value >=
           tfm_spm_partition_get_zi_start(partition_idx) &&
        end_addr_value <=
           tfm_spm_partition_get_zi_limit(partition_idx)) {
        return 1;
    }
#endif
    return 0;
}

void tfm_core_get_caller_client_id_handler(uint32_t *svc_args)
{
    uintptr_t result_ptr_value = svc_args[0];
    uint32_t running_partition_idx =
            tfm_spm_partition_get_running_partition_idx();
    const uint32_t running_partition_flags =
            tfm_spm_partition_get_flags(running_partition_idx);
    const struct spm_partition_runtime_data_t *curr_part_data =
            tfm_spm_partition_get_runtime_data(running_partition_idx);
    int res = 0;

    if (!(running_partition_flags & SPM_PART_FLAG_APP_ROT))  {
        /* This handler shouldn't be called from outside partition context.
         * Partitions are only allowed to run while S domain is locked.
         */
        svc_args[0] = TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    /* Make sure that the output pointer points to a memory area that is owned
     * by the partition
     */
    res = tfm_core_check_buffer_access(running_partition_idx,
                                       (void *)result_ptr_value,
                                       sizeof(curr_part_data->caller_client_id),
                                       2);
    if (!res) {
        /* Not in accessible range, return error */
        svc_args[0] = TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    *((int32_t *)result_ptr_value) = curr_part_data->caller_client_id;

    /* Store return value in r0 */
    svc_args[0] = TFM_SUCCESS;
}

void tfm_core_memory_permission_check_handler(uint32_t *svc_args)
{
    uint32_t ptr = svc_args[0];
    uint32_t size = svc_args[1];
    int32_t access = svc_args[2];

    uint32_t max_buf_size, ptr_start, range_limit, range_check = false;
    int32_t res;
    uint32_t running_partition_idx =
            tfm_spm_partition_get_running_partition_idx();
    const struct spm_partition_runtime_data_t *curr_part_data =
            tfm_spm_partition_get_runtime_data(running_partition_idx);
    uint32_t running_partition_flags =
            tfm_spm_partition_get_flags(running_partition_idx);
    int32_t flags = 0;
    void *rangeptr;

    if (!(running_partition_flags & SPM_PART_FLAG_APP_ROT) || (size == 0)) {
        /* This handler should only be called from a secure partition. */
        svc_args[0] = TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    if (curr_part_data->share != TFM_BUFFER_SHARE_PRIV) {
        flags |= CMSE_MPU_UNPRIV;
    }

    if (access == TFM_MEMORY_ACCESS_RW) {
        flags |= CMSE_MPU_READWRITE;
    } else {
        flags |= CMSE_MPU_READ;
    }

    /* Check if partition access to address would fail */
    rangeptr = cmse_check_address_range((void *)ptr, size, flags);

    /* Get regions associated with address */
    cmse_address_info_t addr_info = cmse_TT((void *)ptr);

    if (rangeptr == NULL) {
        svc_args[0] = TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    if (addr_info.flags.secure) {
#if TFM_LVL == 1
        /* For privileged partition execution, all secure data memory is
         * accessible
         */
        max_buf_size = S_DATA_SIZE;
        ptr_start = S_DATA_START;
        range_limit = S_DATA_LIMIT;
#else
        /* Only scratch is permitted in secure memory */
        max_buf_size = (uint32_t)tfm_scratch_area_size;
        ptr_start = (uint32_t)tfm_scratch_area;
        range_limit = (uint32_t)tfm_scratch_area + tfm_scratch_area_size - 1;
#endif
        range_check = true;
    } else {
        if (!addr_info.flags.sau_region_valid) {
            /* If address is NS, TF-M expects SAU to be configured
             */
            svc_args[0] = TFM_ERROR_INVALID_PARAMETER;
            return;
        }
        switch (addr_info.flags.sau_region) {
        case TFM_NS_REGION_CODE:
            if (access == TFM_MEMORY_ACCESS_RW) {
                res = TFM_ERROR_INVALID_PARAMETER;
            } else {
                /* Currently TF-M does not support checks for NS Memory
                 * accesses by partitions
                 */
                res = TFM_SUCCESS;
            }
            break;
        case TFM_NS_REGION_DATA:
            /* Currently TF-M does not support checks for NS Memory
             * accesses by partitions
             */
            res = TFM_SUCCESS;
            break;
        default:
            /* Only NS data and code regions can be accessed as buffers */
            res = TFM_ERROR_INVALID_PARAMETER;
            break;
        }
    }

    if (range_check == true) {
        if ((size <= max_buf_size) && (ptr >= ptr_start)
            && (ptr <= range_limit + 1 - size)) {
            res = TFM_SUCCESS;
        } else {
            res = TFM_ERROR_INVALID_PARAMETER;
        }
    }

    /* Store return value in r0 */
    svc_args[0] = res;
}

/* This SVC handler is called if veneer is running in thread mode */
uint32_t tfm_core_partition_request_svc_handler(
        const struct tfm_exc_stack_t *svc_ctx, uint32_t excReturn)
{
    struct tfm_sfn_req_s *desc_ptr;

    if (!(excReturn & EXC_RETURN_STACK_PROCESS)) {
        /* Service request SVC called with MSP active.
         * Either invalid configuration for Thread mode or SVC called
         * from Handler mode, which is not supported.
         * FixMe: error severity TBD
         */
        ERROR_MSG("Service request SVC called with MSP active!");
        tfm_secure_api_error_handler();
    }

    desc_ptr = (struct tfm_sfn_req_s *)svc_ctx->R0;

    if (tfm_core_sfn_request_handler(desc_ptr, excReturn) != TFM_SUCCESS) {
        tfm_secure_api_error_handler();
    }

    return EXC_RETURN_SECURE_FUNCTION;
}

/* This SVC handler is called when sfn returns */
uint32_t tfm_core_partition_return_handler(uint32_t lr)
{
    int32_t res;

    if (!(lr & EXC_RETURN_STACK_PROCESS)) {
        /* Partition return SVC called with MSP active.
         * This should not happen!
         */
        ERROR_MSG("Partition return SVC called with MSP active!");
        tfm_secure_api_error_handler();
    }

    /* Store return value from secure partition */
    int32_t retVal = *(int32_t *)__get_PSP();

    if (!is_iovec_api_call()) {
        if ((retVal > TFM_SUCCESS) &&
            (retVal < TFM_PARTITION_SPECIFIC_ERROR_MIN)) {
            /* Secure function returned a reserved value */
#ifdef TFM_CORE_DEBUG
            LOG_MSG("Invalid return value from secure partition!");
#endif
            /* FixMe: error can be traced to specific secure partition
             * and Core is not compromised. Error handling flow can be
             * refined
             */
            tfm_secure_api_error_handler();
        }
    }

    res = tfm_return_from_partition(&lr);
    if (res != TFM_SUCCESS) {
        /* Unlock errors indicate ctx database corruption or unknown anomalies
         * Halt execution
         */
        ERROR_MSG("Secure API error during unlock!");
        tfm_secure_api_error_handler();
    }

    return lr;
}

void tfm_core_set_buffer_area_handler(uint32_t *args)
{
    /* r0 is stored in args[0] in exception stack frame
     * Store input parameter before writing return value to that address
     */
    enum tfm_buffer_share_region_e share;
    uint32_t running_partition_idx =
            tfm_spm_partition_get_running_partition_idx();
    const struct spm_partition_runtime_data_t *curr_part_data =
            tfm_spm_partition_get_runtime_data(running_partition_idx);
    uint32_t caller_partition_idx = curr_part_data->caller_partition_idx;
    uint32_t running_partition_flags =
            tfm_spm_partition_get_flags(running_partition_idx);
    uint32_t caller_partition_flags =
            tfm_spm_partition_get_flags(caller_partition_idx);

     /* tfm_core_set_buffer_area() returns int32_t */
    int32_t *res_ptr = (int32_t *)&args[0];

    if (!(running_partition_flags & SPM_PART_FLAG_APP_ROT)) {
        /* This handler should only be called from a secure partition. */
        *res_ptr = TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    switch (args[0]) {
    case TFM_BUFFER_SHARE_DEFAULT:
        share = (!(caller_partition_flags & SPM_PART_FLAG_APP_ROT)) ?
            (TFM_BUFFER_SHARE_NS_CODE) : (TFM_BUFFER_SHARE_SCRATCH);
        break;
    case TFM_BUFFER_SHARE_SCRATCH:
    case TFM_BUFFER_SHARE_NS_CODE:
        share = args[0];
        break;
    default:
        *res_ptr = TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    if (tfm_spm_partition_set_share(running_partition_idx, share) ==
            SPM_ERR_OK) {
        *res_ptr = TFM_SUCCESS;
    } else {
        *res_ptr = TFM_ERROR_INVALID_PARAMETER;
    }

    return;
}
