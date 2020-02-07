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

#ifndef __PMU_GAP8_H__
#define __PMU_GAP8_H__

#include "pmsis.h"
#include "pmsis_hal/pmsis_hal.h"
#include "pmsis_pmu.h"

/*!
 * @addtogroup pmu
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* PMU error type */
#define    PMU_ERROR_NO_ERROR                      (0)
#define    PMU_ERROR_FREQ_TOO_HIGH                 (0x1<<0)
#define    PMU_ERROR_NEED_STATE_CHANGE             (0x1<<1)
#define    PMU_ERROR_VDD_CHANGE_TOO_HIGH           (0x1<<2)
#define    PMU_ERROR_SOC_STATE_CHANGE_FAILED       (0x1<<3)
#define    PMU_ERROR_CLUSTER_STATE_CHANGE_FAILED   (0x1<<4)
#define    PMU_ERROR_VDD_OUT_OF_RANGE              (0x1<<5)

extern pmu_state_t pmu_state;

/* Maestro internal events */
#define    PMU_MAESTRO_EVENT_ICU_OK              (1<<0)
#define    PMU_MAESTRO_EVENT_ICU_DELAYED         (1<<1)
#define    PMU_MAESTRO_EVENT_MODE_CHANGED        (1<<2)
#define    PMU_MAESTRO_EVENT_PICL_OK             (1<<3)
#define    PMU_MAESTRO_EVENT_SCU_OK              (1<<4)

#define    PMU_EVENT_CLUSTER_ON_OFF     (1 << 0) /* GAP_SOC_EVENT_CLUSTER_ON_OFF */
#define    PMU_EVENT_MSP                (1 << 1) /* GAP_SOC_EVENT_MSP) */
#define    PMU_EVENT_ICU_MODE_CHANGED   (1 << 2) /* GAP_SOC_EVENT_ICU_MODE_CHANGED) */
#define    PMU_EVENT_ICU_OK             (1 << 3) /* GAP_SOC_EVENT_ICU_OK) */
#define    PMU_EVENT_CLUSTER_CLOCK_GATE (1 << 4) /* GAP_SOC_EVENT_ICU_DELAYED) */
#define    PMU_EVENT_PICL_OK            (1 << 5) /* GAP_SOC_EVENT_PICL_OK) */
#define    PMU_EVENT_SCU_OK             (1 << 6) /* GAP_SOC_EVENT_SCU_OK) */
#define    PMU_EVENT_SCU_PENDING        (1 << 7) /* GAP_SOC_EVENT_PMU_NB_EVENTS) */

typedef enum _pmu_scu_seq {
  PMU_SCU_DEEP_SLEEP     = 0,
  PMU_SCU_RETENTIVE      = 1,
  PMU_SCU_SOC_HP         = 2,
  PMU_SCU_SOC_LP         = 3,
  PMU_SCU_SOC_CLUSTER_HP = 4,
  PMU_SCU_SOC_CLUSTER_LP = 5
} pmu_scu_seq_t;

/*! @brief PMU wakeup boot state  */
typedef enum _pmu_wakeup_state {    /* Enum encoding follows the definition of the boot_type field of PMU_RetentionStateT */
  PMU_COLD_BOOT       = 0, /* SoC cold boot, from Flash usually */
  PMU_DEEP_SLEEP_BOOT = 1, /* Reboot from deep sleep state, state has been lost, somehow equivalent to COLD_BOOT */
  PMU_RETENTIVE_SLEEP_BOOT  = 2 /* Reboot from Retentive state, state has been retained, should bypass flash reload */
} pmu_wakeup_state_t;

/*! @brief PMU boot mode, from L2 or ROM  */
typedef enum _pmu_boot_mode {
  PMU_BOOT_FROM_ROM=0,
  PMU_BOOT_FROM_L2=1
} pmu_boot_mode_t;

/*! @brief PMU external wakeup mode  */
typedef enum _pmu_wakeup_mode {
  PMU_EXT_WAKEUP_RAISING_EDGE = 0,
  PMU_EXT_WAKEUP_FALLING_EDGE = 1,
  PMU_EXT_WAKEUP_HIGH = 2,
  PMU_EXT_WAKEUP_LOW = 3
} pmu_wakeup_mode_t;

/*! @brief PMU switch state  */
typedef enum pmu_switch_state {
  PMU_SWITCH_RETENTIVE_SLEEP = 0,
  PMU_SWITCH_DEEP_SLEEP = 1
} pmu_switch_state_t;

/*! @brief PMU switch state mode state  */
typedef enum pmu_switch_mode {
  PMU_SWITCH_FAST = 0
} pmu_switch_mode_t;

/*! @brief PMU change status  */
typedef enum pmu_change_status {
  PMU_CHANGE_OK = 0,
  PMU_CHANGE_ERROR = 1
} pmu_change_status_t;

typedef enum _pmu_dcdc_hw_operatingpoint {
  PMU_DCDC_Nominal   = 0,
  PMU_DCDC_Medium    = 1,
  PMU_DCDC_Low       = 2,
  PMU_DCDC_Retentive = 3
} pmu_dcdc_hw_operatingpoint_t;

/*! @brief PMU cluster status  */
typedef enum _pmu_cluster_state {
  PMU_CLUSTER_OFF = 0,
  PMU_CLUSTER_ON  = 1
} pmu_cluster_state_t;

/*! @brief PMU Regulator status  */
typedef enum _pmu_regulator_state {
  PMU_REGU_NV  = 0,
  PMU_REGU_LV  = 1,
  PMU_REGU_RET = 2,
  PMU_REGU_OFF = 3
} pmu_regulator_state_t;

/*! @brief PMU system status  */
typedef enum _pmu_system_state {
  PMU_SOC_CLUSTER_HP = ((PMU_REGU_NV<<1)  | PMU_CLUSTER_ON),  /* 001 = 1 */
  PMU_SOC_CLUSTER_LP = ((PMU_REGU_LV<<1)  | PMU_CLUSTER_ON),  /* 011 = 3 */
  PMU_SOC_HP         = ((PMU_REGU_NV<<1)  | PMU_CLUSTER_OFF), /* 000 = 0 */
  PMU_SOC_LP         = ((PMU_REGU_LV<<1)  | PMU_CLUSTER_OFF), /* 010 = 2 */
  PMU_RETENTIVE      = ((PMU_REGU_RET<<1) | PMU_CLUSTER_OFF), /* 100 = 4 */
  PMU_DEEP_SLEEP     = ((PMU_REGU_OFF<<1) | PMU_CLUSTER_OFF)  /* 110 = 6 */
} pmu_system_state_t;

/*! @brief Configures the interrupt generation condition. */
typedef enum _pmu_gpio_wakeup
{
    GPIO_WakeUpRisingEdge     = 0x0U,  /*!< WakeUp on rising edge. */
    GPIO_WakeUpFallingEdge    = 0x1U,  /*!< WakeUp on falling edge. */
    GPIO_WakeUpHigh           = 0x2U,  /*!< WakeUp on high. */
    GPIO_WakeUpLow            = 0x3U,  /*!< WakeUp on low. */
} pmu_gpio_wakeup_t;

#define PMU_CLUSTER_STATE_MASK              (0x1U)
#define PMU_CLUSTER_STATE_SHIFT             (0U)
#define PMU_CLUSTER_STATE(x)                (((uint32_t)(((uint32_t)(x)) /* << PMU_CLUSTER_STATE_SHIFT)*/)) & PMU_CLUSTER_STATE_MASK)
#define READ_PMU_CLUSTER_STATE(x)           (((uint32_t)(((uint32_t)(x)) & PMU_CLUSTER_STATE_MASK)) /*>> PMU_CLUSTER_STATE_SHIFT*/)

#define PMU_REGULATOR_STATE_MASK            (0x6U)
#define PMU_REGULATOR_STATE_SHIFT           (1U)
#define PMU_REGULATOR_STATE(x)              (((uint32_t)(((uint32_t)(x)) << PMU_REGULATOR_STATE_SHIFT))) & PMU_REGULATOR_STATE_MASK)
#define READ_PMU_REGULATOR_STATE(x)         (((uint32_t)(((uint32_t)(x)) & PMU_REGULATOR_STATE_MASK)) >> PMU_REGULATOR_STATE_SHIFT)

#define PMU_POWER_SYSTEM_STATE(ReguState, ClusterState) ((pmu_system_state_t) ((ReguState << PMU_REGULATOR_STATE_SHIFT) | ClusterState))

#define mV_TO_DCDC(mV)    ((uint32_t) (((mV) - DCDC_BASE_VALUE) / DCDC_STEP))
#define DCDC_TO_mV(Dc)    ((uint32_t) ((Dc) * DCDC_STEP + DCDC_BASE_VALUE))

/*******************************************************************************
 * APIs
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief Initializes the PMU.
 *
 * This function
 *
 * @note .
 */
void pi_pmu_init();

/*!
 * @brief Set PUM voltage and check frequency at the same time .
 *
 * @param voltage  PMU votage to set .
 * @param frequency_check Check the relationship bwtween voltage and frequency.
 * @retval Frequency check result
 *        - -1   : Error, can not set voltage
 *        - other: Voltage is set

 * @note .
 */
int pi_pmu_set_voltage(uint32_t voltage, uint32_t frequency_check);

/*!
 * @brief De-Initializes and shut down the PMU.
 *
 * This function
 * @param retentive  PMU need to be retentive or not .
 * @param wakeup_state  When in sleep or deep sleep mode, set the wakeup state.
 *
 * @note .
 */
void pi_pmu_deinit(int retentive, pmu_system_state_t wakeup_state);

/*!
 * @brief Switch the chip to the specified power state.
 *
 * @param state  the state which the PMU switch to .
 * @param mode   PMU change state mode.
 *
 * @return  0 if the operation is successful, -1 otherwise.
 * @note .
 */
int pi_pmu_state_switch(pmu_switch_state_t state, pmu_switch_mode_t mode);

/*!
 * @brief Get power state before wake-up
 *
 * This function
 *
 * @return the power state of the chip before the last wake-up.
 * @note .
 */
pmu_wakeup_state_t pi_pmu_wakeup_state();

/*!
 * @brief Enable using external GPIO to wake-up PMU
 *
 * @param gpio_number  PMU external wakeup source GPIO number.
 * @param type         PMU external GPIO wakeup type.
 *
 * @return  0 if the operation is successful, -1 otherwise.
 * @note .
 */
int pi_pmu_enable_gpio_wakeup(int gpio_number, pmu_gpio_wakeup_t type);


/*!
 * @brief Disable using external GPIO to wake-up PMU
 *
 * @note .
 */
void pi_pmu_disable_gpio_wakeup();

#ifdef FEATURE_CLUSTER
/*!
 * @brief Check cluster power is on.
 *
 * This function detects if cluster power is on or not
 *
 * @return cluster power is on or not.
 */
int pi_pmu_cluster_is_on();

/*!
 * @brief Check cluster power is off.
 *
 * This function detects if cluster power is off or not
 *
 * @return cluster power is off or not.
 */
int pi_pmu_cluster_is_off();

/*!
 * @brief Cluster power on management.
 *
 * This function
 *
 * @note .
 */
void pi_pmu_cluster_poweron();

/*!
 * @brief Cluster power off management.
 *
 * This function
 *
 * @note .
 */
void pi_pmu_cluster_poweroff();
#endif

/*!
 * @brief PMU event handler.
 *
 * This function handler pmu cluster event
 *
 * @param  num   event number
 * @note .
 */
void pi_pmu_irq_handler(void *arg);


/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_PMU_H_*/
