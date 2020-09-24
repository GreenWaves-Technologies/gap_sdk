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

#ifndef __PMSIS_DRIVERS_GPIO_H__
#define __PMSIS_DRIVERS_GPIO_H__

#include "pmsis/pmsis_types.h"
#include "pmsis/drivers/pad.h"


/**
 * @ingroup groupDrivers
 *
 * @defgroup GPIO GPIO
 *
 * \brief GPIO (General Peripheral Input/Output)
 *
 * The GPIO driver provides support for controlling GPIOs.
 * The available GPIOs are listed in pmsis/chips folder.
 *
 * @addtogroup GPIO
 * @{
 */


/**
 * \cond IMPLEM
 */
#define PI_GPIO_PULL_OFFSET     ( 0 )
#define PI_GPIO_DRIVE_OFFSET    ( 1 )
#define PI_GPIO_MODE_OFFSET     ( 2 )

#define PI_GPIO_IRQ_TYPE_OFFSET ( 0 )
/**
 * \endcond
 */


/**
 * \enum pi_gpio_flags_e
 * \brief GPIO configuration flags.
 *
 * Flags to configure gpio : input/output mode, drive strength, pull activation.
 */
typedef enum
{
    PI_GPIO_PULL_DISABLE        = (0 << PI_GPIO_PULL_OFFSET),  /*!< Disable pull. */
    PI_GPIO_PULL_ENABLE         = (1 << PI_GPIO_PULL_OFFSET),  /*!< Enable pull. */
    PI_GPIO_DRIVE_STRENGTH_LOW  = (0 << PI_GPIO_DRIVE_OFFSET), /*!< Low drive strength. */
    PI_GPIO_DRIVE_STRENGTH_HIGH = (1 << PI_GPIO_DRIVE_OFFSET), /*!< High drive strength. */
    PI_GPIO_INPUT               = (0 << PI_GPIO_MODE_OFFSET),  /*!< GPIO is an input. */
    PI_GPIO_OUTPUT              = (1 << PI_GPIO_MODE_OFFSET)   /*!< GPIO is an output. */
} pi_gpio_flags_e;

/**
 * \enum pi_gpio_notif_e
 * \brief Sensitivity of a GPIO for notifications.
 *
 * This is used to tell which GPIO value modification will trigger a
 * notification(IRQ).
 */
typedef enum
{
    PI_GPIO_NOTIF_FALL = (0 << PI_GPIO_IRQ_TYPE_OFFSET), /*!< IRQ are sent on a falling
                                                           edge on the GPIO value. */
    PI_GPIO_NOTIF_RISE = (1 << PI_GPIO_IRQ_TYPE_OFFSET), /*!< IRQ are sent on a rising
                                                           edge on the GPIO value. */
    PI_GPIO_NOTIF_EDGE = (2 << PI_GPIO_IRQ_TYPE_OFFSET), /*!< IRQ are sent on a rising
                                                         or a falling edge on the GPIO value. */
    PI_GPIO_NOTIF_NONE = (3 << PI_GPIO_IRQ_TYPE_OFFSET)  /*!< No IRQ. */
} pi_gpio_notif_e;


/**
 * \struct pi_gpio_conf
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

/**
 * \brief Initialize a GPIO configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * device is opened.
 *
 * \param conf           A pointer to the GPIO configuration.
 */
void pi_gpio_conf_init(struct pi_gpio_conf *conf);

/**
 * \brief Open a GPIO device.
 *
 * This function must be called before the GPIO device can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 * This operation must be done once for each GPIO port, i.e. for every group
 * of 32 GPIOs. The opened device can then be used to drive the 32 GPIOs.
 *
 * \param device         A pointer to the device structure of the device to open.
 *
 * \retval 0             If the operation is successfull,
 * \retval ERRNO         An error code otherwise.
 *
 * \note The device structure is allocated by the caller and must be kept alive
 *       until the device is closed.
 */
int pi_gpio_open(struct pi_device *device);

/**
 * \brief Close an opened GPIO device.
 *
 * This function closes a GPIO device. If memory was allocated, it is freed.
 *
 * \param device         A pointer to the device structure of the device to open.
 *
 * \note Closing a GPIO device will disable all GPIOs.
 */
void pi_gpio_close(struct pi_device *device);

/**
 * \brief Configure a GPIO.
 *
 * This function can be used to configure several aspects of a GPIO, like
 * the direction.
 *
 * \param device         A pointer to the device structure this GPIO belongs to.
 * \param gpio           GPIO enumerate type or GPIO number within the port (from 0 to 31).
 * \param flags          A bitfield of flags specifying how to configure the GPIO.
 *
 * \retval 0             If the operation is successfull.
 * \retval ERRNO         An error code otherwise.
 */
int pi_gpio_pin_configure(struct pi_device *device, pi_gpio_e gpio,
                          pi_gpio_flags_e flags);

/**
 * \brief Set value of a single GPIO.
 *
 * This function can be used to change the value of a single GPIO.
 *
 * \param device         A pointer to the device structure this GPIO belongs to.
 * \param pin            The GPIO number within the port (from 0 to 31).
 * \param value          The value to be set. This can be either 0 or 1.
 *
 * \retval 0             If the operation is successfull.
 * \retval ERRNO         An error code otherwise.
 *
 * \note The pin parameter can be a GPIO enumerate type \ref pi_gpio_e.
 * \note This function is to be used on GPIO pins configured as \ref PI_GPIO_OUTPUT.
 */
int pi_gpio_pin_write(struct pi_device *device, uint32_t pin, uint32_t value);

/**
 * \brief Get value of a single GPIO.
 *
 * This function can be used to get the value of a single GPIO.
 * It will store the current input value of a GPIO pin into a given buffer.
 *
 * \param device         A pointer to the device structure this GPIO belongs to.
 * \param pin            The GPIO number within the port (from 0 to 31).
 * \param value          A pointer to the variable where the GPIO value should be
 *                       returned. The value will be either 0 or 1.
 *
 * \retval 0             If the operation is successfull.
 * \retval ERRNO         An error code otherwise.
 *
 * \note The pin parameter can be a GPIO enumerate type \ref pi_gpio_e.
 * \note This function should be used on GPIO pins configured as \ref PI_GPIO_INPUT.
 *       Although this function can be used to retrieve current value set on a
 *       GPIO pin configured as \ref PI_GPIO_OUTPUT.
 */
int pi_gpio_pin_read(struct pi_device *device, uint32_t pin, uint32_t *value);

/**
 * \brief Configure notifications for a GPIO.
 *
 * This function can be used to configure how to be notified
 * by GPIO value modifications. By default, notifications will be buffered
 * and can be read and cleared.
 *
 * \param device         A pointer to the device structure this GPIO belongs to.
 * \param pin            The GPIO number within the port (from 0 to 31).
 * \param flags          The flags to configure how the notification should be
 *                       triggered.
 *
 * \note The pin parameter can be a GPIO enumerate type \ref pi_gpio_e.
 */
void pi_gpio_pin_notif_configure(struct pi_device *device, uint32_t pin,
                                 pi_gpio_notif_e flags);

/**
 * \brief Clear notification for a GPIO.
 *
 * This function can be used to clear the notification of a GPIO. A GPIO
 * notification is buffered and ths function must be called to clear it so
 * that a new one can be seen.
 *
 * \param device         A pointer to the device structure this GPIO belongs to.
 * \param pin            The GPIO number within the port (from 0 to 31).
 *
 * \note The pin parameter can be a GPIO enumerate type \ref pi_gpio_e.
 */
void pi_gpio_pin_notif_clear(struct pi_device *device, uint32_t pin);

/**
 * \brief Get the value of a notification for a GPIO.
 *
 * This function can be used to get the value of a notification of a GPIO.
 * It returns 1 if at least one notification was received since the last time
 * it was cleared.
 * Reading the notification does not clear it.
 *
 * \param device         A pointer to the device structure this GPIO belongs to.
 * \param pin            The GPIO number within the port (from 0 to 31).
 *
 * \retval 0             If a notification was received.
 * \retval 1             Otherwise.
 *
 * \note The pin parameter can be a GPIO enumerate type \ref pi_gpio_e.
 */
int pi_gpio_pin_notif_get(struct pi_device *device, uint32_t pin);

/**
 * \brief Attach an event task callback to a GPIO pin.
 *
 * This function is used to attach a callback that will be called when a GPIO
 * triggers an IRQ.
 *
 * This callback is executed by the event kernel.
 *
 * \param device         A pointer to the device structure this GPIO belongs to.
 * \param pin            The GPIO number within the port (from 0 to 31).
 * \param task           Event task executed when an IRQ is triggered.
 * \param flags          The flags to configure how the notification should be
 *                       triggered.
 *
 * \retval 0             If the operation is successfull.
 * \retval ERRNO         An error code otherwise.
 *
 * \note The pin parameter can be a GPIO enumerate type \ref pi_gpio_e.
 */
int pi_gpio_pin_task_add(struct pi_device *device, uint32_t pin, pi_task_t *task,
                         pi_gpio_notif_e flags);

/**
 * \brief Remove an event task callback attached to a GPIO pin.
 *
 * This function is used to remove an attached callback to a GPIO pin.
 * If a IRQ is triggered on the given pin, after removal of the event task, no
 * handler will executed.
 *
 * \param device         A pointer to the device structure this GPIO belongs to.
 * \param pin            The GPIO number within the port (from 0 to 31).
 *
 * \retval 0             If the operation is successfull.
 * \retval ERRNO         An error code otherwise.
 *
 * \note The pin parameter can be a GPIO enumerate type \ref pi_gpio_e.
 */
int pi_gpio_pin_task_remove(struct pi_device *device, uint32_t pin);

/**
 * \struct pi_gpio_callback_t
 *
 * \brief GPIO callback struct.
 *
 * This structure is used by IRQ handler to replace regular GPIO IRQ handler.
 */
typedef struct pi_gpio_callback_s
{
    uint32_t pin_mask;               /*!< Mask of GPIO pins. */
    pi_callback_func_t handler;      /*!< Callback handler. */
    void *args;                      /*!< Callback user args. */
    struct pi_gpio_callback_s *next; /*!< Next callback pointer. */
    struct pi_gpio_callback_s *prev; /*!< Previous callback pointer. */
} pi_gpio_callback_t;

/**
 * \brief Init GPIO callback.
 *
 * Intialize a GPIO callback with the handler to call and the user args.
 *
 * \param cb             Pointer to callback to initialize.
 * \param pin_mask       Mask of GPIO pins.
 * \param handler        Callback function.
 * \param arg            Callback function arg.
 */
static inline void pi_gpio_callback_init(pi_gpio_callback_t *cb,
                                         uint32_t pin_mask,
                                         pi_callback_func_t handler,
                                         void *arg);

/**
 * \brief Attach a callback.
 *
 * Add an application callback to a GPIO device.
 * Multiple callbacks can be attached to a device, and to a GPIO pin.
 *
 * \param device         Pointer to device struct.
 * \param cb             Callback to add to the list.
 *
 * \retval 0             If operation is successfull.
 * \retval ERRNO         An error code otherwise.
 *
 * \note The callback can initialized with pi_gpio_callback_init().
 * \note Using callbacks is incompatible with event handling model(using event tasks
 *       cf. pi_gpio_pin_task_add()).
 * \note A callback is executed when a GPIO's corresponding bit is set in the
 *       callbask mask.
 */
int pi_gpio_callback_add(struct pi_device *device, pi_gpio_callback_t *cb);

/**
 * \brief Remove a callback.
 *
 * Remove an application callback from a GPIO device.
 *
 * \param device         Pointer to device struct.
 * \param cb             Callback to remove from the list.
 *
 * \retval 0             If operation is successfull.
 * \retval ERRNO         An error code otherwise.
 */
int pi_gpio_callback_remove(struct pi_device *device, pi_gpio_callback_t *cb);


/**
 * @} end of GPIO
 */


/**
 * \cond IMPLEM
 */
int pi_gpio_mask_configure(struct pi_device *device, uint32_t mask,
                           pi_gpio_flags_e flags);

int pi_gpio_mask_write(struct pi_device *device, uint32_t mask, uint32_t value);

int pi_gpio_mask_read(struct pi_device *device, uint32_t mask, uint32_t *value);

int pi_gpio_mask_task_add(struct pi_device *device, uint32_t mask,
                          pi_task_t *task, pi_gpio_notif_e flags);

int pi_gpio_mask_task_remove(struct pi_device *device, uint32_t mask);


static inline void pi_gpio_callback_init(pi_gpio_callback_t *cb,
                                         uint32_t pin_mask,
                                         pi_callback_func_t handler,
                                         void *args)
{
    cb->pin_mask = pin_mask;
    cb->handler = handler;
    cb->args = args;
    cb->next = NULL;
    cb->prev = NULL;
}
/**
 * \endcond
 */


#endif  /* __PMSIS_DRIVERS_GPIO_H__ */
