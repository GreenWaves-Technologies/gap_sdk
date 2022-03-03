/*
 * Copyright (C) 2021 GreenWaves Technologies
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

#include "pmsis.h"

#pragma once

/**
 * @addtogroup ADC
 * @{
 */

/**
 * @defgroup ADS1014 ADS1014
 *
 * TI ADS1014 Analog-To-Digital-Converter
 *
 * @warning Support for comparator IRQ handling is not implemented.
 */

/**
 * @addtogroup ADS1014
 * @{
 */

/**
 * ADS1014 PGA (Programmable Gain Amplifier) values
 *
 * It is the range of the measured value.
 * The absolute value of the measured voltage will never go above the
 * power supply voltage value.
 */
enum ads1014_pga {
    ADS1014_PGA_FSR_6V144 = 0x0,
    ADS1014_PGA_FSR_4V096 = 0x1,
    ADS1014_PGA_FSR_2V048 = 0x2,
    ADS1014_PGA_FSR_1V024 = 0x3,
    ADS1014_PGA_FSR_0V512 = 0x4,
    ADS1014_PGA_FSR_0V256 = 0x5,
};

/**
 * ADS1014 operating mode values
 */
enum ads1014_operating_mode {
    /** The ADC measures continuously. */
    ADS1014_OPERATING_MODE_CONTINUOUS  = 0x0,
    /** The ADC only measures once, and goes back to power-saving mode */
    ADS1014_OPERATING_MODE_SINGLE_SHOT = 0x1,
};

/**
 * ADS1014 Sampling rate (samples per second)
 */
enum ads1014_data_rate {
    ADS1014_DATA_RATE_SPS_128  = 0x0,
    ADS1014_DATA_RATE_SPS_250  = 0x1,
    ADS1014_DATA_RATE_SPS_490  = 0x2,
    ADS1014_DATA_RATE_SPS_920  = 0x3,
    ADS1014_DATA_RATE_SPS_1600 = 0x4,
    ADS1014_DATA_RATE_SPS_2400 = 0x5,
    ADS1014_DATA_RATE_SPS_3300 = 0x6,
};

/**
 * ADS1014 Comparator mode
 */
enum ads1014_comparator_mode {
    /**
     * The comparator triggers when the measured value goes above the high
     * threshold, and resets when the value goes below the low threshold.
     */
    ADS1014_COMPARATOR_MODE_TRADITIONAL = 0x0,
    /**
     * The comparator triggers if the measured value goes outside the window,
     * i.e. above the high threshold or below the low threshold.
     */
    ADS1014_COMPARATOR_MODE_WINDOW      = 0x1,
};

/**
 * ADS1014 Alert/Ready comparator pin polarity
 *
 * Controls the ADC Alert/Ready pin active polarity
 */
enum ads1014_comparator_polarity {
    ADS1014_COMPARATOR_POLARITY_ACTIVE_LOW  = 0x0,
    ADS1014_COMPARATOR_POLARITY_ACTIVE_HIGH = 0x1,
};

/**
 * ADS1014 comparator latching mode
 *
 * Determines whether the comparator latches after triggering.
 * When the comparator is set to latch, it will only be cleared by reading
 * the ADC measured value.
 */
enum ads1014_comparator_latch {
    ADS1014_COMPARATOR_LATCH_DISABLED = 0x0,
    ADS1014_COMPARATOR_LATCH_ENABLED  = 0x1,
};

/**
 * ADS1014 comparator status
 *
 * Determines after how many out-of-bounds conversions the comparator will
 * trigger.
 */
enum ads1014_comparator_status {
    /** triggers after 1 conversion */
    ADS1014_COMPARATOR_STATUS_ASSERT_ONE   = 0x0,
    /** triggers after 2 out-of-bounds conversions */
    ADS1014_COMPARATOR_STATUS_ASSERT_TWO   = 0x1,
    /** triggers after 3 out-of-bounds conversions */
    ADS1014_COMPARATOR_STATUS_ASSERT_THREE = 0x2,
    /** the comparator is disabled */
    ADS1014_COMPARATOR_STATUS_DISABLED     = 0x3,
};

/* @brief Structure holding ADS1014 configuration */
struct pi_ads1014_conf
{
    /** I2C interface which is connected to the ADC */
    uint8_t i2c_itf;
    /** Address of the ADC */
    uint8_t i2c_addr;

    /** ADC operating mode (single or continous) */
    enum ads1014_operating_mode operating_mode;
    /** range of the measured value */
    enum ads1014_pga pga;
    /** sampling rate */
    enum ads1014_data_rate data_rate;

    /** ADC comparator status (enabled&trigger conditions, or disabled) */
    enum ads1014_comparator_status comparator_status;
    /** ADC comparator mode (traditional or window) */
    enum ads1014_comparator_mode comparator_mode;
    /** ADC comparator latch setting */
    enum ads1014_comparator_latch comparator_latch;
    /** ADC comparator triggered polarity */
    enum ads1014_comparator_polarity comparator_polarity;
};

/**
 * @brief Initialize an ADS1014 configuration with default values.
 *
 * The structure containing the configuration must be kept alive until
 * the device is opened.
 * It can only be called from fabric-controller side.
 *
 * @param[inout] conf Pointer to the device configuration.
 */
void pi_ads1014_conf_init(struct pi_ads1014_conf *conf);

/**
 * Open a ADS1014 device
 *
 * @param[inout] device pointer to the ADS1014 device
 *
 * @return PI_OK if operation was sucessful,
 *         an error code otherwise
 */
int pi_ads1014_open(pi_device_t *device);

/**
 * Close a ADS1014 device
 *
 * @param[inout] device pointer to the ADS1014 device
 */
void pi_ads1014_close(pi_device_t *device);


/**
 * Read the value measured by the ADC.
 *
 * @param[in] device pointer to the ads1014 device
 * @param[out] value value in mV returned by the ADC
 *
 * @return PI_OK if operation was successful,
 *         an error code otherwise
 */
int pi_ads1014_read(pi_device_t *device, float *value);

/**
 * Set the comparator thresholds (low and high)
 *
 * @param[in] device pointer to the ads1014 device
 * @param[in] threshold_low new value for comparator low threshold (in mV)
 * @param[in] threshold_high new value for comparator high threshold (in mV)
 *
 * @return PI_OK if operation was successful,
 *         an error code otherwise
 */
int pi_ads1014_set_comparator_thresholds(pi_device_t *device,
        float threshold_low, float threshold_high);


/**
 * @}
 */

/**
 * @}
 */
