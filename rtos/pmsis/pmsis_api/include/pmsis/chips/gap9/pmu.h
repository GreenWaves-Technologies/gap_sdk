/*
 * Copyright (C) 2020 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __PI_CHIPS_GAP9_PMU_H__
#define __PI_CHIPS_GAP9_PMU_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup groupChips
 */

/**
 * \addtogroup GAP9
 * @{
 */

/**
 * \defgroup GAP9_PMU GAP9 PMU
 *
 * This part enumerates GAP8 power domains and other elements needed to configure
 * PMU.
 *
 * 3 devices can be used to wake up the chip form deep sleep or retentive deep sleep:
 * * RTC
 * * GPIO(16 available from GPIO_00 to GPIO_15.
 * * SPI slave
 *
 * \addtogroup GAP9_PMU
 * \{
 */

/**
 * \cond IMPLEM
 */
/* Wakeup mode. */
#define PI_PMU_WAKEUP_MODE_RTC_OFF  ( 0 )
#define PI_PMU_WAKEUP_MODE_GPIO_OFF ( 1 )
#define PI_PMU_WAKEUP_MODE_SPIS_OFF ( 2 )
#define PI_PMU_WAKEUP_MODE(mode)    ( 1 << (mode) )

/* Wakeup sequence. */
#define PI_PMU_WAKEUP_MRAM_OFF     ( 0 )
#define PI_PMU_WAKEUP_CL_OFF       ( 1 )
#define PI_PMU_WAKEUP_SFU_OFF      ( 26 )
#define PI_PMU_WAKEUP_SEQUENCE(mram, cl, sfu) ((((mram) << PI_PMU_WAKEUP_MRAM_OFF) | \
                                                ((cl) << PI_PMU_WAKEUP_CL_OFF)) + \
                                               ((sfu) * PI_PMU_WAKEUP_SFU_OFF))
/**
 * \endcond IMPLEM
 */

/**
 * \enum pi_pmu_domain_e
 *
 * \brief PMU domains.
 *
 * This is used to identify power domains on GAP9.
 */
typedef enum
{
    PI_PMU_DOMAIN_SOC    = 0,   /*!< SOC power domain. */
    PI_PMU_DOMAIN_CL     = 1,   /*!< Cluster power domain. */
    PI_PMU_DOMAIN_CSI    = 2,   /*!< CSI power domain. */
    PI_PMU_DOMAIN_MRAM   = 3,   /*!< MRAM power domain. */
    PI_PMU_DOMAIN_SFU    = 4    /*!< SFU power domain. */
} pi_pmu_domain_e;


/**
 * \enum pi_pmu_boot_state_e
 *
 * \brief Wakeup boot state
 */
typedef enum _pi_pmu_boot_state
{
    PI_PMU_BOOT_COLD           = 0, /*!< Cold boot : reset boot, powerup boot. */
    PI_PMU_BOOT_DEEP_SLEEP     = 1, /*!< Warm boot : boot after sleep, cold boot after. */
    PI_PMU_BOOT_RET_DEEP_SLEEP = 2  /*!< Warm boot : boot after deep sleep, saved FLL and PMU config are restored. */
} pi_pmu_boot_state_e;

/**
 * \enum pi_pmu_sleep_mode_e
 *
 * \brief Sleep mode
 */
typedef enum _pi_pmu_sleep_mode
{
    PI_PMU_SLEEP_MODE_DS     = 0, /*!< Deep sleep. Cold boot at reboot. */
    PI_PMU_SLEEP_MODE_RET_DS = 1  /*!< Sleep/retentive mode. Memories can be saved. */
} pi_pmu_sleep_mode_e;

/**
 * \enum pi_pmu_domain_state_e
 *
 * \brief Domain state.
 */
typedef enum _pi_pmu_domain_state
{
    PI_PMU_DOMAIN_STATE_OFF = 0, /*!< Domain is off. */
    PI_PMU_DOMAIN_STATE_ON  = 1  /*!< Domain is on. */
} pi_pmu_domain_state_e;

/**
 * \enum pi_pmu_gpio_wakeup_event_e
 *
 * \brief GPIO wake up event
 */
typedef enum _pi_pmu_gpio_wakeup_event
{
    PI_PMU_GPIO_DISABLED = 0x00, /*!< GPIO disabled. */
    PI_PMU_GPIO_HIGH     = 0x01, /*!< Wake up on high signal. */
    PI_PMU_GPIO_LOW      = 0x02  /*!< Wake up on low signal. */
} pi_pmu_gpio_wakeup_event_e;

/**
 * \enum pi_pmu_wakeup_mode_e
 *
 * \brief External wake up device.
 */
typedef enum _pi_pmu_wakeup_mode
{
    PI_PMU_WAKEUP_RTC      = PI_PMU_WAKEUP_MODE(PI_PMU_WAKEUP_MODE_RTC_OFF),  /*!< Wake up by RTC. */
    PI_PMU_WAKEUP_GPIO     = PI_PMU_WAKEUP_MODE(PI_PMU_WAKEUP_MODE_GPIO_OFF), /*!< Wake up by GPIO. */
    PI_PMU_WAKEUP_SPISLAVE = PI_PMU_WAKEUP_MODE(PI_PMU_WAKEUP_MODE_SPIS_OFF)  /*!< Wake up by SPI Slave. */
} pi_pmu_wakeup_mode_e;

/**
 * \enum pi_pmu_wakeup_state_e
 *
 * \brief PMU wakeup domains state after DS or DS_RET.
 *
 * This is used to identify power domains turned on/off at wakeup after
 * a deep sleep or deep sleep retentive.
 */
typedef enum _pi_pmu_wakeup_state
{
    PI_PMU_WAKEUP_SEQUENCE_SOC_ON             = 0x0, /*!< ON: SOC | OFF: MRAM, CL, SFU.  */
    PI_PMU_WAKEUP_SEQUENCE_SOC_MRAM_ON        = 0x1, /*!< ON: SOC, MRAM | OFF: CL, SFU.  */
    PI_PMU_WAKEUP_SEQUENCE_SOC_CL_ON          = 0x2, /*!< ON: SOC, CL | OFF: MRAM, SFU.  */
    PI_PMU_WAKEUP_SEQUENCE_SOC_CL_MRAM_ON     = 0x3, /*!< ON: SOC, MRAM, CL | OFF: SFU.  */
    PI_PMU_WAKEUP_SEQUENCE_SOC_SFU_ON         = 0x4, /*!< ON: SOC, SFU | OFF: MRAM, CL.  */
    PI_PMU_WAKEUP_SEQUENCE_SOC_SFU_MRAM_ON    = 0x5, /*!< ON: SOC, SFU, MRAM | OFF: CL.  */
    PI_PMU_WAKEUP_SEQUENCE_SOC_SFU_CL_ON      = 0x6, /*!< ON: SOC, SFU, CL | OFF: MRAM.  */
    PI_PMU_WAKEUP_SEQUENCE_SOC_SFU_CL_MRAM_ON = 0x7  /*!< ON: SOC, SFU, MRAM, CL | OFF:. */
} pi_pmu_wakeup_state_e;

/**
 * \struct pi_pmu_sleep_conf_s
 *
 * \brief Sleep config structure.
 */
struct pi_pmu_sleep_conf_s
{
    uint32_t gpio_mask;                 /*!< GPIO pin used for wake up. */
    uint8_t fll_on;                     /*!< Restore fll settings. */
    pi_pmu_sleep_mode_e sleep_mode;     /*!< Sleep mode. */
    pi_pmu_wakeup_mode_e wakeup_mode;   /*!< Wakeup mode RTC, GPIO or SPI slave. */
    pi_pmu_wakeup_state_e wakeup_state; /*!< Wakeup sequence. */
};


/**
 * \brief Change power domain state.
 *
 * This function is used to power on/off a power domain(cf \ref pi_pmu_domain_e).
 *
 * \param pd             Power domain.
 * \param state          Power ON/OF.
 */
void pi_pmu_power_domain_change(pi_pmu_domain_e domain, pi_pmu_domain_state_e state);

/**
 * \brief Change power domain state.
 *
 * This function is used to power on/off a power domain(cf \ref pi_pmu_domain_e).
 * Same as pi_pmu_power_domain_change(), with this function, user has the possibilty
 * to do other things while waiting for the end of power domain change.
 *
 * \param pd             Power domain.
 * \param state          Power ON/OF.
 * \param task           Event task used to notify when action is completed.
 */
void pi_pmu_power_domain_change_async(pi_pmu_domain_e domain, pi_pmu_domain_state_e state,
                                      pi_task_t *task);

/**
 * \brief Get event that woke up SoC.
 *
 * This function is used to know which event woke up the chip from deep sleep.
 *
 * \param domain         Power domain(SoC).
 *
 * \return WAKEUP_EVENT  Event that woke up the chip(\ref pi_pmu_wakeup_mode_e).
 */
pi_pmu_wakeup_mode_e pi_pmu_wakeup_event_get(pi_pmu_domain_e domain);

/**
 * \brief Get external wakeup pins.
 *
 * This function returns the GPIO pin number that has been used to wakeup the
 * chip, in case chip has been woken up by a GPIO.
 *
 * \param domain         Power domain(SoC).
 *
 * \return GPIO_ID       GPIO pin number.
 */
uint32_t pi_pmu_wakeup_ext_event_get(pi_pmu_domain_e domain);

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif
#endif  /* __PI_CHIPS_GAP9_PMU_H__ */
