/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __PI_DRIVERS_GPIO_H__
#define __PI_DRIVERS_GPIO_H__

#include "pmsis/pmsis_types.h"

/**
* @ingroup groupDrivers
*/



/**
 * @defgroup GPIO GPIO
 *
 * The GPIO driver provides support for controlling GPIOs.
 *
 */

/**
 * @addtogroup GPIO
 * @{
 */

/**@{*/


/** \enum pi_gpio_flags_e
 * \brief GPIO configuration flags.
 *
 * This is used to tell if the GPIO can receive or transmit a value.
 */
typedef enum
{
    PI_GPIO_INPUT = 0<<1,               /*!< The GPIO is an input, the chip can
      receive a value. */
    PI_GPIO_OUTPUT = 1<<1,              /*!< The GPIO is an output, the chip
      can transmit a value. */
    PI_GPIO_PULL_DISABLE = 0<<0,        /*!< Disable pull.  */
    PI_GPIO_PULL_ENABLE  = 1<<0,        /*!< Enable pull.  */
    PI_GPIO_DRIVE_STRENGTH_LOW  = 0<<2, /*!< Low drive strength.  */
    PI_GPIO_DRIVE_STRENGTH_HIGH = 1<<2  /*!< High drive strength. */
} pi_gpio_flags_e;

/** \enum pi_gpio_notif_e
 * \brief Sensitivity of a GPIO for notifications.
 *
 * This is used to tell which GPIO value modification will trigger a
 * notification.
 */
typedef enum
{
    PI_GPIO_NOTIF_FALL = 0x0,    /*!< Notifications are sent when there is both
      a rising edge or a falling edge on the GPIO value. */
    PI_GPIO_NOTIF_RISE = 0x1,    /*!< Notifications are sent when there is a
      rising edge on the GPIO value. */
    PI_GPIO_NOTIF_EDGE = 0x2,    /*!< Notifications are sent when there is a
      falling edge on the GPIO value. */
    PI_GPIO_NOTIF_NONE = 0x3     /*!< No notification is sent. */
} pi_gpio_notif_e;


/** \struct pi_gpio_conf
 * \brief GPIO configuration structure.
 *
 * This structure is used to pass the desired GPIO configuration to the
 * runtime when opening the device.
 */
struct pi_gpio_conf
{
    pi_device_e device; /*!< Interface type. */
    int32_t port;       /*!< GPIO port. Each port can contain up to 32 GPIOs. */
};

/** \brief Initialize a GPIO configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * device is opened.
 *
 * \param conf A pointer to the GPIO configuration.
 */
void pi_gpio_conf_init(struct pi_gpio_conf *conf);

/** \brief Open a GPIO device.
 *
 * This function must be called before the GPIO device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 * This operation must be done once for each GPIO port, i.e. for every group
 * of 32 GPIOs. The opened device can then be used to drive the 32 GPIOs.
 *
 * \param device    A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_gpio_open(struct pi_device *device);

/** \brief Configure a GPIO.
 *
 * This function can be used to configure several aspects of a GPIO, like
 * the direction.
 *
 * \param device A pointer to the device structure this GPIO belongs to.
 * \param pin    The GPIO number within the port (from 0 to 31).
 * \param flags  A bitfield of flags specifying how to configure the GPIO.
 */
int pi_gpio_pin_configure(struct pi_device *device, int pin,
    pi_gpio_flags_e flags);

/** \brief Set value of a single GPIO.
 *
 * This function can be used to change the value of a single GPIO.
 *
 * \param device A pointer to the device structure this GPIO belongs to.
 * \param pin    The GPIO number within the port (from 0 to 31).
 * \param value  The value to be set. This can be either 0 or 1.
 */
int pi_gpio_pin_write(struct pi_device *device, int pin, uint32_t value);

/** \brief Get value of a single GPIO.
 *
 * This function can be used to get the value of a single GPIO.
 *
 * \param device A pointer to the device structure this GPIO belongs to.
 * \param pin   The GPIO number. Must be between 0 and 31.
 * \param value   A pointer to the variable where the GPIO value should be 
 *   returned. The value will be either 0 or 1.
 * \return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_gpio_pin_read(struct pi_device *device, int pin, uint32_t *value);

/** \brief Configure notifications for a GPIO.
 *
 * This function can be used to configure how to be notified
 * by GPIO value modifications. By default, notifications will be buffered
 * and can be read and cleared.
 *
 * \param device A pointer to the device structure this GPIO belongs to.
 * \param pin   The GPIO number. Must be between 0 and 31.
 * \param flags  The flags to configure how the notification should be
 *   triggered.
 */
void pi_gpio_pin_notif_configure(struct pi_device *device, int pin,
    pi_gpio_notif_e flags);

/** \brief Clear notification for a GPIO.
 *
 * This function can be used to clear the notification of a GPIO. A GPIO
 * notification is buffered and ths function must be called to clear it so
 * that a new one can be seen.
 *
 * \param device A pointer to the device structure this GPIO belongs to.
 * \param pin   The GPIO number. Must be between 0 and 31.
 */
void pi_gpio_pin_notif_clear(struct pi_device *device, int pin);

/** \brief Get the value of a notification for a GPIO.
 *
 * This function can be used to get the value of a notification of a GPIO.
 * It returns 1 if at least one notification was received since the last time
 * it was cleared.
 * Reading the notification does not clear it.
 *
 * \param device A pointer to the device structure this GPIO belongs to.
 * \param pin   The GPIO number. Must be between 0 and 31.
 * \return       1 if a notification was received, otherwise 0.
 */
int pi_gpio_pin_notif_get(struct pi_device *device, int pin);

//!@}

/**
 * @} end of GPIO
 */



/// @cond IMPLEM

int pi_gpio_mask_configure(struct pi_device *device, uint32_t mask,
    pi_gpio_flags_e flags);

int pi_gpio_mask_write(struct pi_device *device, uint32_t mask, uint32_t value);

int pi_gpio_mask_read(struct pi_device *device, uint32_t mask, uint32_t *value);

int pi_gpio_mask_task_add(struct pi_device *device, uint32_t mask,
    pi_task_t *task, pi_gpio_notif_e flags);

int pi_gpio_mask_task_remove(struct pi_device *device, uint32_t mask);

int pi_gpio_pin_task_add(struct pi_device *device, int pin, pi_task_t *task,
    pi_gpio_notif_e flags);

int pi_gpio_pin_task_remove(struct pi_device *device, int pin);

/// @endcond


#endif  /* __PI_DRIVERS_GPIO_H__ */
