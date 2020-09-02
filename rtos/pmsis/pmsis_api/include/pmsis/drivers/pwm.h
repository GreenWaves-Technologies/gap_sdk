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

#ifndef __PI_DRIVERS_PWM_H__
#define __PI_DRIVERS_PWM_H__

#include "pmsis/pmsis_types.h"

/**
 * @ingroup groupDrivers
 */

/**
 * @defgroup PWM PWM
 */

/**
 * @addtogroup PWM
 * @{
 */

/**
 * \enum pi_pwm_cmd_e
 * \brief PWM timer commands.
 *
 * PWM module timer commands.
 */
typedef enum
{
    PI_PWM_CMD_START  = 0x01, /*!< PWM module timer START command. */
    PI_PWM_CMD_STOP   = 0x02, /*!< PWM module timer STOP command. */
    PI_PWM_CMD_UPDATE = 0x04, /*!< PWM module timer UPDATE command. */
    PI_PWM_CMD_RESET  = 0x08, /*!< PWM module timer RESET command. */
    PI_PWM_CMD_ARM    = 0x10  /*!< PWM module timer ARM command. */
} pi_pwm_cmd_e;


#define PI_PWM_CONFIG_INSEL_OFFSET     ( 0 )  /*!< PWM timer input source selection offset. */
#define PI_PWM_CONFIG_EVT_OFFSET       ( 8 )  /*!< PWM timer event trigger offset. */
#define PI_PWM_CONFIG_CLKSEL_OFFSET    ( 11 ) /*!< PWM timer clock source selection offset. */
#define PI_PWM_CONFIG_UPDOWNSEL_OFFSET ( 12 ) /*!< PWM timer counting method offset. */
#define PI_PWM_CONFIG_PRESCALER_OFFSET ( 16 ) /*!< PWM timer prescaler value offset. */
/**
 * \enum pi_pwm_timer_conf_e
 * \brief PWM timer config.
 *
 * PWM timer configuration.
 */
typedef enum
{
    PI_PWM_EVT_EACH_CLK_CYCLE = ( 0x00 << PI_PWM_CONFIG_EVT_OFFSET ),       /*!< Trigger each clock cycle. */
    PI_PWM_EVT_SOURCE_0       = ( 0x01 << PI_PWM_CONFIG_EVT_OFFSET ),       /*!< Trigger if input source = '0'. */
    PI_PWM_EVT_SOURCE_1       = ( 0x02 << PI_PWM_CONFIG_EVT_OFFSET ),       /*!< Trigger if input source = '1'. */
    PI_PWM_EVT_RISE           = ( 0x03 << PI_PWM_CONFIG_EVT_OFFSET ),       /*!< Trigger on input source rising edge. */
    PI_PWM_EVT_FALL           = ( 0x04 << PI_PWM_CONFIG_EVT_OFFSET ),       /*!< Trigger on input source falling edge. */
    PI_PWM_EVT_EDGE           = ( 0x05 << PI_PWM_CONFIG_EVT_OFFSET ),       /*!< Trigger on input source rising or falling edge. */
    PI_PWM_EVT_RISE_ARMED     = ( 0x06 << PI_PWM_CONFIG_EVT_OFFSET ),       /*!< Trigger : on input source rising edge when armed. */
    PI_PWM_EVT_FALL_ARMED     = ( 0x07 << PI_PWM_CONFIG_EVT_OFFSET ),       /*!< Trigger : on input source falling edge when armed. */
    PI_PWM_CLKSEL_FLL         = ( 0x00 << PI_PWM_CONFIG_CLKSEL_OFFSET ),    /*!< FLL : configurable frequency. */
    PI_PWM_CLKSEL_REFCLK_32K  = ( 0x01 << PI_PWM_CONFIG_CLKSEL_OFFSET ),    /*!< 32kHz Ref Clock. */
    PI_PWM_UPDOWNSEL_ALT      = ( 0x00 << PI_PWM_CONFIG_UPDOWNSEL_OFFSET ), /*!< Mountain shape : Count up then down alternatively. /\/\/\ */
    PI_PWM_UPDOWNSEL_RESET    = ( 0x01 << PI_PWM_CONFIG_UPDOWNSEL_OFFSET )  /*!< Saw shape : Count up then reset to 0. /|/|/|. */
} pi_pwm_timer_conf_e;


/**
 * \enum pi_pwm_ch_config_e
 * \brief PWM channel configuration.
 *
 * Action to perform when a PWM channel's threshold match.
 */
typedef enum
{
    PI_PWM_SET          = 0x00, /*!< Set. Output is high. */
    PI_PWM_TOGGLE_CLEAR = 0x01, /*!< Toggle, next threshold action match is Clear. Output is toggled then next match low. */
    PI_PWM_SET_CLEAR    = 0x02, /*!< Set, next threshold action match is Clear. Output is high then next match low. */
    PI_PWM_TOGGLE       = 0x03, /*!< Toggle. Output is toggled at each match. */
    PI_PWM_CLEAR        = 0x04, /*!< Clear. Output is low. */
    PI_PWM_TOGGLE_SET   = 0x05, /*!< Toggle, next threshold action match is Set. Output is toggled then next match high. */
    PI_PWM_CLEAR_SET    = 0x06  /*!< Clear, next threshold action match is Set. Output is low then next match high. */
} pi_pwm_ch_config_e;

/**
 * \enum pi_pwm_channel_e
 * \brief PWM channels.
 *
 * PWM device channel selection.
 */
typedef enum
{
    PI_PWM_CHANNEL0 = 0x00,  /*!< PWM module channel 0. */
    PI_PWM_CHANNEL1 = 0x01,  /*!< PWM module channel 1. */
    PI_PWM_CHANNEL2 = 0x02,  /*!< PWM module channel 2. */
    PI_PWM_CHANNEL3 = 0x03   /*!< PWM module channel 3. */
} pi_pwm_channel_e;

/**
 * \enum pi_pwm_evt_sel_e
 * \brief Event output selection.
 *
 * Output event selection(channel).
 */
typedef enum
{
    PI_PWM_EVENT_SEL0 = 0x00,   /*!< PWM output event 0. */
    PI_PWM_EVENT_SEL1 = 0x01,   /*!< PWM output event 1. */
    PI_PWM_EVENT_SEL2 = 0x02,   /*!< PWM output event 2. */
    PI_PWM_EVENT_SEL3 = 0x03    /*!< PWM output event 3. */
} pi_pwm_evt_sel_e;

#define PI_PWM_EVENT_OUTPUT(pwm, channel) ( (pwm << 2) | (channel << 0) ) /*!< PWM output event source selection. */
/**
 * \enum pi_pwm_output_evt_e
 * \brief Output event source selection.
 *
 * Ouput event source selection(actual event).
 */
typedef enum
{
    PI_PWM_EVT_PWM_0_CH_0 = PI_PWM_EVENT_OUTPUT(0, 0), /*!< Output event source : PWM_0 CH_0. */
    PI_PWM_EVT_PWM_0_CH_1 = PI_PWM_EVENT_OUTPUT(0, 1), /*!< Output event source : PWM_0 CH_1. */
    PI_PWM_EVT_PWM_0_CH_2 = PI_PWM_EVENT_OUTPUT(0, 2), /*!< Output event source : PWM_0 CH_2. */
    PI_PWM_EVT_PWM_0_CH_3 = PI_PWM_EVENT_OUTPUT(0, 3), /*!< Output event source : PWM_0 CH_3. */
    PI_PWM_EVT_PWM_1_CH_0 = PI_PWM_EVENT_OUTPUT(1, 0), /*!< Output event source : PWM_1 CH_0. */
    PI_PWM_EVT_PWM_1_CH_1 = PI_PWM_EVENT_OUTPUT(1, 1), /*!< Output event source : PWM_1 CH_1. */
    PI_PWM_EVT_PWM_1_CH_2 = PI_PWM_EVENT_OUTPUT(1, 2), /*!< Output event source : PWM_1 CH_2. */
    PI_PWM_EVT_PWM_1_CH_3 = PI_PWM_EVENT_OUTPUT(1, 3), /*!< Output event source : PWM_1 CH_3. */
    PI_PWM_EVT_PWM_2_CH_0 = PI_PWM_EVENT_OUTPUT(2, 0), /*!< Output event source : PWM_2 CH_0. */
    PI_PWM_EVT_PWM_2_CH_1 = PI_PWM_EVENT_OUTPUT(2, 1), /*!< Output event source : PWM_2 CH_1. */
    PI_PWM_EVT_PWM_2_CH_2 = PI_PWM_EVENT_OUTPUT(2, 2), /*!< Output event source : PWM_2 CH_2. */
    PI_PWM_EVT_PWM_2_CH_3 = PI_PWM_EVENT_OUTPUT(2, 3), /*!< Output event source : PWM_2 CH_3. */
    PI_PWM_EVT_PWM_3_CH_0 = PI_PWM_EVENT_OUTPUT(3, 0), /*!< Output event source : PWM_3 CH_0. */
    PI_PWM_EVT_PWM_3_CH_1 = PI_PWM_EVENT_OUTPUT(3, 1), /*!< Output event source : PWM_3 CH_1. */
    PI_PWM_EVT_PWM_3_CH_2 = PI_PWM_EVENT_OUTPUT(3, 2), /*!< Output event source : PWM_3 CH_2. */
    PI_PWM_EVT_PWM_3_CH_3 = PI_PWM_EVENT_OUTPUT(3, 3), /*!< Output event source : PWM_3 CH_3. */
} pi_pwm_output_evt_e;

/**
 * \struct pi_pwm_conf
 * \brief PWM configuration structure.
 *
 * This structure holds PWM configuration. It contains the id of PWM used.
 * It should be kept until closing device.
 */
struct pi_pwm_conf
{
    pi_device_e device;             /*!< Device type. */
    uint8_t pwm_id;                 /*!< PWM ID. */
    pi_pwm_channel_e ch_id;         /*!< PWM channel ID. */
    uint8_t input_src;              /*!< Timer input source. */
    pi_pwm_timer_conf_e timer_conf; /*!< Event trigger, clock source, counting config. */
    uint8_t prescaler;              /*!< Prescaler value. */
} __attribute__((packed));

/**
 * \enum pi_pwm_ioctl_cmd_e
 * \brief PWM ioctl commands.
 *
 * PWM ioctl commands to set timer threshold, change channel configuration,...
 */
typedef enum
{
    PI_PWM_TIMER_COMMAND = 0, /*!< Timer execution command. */
    PI_PWM_TIMER_CONFIG  = 1, /*!< Timer config command. */
    PI_PWM_TIMER_THRESH  = 2, /*!< Timer threshold setting command. */
    PI_PWM_CH_CONFIG     = 3, /*!< PWM channel config command. */
    PI_PWM_EVENT_SET     = 4, /*!< PWM output event set command.  */
    PI_PWM_EVENT_CLEAR   = 5, /*!< PWM output event clear command.  */
    PI_PWM_RESET_FREQ    = 6, /*!< Stop PWM timer and zero frequency. This is to
                                be used when reconfiguring timer frequency that has
                                been set with pi_pwm_duty_cycle_set(). */
    PI_PWM_ATTACH_CB     = 7  /*!< Attach user callback to an IRQ. */
} pi_pwm_ioctl_cmd_e;

/**
 * \struct pi_pwm_ioctl_ch_config
 * \brief PWM ioctl channel config arg struct.
 *
 * This structure is to be used with PI_PWM_TIMER_CH_CONFIG ioctl command.
 * It contains channel configuration information.
 */
struct pi_pwm_ioctl_ch_config
{
    uint16_t ch_threshold;
    pi_pwm_channel_e channel;
    pi_pwm_ch_config_e config;
} __attribute__((packed));

/**
 * \struct pi_pwm_ioctl_evt
 * \brief PWM ioctl event output arg struct.
 *
 * This structure is to be used with PI_PWM_TIMER_EVENT_CLEAR and
 * PI_PWM_TIMER_EVENT_SET ioctl commands.
 * It contains information about whether disable or enable an output and
 * select right output event source.
 */
struct pi_pwm_ioctl_evt
{
    pi_pwm_evt_sel_e evt_sel;
    pi_pwm_output_evt_e evt_output;
} __attribute__((packed));

/**
 * \brief Initialize a PWM configuration structure.
 *
 * This function initializes a PWM configuration structure with default values.
 *
 * \param conf           PWM configuration structure.
 */
void pi_pwm_conf_init(struct pi_pwm_conf *conf);

/**
 * \brief Open a PWM device.
 *
 * This function opens a PWM device. It disables PWM device clock gating,
 * sets up the PWM with given value and enables events of the selected PWM device.
 *
 * \param device         Device structure.
 *
 * \retval 0             If operation is successful.
 * \retval ERR_CODE      Otherwise.
 */
int32_t pi_pwm_open(struct pi_device *device);

/**
 * \brief Close a PWM device.
 *
 * This function closes a PWM device.
 * It enables PWM device clock gating and disable events of the selected PWM device.
 *
 * \param device         Device structure.
 */
void pi_pwm_close(struct pi_device *device);

/**
 * \brief PWM ioctl command.
 *
 * This function allows to send different commands to PWM device.
 * The commands are listed above in pi_pwm_ioctl_cmd_e.
 *
 * Please note that some functions need arguments such as :
 * * PI_PWM_TIMER_COMMAND -> pi_pwm_cmd_e.
 * * PI_PWM_TIMER_CONFIG  -> struct pi_pwm_conf.
 * * PI_PWM_TIMER_THRESH  -> an uint32_t with :
 *                           * high 16 bits = counter end
 *                           * low 16 bits = counter start
 * * PI_PWM_CH_CONFIG     -> struct pi_pwm_ioctl_ch_config.
 * * PI_PWM_EVENT_SET     -> struct pi_pwm_ioctl_evt.
 * * PI_PWM_EVENT_CLEAR   -> struct pi_pwm_ioctl_evt.
 * * PI_PWM_ATTACH_CB     -> struct pi_task.
 *
 * \param device         Device structure.
 * \param cmd            Ioctl command.
 * \param arg            Ioctl command args.
 *
 * \retval -1            If wrong ioctl command.
 * \retval Value         Otherwise return value depending on ioctl command.
 */
int32_t pi_pwm_ioctl(struct pi_device *device, pi_pwm_ioctl_cmd_e cmd, void *arg);

/**
 * \brief Get PWM timer value.
 *
 * This function returns current timer value of a PWM device.
 *
 * \param device         Device structure.
 *
 * \retval Value         PWM timer counter.
 */
uint32_t pi_pwm_counter_get(struct pi_device *device);

/**
 * \brief Start PWM timer.
 *
 * This function starts a PWM timer.
 *
 * \param device         Device structure.
 */
static inline void pi_pwm_timer_start(struct pi_device *device);

/**
 * \brief Stop PWM timer.
 *
 * This function stops a PWM timer.
 *
 * \param device         Device structure.
 */
static inline void pi_pwm_timer_stop(struct pi_device *device);

/**
 * \brief Set PWM frequency and duty_cycle.
 *
 * This function sets PWM frequency and duty cycle(also referred to pulse width
 * measured in seconds).
 *
 * \param device         Device structure.
 * \param frequency      Frequency of PWM.
 * \param duty_cycle     Output high ratio.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         Otherwise error code.
 *
 * \note Frequency is given in Hz, duty cycle in percentage * 100.
 */
int32_t pi_pwm_duty_cycle_set(struct pi_device *device,
                              uint32_t frequency, uint8_t duty_cycle);

/**
 * @} end of PWM
 */

#endif  /* __PI_DRIVERS_PWM_H__ */
