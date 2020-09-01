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

#ifndef __PMSIS_DRIVERS_PMU_H__
#define __PMSIS_DRIVERS_PMU_H__

/**
 * \ingroup groupDrivers
 *
 * \defgroup PMU PMU
 *
 * \brief Power Management Unit.
 *
 * The PMU API provides support for power management of each domains,
 * sleep and wake up handling.
 *
 * \addtogroup PMU
 * \{
 */


/**
 * \enum pi_pmu_domain_state_e
 *
 * \brief Domain state.
 */
typedef enum
{
    PI_PMU_DOMAIN_STATE_OFF = 0, /*!< Domain is off. */
    PI_PMU_DOMAIN_STATE_ON  = 1  /*!< Domain is on. */
} pi_pmu_domain_state_e;

/**
 * \enum pi_pmu_sleep_mode_e
 *
 * \brief Sleep mode
 */
typedef enum
{
    PI_PMU_MODE_DEEP_SLEEP     = 0, /*!< Deep sleep. Cold boot at reboot. */
    PI_PMU_MODE_RET_DEEP_SLEEP = 1  /*!< Sleep/retentive mode. Memories can be saved. */
} pi_pmu_sleep_mode_e;

/**
 * \enum pi_pmu_boot_state_e
 *
 * \brief Wakeup boot state
 */
typedef enum
{
    PI_PMU_BOOT_COLD           = 0, /*!< Cold boot : reset boot, powerup boot. */
    PI_PMU_BOOT_DEEP_SLEEP     = 1, /*!< Warm boot : boot after sleep, cold boot after. */
    PI_PMU_BOOT_RET_DEEP_SLEEP = 2  /*!< Warm boot : boot after deep sleep, saved FLL and PMU config are restored. */
} pi_pmu_boot_state_e;

/**
 * \enum pi_pmu_wakeup_mode_e
 *
 * \brief External wake up device.
 */
typedef enum
{
    PI_PMU_WAKEUP_RTC  = 0,     /*!< Wake up by RTC. */
    PI_PMU_WAKEUP_GPIO = 1      /*!< Wake up by GPIO. */
} pi_pmu_wakeup_mode_e;

/**
 * \enum pi_pmu_gpio_wakeup_event_e
 *
 * \brief GPIO wake up event
 */
typedef enum
{
    PI_PMU_GPIO_RISE = 0,       /*!< Wake up on rising edge of GPIO. */
    PI_PMU_GPIO_FALL = 1,       /*!< Wake up on falling edge of GPIO. */
    PI_PMU_GPIO_HIGH = 2,       /*!< Wake up high. */
    PI_PMU_GPIO_LOW  = 3        /*!< Wake up low. */
} pi_pmu_gpio_wakeup_event_e;

/**
 */
typedef enum
{
    PI_PMU_WAKEUP_STATE_POWER_LOW  = 0, /*!< At wakeup, low power. */
    PI_PMU_WAKEUP_STATE_POWER_HIGH = 1  /*!< At wakeup, high power. */
} pi_pmu_wakeup_power_state_e;

/**
 * \struct pi_pmu_sleep_conf_s
 *
 * \brief Sleep config structure.
 */
struct pi_pmu_sleep_conf_s
{
    uint8_t gpio_pin;                        /*!< GPIO pin used for wake up. */
    pi_pmu_domain_state_e pmu_cluster_state; /*!< Cluster state at wakeup. */
    pi_pmu_wakeup_power_state_e power_state; /*!< Power state when waking up. */
    pi_pmu_wakeup_mode_e wakeup;             /*!< 0 for RTC, 1 for GPIO */
    pi_pmu_gpio_wakeup_event_e gpio_notif;   /*!< GPIO wake up event. */
    pi_pmu_sleep_mode_e sleep_mode;          /*!< Sleep mode. */
};

/**
 * \brief Set voltage.
 *
 * This function allows to set voltage of a given power domain.
 *
 * \param domain         Power domain to configure.
 * \param voltage        Voltage to set, in mV.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 *
 * \note When changing frequency, voltage may need to be changed first.
 */
int pi_pmu_voltage_set(pi_pmu_domain_e domain, uint32_t voltage);

/**
 * \brief Get current PMU state.
 *
 * This function is used to get current state of PMU.
 *
 * \param domain         Power domain to get current state.
 *
 * \return \ref pi_pmu_domain_state_e Power domain state.
 */
int pi_pmu_state_get(pi_pmu_domain_e domain);

/**
 * \brief Get current boot state.
 *
 * This function is used to get current boot state of PMU.
 *
 * \param domain         Power domain to get boot state.
 *
 * \return \ref pi_pmu_boot_state_e Wakeup boot state.
 */
int pi_pmu_boot_state_get(pi_pmu_domain_e domain);

/**
 * \brief Sleep mode setting.
 *
 * This function is to be called prior to enable sleep mode.
 *
 * \param domain         Power domain to configure.
 * \param conf           Pointer to struct pi_pmu_sleep_conf_s.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 *
 * \note Waking device should be set up before calling this function(RTC or GPIO).
 */
int pi_pmu_sleep_mode_set(pi_pmu_domain_e domain, struct pi_pmu_sleep_conf_s *conf);

/**
 * \brief Activate sleep mode.
 *
 * \param domain         Power domain to configure.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 */
int pi_pmu_sleep_mode_enable(pi_pmu_domain_e domain);

/**
 * \} addtogroup PMU
 */

#endif  /* __PMSIS_DRIVERS_PMU_H__ */
