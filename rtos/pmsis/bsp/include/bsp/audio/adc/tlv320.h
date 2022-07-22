/*
 * Copyright (C) 2019 GreenWaves Technologies
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
 * limitations under the License
 */


#pragma once

/**
 * @addtogroup ADC
 * @{
 */

/**
 * @defgroup TLV320 TLV320
 *
 * ADC TLV320
 */

/**
 * @addtogroup TLV320
 * @{
 */



/**
 * @brief Generic inclusion test. Helper on register value validness tests.
 * 
 * @param[in] val Value to test.
 * @param[in] min Range's minimal value (included).
 * @param[in] max Range's maximal value (included).
 */
#define TLV320_IS_INCLUDED(val, min, max) (((val) >= (min)) && ((val) <= (max))) 
      
// TLV320 configurable slave addresses

#define TLV320_SLAVE_ADDRESS_1      ((uint8_t)0b1001100) ///< Default slave address that match the following configuration : ADDR1_MISO = 0 & ADDR0_SCLK = 0
#define TLV320_SLAVE_ADDRESS_2      ((uint8_t)0b1001101) ///< Slave address that match the following configuration : ADDR1_MISO = 0 & ADDR0_SCLK = 1
#define TLV320_SLAVE_ADDRESS_3      ((uint8_t)0b1001110) ///< Slave address that match the following configuration : ADDR1_MISO = 1 & ADDR0_SCLK = 0
#define TLV320_SLAVE_ADDRESS_4      ((uint8_t)0b1001111) ///< Slave address that match the following configuration : ADDR1_MISO = 1 & ADDR0_SCLK = 1

#define TLV320_ADDR0_SLAVE_ADDR_BITSHIFT ((uint8_t)0) ///< TLV320 addr0 pin impact on slave address value bit n°0
#define TLV320_ADDR1_SLAVE_ADDR_BITSHIFT ((uint8_t)1) ///< TLV320 addr1 pin impact on slave address value bit n°1

// TLV320 Pages definition
#define TLV320_PAGE_CONFIG_REG          ((uint8_t)0x00) ///< Register page about configuration register
#define TLV320_PAGE_COEF_BIQUAD_1_6     ((uint8_t)0x02) ///< Register page about programmable coefficients for the biquad 7 to biquad 12 filters
#define TLV320_PAGE_COEF_BIQUAD_7_12    ((uint8_t)0x03) ///< Register page about programmable coefficients for the biquad 7 to biquad 12 filters
#define TLV320_PAGE_COEF_MIXER_1_4      ((uint8_t)0x04) ///< Register page about Programmable coefficients for mixer 1 to mixer 4

#define TLV320_I2C_MSG_SIZE 2                                           ///< TLV320 typical message size (adress & data)
#define TLV320_I2C_WRITE_FLAGS (PI_I2C_XFER_START | PI_I2C_XFER_STOP)   ///< TLV320 I2C write flags. (One start bit & one stop bit)
#define TLV320_I2C_READ_TX_SIZE 1                                       ///< TLV320 I2C read tx size 
#define TLV320_I2C_READ_RX_SIZE 1                                       ///< TLV320 I2C read rx size 
#define TLV320_I2C_MAX_BAUDRATE ((uint32_t)100000)                      ///< TLV320 I2C max baudrate

//! TLV320 Error codes
#define TLV320_NO_ERROR             ((uint8_t)0x00) ///< Code returned when nothing wrong happened  
#define TLV320_GENERIC_ERROR        ((uint8_t)0x01) ///< Code returned when a non specified error occured
#define TLV320_WRITE_ERROR          ((uint8_t)0x02) ///< Code returned when an i2C write error occured
#define TLV320_SLAVEADDRESS_ERROR   ((uint8_t)0x03) ///< Code returned when the slave adress has not been recognized
#define TLV320_READ_ERROR           ((uint8_t)0x04) ///< Code returned when a read operation failed (mostly due to a page cfg write error)

// TLV320 Delays
#define TLV320_SLEEPMODE_WAKEUP_DELAY   1000     ///< Delay needed after recovering from sleep mode. 1ms
#define TLV320_SLEEPMODE_SLEEP_DELAY    1000000  ///< Delay needed after entering to sleep mode. 10ms

// Generic macro about channels
#define TLV320_CHANNEL_1                            ((uint8_t)0x00)  ///< Channel 1 id
#define TLV320_CHANNEL_2                            ((uint8_t)0x01)  ///< Channel 2 id
#define TLV320_CHANNEL_3                            ((uint8_t)0x02)  ///< Channel 3 id
#define TLV320_CHANNEL_4                            ((uint8_t)0x03)  ///< Channel 4 id
#define TLV320_CHANNEL_5                            ((uint8_t)0x04)  ///< Channel 5 id
#define TLV320_CHANNEL_6                            ((uint8_t)0x05)  ///< Channel 6 id
#define TLV320_CHANNEL_7                            ((uint8_t)0x06)  ///< Channel 7 id
#define TLV320_CHANNEL_8                            ((uint8_t)0x07)  ///< Channel 8 id
#define TLV320_CHANNEL_NB                           ((uint8_t)8)     ///< Total amount of channels
#define TLV320_ANALOG_CHANNEL_NB                    ((uint8_t)4)     ///< Number of analog channels

/**
 * @brief       TLV320 analog channel id validness test
 * @param[in]   id Analog channel id to test. Accepted values:
 * @arg         It can be a value from @ref TLV320_CHANNEL_1 and @ref TLV320_CHANNEL_4.
 * @note        TLV320 device only have 4 analog channels.
 */
#define TLV320_IS_VALID_ANALOG_CHANNEL_ID(id)       TLV320_IS_INCLUDED(id, TLV320_CHANNEL_1, TLV320_CHANNEL_4)

/**
 * @brief       TLV320 channel id validness test
 * @param[in]   id channel id to test. Accepted values:
 * @arg         It can be a value from @ref TLV320_CHANNEL_1 and @ref TLV320_CHANNEL_8.
 * @note        TLV320 device only have 4 analog channels.
 */
#define TLV320_IS_VALID_CHANNEL_ID(id)       TLV320_IS_INCLUDED(id, TLV320_CHANNEL_1, TLV320_CHANNEL_8)

/**
 * @brief State command definition
 */
typedef enum
{
    TLV320_DISABLE = 0, //!< Generic value used to disable a feature 
    TLV320_ENABLE = 1   //!< Generic value used to enable a feature
}tlv320_state_t;

#define TLV320_IS_VALID_STATE(state) (((state) == TLV320_DISABLE) || \
                                      ((state) == TLV320_ENABLE))

/**
 * @brief Device's hardware functional mode
 */
typedef enum
{
    TLV320_SHUTDOWN = 0, //!< Generic value used to disable a feature 
    TLV320_ACTIVE = 1   //!< Generic value used to enable a feature
}tlv320_hw_mode_t;

#define TLV320_IS_VALID_HW_MODE(state) (((state) == TLV320_SHUTDOWN) || \
                                        ((state) == TLV320_ACTIVE))

//! @brief Enumeration of TLV320's configuration registers 
typedef enum
{
    TLV320_PAGE_CFG			= 0x00, //!< Device page register address
    TLV320_SW_RESET			= 0x01, //!< Software reset register
    TLV320_SLEEP_CFG		= 0x02, //!< Sleep mode register
    TLV320_SHDN_CFG			= 0x05, //!< Shutdown configuration register
    TLV320_ASI_CFG0			= 0x07, //!< ASI configuration register 0
    TLV320_ASI_CFG1			= 0x08, //!< ASI configuration register 1
    TLV320_ASI_CFG2			= 0x09, //!< ASI configuration register 2
    TLV320_ASI_CH1			= 0x0B, //!< Channel 1 ASI slot configuration register
    TLV320_ASI_CH2			= 0x0C, //!< Channel 2 ASI slot configuration register
    TLV320_ASI_CH3			= 0x0D, //!< Channel 3 ASI slot configuration register
    TLV320_ASI_CH4			= 0x0E, //!< Channel 4 ASI slot configuration register
    TLV320_ASI_CH5			= 0x0F, //!< Channel 5 ASI slot configuration register
    TLV320_ASI_CH6			= 0x10, //!< Channel 6 ASI slot configuration register
    TLV320_ASI_CH7			= 0x11, //!< Channel 7 ASI slot configuration register
    TLV320_ASI_CH8			= 0x12, //!< Channel 8 ASI slot configuration register
    TLV320_MST_CFG0			= 0x13, //!< ASI master mode configuration register 0
    TLV320_MST_CFG1			= 0x14, //!< ASI master mode configuration register 1
    TLV320_ASI_STS			= 0x15, //!< ASI bus clock monitor status register
    TLV320_CLK_SRC			= 0x16, //!< Clock source configuration register 0
    TLV320_PDMCLK_CFG		= 0x1F, //!< PDM clock generation configuration register
    TLV320_PDMIN_CFG		= 0x20, //!< PDM DINx sampling edge register
    TLV320_GPIO_CFG0		= 0x21, //!< GPIO configuration register 0
    TLV320_GPO_CFG0			= 0x22, //!< GPO configuration register 0
    TLV320_GPO_CFG1			= 0x23, //!< GPO configuration register 1
    TLV320_GPO_CFG2			= 0x24, //!< GPO configuration register 2
    TLV320_GPO_CFG3			= 0x25, //!< GPO configuration register 3
    TLV320_GPO_VAL			= 0x29, //!< GPIO, GPO output value register
    TLV320_GPIO_MON			= 0x2A, //!< GPIO monitor value register
    TLV320_GPI_CFG0			= 0x2B, //!< GPI configuration register 0
    TLV320_GPI_CFG1			= 0x2C, //!< GPI configuration register 1
    TLV320_GPI_MON			= 0x2F, //!< GPI monitor value register
    TLV320_INT_CFG			= 0x32, //!< Interrupt configuration register
    TLV320_INT_MASK0		= 0x33, //!< Interrupt mask register 0
    TLV320_INT_LTCH0		= 0x36, //!< Latched interrupt readback register 0
    TLV320_BIAS_CFG			= 0x3B, //!< Bias and ADC configuration register
    TLV320_CH1_CFG0			= 0x3C, //!< Channel 1 configuration register 0
    TLV320_CH1_CFG1			= 0x3D, //!< Channel 1 configuration register 1
    TLV320_CH1_CFG2			= 0x3E, //!< Channel 1 configuration register 2
    TLV320_CH1_CFG3			= 0x3F, //!< Channel 1 configuration register 3
    TLV320_CH1_CFG4			= 0x40, //!< Channel 1 configuration register 4
    TLV320_CH2_CFG0			= 0x41, //!< Channel 2 configuration register 0
    TLV320_CH2_CFG1			= 0x42, //!< Channel 2 configuration register 1
    TLV320_CH2_CFG2			= 0x43, //!< Channel 2 configuration register 2
    TLV320_CH2_CFG3			= 0x44, //!< Channel 2 configuration register 3
    TLV320_CH2_CFG4			= 0x45, //!< Channel 2 configuration register 4
    TLV320_CH3_CFG0			= 0x46, //!< Channel 3 configuration register 0
    TLV320_CH3_CFG1			= 0x47, //!< Channel 3 configuration register 1
    TLV320_CH3_CFG2			= 0x48, //!< Channel 3 configuration register 2
    TLV320_CH3_CFG3			= 0x49, //!< Channel 3 configuration register 3
    TLV320_CH3_CFG4			= 0x4A, //!< Channel 3 configuration register 4
    TLV320_CH4_CFG0			= 0x4B, //!< Channel 4 configuration register 0
    TLV320_CH4_CFG1			= 0x4C, //!< Channel 4 configuration register 1
    TLV320_CH4_CFG2			= 0x4D, //!< Channel 4 configuration register 2
    TLV320_CH4_CFG3			= 0x4E, //!< Channel 4 configuration register 3
    TLV320_CH4_CFG4			= 0x4F, //!< Channel 4 configuration register 4
    TLV320_CH5_CFG2			= 0x52, //!< Channel 5 (PDM only) configuration register 2
    TLV320_CH5_CFG3			= 0x53, //!< Channel 5 (PDM only) configuration register 3
    TLV320_CH5_CFG4			= 0x54, //!< Channel 5 (PDM only) configuration register 4
    TLV320_CH6_CFG2			= 0x57, //!< Channel 6 (PDM only) configuration register 2
    TLV320_CH6_CFG3			= 0x58, //!< Channel 6 (PDM only) configuration register 3
    TLV320_CH6_CFG4			= 0x59, //!< Channel 6 (PDM only) configuration register 4
    TLV320_CH7_CFG2			= 0x5C, //!< Channel 7 (PDM only) configuration register 2
    TLV320_CH7_CFG3			= 0x5D, //!< Channel 7 (PDM only) configuration register 3
    TLV320_CH7_CFG4			= 0x5E, //!< Channel 7 (PDM only) configuration register 4
    TLV320_CH8_CFG2			= 0x61, //!< Channel 8 (PDM only) configuration register 2
    TLV320_CH8_CFG3			= 0x62, //!< Channel 8 (PDM only) configuration register 3
    TLV320_CH8_CFG4			= 0x63, //!< Channel 8 (PDM only) configuration register 4
    TLV320_DSP_CFG0			= 0x6B, //!< DSP configuration register 0
    TLV320_DSP_CFG1			= 0x6C, //!< DSP configuration register 1
    TLV320_DRE_CFG0			= 0x6D, //!< DRE configuration register 0
    TLV320_AGC_CFG0			= 0x70, //!< AGC configuration register 0
    TLV320_IN_CH_EN			= 0x73, //!< Input channel enable configuration register
    TLV320_ASI_OUT_CH_EN   	= 0x74, //!< ASI output channel enable configuration register
    TLV320_PWR_CFG			= 0x75, //!< Power up configuration register
    TLV320_DEV_STS0			= 0x76, //!< Device status value register 0
    TLV320_DEV_STS1			= 0x77, //!< Device status value register 1
    TLV320_I2C_CKSUM		= 0x7E  //!< I2C checksum register
}tlv320_config_register_t;

/**
 * @brief  Configuration register code validness test
 * @param[in] reg Register value to verify.
 */
#define TLV320_IS_VALID_CONFIG_REG(reg) (((reg) == TLV320_PAGE_CFG)			|| \
                                        ((reg) == TLV320_SW_RESET)			|| \
                                        ((reg) == TLV320_SLEEP_CFG)			|| \
                                        ((reg) == TLV320_SHDN_CFG)			|| \
                                        ((reg) == TLV320_ASI_CFG0)			|| \
                                        ((reg) == TLV320_ASI_CFG1)			|| \
                                        ((reg) == TLV320_ASI_CFG2)			|| \
                                        ((reg) == TLV320_ASI_CH1)			|| \
                                        ((reg) == TLV320_ASI_CH2)			|| \
                                        ((reg) == TLV320_ASI_CH3)			|| \
                                        ((reg) == TLV320_ASI_CH4)			|| \
                                        ((reg) == TLV320_ASI_CH5)			|| \
                                        ((reg) == TLV320_ASI_CH6)			|| \
                                        ((reg) == TLV320_ASI_CH7)			|| \
                                        ((reg) == TLV320_ASI_CH8)			|| \
                                        ((reg) == TLV320_MST_CFG0)			|| \
                                        ((reg) == TLV320_MST_CFG1)			|| \
                                        ((reg) == TLV320_ASI_STS)			|| \
                                        ((reg) == TLV320_CLK_SRC)			|| \
                                        ((reg) == TLV320_PDMCLK_CFG)		|| \
                                        ((reg) == TLV320_PDMIN_CFG)			|| \
                                        ((reg) == TLV320_GPIO_CFG0)			|| \
                                        ((reg) == TLV320_GPOCFG0)			|| \
                                        ((reg) == TLV320_GPOCFG1)			|| \
                                        ((reg) == TLV320_GPOCFG2)			|| \
                                        ((reg) == TLV320_GPOCFG3)			|| \
                                        ((reg) == TLV320_GPOVAL)			|| \
                                        ((reg) == TLV320_GPIOMON)			|| \
                                        ((reg) == TLV320_GPICFG0)			|| \
                                        ((reg) == TLV320_GPI_CFG1)			|| \
                                        ((reg) == TLV320_GPI_MON)			|| \
                                        ((reg) == TLV320_INT_CFG)			|| \
                                        ((reg) == TLV320_INT_MASK0)			|| \
                                        ((reg) == TLV320_INT_LTCH0)			|| \
                                        ((reg) == TLV320_BIAS_CFG)			|| \
                                        ((reg) == TLV320_CH1_CFG0)			|| \
                                        ((reg) == TLV320_CH1_CFG1)			|| \
                                        ((reg) == TLV320_CH1_CFG2)			|| \
                                        ((reg) == TLV320_CH1_CFG3)			|| \
                                        ((reg) == TLV320_CH1_CFG4)			|| \
                                        ((reg) == TLV320_CH2_CFG0)			|| \
                                        ((reg) == TLV320_CH2_CFG1)			|| \
                                        ((reg) == TLV320_CH2_CFG2)			|| \
                                        ((reg) == TLV320_CH2_CFG3)			|| \
                                        ((reg) == TLV320_CH2_CFG4)			|| \
                                        ((reg) == TLV320_CH3_CFG0)			|| \
                                        ((reg) == TLV320_CH3_CFG1)			|| \
                                        ((reg) == TLV320_CH3_CFG2)			|| \
                                        ((reg) == TLV320_CH3_CFG3)			|| \
                                        ((reg) == TLV320_CH3_CFG4)			|| \
                                        ((reg) == TLV320_CH4_CFG0)			|| \
                                        ((reg) == TLV320_CH4_CFG1)			|| \
                                        ((reg) == TLV320_CH4_CFG2)			|| \
                                        ((reg) == TLV320_CH4_CFG3)			|| \
                                        ((reg) == TLV320_CH4_CFG4)			|| \
                                        ((reg) == TLV320_CH5_CFG2)			|| \
                                        ((reg) == TLV320_CH5_CFG3)			|| \
                                        ((reg) == TLV320_CH5_CFG4)			|| \
                                        ((reg) == TLV320_CH6_CFG2)			|| \
                                        ((reg) == TLV320_CH6_CFG3)			|| \
                                        ((reg) == TLV320_CH6_CFG4)			|| \
                                        ((reg) == TLV320_CH7_CFG2)			|| \
                                        ((reg) == TLV320_CH7_CFG3)			|| \
                                        ((reg) == TLV320_CH7_CFG4)			|| \
                                        ((reg) == TLV320_CH8_CFG2)			|| \
                                        ((reg) == TLV320_CH8_CFG3)			|| \
                                        ((reg) == TLV320_CH8_CFG4)			|| \
                                        ((reg) == TLV320_DSP_CFG0)			|| \
                                        ((reg) == TLV320_DSP_CFG1)			|| \
                                        ((reg) == TLV320_DRE_CFG0)			|| \
                                        ((reg) == TLV320_AGC_CFG0)			|| \
                                        ((reg) == TLV320_IN_CH_EN)			|| \
                                        ((reg) == TLV320_ASI_OUT_CH_EN)		|| \
                                        ((reg) == TLV320_PWR_CFG)			|| \
                                        ((reg) == TLV320_DEV_STS0)			|| \
                                        ((reg) == TLV320_DEV_STS1)			|| \
                                        ((reg) == TLV320_I2C_CKSUM))

//*************************************************************************************************
//
//         Registers bit definition
//
//*************************************************************************************************

// SW_RESET (0x01) Register field values **********************************************************

#define TLV320_SW_RESET_RESET                       ((uint8_t)0x01) ///< Software Reset

// SLEEP_CFG (0x02) Register field values *********************************************************

#define TLV320_SLEEP_CFG_AREG_SELECT_EXTERNAL       ((uint8_t)0x00) ///< AREG_SELECT bit setting for an external 1.8-V AREG supply 
#define TLV320_SLEEP_CFG_AREG_SELECT_INTERNAL       ((uint8_t)0x01) ///< AREG_SELECT bit setting for an internally generated 1.8-V AREG supply using an on-chip regulator

#define TLV320_SLEEP_CFG_VREF_QCHG_3_5_MS           ((uint8_t)0x00) ///< VREF_QCHG bits setting for a VREF quick-charge duration of 3.5 ms 
#define TLV320_SLEEP_CFG_VREF_QCHG_10_MS            ((uint8_t)0x01) ///< VREF_QCHG bits setting for a VREF quick-charge duration of 10 ms 
#define TLV320_SLEEP_CFG_VREF_QCHG_50_MS            ((uint8_t)0x02) ///< VREF_QCHG bits setting for a VREF quick-charge duration of 50 ms 
#define TLV320_SLEEP_CFG_VREF_QCHG_100_MS           ((uint8_t)0x03) ///< VVREF_QCH bits setting for a GREF quick-charge duration of 100 ms 

#define TLV320_SLEEP_CFG_I2C_BRDCAST_EN_DISABLED    ((uint8_t)0x00) ///< I2C_BRDCAST_EN bit setting for I2C broadcast mode disabled; the I2C slave address is determined based on ADDR pins
#define TLV320_SLEEP_CFG_I2C_BRDCAST_EN_ENABLED     ((uint8_t)0x01) ///< I2C_BRDCAST_EN bit setting for I2C broadcast mode enabled; the I2C slave address is fixed at 1001100

#define TLV320_SLEEP_CFG_SLEEP_ENZ_ENABLED          ((uint8_t)0x00) ///< SLEEP_ENZ bit setting to set the device in sleep mode
#define TLV320_SLEEP_CFG_SLEEP_ENZ_DISABLED         ((uint8_t)0x01) ///< SLEEP_ENZ bit setting to not set the device in sleep mode (active mode)

/**
 * @brief TLV320 sleep mode validness test
 * @param[in] mode Sleep mode value to test. Accepted values:
 * @arg @ref TLV320_SLEEP_CFG_SLEEP_ENZ_ENABLED
 * @arg @ref TLV320_SLEEP_CFG_SLEEP_ENZ_DISABLED
 */
#define TLV320_IS_VALID_SLEEP_MODE(mode)           (((mode) == TLV320_SLEEP_CFG_SLEEP_ENZ_ENABLED) || \
                                                    ((mode) == TLV320_SLEEP_CFG_SLEEP_ENZ_DISABLED))

// ASI_CFG0 (0x07) Register field values **********************************************************

#define TLV320_ASI_CFG0_ASI_FORMAT_TDM              ((uint8_t)0x00) ///< ASI_FORMAT bits setting of Time division multiplexing mode
#define TLV320_ASI_CFG0_ASI_FORMAT_I2S              ((uint8_t)0x01) ///< ASI_FORMAT bits setting of Inter IC sound mode 
#define TLV320_ASI_CFG0_ASI_FORMAT_LJ               ((uint8_t)0x02) ///< ASI_FORMAT bits setting of Left-justified mode

#define TLV320_ASI_CFG0_ASI_WLEN_16BITS             ((uint8_t)0x00) ///< ASI_WLEN bits settings for 16 bits word lenght
#define TLV320_ASI_CFG0_ASI_WLEN_20BITS             ((uint8_t)0x01) ///< ASI_WLEN bits settings for 20 bits word lenght
#define TLV320_ASI_CFG0_ASI_WLEN_24BITS             ((uint8_t)0x02) ///< ASI_WLEN bits settings for 24 bits word lenght
#define TLV320_ASI_CFG0_ASI_WLEN_32BITS             ((uint8_t)0x03) ///< ASI_WLEN bits settings for 32 bits word lenght

#define TLV320_ASI_CFG0_FSYNC_POL_NORMAL            ((uint8_t)0x00) ///< FSYNC_POL bit setting for a normal FSYNC polarity
#define TLV320_ASI_CFG0_FSYNC_POL_INVERTED          ((uint8_t)0x01) ///< FSYNC_POL bit setting for a inverted FSYNC polarity

#define TLV320_ASI_CFG0_BCLK_POL_NORMAL             ((uint8_t)0x00) ///< BCLK_POL bit setting for a normal bit clock polarity
#define TLV320_ASI_CFG0_BCLK_POL_INVERTED           ((uint8_t)0x01) ///< BCLK_POL bit setting for an inverted bit clock polarity

#define TLV320_ASI_CFG0_TX_EDGE_NORMAL              ((uint8_t)0x00) ///< TX_EDGE bit setting for a default edge matching BCLK_POL bit setting
#define TLV320_ASI_CFG0_TX_EDGE_INVERTED            ((uint8_t)0x01) ///< TX_EDGE bit setting for an inverted following edge (half cycle delay) with respect to the default edge setting

#define TLV320_ASI_CFG0_TX_FILL_ZERO                ((uint8_t)0x00) ///< TX_FILL bit setting to always transmit 0 for unused cycles
#define TLV320_ASI_CFG0_TX_FILL_HI_Z                ((uint8_t)0x01) ///< TX_FILL bit setting to use High-Z for unused cycles

/**
 * @brief   TLV320 ASI format validness test  
 * @param[in] format ASI format to test. Accepted values:
 * @arg @ref TLV320_ASI_CFG0_ASI_FORMAT_TDM 
 * @arg @ref TLV320_ASI_CFG0_ASI_FORMAT_I2S 
 * @arg @ref TLV320_ASI_CFG0_ASI_FORMAT_LJ
 */
#define TLV320_IS_VALID_ASI_FORMAT(format)         (((format) == TLV320_ASI_CFG0_ASI_FORMAT_TDM) || \
                                                    ((format) == TLV320_ASI_CFG0_ASI_FORMAT_I2S) || \
                                                    ((format) == TLV320_ASI_CFG0_ASI_FORMAT_LJ)) 
/**
 * @brief TLV320 ASI word lenght validness  
 * @param[in] wlen word lenght to test. Accepted values:
 * @arg @ref TLV320_ASI_CFG0_ASI_WLEN_16BITS 
 * @arg @ref TLV320_ASI_CFG0_ASI_WLEN_20BITS 
 * @arg @ref TLV320_ASI_CFG0_ASI_WLEN_24BITS 
 * @arg @ref TLV320_ASI_CFG0_ASI_WLEN_32BITS
 */
#define TLV320_IS_VALID_ASI_WLEN(wlen)             (((wlen) == TLV320_ASI_CFG0_ASI_WLEN_16BITS) || \
                                                    ((wlen) == TLV320_ASI_CFG0_ASI_WLEN_20BITS) || \
                                                    ((wlen) == TLV320_ASI_CFG0_ASI_WLEN_24BITS) || \
                                                    ((wlen) == TLV320_ASI_CFG0_ASI_WLEN_32BITS))
/**
 * @brief TLV320 FSync polarity validness test
 * @param[in] pol polarity to test. Accepted values:
 * @arg @ref TLV320_ASI_CFG0_FSYNC_POL_NORMAL
 * @arg @ref TLV320_ASI_CFG0_FSYNC_POL_INVERTED
 */
#define TLV320_IS_VALID_ASI_FSYNC_POL(pol)         (((pol) == TLV320_ASI_CFG0_FSYNC_POL_NORMAL) || \
                                                    ((pol) == TLV320_ASI_CFG0_FSYNC_POL_INVERTED))
/**
 * @brief TLV320 Bit clock polarity validness test
 * @param[in] pol polarity to test. Accepted values:
 * @arg @ref TLV320_ASI_CFG0_BCLK_POL_NORMAL 
 * @arg @ref TLV320_ASI_CFG0_BCLK_POL_INVERTED
 */
#define TLV320_IS_VALID_ASI_BCLK_POL(pol)          (((pol) == TLV320_ASI_CFG0_BCLK_POL_NORMAL) || \
                                                    ((pol) == TLV320_ASI_CFG0_BCLK_POL_INVERTED)) 
/**
 * @brief TLV320 ASI output transmission edge validness test
 * @param[in] edge Tx edge to test. Accepted values:
 * @arg @ref TLV320_ASI_CFG0_TX_EDGE_NORMAL
 * @arg @ref TLV320_ASI_CFG0_TX_EDGE_INVERTED
 */
#define TLV320_IS_VALID_ASI_TX_EDGE(edge)          (((edge) == TLV320_ASI_CFG0_TX_EDGE_NORMAL) || \
                                                    ((edge) == TLV320_ASI_CFG0_TX_EDGE_INVERTED))
/**
 * @brief TLV320 data output for any unused cycles validness test
 * @param[in] edge Tx fill value to test. Accepted values:
 * @arg @ref TLV320_ASI_CFG0_TX_FILL_ZERO
 * @arg @ref TLV320_ASI_CFG0_TX_FILL_HI_Z
 */
#define TLV320_IS_VALID_ASI_TX_FILL(edge)          (((edge) == TLV320_ASI_CFG0_TX_FILL_ZERO) || \
                                                    ((edge) == TLV320_ASI_CFG0_TX_FILL_HI_Z))

// ASI_CFG1 (0x08) Register field values **********************************************************

#define TLV320_ASI_CFG1_TX_LSB_FULL_CYCLE               ((uint8_t)0x00) ///< TX_LSB bit setting to transmit the LSB for a full cycle 
#define TLV320_ASI_CFG1_TX_LSB_HALF_CYCLE               ((uint8_t)0x01) ///< TX_LSB bit setting to transmit the LSB for the first half cycle and Hi-Z for the second half cycle

#define TLV320_ASI_CFG1_TX_KEEPER_DISABLED              ((uint8_t)0x00) ///< TX_KEEPER bits setting to always disable the bus keeper
#define TLV320_ASI_CFG1_TX_KEEPER_ENABLED               ((uint8_t)0x01) ///< TX_KEEPER bits setting to always enable the bus keeper
#define TLV320_ASI_CFG1_TX_KEEPER_ENABLED_1_CYCLE       ((uint8_t)0x02) ///< TX_KEEPER bits setting to enable the bus keeper during LSB transmissions only for one cycle
#define TLV320_ASI_CFG1_TX_KEEPER_ENABLED_1_5_CYCLES    ((uint8_t)0x03) ///< TX_KEEPER bits setting to enable the bus keeper during LSB transmissions only for one and half cycle

#define TLV320_ASI_CFG1_TX_OFFSET_MIN                   ((uint8_t)0x00)                                         ///< TX_OFFSET minimal accepted value
#define TLV320_ASI_CFG1_TX_OFFSET_DISABLED              TLV320_ASI_CFG1_TX_OFFSET_MIN                           ///< TX_OFFSET bits setting to disable the offset feature
#define TLV320_ASI_CFG1_TX_OFFSET_SIZE                  ((uint8_t)5)                                            ///< TX_OFFSET setting bits number
#define TLV320_ASI_CFG1_TX_OFFSET_MAX                   ((uint8_t)((1 << TLV320_ASI_CFG1_TX_OFFSET_SIZE) - 1))  ///< TX_OFFSET maximal accepted value
#define TLV320_ASI_CFG1_TX_OFFSET_DEFAULT               ((uint8_t)0x01)                                         ///< TX_OFFSET default value
 
/**
 * @brief TLV320 ASI data output for LSB transmission validness test
 * @param[in] cycle Value to test. Accepted values:
 * @arg @ref TLV320_ASI_CFG1_TX_LSB_FULL_CYCLE
 * @arg @ref TLV320_ASI_CFG1_TX_LSB_HALF_CYCLE
 */
#define TLV320_IS_VALID_ASI_TX_LSB_CYCLE(cycle)    (((cycle) == TLV320_ASI_CFG1_TX_LSB_FULL_CYCLE) || \
                                                    ((cycle) == TLV320_ASI_CFG1_TX_LSB_HALF_CYCLE))
/**
 * @brief TLV320 ASI data output bus keeper validness test
 * @param[in] keeper Value to test. Accepted values:
 * @arg @ref TLV320_ASI_CFG1_TX_KEEPER_DISABLED
 * @arg @ref TLV320_ASI_CFG1_TX_KEEPER_ENABLED
 * @arg @ref TLV320_ASI_CFG1_TX_KEEPER_ENABLED_1_CYCLE
 * @arg @ref TLV320_ASI_CFG1_TX_KEEPER_ENABLED_1_5_CYCLES
 */
#define TLV320_IS_VALID_ASI_TX_KEEPER(keeper)      (((keeper) == TLV320_ASI_CFG1_TX_KEEPER_DISABLED)         || \
                                                    ((keeper) == TLV320_ASI_CFG1_TX_KEEPER_ENABLED)          || \
                                                    ((keeper) == TLV320_ASI_CFG1_TX_KEEPER_ENABLED_1_CYCLE)  || \
                                                    ((keeper) == TLV320_ASI_CFG1_TX_KEEPER_ENABLED_1_5_CYCLES)) 
/**
 * @brief TLV320 ASI data MSB slot 0 offset validness test
 * @param[in] offset Value to test. Accepted values:
 * @arg Between @ref TLV320_ASI_CFG1_TX_OFFSET_MIN and @ref TLV320_ASI_CFG1_TX_OFFSET_MAX
 */
#define TLV320_IS_VALID_TX_OFFSET(offset)          TLV320_IS_INCLUDED(offset, TLV320_ASI_CFG1_TX_OFFSET_MIN, TLV320_ASI_CFG1_TX_OFFSET_MAX)

// ASI_CH1 to ASI_CH8(0x0B to 0x12) Register fields values ****************************************

#define TLV320_ASI_CHx_OUTPUT_PRIMARY               ((uint8_t)0x00) ///< Channel output is on the ASI primary output pin (SDOUT)
#define TLV320_ASI_CHx_OUTPUT_SECONDARY             ((uint8_t)0x01) ///< Channel output is on the ASI secondary output pin (GPIO1 or GPOx)

#define TLV320_TDM_SLOT_MIN                         ((uint8_t)0)  ///< Minimal acceptable slot value
#define TLV320_TDM_SLOT_MAX                         ((uint8_t)63) ///< Maximal acceptable slot value 
#define TLV320_TDM_SLOT_DEFAULT                     ((uint8_t)0)  ///< Default slot value

/**
 * @brief TLV320 ASI data output for LSB transmission validness test
 * @param[in] output ASI output type value to test. Accepted values:
 * @arg @ref TLV320_ASI_CHx_OUTPUT_PRIMARY
 * @arg @ref TLV320_ASI_CHx_OUTPUT_SECONDARY
 */
#define TLV320_IS_VALID_ASI_CHx_OUTPUT(output)     (((output) == TLV320_ASI_CHx_OUTPUT_PRIMARY) || \
                                                    ((output) == TLV320_ASI_CHx_OUTPUT_SECONDARY))
/**
 * @brief Slot ID validness test
 * @param[in] slot Value to test. Accepted values:
 * @arg Between @ref TLV320_SLOT_MIN and @ref TLV320_SLOT_MAX
 */
#define TLV320_IS_VALID_TDM_SLOT_ID(slot)           TLV320_IS_INCLUDED(slot, TLV320_TDM_SLOT_MIN, TLV320_TDM_SLOT_MAX)

// MST_CFG0 (0x13) Register field values **********************************************************

#define TLV320_MSTCFG0_SLAVE                        ((uint8_t)0x00) ///< Device is in slave mode (both BCLK and FSYNC are inputs to the device)
#define TLV320_MSTCFG0_MASTER                       ((uint8_t)0x01) ///< Device is in master mode (both BCLK and FSYNC are generated from the device)

#define TLV320_MSTCFG0_AUTOCLK_ENABLE               ((uint8_t)0x00) ///< Auto clock configuration is enabled (all internal clock divider and PLL configurations are auto derived)
#define TLV320_MSTCFG0_AUTOCLK_DISABLE              ((uint8_t)0x01) ///< Auto clock configuration is disabled (custom mode and device GUI must be used for the device configuration settings)

#define TLV320_MSTCFG0_PLLAUTOMODE_ENABLE           ((uint8_t)0x00) ///< PLL is enabled in auto clock configuration
#define TLV320_MSTCFG0_PLLAUTOMODE_DISABLE          ((uint8_t)0x01) ///< PLL is disabled in auto clock configuration

#define TLV320_MSTCFG0_BCLKFSYNCGATE_DISABLE        ((uint8_t)0x00) ///< Do not gate BCLK and FSYNC
#define TLV320_MSTCFG0_BCLKFSYNCGATE_ENABLE         ((uint8_t)0x01) ///< Force gate BCLK and FSYNC when being transmitted from the device in master mode

#define TLV320_MSTCFG0_FSMODE_48KHZ                 ((uint8_t)0x00) ///< FS is a multiple (or submultiple) of 48 kHz
#define TLV320_MSTCFG0_FSMODE_44_1KHZ               ((uint8_t)0x01) ///< FS is a multiple (or submultiple) of 44.1 kHz

#define TLV320_MSTCFG0_MCLK_12_MHZ                  ((uint8_t)0x00) ///< MCLK (GPIO or GPIx) frequency is set to 12 MHz for the PLL source clock input
#define TLV320_MSTCFG0_MCLK_12_288_MHZ              ((uint8_t)0x01) ///< MCLK (GPIO or GPIx) frequency is set to 12,288 MHz for the PLL source clock input
#define TLV320_MSTCFG0_MCLK_13_MHZ                  ((uint8_t)0x02) ///< MCLK (GPIO or GPIx) frequency is set to 13 MHz for the PLL source clock input
#define TLV320_MSTCFG0_MCLK_16_MHZ                  ((uint8_t)0x03) ///< MCLK (GPIO or GPIx) frequency is set to 16 MHz for the PLL source clock input
#define TLV320_MSTCFG0_MCLK_19_2_MHZ                ((uint8_t)0x04) ///< MCLK (GPIO or GPIx) frequency is set to 19,2 MHz for the PLL source clock input
#define TLV320_MSTCFG0_MCLK_19_68_MHZ               ((uint8_t)0x05) ///< MCLK (GPIO or GPIx) frequency is set to 19,68 MHz for the PLL source clock input
#define TLV320_MSTCFG0_MCLK_24_MHZ                  ((uint8_t)0x06) ///< MCLK (GPIO or GPIx) frequency is set to 24 MHz for the PLL source clock input
#define TLV320_MSTCFG0_MCLK_24_576_MHZ              ((uint8_t)0x07) ///< MCLK (GPIO or GPIx) frequency is set to 24,576 MHz for the PLL source clock input

/**
 * @brief ASI master or slave configuration setting validness test.
 * @param[in] cfg ASI master slave configuration to test. Accepted values:
 * @arg @ref TLV320_MSTCFG0_SLAVE
 * @arg @ref TLV320_MSTCFG0_MASTER
 */
#define TLV320_IS_VALID_ASI_MASTER_SLAVE_CFG(cfg)  (((cfg) == TLV320_MSTCFG0_SLAVE) || \
                                                    ((cfg) == TLV320_MSTCFG0_MASTER))

/**
 * @brief Automatic clock configuration setting validness test.
 * @param[in] cfg ASI master slave configuration to test. Accepted values:
 * @arg @ref TLV320_MSTCFG0_AUTOCLK_DISABLE
 * @arg @ref TLV320_MSTCFG0_AUTOCLK_ENABLE
 */
#define TLV320_IS_VALID_AUTO_CLK(cfg)              (((cfg) == TLV320_MSTCFG0_AUTOCLK_DISABLE) || \
                                                    ((cfg) == TLV320_MSTCFG0_AUTOCLK_ENABLE))

/**
 * @brief Automatic mode PLL setting validness test.
 * @param[in] mode ASI master slave configuration to test. Accepted values:
 * @arg @ref TLV320_MSTCFG0_PLLAUTOMODE_DISABLE
 * @arg @ref TLV320_MSTCFG0_PLLAUTOMODE_ENABLE
 */
#define TLV320_IS_VALID_PLLAUTOMODE(mode)          (((mode) == TLV320_MSTCFG0_PLLAUTOMODE_DISABLE) || \
                                                    ((mode) == TLV320_MSTCFG0_PLLAUTOMODE_ENABLE))

/**
 * @brief BCLK and FSYNC clock gate setting validness test.
 * @param[in] gate ASI master slave configuration to test. Accepted values:
 * @arg @ref TLV320_MSTCFG0_BCLKFSYNCGATE_DISABLE
 * @arg @ref TLV320_MSTCFG0_BCLKFSYNCGATE_ENABLE
 */
#define TLV320_IS_VALID_BCLKFSYNCGATE(gate)        (((gate) == TLV320_MSTCFG0_BCLKFSYNCGATE_DISABLE) || \
                                                    ((gate) == TLV320_MSTCFG0_BCLKFSYNCGATE_ENABLE))

/**
 * @brief Sample rate setting validness test.
 * @param[in] freq ASI master slave configuration to test. Accepted values:
 * @arg @ref TLV320_MSTCFG0_FSMODE_48KHZ
 * @arg @ref TLV320_MSTCFG0_FSMODE_44_1KHZ
 */
#define TLV320_IS_VALID_FSMODE(freq)               (((freq) == TLV320_MSTCFG0_FSMODE_48KHZ) || \
                                                    ((freq) == TLV320_MSTCFG0_FSMODE_44_1KHZ))

/**
 * @brief MCLK frequency validness test.
 * @param[in] freq Master clk to test. Accepted values:
 * @arg @ref TLV320_MSTCFG0_MCLK_12_MHZ
 * @arg @ref TLV320_MSTCFG0_MCLK_12_288_MHZ
 * @arg @ref TLV320_MSTCFG0_MCLK_13_MHZ
 * @arg @ref TLV320_MSTCFG0_MCLK_16_MHZ
 * @arg @ref TLV320_MSTCFG0_MCLK_19_2_MHZ
 * @arg @ref TLV320_MSTCFG0_MCLK_19_68_MHZ
 * @arg @ref TLV320_MSTCFG0_MCLK_24_MHZ
 * @arg @ref TLV320_MSTCFG0_MCLK_24_576_MHZ
 */
#define TLV320_IS_VALID_MCLK(freq)                 (((freq) == TLV320_MSTCFG0_MCLK_12_MHZ)       || \
                                                    ((freq) == TLV320_MSTCFG0_MCLK_12_288_MHZ)   || \
                                                    ((freq) == TLV320_MSTCFG0_MCLK_13_MHZ)       || \
                                                    ((freq) == TLV320_MSTCFG0_MCLK_16_MHZ)       || \
                                                    ((freq) == TLV320_MSTCFG0_MCLK_19_2_MHZ)     || \
                                                    ((freq) == TLV320_MSTCFG0_MCLK_19_68_MHZ)    || \
                                                    ((freq) == TLV320_MSTCFG0_MCLK_24_MHZ)       || \
                                                    ((freq) == TLV320_MSTCFG0_MCLK_24_576_MHZ))

// MST_CFG1 (0x14) Register field values **********************************************************

#define TLV320_MSTCFG1_FSRATE_7_35_OR_8_KHZ         ((uint8_t)0x00) ///< 7.35 kHz or 8 kHz
#define TLV320_MSTCFG1_FSRATE_14_7_OR_16_KHZ        ((uint8_t)0x01) ///< 14.7 kHz or 16 kHz
#define TLV320_MSTCFG1_FSRATE_22_05_OR_24_KHZ       ((uint8_t)0x02) ///< 22.05 kHz or 24 kHz
#define TLV320_MSTCFG1_FSRATE_29_4_OR_32_KHZ        ((uint8_t)0x03) ///< 29.4 kHz or 32 kHz
#define TLV320_MSTCFG1_FSRATE_44_1_OR_48_KHZ        ((uint8_t)0x04) ///< 44.1 kHz or 48 kHz
#define TLV320_MSTCFG1_FSRATE_88_2_OR_96_KHZ        ((uint8_t)0x05) ///< 88.2 kHz or 96 kHz
#define TLV320_MSTCFG1_FSRATE_176_4_OR_192_KHZ      ((uint8_t)0x06) ///< 176.4 kHz or 192 kHz
#define TLV320_MSTCFG1_FSRATE_352_8_OR_384_KHZ      ((uint8_t)0x07) ///< 352.8 kHz or 384 kHz
#define TLV320_MSTCFG1_FSRATE_705_6_OR_768_KHZ      ((uint8_t)0x08) ///< 705.6 kHz or 768 kHz

#define TLV320_MSTCFG1_FSBCLK_RATIO_16              ((uint8_t)0x00) ///< Ratio of 16
#define TLV320_MSTCFG1_FSBCLK_RATIO_24              ((uint8_t)0x01) ///< Ratio of 24 
#define TLV320_MSTCFG1_FSBCLK_RATIO_32              ((uint8_t)0x02) ///< Ratio of 32 
#define TLV320_MSTCFG1_FSBCLK_RATIO_48              ((uint8_t)0x03) ///< Ratio of 48 
#define TLV320_MSTCFG1_FSBCLK_RATIO_64              ((uint8_t)0x04) ///< Ratio of 64 
#define TLV320_MSTCFG1_FSBCLK_RATIO_96              ((uint8_t)0x05) ///< Ratio of 96 
#define TLV320_MSTCFG1_FSBCLK_RATIO_128             ((uint8_t)0x06) ///< Ratio of 128 
#define TLV320_MSTCFG1_FSBCLK_RATIO_192             ((uint8_t)0x07) ///< Ratio of 192 
#define TLV320_MSTCFG1_FSBCLK_RATIO_256             ((uint8_t)0x08) ///< Ratio of 256 
#define TLV320_MSTCFG1_FSBCLK_RATIO_384             ((uint8_t)0x09) ///< Ratio of 384 
#define TLV320_MSTCFG1_FSBCLK_RATIO_512             ((uint8_t)0x0A) ///< Ratio of 512 
#define TLV320_MSTCFG1_FSBCLK_RATIO_1024            ((uint8_t)0x0B) ///< Ratio of 1024 
#define TLV320_MSTCFG1_FSBCLK_RATIO_2048            ((uint8_t)0x0C) ///< Ratio of 2048 

/**
 * @brief ASI bus sample rate validness test.
 * @param[in] freq Frequency to test. Accepted values:
 * @arg @ref TLV320_MSTCFG1_FSRATE_7_35_OR_8_KHZ
 * @arg @ref TLV320_MSTCFG1_FSRATE_14_7_OR_16_KHZ
 * @arg @ref TLV320_MSTCFG1_FSRATE_22_05_OR_24_KHZ
 * @arg @ref TLV320_MSTCFG1_FSRATE_29_4_OR_32_KHZ
 * @arg @ref TLV320_MSTCFG1_FSRATE_44_1_OR_48_KHZ
 * @arg @ref TLV320_MSTCFG1_FSRATE_88_2_OR_96_KHZ
 * @arg @ref TLV320_MSTCFG1_FSRATE_176_4_OR_192_KHZ
 * @arg @ref TLV320_MSTCFG1_FSRATE_352_8_OR_384_KHZ
 * @arg @ref TLV320_MSTCFG1_FSRATE_705_6_OR_768_KHZ
 */
#define TLV320_IS_VALID_FSRATE(freq)               (((freq) == TLV320_MSTCFG1_FSRATE_7_35_OR_8_KHZ)     || \
                                                    ((freq) == TLV320_MSTCFG1_FSRATE_14_7_OR_16_KHZ)    || \
                                                    ((freq) == TLV320_MSTCFG1_FSRATE_22_05_OR_24_KHZ)   || \
                                                    ((freq) == TLV320_MSTCFG1_FSRATE_29_4_OR_32_KHZ)    || \
                                                    ((freq) == TLV320_MSTCFG1_FSRATE_44_1_OR_48_KHZ)    || \
                                                    ((freq) == TLV320_MSTCFG1_FSRATE_88_2_OR_96_KHZ)    || \
                                                    ((freq) == TLV320_MSTCFG1_FSRATE_176_4_OR_192_KHZ)  || \
                                                    ((freq) == TLV320_MSTCFG1_FSRATE_352_8_OR_384_KHZ)  || \
                                                    ((freq) == TLV320_MSTCFG1_FSRATE_705_6_OR_768_KHZ))
/**
 * @brief BCLK to FSYNC frequency ratio validness test.
 * @param[in] ratio FS/BCLK Ratio to test. Accepted values:
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_16
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_24
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_32
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_48
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_64
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_96
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_128
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_192
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_256
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_384
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_512
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_1024
 * @arg @ref TLV320_MSTCFG1_FSBCLK_RATIO_2048
 */
#define TLV320_IS_VALID_FSBCLK_RATIO(ratio)        (((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_16)     || \
                                                    ((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_24)     || \
                                                    ((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_32)     || \
                                                    ((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_48)     || \
                                                    ((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_64)     || \
                                                    ((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_96)     || \
                                                    ((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_128)    || \
                                                    ((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_192)    || \
                                                    ((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_256)    || \
                                                    ((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_384)    || \
                                                    ((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_512)    || \
                                                    ((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_1024)   || \
                                                    ((ratio) == TLV320_MSTCFG1_FSBCLK_RATIO_2048))


// CLK_SRC (0x16) Register field values **********************************************************

#define TLV320_CLKSRC_AUDIOROOT_BCLK                ((uint8_t)0x00) ///< BCLK is used as the audio root clock source
#define TLV320_CLKSRC_AUDIOROOT_MCLK                ((uint8_t)0x01) ///< MCLK is used as the audio root clock source
 
#define TLV320_CLKSRC_MCLK_MODE_FREQ_SEL            ((uint8_t)0x00) ///< MCLK frequency is based on the MCLK_FREQ_SEL
#define TLV320_CLKSRC_MCLK_MODE_FSYNC_RATIO         ((uint8_t)0x01) ///< MCLK frequency is specified as a multiple of FSYNC in the MCLK_RATIO_SEL configuration.

#define TLV320_CLKSRC_MCLK_FS_RATIO_64             ((uint8_t)0x00) ///< Set a MCLK to FSYNC ratio to 64
#define TLV320_CLKSRC_MCLK_FS_RATIO_256            ((uint8_t)0x01) ///< Set a MCLK to FSYNC ratio to 256
#define TLV320_CLKSRC_MCLK_FS_RATIO_384            ((uint8_t)0x02) ///< Set a MCLK to FSYNC ratio to 384
#define TLV320_CLKSRC_MCLK_FS_RATIO_512            ((uint8_t)0x03) ///< Set a MCLK to FSYNC ratio to 512
#define TLV320_CLKSRC_MCLK_FS_RATIO_768            ((uint8_t)0x04) ///< Set a MCLK to FSYNC ratio to 768
#define TLV320_CLKSRC_MCLK_FS_RATIO_1024           ((uint8_t)0x05) ///< Set a MCLK to FSYNC ratio to 1024
#define TLV320_CLKSRC_MCLK_FS_RATIO_1536           ((uint8_t)0x06) ///< Set a MCLK to FSYNC ratio to 1536
#define TLV320_CLKSRC_MCLK_FS_RATIO_2304           ((uint8_t)0x07) ///< Set a MCLK to FSYNC ratio to 2304

/**
 * @brief Audio root clock source setting validness test.
 * @param[in] bclk clock source to test. Accepted values:
 * @arg @ref TLV320_CLKSRC_AUDIOROOT_BCLK
 * @arg @ref TLV320_CLKSRC_AUDIOROOT_MCLK
 */
#define TLV320_IS_VALID_AUDIOROOT_CLK_SRC(bclk)   ( ((bclk) == TLV320_CLKSRC_AUDIOROOT_BCLK) || \
                                                    ((bclk) == TLV320_CLKSRC_AUDIOROOT_MCLK))

/**
 * @brief Audio root clock source setting validness test.
 * @param[in] bclk clock source to test. Accepted values:
 * @arg @ref TLV320_CLKSRC_MCLK_MODE_FREQ_SEL
 * @arg @ref TLV320_CLKSRC_MCLK_MODE_FSYNC_RATIO
 */
#define TLV320_IS_VALID_MCLK_FREQ_MODE(mode)       (((mode) == TLV320_CLKSRC_MCLK_MODE_FREQ_SEL) || \
                                                    ((mode) == TLV320_CLKSRC_MCLK_MODE_FSYNC_RATIO))

/**
 * @brief Audio root clock source setting validness test.
 * @param[in] ratio clock source to test. Accepted values:
 * @arg @ref TLV320_CLKSRC_MCLK_FS_RATIO_64
 * @arg @ref TLV320_CLKSRC_MCLK_FS_RATIO_256
 * @arg @ref TLV320_CLKSRC_MCLK_FS_RATIO_384
 * @arg @ref TLV320_CLKSRC_MCLK_FS_RATIO_512
 * @arg @ref TLV320_CLKSRC_MCLK_FS_RATIO_768
 * @arg @ref TLV320_CLKSRC_MCLK_FS_RATIO_1024
 * @arg @ref TLV320_CLKSRC_MCLK_FS_RATIO_1536
 * @arg @ref TLV320_CLKSRC_MCLK_FS_RATIO_2304
 */
#define TLV320_IS_VALID_MCLK_RATIO_SEL(ratio)      (((ratio) == TLV320_CLKSRC_MCLK_FS_RATIO_64)    || \
                                                    ((ratio) == TLV320_CLKSRC_MCLK_FS_RATIO_256)   || \
                                                    ((ratio) == TLV320_CLKSRC_MCLK_FS_RATIO_384)   || \                                                    ((ratio) == TLV320_CLKSRC_MCLK_FS_RATIO_512)   || \
                                                    ((ratio) == TLV320_CLKSRC_MCLK_FS_RATIO_768)   || \
                                                    ((ratio) == TLV320_CLKSRC_MCLK_FS_RATIO_1024)  || \
                                                    ((ratio) == TLV320_CLKSRC_MCLK_FS_RATIO_1536)  || \
                                                    ((ratio) == TLV320_CLKSRC_MCLK_FS_RATIO_2304))

/**
 * @brief @ref tlv320_clk_mode_t structure validness test.
 * @param[in] mode @ref tlv320_clk_mode_t structure to test.
 */
#define TLV320_IS_VALID_CLK_MODE_CFG(mode)         ((TLV320_IS_VALID_ASI_MASTER_SLAVE_CFG(mode.master)) && \
                                                    (TLV320_IS_VALID_AUTO_CLK(mode.auto_clk)) && \
                                                    (TLV320_IS_VALID_PLLAUTOMODE(mode.auto.pll)) && \
                                                    (TLV320_IS_VALID_AUDIOROOT_CLK_SRC(mode.clk_src)))


/**
 * @brief @ref tlv320_fs_t structure validness test.
 * @param[in] fs @ref tlv320_fs_t structure to test.
 */
#define TLV320_IS_VALID_CLK_FS_CFG(fs)             ((TLV320_IS_VALID_FSMODE(fs.mode)) && \
                                                    (TLV320_IS_VALID_FSRATE(fs.rate)) && \
                                                    (TLV320_IS_VALID_BCLKFSYNCGATE(fs.gate)) && \
                                                    (TLV320_IS_VALID_FSBCLK_RATIO(fs.bclk_ratio)))

/**
 * @brief @ref tlv320_mclk_t structure validness test.
 * @param[in] mclk @ref tlv320_mclk_t structure to test.
 */
#define TLV320_IS_VALID_CLK_MCLK_CFG(mclk)         ((TLV320_IS_VALID_MCLK_FREQ_MODE(mclk.mode)) && \
                                                    (TLV320_IS_VALID_MCLK_RATIO_SEL(mclk.fs_ratio)) && \
                                                    (TLV320_IS_VALID_MCLK(mclk.select)))

/**
 * @brief @ref tlv320_clk_cfg_t structure validness test.
 * @param[in] clk @ref tlv320_clk_cfg_t structure to test.
 */
#define TLV320_IS_VALID_CLK_CFG(clk)               ((TLV320_IS_VALID_CLK_MODE_CFG(clk.mode))    && \
                                                    (TLV320_IS_VALID_CLK_FS_CFG(clk.fs))        && \
                                                    (TLV320_IS_VALID_CLK_MODE_CFG(clk.mclk)))


// GPO_CFGx (0x22 to 0x25) Register field values **************************************************

#define TLV320_GPOx_CFG_DISABLED                   ((uint8_t)0x00) ///< GPOx is disabled
#define TLV320_GPOx_CFG_GPO                        ((uint8_t)0x01) ///< GPOx is configured as a general-purpose output (GPO)
#define TLV320_GPOx_CFG_DIO                        ((uint8_t)0x02) ///< GPOx is configured as a device interrupt output (IRQ)
#define TLV320_GPOx_CFG_ASI                        ((uint8_t)0x03) ///< GPOx is configured as a secondary ASI output (SDOUT2)
#define TLV320_GPOx_CFG_PDMCLK                     ((uint8_t)0x04) ///< GPOx is configured as a PDM clock output (PDMCLK)

#define TLV320_GPOx_DRV_HI_Z                       ((uint8_t)0x00) ///< Hi-Z output
#define TLV320_GPOx_DRV_ACTIVE_LOW_ACTIVE_HIGH     ((uint8_t)0x01) ///< Drive active low and active high
#define TLV320_GPOx_DRV_ACTIVE_LOW_WEAK_HIGH       ((uint8_t)0x02) ///< Drive active low and weak high
#define TLV320_GPOx_DRV_ACTIVE_LOW_HI_Z            ((uint8_t)0x03) ///< Drive active low and Hi-Z
#define TLV320_GPOx_DRV_WEAK_LOW_ACTIVE_HIGH       ((uint8_t)0x04) ///< Drive weak low and active high
#define TLV320_GPOx_DRV_HI_Z_ACTIVE_HIGH           ((uint8_t)0x05) ///< Drive Hi-Z and active high

/**
 * @brief TLV320 General purpose output configuration validness test
 * @param[in] cfg General purpose output configuration to test. Accepted values: 
 * @arg @ref TLV320_GPOx_CFG_DISABLED
 * @arg @ref TLV320_GPOx_CFG_GPO
 * @arg @ref TLV320_GPOx_CFG_DIO
 * @arg @ref TLV320_GPOx_CFG_ASI
 * @arg @ref TLV320_GPOx_CFG_PDMCLK
 */
#define TLV320_IS_VALID_GPO_CFG(cfg)               (((cfg) == TLV320_GPOx_CFG_DISABLED) || \
                                                    ((cfg) == TLV320_GPOx_CFG_GPO)      || \
                                                    ((cfg) == TLV320_GPOx_CFG_DIO)      || \
                                                    ((cfg) == TLV320_GPOx_CFG_ASI)      || \
                                                    ((cfg) == TLV320_GPOx_CFG_PDMCLK))


/**
 * @brief TLV320 General purpose output drive validness test
 * @param[in] drv General purpose output drive to test. Accepted values :  
 * @arg @ref TLV320_GPOx_DRV_HI_Z
 * @arg @ref TLV320_GPOx_DRV_ACTIVE_LOW_ACTIVE_HIGH
 * @arg @ref TLV320_GPOx_DRV_ACTIVE_LOW_WEAK_HIGH
 * @arg @ref TLV320_GPOx_DRV_ACTIVE_LOW_HI_Z
 * @arg @ref TLV320_GPOx_DRV_WEAK_LOW_ACTIVE_HIGH
 * @arg @ref TLV320_GPOx_DRV_WEAK_LOW_ACTIVE_HIGH
 */
#define TLV320_IS_VALID_GPO_DRV(drv)               (((drv) == TLV320_GPOx_DRV_HI_Z)                     || \
                                                    ((drv) == TLV320_GPOx_DRV_ACTIVE_LOW_ACTIVE_HIGH)   || \
                                                    ((drv) == TLV320_GPOx_DRV_ACTIVE_LOW_WEAK_HIGH)     || \
                                                    ((drv) == TLV320_GPOx_DRV_ACTIVE_LOW_HI_Z)          || \
                                                    ((drv) == TLV320_GPOx_DRV_WEAK_LOW_ACTIVE_HIGH)     || \
                                                    ((drv) == TLV320_GPOx_DRV_HI_Z_ACTIVE_HIGH))


// GPI_CFGx (0x2B & 0x2C) Register field values ***************************************************

#define TLV320_GPIx_CFG_DISABLED                    ((uint8_t)0x00) ///< GPOx is disabled
#define TLV320_GPIx_CFG_GPI                         ((uint8_t)0x01) ///< GPOx is configured as a general-purpose output (GPO)
#define TLV320_GPIx_CFG_MCLK                        ((uint8_t)0x02) ///< GPOx is configured as a device interrupt output (IRQ)
#define TLV320_GPIx_CFG_SDIN                        ((uint8_t)0x03) ///< GPOx is configured as a secondary ASI output (SDOUT2)
#define TLV320_GPIx_CFG_PDMIN1                      ((uint8_t)0x04) ///< GPOx is configured as a PDM clock output (PDMCLK)
#define TLV320_GPIx_CFG_PDMIN2                      ((uint8_t)0x05) ///< GPOx is configured as a PDM clock output (PDMCLK)
#define TLV320_GPIx_CFG_PDMIN3                      ((uint8_t)0x06) ///< GPOx is configured as a PDM clock output (PDMCLK)
#define TLV320_GPIx_CFG_PDMIN4                      ((uint8_t)0x07) ///< GPOx is configured as a PDM clock output (PDMCLK)

/**
 * @brief TLV320 General purpose input configuration validness test
 * @param[in] cfg General purpose input configuration to test. Accepted values:
 * @arg @ref TLV320_GPIx_CFG_DISABLED
 * @arg @ref TLV320_GPIx_CFG_GPI
 * @arg @ref TLV320_GPIx_CFG_MCLK
 * @arg @ref TLV320_GPIx_CFG_SDIN
 * @arg @ref TLV320_GPIx_CFG_PDMIN1
 * @arg @ref TLV320_GPIx_CFG_PDMIN2
 * @arg @ref TLV320_GPIx_CFG_PDMIN3
 * @arg @ref TLV320_GPIx_CFG_PDMIN4
 */
#define TLV320_IS_VALID_GPI_CFG(cfg)               (((cfg) == TLV320_GPIx_CFG_DISABLED) || \
                                                    ((cfg) == TLV320_GPIx_CFG_GPI)      || \
                                                    ((cfg) == TLV320_GPIx_CFG_MCLK)     || \
                                                    ((cfg) == TLV320_GPIx_CFG_SDIN)     || \
                                                    ((cfg) == TLV320_GPIx_CFG_PDMIN1)   || \
                                                    ((cfg) == TLV320_GPIx_CFG_PDMIN2)   || \
                                                    ((cfg) == TLV320_GPIx_CFG_PDMIN3)   || \
                                                    ((cfg) == TLV320_GPIx_CFG_PDMIN4))

// BIAS_CFG (0x3B) register definition ************************************************************

#define TLV320_BIASCFG_BIAS_VREF            ((uint8_t)0x00) ///< Microphone bias is set to VREF (2.750 V, 2.500 V, or 1.375 V)
#define TLV320_BIASCFG_BIAS_1_096_VREF      ((uint8_t)0x01) ///< Microphone bias is set to VREF × 1.096 (3.014 V, 2.740 V, or 1.507 V)
#define TLV320_BIASCFG_BIAS_AVDD            ((uint8_t)0x06) ///< Microphone bias is set to AVDD

#define TLV320_BIASCFG_ADCFSCALE_VREF_2_75  ((uint8_t)0x00) ///< VREF is set to 2.75 V to support 2 VRMS for the differential input or 1 VRMS for the single-ended input
#define TLV320_BIASCFG_ADCFSCALE_VREF_2_5   ((uint8_t)0x01) ///< VREF is set to 2.5 V to support 1.818 VRMS for the differential input or 0.909 VRMS for the single-ended input
#define TLV320_BIASCFG_ADCFSCALE_VREF_1_375 ((uint8_t)0x02) ///< VREF is set to 1.375 V to support 1 VRMS for the differential input or 0.5 VRMS for the single-ended input

/**
 * @brief TLV320 ADC full scale validness test
 * @param[in] fscale ADC Full scale configuration to test. Accepted value:
 * @arg @ref TLV320_BIASCFG_ADCFSCALE_VREF_2_75
 * @arg @ref TLV320_BIASCFG_ADCFSCALE_VREF_2_5
 * @arg @ref TLV320_BIASCFG_ADCFSCALE_VREF_1_375
 */
#define TLV320_IS_VALID_ADC_FSCALE(fscale) (((fscale) == TLV320_BIASCFG_ADCFSCALE_VREF_2_75)    || \
                                            ((fscale) == TLV320_BIASCFG_ADCFSCALE_VREF_2_5)     || \
                                            ((fscale) == TLV320_BIASCFG_ADCFSCALE_VREF_1_375))

/**
 * @brief TLV320 Microphone bias validness test
 * @param[in] bias Microphone bias configuration to test. Accepted value:
 * @arg @ref TLV320_BIASCFG_BIAS_VREF
 * @arg @ref TLV320_BIASCFG_BIAS_1_096_VREF
 * @arg @ref TLV320_BIASCFG_BIAS_AVDD
 */
#define TLV320_IS_VALID_MICBIAS(bias)  (((bias) == TLV320_BIASCFG_BIAS_VREF)        || \
                                        ((bias) == TLV320_BIASCFG_BIAS_1_096_VREF)  || \
                                        ((bias) == TLV320_BIASCFG_BIAS_AVDD))

// CHx_CFG0 (x=1,2,3,4) (0x3C, 0x41, 0x46, 0x4A) Register fields values ***************************

#define TLV320_CHx_CFG0_INTYP_MICRO                 ((uint8_t)0x00) ///< Configure the channel as a microphone input
#define TLV320_CHx_CFG0_INTYP_LINE                  ((uint8_t)0x01) ///< Configure the channel as a line input

#define TLV320_CHx_CFG0_INSRC_ANALOG_DIFF           ((uint8_t)0x00) ///< Configure the channel as a analog differential input  
#define TLV320_CHx_CFG0_INSRC_ANALOG_SINGLE         ((uint8_t)0x01) ///< Configure the channel as a analog single-ended input 
#define TLV320_CHx_CFG0_INSRC_DIGITAL               ((uint8_t)0x02) ///< Configure the channel as a digital input

#define TLV320_CHx_CFG0_AC_COUPLED                  ((uint8_t)0x00) ///< AC-coupled input
#define TLV320_CHx_CFG0_DC_COUPLED                  ((uint8_t)0x01) ///< DC-coupled input

#define TLV320_CHx_CFG0_Z_2_5_KOHMS                 ((uint8_t)0x00) ///< Typical 2.5-kΩ input impedance
#define TLV320_CHx_CFG0_Z_10_KOHMS                  ((uint8_t)0x01) ///< Typical 10-kΩ input impedance
#define TLV320_CHx_CFG0_Z_20_KOHMS                  ((uint8_t)0x02) ///< Typical 20-kΩ input impedance

#define TLV320_CHx_CFG0_DRE_AGC_DISABLED            ((uint8_t)0x00) ///< DRE and AGC disabled
#define TLV320_CHx_CFG0_DRE_AGC_ENABLE              ((uint8_t)0x01) ///< DRE or AGC enabled based on the configuration of bit 3 in register DSP_CFG1

// CHx_CFG1 Register fields value *****************************************************************

#define TLV320_CHx_GAIN_MIN                         ((uint8_t)0)        ///< Minimal acceptable channel gain (0dB)
#define TLV320_CHx_GAIN_MAX                         ((uint8_t)42)       ///< Maximal acceptable channel gain (42dB)
#define TLV320_CHx_GAIN_DEFAULT                     TLV320_CHx_GAIN_MIN ///< Default channel gain

// CHx_CFG2 Register fields value
#define TLV320_CHx_VOLUME_MIN                       ((float)-100)   ///< Minimal volume (dB)
#define TLV320_CHx_VOLUME_MAX                       ((float)27)     ///< Maximal volume (dB)
#define TLV320_CHx_VOLUME_DEFAULT                   ((float)0)      ///< Default volume (dB)
#define TLV320_CHx_VOLUME_CODE_O_DB                 ((uint8_t)201)  ///< Code value to set the digital volume to 0dB
#define TLV320_CHx_VOLUME_STEP                      ((float)0.5)    ///< Step between two volume settings
#define TLV320_CHx_VOLUME_MUTED                     ((float)-100.5) ///< Float code to mute the channel

// CHx_CFG3 Register fields value
#define TLV320_CHx_GAIN_CALIB_MIN                   ((float)-0.8) ///< Minimal gain calibration (dB)
#define TLV320_CHx_GAIN_CALIB_MAX                   ((float)0.7)  ///< Maximal gain calibration (dB)
#define TLV320_CHx_GAIN_CALIB_DEFAULT               ((float)0)    ///< Default gain calibration (dB)
#define TLV320_CHx_GAIN_CALIB_NB                    ((uint8_t)16) ///< Number of gain calibration settings
#define TLV320_CHx_GAIN_CALIB_CODE_0_DB             ((uint8_t)8)  ///< Value to set to get a calibration gain of 0dB  
#define TLV320_CHx_GAIN_CALIB_STEP                  ((float)0.1)  ///< Step between two gain calibration settings

// CHx_CFG4 Register fields value
#define TLV320_CHx_PHASE_CALIB_MIN                  ((uint8_t)0)                 ///< Minimal gain calibration (n cycles)
#define TLV320_CHx_PHASE_CALIB_MAX                  ((uint8_t)255)               ///< Maximal gain calibration (n cycles)
#define TLV320_CHx_PHASE_CALIB_DEFAULT              TLV320_CHx_PHASE_CALIB_MIN   ///< Default phase calibration (n cycles)

/**
 * @brief Channel gain validness test
 * @param[in] gain Value to test. Accepted values:
 * @arg  Between @ref TLV320_CHx_GAIN_MIN and @ref TLV320_CHx_GAIN_MAX
 */
#define TLV320_IS_VALID_CH_GAIN(gain)               TLV320_IS_INCLUDED(gain, TLV320_CHx_GAIN_MIN, TLV320_CHx_GAIN_MAX)

/**
 * @brief Channel volume validness test
 * @param[in] vol Value to test. Accepted values:
 * @arg Between @ref TLV320_CHx_VOLUME_MUTED and @ref TLV320_CHx_VOLUME_MAX
 */
#define TLV320_IS_VALID_CH_VOLUME(vol)              TLV320_IS_INCLUDED(vol, TLV320_CHx_VOLUME_MUTED, TLV320_CHx_VOLUME_MAX)

/**
 * @brief Channel gain calibration validness test
 * @param[in] gain Value to test. Accepted values:
 * @arg between @ref TLV320_CHx_GAIN_CALIB_MIN and @ref TLV320_CHx_GAIN_CALIB_MAX
 */
#define TLV320_IS_VALID_CH_GAIN_CALIB(gain)         TLV320_IS_INCLUDED(gain, TLV320_CHx_GAIN_CALIB_MIN, TLV320_CHx_GAIN_CALIB_MAX)

/**
 * @brief Channel phase calibration validness test
 * @param[in] phase Value to test. Accepted values:
 * @arg between @ref TLV320_CHx_PHASE_CALIB_MIN and @ref TLV320_CHx_PHASE_CALIB_MAX
 */
#define TLV320_IS_VALID_CH_PHASE_CALIB(phase)       TLV320_IS_INCLUDED(phase, TLV320_CHx_PHASE_CALIB_MIN, TLV320_CHx_PHASE_CALIB_MAX)

/**
 * @brief TLV320 input type validness test
 * @param[in] intyp Value to test. Accepted values:
 * @arg @ref TLV320_CHx_CFG0_INTYP_MICRO 
 * @arg @ref TLV320_CHx_CFG0_INTYP_LINE
 */
#define TLV320_IS_VALID_INTYP(intyp)               (((intyp) == TLV320_CHx_CFG0_INTYP_MICRO) || \
                                                    ((intyp) == TLV320_CHx_CFG0_INTYP_LINE)) 
/**
 * @brief TLV320 input configuration validness test
 * @param[in] insrc Value to test. Accepted values:
 * @arg @ref TLV320_CHx_CFG0_INSRC_ANALOG_DIFF
 * @arg @ref TLV320_CHx_CFG0_INSRC_ANALOG_SINGLE
 * @arg @ref TLV320_CHx_CFG0_INSRC_DIGITAL
 */
#define TLV320_IS_VALID_INSRC(insrc)               (((insrc) == TLV320_CHx_CFG0_INSRC_ANALOG_DIFF)   || \
                                                    ((insrc) == TLV320_CHx_CFG0_INSRC_ANALOG_SINGLE) || \
                                                    ((insrc) == TLV320_CHx_CFG0_INSRC_DIGITAL))
/**
 * @brief TLV320 input analog configuration validness test
 * @param[in] an Value to test. Accepted values:
 * @arg @ref TLV320_CHx_CFG0_INSRC_ANALOG_DIFF
 * @arg @ref TLV320_CHx_CFG0_INSRC_ANALOG_SINGLE
 */
#define TLV320_IS_VALID_ANALOG_CH(insrc)           (((insrc) == TLV320_CHx_CFG0_INSRC_ANALOG_DIFF)   || \
                                                    ((insrc) == TLV320_CHx_CFG0_INSRC_ANALOG_SINGLE))
/**
 * @brief TLV320 input coupling validness test
 * @param[in] coupling Value to test. Accepted values:
 * @arg @ref TLV320_CHx_CFG0_AC_COUPLED
 * @arg @ref TLV320_CHx_CFG0_DC_COUPLED
 */
#define TLV320_IS_VALID_COUPLING(coupling)         (((coupling) == TLV320_CHx_CFG0_AC_COUPLED) || \
                                                    ((coupling) == TLV320_CHx_CFG0_DC_COUPLED))
/**
 * @brief TLV320 input impedance validness test
 * @param[in] z Value to test. Accepted values:
 * @arg @ref TLV320_CHx_CFG0_Z_2_5_KOHMS
 * @arg @ref TLV320_CHx_CFG0_Z_10_KOHMS
 * @arg @ref TLV320_CHx_CFG0_Z_20_KOHMS
 */
#define TLV320_IS_VALID_INPUT_Z(z)                 (((z) == TLV320_CHx_CFG0_Z_2_5_KOHMS) || \
                                                    ((z) == TLV320_CHx_CFG0_Z_10_KOHMS)  || \
                                                    ((z) == TLV320_CHx_CFG0_Z_20_KOHMS))
//! 
/**
 * @brief TLV320 input DRE & AGC validness test
 * @param[in] dreagc Value to test. Accepted values:
 * @arg @ref TLV320_CHx_CFG0_DRE_AGC_DISABLED
 * @arg @ref TLV320_CHx_CFG0_DRE_AGC_ENABLE
 */
#define TLV320_IS_VALID_DRE_AGC(dreagc)            (((dreagc) == TLV320_CHx_CFG0_DRE_AGC_DISABLED) || \
                                                    ((dreagc) == TLV320_CHx_CFG0_DRE_AGC_ENABLE))

// IN_CH_EN (0x73) Register field values *********************************************************

#define TLV320_IN_CH_EN_DISABLED            ((uint8_t)0x00) ///< Value to disable an input channel 
#define TLV320_IN_CH_EN_ENABLED             ((uint8_t)0x01) ///< Value to enable an input channel

// ASI_OUT_CH_EN (0x74) Register field values *****************************************************

#define TLV320_ASI_OUT_CH_EN_TRISTATE       ((uint8_t)0x00) ///< Value that disable an output channel
#define TLV320_ASI_OUT_CH_EN_ENABLED        ((uint8_t)0x01) ///< Value that enable an output channel

// PWR_CFG (0x75) Register field values ***********************************************************

#define TLV320_PWR_CFG_MICBIAS_POWER_DOWN   ((uint8_t)0x00) ///<  Power down MICBIAS
#define TLV320_PWR_CFG_MICBIAS_POWER_UP     ((uint8_t)0x01) ///<  Power up MICBIAS

#define TLV320_PWR_CFG_ADC_POWER_DOWN       ((uint8_t)0x00) ///<  Power down all ADC and PDM channels
#define TLV320_PWR_CFG_ADC_POWER_UP         ((uint8_t)0x01) ///<  Power up all enabled ADC and PDM channels  

#define TLV320_PWR_CFG_PLL_POWER_DOWN       ((uint8_t)0x00) ///<  Power down the PLL  
#define TLV320_PWR_CFG_PLL_POWER_UP         ((uint8_t)0x01) ///<  Power up the PLL

#define TLV320_PWR_CFG_DYN_CH_PUPD_DISABLED ((uint8_t)0x00) ///< Channel power-up, power-down is not supported if any channel recording is on
#define TLV320_PWR_CFG_DYN_CH_PUPD_ENABLED  ((uint8_t)0x01) ///<  Channel can be powered up or down individually, even if channel recording is on

#define TLV320_PWR_CFG_DYN_MAXCH_SEL_1_2    ((uint8_t)0x00) ///< Channel 1 and channel 2 are used with dynamic channel power-up, power-down feature enabled
#define TLV320_PWR_CFG_DYN_MAXCH_SEL_1_4    ((uint8_t)0x01) ///< Channel 1 and channel 4 are used with dynamic channel power-up, power-down feature enabled
#define TLV320_PWR_CFG_DYN_MAXCH_SEL_1_6    ((uint8_t)0x02) ///< Channel 1 and channel 6 are used with dynamic channel power-up, power-down feature enabled
#define TLV320_PWR_CFG_DYN_MAXCH_SEL_1_8    ((uint8_t)0x03) ///< Channel 1 and channel 8 are used with dynamic channel power-up, power-down feature enabled

//*************************************************************************************************
//
//         API types definition
//
//*************************************************************************************************

//!@brief Enumeration of ASI protocol format 
typedef enum
{
    TLV320_TDM = TLV320_ASI_CFG0_ASI_FORMAT_TDM, //!< Time division multiplexing mode
    TLV320_I2S = TLV320_ASI_CFG0_ASI_FORMAT_I2S, //!< Inter IC sound mode        
    TLV320_LJ  = TLV320_ASI_CFG0_ASI_FORMAT_LJ   //!< Left-justified mode
}tlv320_asi_format_t;

//!@brief Enumeration of audio output channel data word-lenght
typedef enum
{
    TLV320_WLEN_16 = TLV320_ASI_CFG0_ASI_WLEN_16BITS, //!< Output channel data word-length set to 16 bits
    TLV320_WLEN_20 = TLV320_ASI_CFG0_ASI_WLEN_20BITS, //!< Output channel data word-length set to 20 bits
    TLV320_WLEN_24 = TLV320_ASI_CFG0_ASI_WLEN_24BITS, //!< Output channel data word-length set to 24 bits
    TLV320_WLEN_32 = TLV320_ASI_CFG0_ASI_WLEN_32BITS  //!< Output channel data word-length set to 32 bits
}tlv320_asi_wlen_t;

//! @brief Enumeration of audio serial data interface bus frame synchronization polarity
typedef enum 
{
    TLV320_FSYNCPOL_NORMAL   = TLV320_ASI_CFG0_FSYNC_POL_NORMAL,    //!<  Default polarity as per standard protocol 
    TLV320_FSYNCPOL_INVERTED = TLV320_ASI_CFG0_FSYNC_POL_INVERTED   //!<  Inverted polarity with respect to standard protocol
}tlv320_asi_fsync_pol_t;


//!@brief Enumeration of audio serial data interface bus bit clock polarity
typedef enum 
{
    TLV320_BCLKPOL_NORMAL   = TLV320_ASI_CFG0_BCLK_POL_NORMAL,  //!< Default polarity as per standard protocol
    TLV320_BCLKPOL_INVERTED = TLV320_ASI_CFG0_BCLK_POL_INVERTED //!< Inverted polarity with respect to standard protocol
}tlv320_asi_bclk_pol_t;

//! @brief Enumeration of ASI data output transmit edge 
typedef enum 
{
    TLV320_TXEDGE_NORMAL    = TLV320_ASI_CFG0_TX_EDGE_NORMAL,    //!< Default edge as per the protocol configuration setting : @ref tlv320_asi_bclk_pol_t
    TLV320_TXEDGE_INVERTED  = TLV320_ASI_CFG0_TX_EDGE_INVERTED   //!< Inverted following edge (half cycle delay) with respect to the default edge setting
}tlv320_asi_tx_edge_t;

//! @brief Enumeration of ASI data output selection for any unused cycles
typedef enum 
{
    TLV320_TXFILL_ZERO = TLV320_ASI_CFG0_TX_FILL_ZERO,  //!< Always transmit 0 for unused cycles
    TLV320_TXFILL_HI_Z = TLV320_ASI_CFG0_TX_FILL_HI_Z   //!< Always use Hi-Z for unused cycles
}tlv320_asi_tx_fill_t;

//! @brief Enumeration of ASI data output (on the primary and secondary data pin) for LSB transmissions  
typedef enum
{
    TLV320_TX_LSB_FULLCYCLE = TLV320_ASI_CFG1_TX_LSB_FULL_CYCLE, //!< Transmit the LSB for a full cycle
    TLV320_TX_LSB_HALFCYCLE = TLV320_ASI_CFG1_TX_LSB_HALF_CYCLE, //!< Transmit the LSB for the first half cycle and Hi-Z for the second half cycle
}tlv320_tx_lsb_t;

//! @brief Enumeration of ASI data output (on the primary and secondary data pin) bus keeper.
typedef enum
{
    TLV320_TX_KEEPER_DISABLED   = TLV320_ASI_CFG1_TX_KEEPER_DISABLED,           //!< Bus keeper is always disabled
    TLV320_TX_KEEPER_EN         = TLV320_ASI_CFG1_TX_KEEPER_ENABLED,            //!< Bus keeper is always enabled
    TLV320_TX_KEEPER_1CYCLE     = TLV320_ASI_CFG1_TX_KEEPER_ENABLED_1_CYCLE,    //!< Bus keeper is enabled during LSB transmissions only for one cycle.
    TLV320_TX_KEEPER_1_5_CYCLES = TLV320_ASI_CFG1_TX_KEEPER_ENABLED_1_5_CYCLES  //!< Bus keeper is enabled during LSB transmissions only for one and half cycles.
}tlv320_tx_keeper_t;   

/**
 * @brief Audio serial interface configuration structure
 */
typedef struct
{
    tlv320_asi_format_t format;                         //!< ASI protocol format selection
    tlv320_asi_wlen_t wlen;                             //!< ASI word/slot lenght
    tlv320_asi_fsync_pol_t fsync_pol;                   //!< ASI FSYNC polarity 
    tlv320_asi_bclk_pol_t bclk_pol;                     //!< ASI BCLK polarity 
    tlv320_asi_tx_edge_t tx_edge;                       //!< ASI data output transmit edge
    tlv320_asi_tx_fill_t tx_fill;                       //!< ASI data output for any unused cycles
    tlv320_tx_lsb_t tx_lsb;                             //!< ASI data output for LSB transmissions
    tlv320_tx_keeper_t tx_keeper;                       //!< ASI data output bus keeper
    uint8_t tx_offset : TLV320_ASI_CFG1_TX_OFFSET_SIZE; //!< ASI data MSB slot 0 offset
}tlv320_asi_t;   


//! @brief Channel input type possible configurations.
typedef enum
{
    TLV320_INTYP_MICRO = TLV320_CHx_CFG0_INTYP_MICRO,
    TLV320_INTYP_LINE = TLV320_CHx_CFG0_INTYP_LINE
}tlv320_ch_intyp_t;



//! @brief Channel input possible configuration
typedef enum
{
    TLV320_INSRC_ANALOG_DIFF    = TLV320_CHx_CFG0_INSRC_ANALOG_DIFF,
    TLV320_INSRC_ANALOG_SINGLE  = TLV320_CHx_CFG0_INSRC_ANALOG_SINGLE,
    TLV320_INSRC_DIGITAL        = TLV320_CHx_CFG0_INSRC_DIGITAL
}tlv320_ch_insrc_t;

//! @brief Channel input coupling possible configurations.
typedef enum
{
    TLV320_INPUT_AC_COUPLED = TLV320_CHx_CFG0_AC_COUPLED,
    TLV320_INPUT_DC_COUPLED = TLV320_CHx_CFG0_DC_COUPLED
}tlv320_ch_input_coupling_t;

//! @brief Channel input impedance possible configurations.
typedef enum
{
    TLV320_INPUT_Z_2_5_KOHM = TLV320_CHx_CFG0_Z_2_5_KOHMS,
    TLV320_INPUT_Z_10_KOHM  = TLV320_CHx_CFG0_Z_10_KOHMS,
    TLV320_INPUT_Z_20_KOHM  = TLV320_CHx_CFG0_Z_20_KOHMS,
}tlv320_ch_input_z_t;

//! @brief Channel DRE & AGC possible configurations.
typedef enum
{
    TLV320_AGCDRE_DISABLED = TLV320_CHx_CFG0_DRE_AGC_DISABLED,
    TLV320_AGCDRE_ENABLED = TLV320_CHx_CFG0_DRE_AGC_ENABLE
}tlv320_ch_input_agcdre_t;

//! @brief Channel output possible configurations.
typedef enum
{
    TLV320_OUTPUT_PRIMARY    = TLV320_ASI_CHx_OUTPUT_PRIMARY,
    TLV320_OUTPUT_SECONDARY  = TLV320_ASI_CHx_OUTPUT_SECONDARY
}tlv320_ch_output_line_t;

//! @brief Analog channel input parameters structure definition
typedef struct
{
    tlv320_ch_intyp_t           type;       //!< Channel's input type
    tlv320_ch_insrc_t           src;        //!< Channel's input configuration
    tlv320_ch_input_coupling_t  coupling;   //!< Channel's input coupling
    tlv320_ch_input_z_t         z;          //!< Channel's input impedance
    tlv320_ch_input_agcdre_t    agcdre;     //!< Channel's Dynamic range enhancer and automatic gain controller setting
}tlv320_ch_input_t;

//! @brief Analog channel output parameters structure definition
typedef struct
{
    uint8_t                 slot; //!< Channel Slot assignement
    tlv320_ch_output_line_t line; //!< Channel output selection
}tlv320_ch_output_t;

//! @brief Analog channel structure definition
typedef struct 
{
    uint8_t                 id;             //!< Analog channel id
    tlv320_ch_input_t       input;          //!< Input parameters
    tlv320_ch_output_t      output;         //!< Output parameters
    uint8_t                 gain;           //!< Channel gain 
    float                   volume;         //!< Channel digital volume control
    float                   gain_calib;     //!< Channel gain calibration
    uint8_t                 phase_calib;    //!< Channel phase calibration
}tlv320_analog_ch_t;

///@brief Microphone bias possible configurations.
typedef enum
{
    TLV320_MICBIAS_VREF = TLV320_BIASCFG_BIAS_VREF,
    TLV320_MICBIAS_VREF_1_096 = TLV320_BIASCFG_BIAS_1_096_VREF,
    TLV320_MICBIAS_VREF_AVDD = TLV320_BIASCFG_BIAS_AVDD
}tlv320_micbias_t;

///@brief ADC full scale possible configurations.
typedef enum
{
    TLV320_ADC_FSCALE_VREF_2_75 = TLV320_BIASCFG_ADCFSCALE_VREF_2_75,
    TLV320_ADC_FSCALE_VREF_2_5 = TLV320_BIASCFG_ADCFSCALE_VREF_2_5,
    TLV320_ADC_FSCALE_1_375 = TLV320_BIASCFG_ADCFSCALE_VREF_1_375
}tlv320_afc_fscale_t;


/**
 * @brief TLV320 clk main configuration
 * 
 */
typedef struct
{
    uint8_t master;     /*!< ASI master or slave configuration. 
                             @arg See @ref TLV320_IS_VALID_ASI_MASTER_SLAVE_CFG for possible values */

    uint8_t auto_clk;   /*!< Automatic clock configuration. 
                             @arg See @ref TLV320_IS_VALID_AUTO_CLK for possible values */

    uint8_t auto_pll;   /*!< Automatic mode PLL setting.
                             @arg See @ref TLV320_IS_VALID_PLLAUTOMODE for possible values */

    uint8_t clk_src;    /*!< Audio root clock source setting.
                             @arg See @ref TLV320_IS_VALID_AUDIOROOT_CLK_SRC for possible values */
}tlv320_clk_mode_t;

/**
 * @brief TLV320 Frame synchronisation (FS) bus configuration
 */
typedef struct
{
    uint8_t mode;       /*!< Sample rate setting.
                             This parameter is valid when the device is in master mode.
                             @arg See @ref TLV320_IS_VALID_FSMODE for possible values */

    uint8_t rate;       /*!< Programmed sample rate of the ASI bus.
                             This parameter is not used when the device is configured in slave mode
                             auto clock configuration. See @arg tlv320_clk_mode_t.auto_clk for more details.
                             @arg See @ref TLV320_IS_VALID_FSRATE for possible values */ 

    uint8_t gate;       /*!< BCLK and FSYNC clock gate.
                             This parameter is valid when the device is in master mode.
                             @arg See @ref TLV320_IS_VALID_BCLKFSYNCGATE for possible values */

    uint8_t bclk_ratio; /*!< Programmed BCLK to FSYNC frequency ratio of the ASI bus. 
                             This parameter is not used when the device is configured in slave mode
                             auto clock configuration. See @arg tlv320_clk_mode_t.auto_clk for more details.
                             @arg See @ref TLV320_IS_VALID_FSBCLK_RATIO for possible values */
}tlv320_fs_t;

/**
 * @brief   TLV320 Master clock configuration.
*/
typedef struct
{
    uint8_t mode;       /*!< Master mode MCLK frequency selection mode. 

                             Select the way the master clock is defined between from a predefined list
                             represented by @ref tlv320_mclk_t.select member or from a multiple of the 
                             frame synchronization signal FS represented by fs_ratio member.

                             This member is only valid when the device is in auto clock configuration. 
                             @ref tlv320_clk_mode_t.auto_pll for more details. 

                             @arg See @ref TLV320_IS_VALID_MCLK_FREQ_MODE for possible values */

    uint8_t fs_ratio;   /*!< Select the MCLK to FSYNC ratio.
    
                             This parameter is valid when the device is in master mode and when
                             @ref tlv320_mclk_t.mode is set to @ref TLV320_CLKSRC_MCLK_FREQ_MODE_FSYNC_RATIO

                             This parameter is valid when the device is in slave mode and master
                             clock MCLK is used as the audio toot clock source in slave mode. 
                             See @ref tlv320_clk_mode_t.clk_src for more info.

                             @arg See @ref TLV320_IS_VALID_MCLK_RATIO_SEL for possible values */

    uint8_t select;     /*!< Select the MCLK frequency for the PLL source. 
    
                             This parameter is valid when the device is in master mode and when
                             @ref tlv320_mclk_t.mode is set to @ref TLV320_CLKSRC_MCLK_FREQ_MODE_FREQ_SEL
                             
                             @arg See @ref TLV320_IS_VALID_MCLK for possible values */
}tlv320_mclk_t;

/**
 * @brief TLV320 main structure to set
 *        ASI clock configuration.
 */
typedef struct
{
    tlv320_clk_mode_t   mode;   //!< Set TLV320 clock modes.
    tlv320_fs_t         fs;     //!< Frame synchonization bus (FSYNC) configuration 
    tlv320_mclk_t       mclk;   //!< Master clock parameters. 
}tlv320_clk_cfg_t;

//*************************************************************************************************
//
//         Registers type definition
//
//*************************************************************************************************

//! @brief SW_RESET (0x01) register definition
typedef union {
    struct {
        uint8_t sw_reset    : 1; //!< Software reset
        uint8_t reserved    : 7; //!< Reserved
    };
    uint8_t raw;
} __attribute__((packed)) tlv320_sw_reset_t;

//! @brief SLEEP_CFG (0x02) register definition
typedef union {
    struct {
        uint8_t sleep_enz      : 1; //!< Sleep mode setting.
        uint8_t reserved_1     : 1; //!< Reserved
        uint8_t i2c_brdcast_en : 1; //!< I2C broadcast addressing setting.
        uint8_t vref_qchg      : 2; //!< Quick charge duration
        uint8_t reserved_5_6   : 2; //!< Reserved
        uint8_t areg_select    : 1; //!< The analog supply selection
    };
    uint8_t raw;               //!< Register content raw version 
} __attribute__((packed)) tlv320_sleep_cfg_t;

//! @brief ASI_CFG0 (0x07) register definition
typedef union {
    struct {
        uint8_t tx_fill     : 1;   //!< ASI data output for any unused cycles
        uint8_t tx_edge     : 1;   //!< ASI data output transmit edge.
        uint8_t bclk_pol    : 1;   //!< ASI BCLK polarity.
        uint8_t fsync_pol   : 1;   //!< ASI FSYNC polarity.
        uint8_t asi_wlen    : 2;   //!< ASI word or slot length.
        uint8_t asi_format  : 2;   //!< ASI protocol format.
    };
    uint8_t raw;              //!< Register content raw version 
} __attribute__((packed)) tlv320_asi_cfg0_t;

//! @brief ASI_CFG1 (0x08) register definition
typedef union {
    struct {
        uint8_t tx_offset   : 5;   //!< ASI data MSB slot 0 offset
        uint8_t tx_keeper   : 2;   //!< ASI data output bus keeper.
        uint8_t tx_lsb      : 1;   //!< ASI data output for LSB transmissions
    };
    uint8_t raw;              //!< Register content raw version 
} __attribute__((packed)) tlv320_asi_cfg1_t;

//! @brief ASI_CHx (0x0B to 0x12) register definition
typedef union {
    struct {
        uint8_t slot     : 6;   //!< Channel slot assignment.
        uint8_t output   : 1;   //!< Channel output line.
        uint8_t reserved : 1;   //!< Reserved 
    };
    uint8_t raw;           //!< Register content raw version 
} __attribute__((packed)) tlv320_asi_ch_t;

//! @brief MST_CFG0 (0x13) register definition
typedef union {
    struct {
        uint8_t mclk_freq_sel       : 3;   //!< MCLK (GPIO or GPIx) frequency for the PLL source Selection
        uint8_t fs_mode             : 1;   //!< Sample rate setting (valid when the device is in master mode). 
        uint8_t bclk_fsync_gate     : 1;   //!< BCLK and FSYNC clock gate (valid when the device is in master mode). 
        uint8_t auto_mode_pll_dis   : 1;   //!< Automatic mode PLL setting. 
        uint8_t auto_clk_cfg        : 1;   //!< Automatic clock configuration setting.
        uint8_t mst_slv_cfg         : 1;   //!< ASI master or slave configuration register setting. 
    };
    uint8_t raw;           //!< Register content raw version 
} __attribute__((packed)) tlv320_mst_cfg0_t;

//! @brief MST_CFG1 (0x14) register definition
typedef union {
    struct {
        uint8_t fs_bclk_ratio   : 4;    //!< Programmed sample rate of the ASI bus 
        uint8_t fs_rate         : 4;    //!< Programmed BCLK to FSYNC frequency ratio of the ASI bus
    };
    uint8_t raw;           //!< Register content raw version 
} __attribute__((packed)) tlv320_mst_cfg1_t;

//! @brief CLK_SRC (0x16) register definition
typedef union {
    struct {
        uint8_t reserved            : 3; //!< Reserved. 
        uint8_t mclk_ratio_sel      : 3; //!< Select the MCLK (GPIO or GPIx) to FSYNC ratio for master mode or when MCLK is used as the audio root clock source in slave mode
        uint8_t mclk_freq_sel_mode  : 1; //!< Master mode MCLK (GPIO or GPIx) frequency selection mode (valid when the device is in auto clock configuration)
        uint8_t dis_pll_slv_clk_src : 1; //!< Audio root clock source setting when the device is configured with the PLL disabled in the auto clock configuration for slave mode
    };
    uint8_t raw;                    //!< Register content raw version 
} __attribute__((packed)) tlv320_clk_src_t;

//! @brief GPO_CFGx (0x22 to 0x25) register structure definition
typedef union {
    struct {
        uint8_t drv         : 3;    //!< GPO output drive configuration
        uint8_t reserved    : 1;    //!< Reserved
        uint8_t cfg         : 4;    //!< GPO Configuration
    };
    uint8_t raw;           //!< Register content raw version 
} __attribute__((packed)) tlv320_gpo_cfg_t;

//! @brief GPI_CFGx (0x2B & 0x2C) register structure definition
typedef union {
    struct {
        uint8_t cfg_y       : 3;    //!< GPI (2 or 4) Configuration
        uint8_t reserved1   : 1;    //!< Reserved
        uint8_t cfg_x       : 3;    //!< GPI (1 or 3) Configuration
        uint8_t reserved2   : 1;    //!< Reserved
    };
    uint8_t raw;               //!< Register content raw version 
} __attribute__((packed)) tlv320_gpi_cfg_t;

/// @brief BIAS_CFG (0x3B) register definition
typedef union {
    struct {
        uint8_t adc_fscale  : 2; //!< ADC full-scale setting  
        uint8_t reserved1   : 2; //!< Bits 2 & 3 reserved
        uint8_t mbias_val   : 3; //!< MICBIAS setting  
        uint8_t reserved2   : 1; //!< Bit 7 reserved
    };
    uint8_t raw;
}__attribute__((packed)) tlv320_bias_cfg_t;

//! @brief CHx_CFG0 (0x3C, 0x41, 0x46, 0x4A) register definition
typedef union {
    struct {
        uint8_t ch_dreen   : 1;   //!< Channel DRE and AGC setting.
        uint8_t reserved   : 1;   //!< Reserved
        uint8_t ch_imp     : 2;   //!< Channel input impedance
        uint8_t ch_dc      : 1;   //!< Channel input coupling
        uint8_t ch_insrc   : 2;   //!< Channel input configuration.
        uint8_t ch_intyp   : 1;   //!< Channel input type.
  };
  uint8_t raw;               //!< Register content raw version 
} __attribute__((packed)) tlv320_ch_cfg0_t;

//! @brief CHx_CFG1 (0x3D, 0x42, 0x47, 0x4B) register definition
typedef union {
    struct {
        uint8_t reserved    : 2;    //!< Reserved
        uint8_t ch_gain     : 6;    //!< Channel gain
  };
  uint8_t raw;                 //!< Register content raw version 
} __attribute__((packed)) tlv320_ch_cfg1_t;

//! @brief CHx_CFG2 (0x3E, 0x43, 0x48, 0x4C) register definition
typedef union {
    struct {
        uint8_t ch_dvol     : 8;    //!< Channel gain
  };
  uint8_t raw;                 //!< Register content raw version 
} __attribute__((packed)) tlv320_ch_cfg2_t;

//! @brief CHx_CFG3 (0x3F, 0x44, 0x49, 0x4D) register definition
typedef union {
    struct {
        uint8_t reserved    : 4; //!< Reserved
        uint8_t ch_gcal     : 4; //!< Channel gain calibration
  };
  uint8_t raw;              //!< Register content raw version 
} __attribute__((packed)) tlv320_ch_cfg3_t;

//! @brief CHx_CFG4 (0x3F, 0x44, 0x49, 0x4D) register definition
typedef union {
    struct {
        uint8_t ch_pcal; //!< Channel gain calibration
  };
  uint8_t raw;                 //!< Register content raw version 
} __attribute__((packed)) tlv320_ch_cfg4_t;



//! @brief IN_CH_EN (0x73) register definition
typedef union {
    struct {
        uint8_t in_ch8_en    : 1;   //!< Input channel 8 enable setting
        uint8_t in_ch7_en    : 1;   //!< Input channel 7 enable setting
        uint8_t in_ch6_en    : 1;   //!< Input channel 6 enable setting
        uint8_t in_ch5_en    : 1;   //!< Input channel 5 enable setting
        uint8_t in_ch4_en    : 1;   //!< Input channel 4 enable setting
        uint8_t in_ch3_en    : 1;   //!< Input channel 3 enable setting
        uint8_t in_ch2_en    : 1;   //!< Input channel 2 enable setting
        uint8_t in_ch1_en    : 1;   //!< Input channel 1 enable setting
  };
  uint8_t raw;                 //!< Register content raw version 
} __attribute__((packed)) tlv320_in_ch_en_t;

//! @brief ASI_OUT_CH_EN (0x74) register structure definition
typedef union {
    struct {
        uint8_t asi_out_ch8_en    : 1;   //!< ASI output channel 8 enable setting.
        uint8_t asi_out_ch7_en    : 1;   //!< ASI output channel 7 enable setting.
        uint8_t asi_out_ch6_en    : 1;   //!< ASI output channel 6 enable setting.
        uint8_t asi_out_ch5_en    : 1;   //!< ASI output channel 5 enable setting.
        uint8_t asi_out_ch4_en    : 1;   //!< ASI output channel 4 enable setting.
        uint8_t asi_out_ch3_en    : 1;   //!< ASI output channel 3 enable setting.
        uint8_t asi_out_ch2_en    : 1;   //!< ASI output channel 2 enable setting.
        uint8_t asi_out_ch1_en    : 1;   //!< ASI output channel 1 enable setting.
  };
  uint8_t raw;                 //! Register content raw version 
} __attribute__((packed)) tlv320_asi_out_ch_en_t;

//! @brief PWR_CFG (0x75) register structure definition
typedef union {
    struct {
        uint8_t reserved        : 2;   //!< Reserved
        uint8_t dyn_maxch_sel   : 2;   //!< Dynamic mode maximum channel select configuration.
        uint8_t dyn_ch_pupd_en  : 1;   //!< Dynamic channel power-up, power-down enable.
        uint8_t pll_pdz         : 1;   //!< Power control for the PLL.
        uint8_t adc_pdz         : 1;   //!< Power control for ADC and PDM channels.
        uint8_t micbias_pdz     : 1;   //!< Power control for MICBIAS.
  };
  uint8_t raw;                     //!< Register content raw version 
} __attribute__((packed)) tlv320_pwr_cfg_t;

// ************************************************************************************************

/**
 * @brief   API that control TLV320 SHDNZ pin.
 *          TLV320 module abstract this pin control in order to be used
 *          in several ways. Either by direct GPIO conrtol from the GAP 
 *          or through a GPIO expander via i2c. 
 */
typedef struct 
{
    int (*set)(void*);     //!< BSP provided. Set the GPIO that power up the TLV320.
    int (*reset)(void*);   //!< BSP provided. Reset the GPIO to power down the TLV320.
} pi_tlv320_shdnz_api_t;

/**
 * @brief   SHDNZ abstraction structure. 
 *          This object handles the SHDNZ pin:
 *          - What triggers it (GAP9 GPIO, GPIO expander via i2C, other...)
 *          - Its features (enable/disable the device) 
 */
typedef struct
{
    int id;
    void* context;
    pi_tlv320_shdnz_api_t api;
}bsp_tlv320_shdnz_t;

/**
 * @brief   TLV320 configuration structure. 
 *          Contains all hardware related parameters of the TLV320.
 */
struct pi_tlv320_conf
{
    int i2c_itf;                //!< I2C interface ID.
    uint32_t i2c_max_baudrate;  //!< I2C baudrate.
    uint8_t addr1_miso;         //!< TLV320 ADDR1 pin state.
    uint8_t addr0_sclk;         //!< TLV320 ADDR0 pin state
    bsp_tlv320_shdnz_t shdnz;   //!< TLV320 shdnz interface.
};

//*************************************************************************************************
//
//         Function prototypes
//
//*************************************************************************************************

/**
 * @brief TLV320 write operation dedicated to configuration registers.
 *        This function automatically set the right page to write a
 *        configuration register.
 * 
 * @param[in] tlv320_dev  TLV320 device instance
 * @param[in] reg         Configuration register to write.    
 * @param[in] value       Value to write into the register
 * @return uint8_t    Error management. 
 *                      @ref TLV320_NO_ERROR if no error occured. 
 *                      @ref TLV320_WRITE_ERROR if an i2C write operation failed
 */
uint8_t pi_tlv320_write_config_register(pi_device_t *tlv320_dev, tlv320_config_register_t reg, uint8_t value);

/**
 * @brief TLV320 read operation dedicated to configuration registers.
 *        This function automatically set the right page before reading a
 *        configuration register.
 * 
 * @param[in]    tlv320_dev TLV320 device instance
 * @param[in]    reg        Configuration register to write.    
 * @param[inout] read_value Pointer to the value read from the register     
 * @retval Error management. The function can return the following error codes: 
 *          @arg @ref TLV320_READ_ERROR Failed to read register value.
 *          @arg @ref TLV320_NO_ERROR No error occured.
 */
uint8_t pi_tlv320_read_config_register(pi_device_t *tlv320_dev, tlv320_config_register_t reg, uint8_t* read_value);


/**
 * @brief Initialize an tlv320 configuration with default values.
 *
 * The structure containing the configuration must be kept alive until
 * the device is opened.
 * It can only be called from fabric-controller side.
 *
 * @param[in] conf           Pointer to the device configuration.
 */
void pi_tlv320_conf_init(struct pi_tlv320_conf *conf);

/**
 * @brief TLV320 initialization. This function must be called after
 * loading a configuration with pi_open_from_conf. These will set the
 * module's configuration (struct pi_tlv320_conf) before initializating it.
 * 
 * @param[in] tlv320_dev  pi_device that store the tlv320 configuration
 * @return    int         Error management. 
 */
int pi_tlv320_open(pi_device_t* tlv320_dev);

/**
 * @brief TLV320 deallocation. 
 * 
 * @param[in] tlv320_dev TLV320 instance pointer 
 */
void pi_tlv320_close(pi_device_t* tlv320_dev);

/**
 *  @brief  Shuts down the device, and restores all device configuration
 *          registers and programmable coefficients to their default values
 * 
 * @param[in]   tlv320_dev tlv320 device instance. 
 */
void pi_tlv320_reset(pi_device_t* tlv320_dev);

/**
 * @brief   Put the device into sleep or active mode. 
 * @note    While putting the device into active, the analog supply selection is 
 *          reseted to "internal" with AREG_SELECT bit setted to
 *          @ref TLV320_SLEEP_CFG_AREG_SELECT_INTERNAL. 
 *          In case mode is set to @ref TLV320_SLEEP_CFG_SLEEP_ENZ_DISABLED, 
 *          The function insert a delay of @ref TLV320_SLEEPMODE_WAKEUP_DELAY 
 *          to wait for TLV320 internal wake-up sequence.
 * 
 * @param[in] tlv320_dev TLV320 device instance pointer
 * @param[in] mode       Mode in which to set the TLV320. It can be a value of : 
 *                      @ref TLV320_SLEEP_CFG_SLEEP_ENZ_DISABLED Set the device into active mode. 
 *                      @ref TLV320_SLEEP_CFG_SLEEP_ENZ_ENABLED Set the device into sleep mode. 
 */
void pi_tlv320_set_sleep_mode(pi_device_t* tlv320_dev, uint8_t mode);

/**
 * @brief Program the device's serial audio interface according to 
 *        @ref tlv320_asi_t data structure. 
 * 
 * @param[in] tlv320_dev TLV320 instance pointer
 * @param[in] asi Audio serial interface to program into the device
 */
void pi_tlv320_set_asi(pi_device_t* tlv320_dev, tlv320_asi_t asi);

/**
 * @brief Return the audio serial interface programmed into the device.
 * 
 * @param[in] tlv320_dev TLV320 instance pointer
 * @param[inout] asi Audio serial interface structure pointer
 * @return Error management
 *          @arg @ref TLV320_NO_ERROR is everything went well
 *          @arg @ref TLV320_READ_ERROR if read operation failed
 */
uint8_t pi_tlv320_get_asi(pi_device_t* tlv320_dev, tlv320_asi_t *asi);

/**
 * @brief   Return a default, ready to be written structure about the TLV320
 *          serial audio interface. Use @ref pi_tlv320_set_asi to send
 *          this configuration to the device.
 *          
 * @param[in] tlv320_dev TLV320 instance pointer
 * @return tlv320_asi_t Default serial audio interface data structure.
 */
tlv320_asi_t pi_tlv320_build_default_asi(void);

/**
 * @brief Return a default analog channel configuration structure ready to be written. 
 *        It is configured as followed: 
 *          1. Micro input
 *          2. Analog differential
 *          3. AC coupled
 *          4. 2.5 kOhm input impedance
 *          5. No Digital range enhancer nor Automatic gain controller activated 
 *          6. The channel output is set on its primary output (SDOUT)
 *          7. Channel gain, volume, gain calib and phase calib are set to their default value.
 *          8. The channel input as the output slot are specified by parameters. 
 * 
 * @param[in] tlv320_dev    TLV320 instance pointer
 * @param[in] channel_id    Channel selection. It can be a value of : 
 *                          @arg @ref TLV320_CHANNEL_1 Channel 1 selected 
 *                          @arg @ref TLV320_CHANNEL_2 Channel 2 selected
 *                          @arg @ref TLV320_CHANNEL_3 Channel 3 selected
 *                          @arg @ref TLV320_CHANNEL_4 Channel 4 selected
 *                      
 * @param[in] slot_id       Output slot selection. It can be a value between @ref TLV320_TDM_SLOT_MIN
 *                          and @ref TLV320_TDM_SLOT_MAX. 
 * 
 * @return tlv320_analog_ch_t Filled analog channel configuration.
 */
tlv320_analog_ch_t pi_tlv320_build_default_analog_ch(uint8_t channel_id, uint8_t slot_id);

/**
 * @brief   Configure a device's channel as an analog one from a configuration.
 *          The structure @ref tlv320_analog_ch_t allows to set the following parameters : 
 *          @arg 1. Channel ID. See @ref TLV320_IS_VALID_ANALOG_CHANNEL_ID for accepted values. 
 *          @arg 2. Input type. See @ref TLV320_IS_VALID_INTYP for accepted values. 
 *          @arg 3. Input source. See @ref TLV320_IS_VALID_INSRC for accepted values. 
 *          @arg 4. Input coupling. See @ref TLV320_IS_VALID_COUPLING for accepted values. 
 *          @arg 5. Input impedance. See @ref TLV320_IS_VALID_INPUT_Z for accepted values. 
 *          @arg 6. DRE and AGC setting. See @ref TLV320_IS_VALID_DRE_AGC for accepted values. 
 *          @arg 7. Slot Id assignement. See @ref TLV320_IS_VALID_TDM_SLOT_ID for accepted values. 
 *          @arg 8. Output line. See @ref TLV320_IS_VALID_ASI_CHx_OUTPUT for accepted values. 
 *          @arg 9. Channel gain. See @ref TLV320_IS_VALID_CH_GAIN for accepted values. 
 *          @arg 10. Channel Volume. See @ref TLV320_IS_VALID_CH_VOLUME for accepted values. 
 *          @arg 11. Gain calibration. See @ref TLV320_IS_VALID_CH_GAIN_CALIB for accepted values. 
 *          @arg 12. Phase calibration. See @ref TLV320_IS_VALID_CH_PHASE_CALIB for accepted values.
 *
 * @param[inout] tlv320_dev TLV320 instance pointer
 * @param[in] channel Channel configuration structure. It must already be initialized
 * @retval Error management. The function can return the following error codes: 
 *          @arg @ref TLV320_READ_ERROR Failed to read GPI register value.
 *          @arg @ref TLV320_WRITE_ERROR Failed to write at least one register.
 *          @arg @ref TLV320_NO_ERROR No error occured.
 */
uint8_t pi_tlv320_write_analog_ch(pi_device_t* tlv320_dev, tlv320_analog_ch_t channel);

/**
 * @brief Read an analog channel configuration from the device.
 * 
 * @param[inout] tlv320_dev TLV320 instance pointer
 * @param[in] channel_id Analog channel to read
 * @param[inout] an_channel Analog channel data structure pointer 
 * @retval Error management. The function can return the following error codes: 
 *          @arg @ref TLV320_READ_ERROR Failed to read register value
 *          @arg @ref TLV320_NO_ERROR No error occured.
 */
uint8_t pi_tlv320_read_analog_ch(pi_device_t* tlv320_dev, uint8_t channel_id, tlv320_analog_ch_t *an_channel);

/**
 * @brief Configure the microphone bias and the ADC full scale 
 * 
 * @param[inout] tlv320_dev TLV320 instance pointer
 * @param[in] micbias Microphone bias value
 * @param[in] adc_fscale ADC full scale
 * @return tlv320_analog_ch_t Analog channel read
 */
void pi_tlv320_set_bias_cfg(pi_device_t* tlv320_dev, tlv320_micbias_t micbias, tlv320_afc_fscale_t adc_fscale);

/**
 * @brief Enable/disable an input channel.
 * 
 * @param tlv320_dev TLV320 instance pointer
 * @param channel Channel input to enable/disable
 * @param state State to apply
 * @note Enabling a channel is different than powering up channels ! 
 * @retval Error management. The function can return the following error codes: 
 *          @arg @ref TLV320_READ_ERROR Failed to read register value.
 *          @arg @ref TLV320_WRITE_ERROR Failed to write at least one register.
 *          @arg @ref TLV320_NO_ERROR No error occured.
 */
uint8_t pi_tlv320_set_channel_input(pi_device_t* tlv320_dev, uint8_t channel, tlv320_state_t state);

/**
 * @brief Enable/disable an output channel.
 * 
 * @param tlv320_dev TLV320 instance pointer
 * @param channel Channel output to enable/disable
 * @param state State to apply
 * @note Enabling a channel is different than powering up channels ! 
 * @retval Error management. The function can return the following error codes: 
 *          @arg @ref TLV320_READ_ERROR Failed to read register value.
 *          @arg @ref TLV320_WRITE_ERROR Failed to write at least one register.
 *          @arg @ref TLV320_NO_ERROR No error occured.
 */
uint8_t pi_tlv320_set_channel_output(pi_device_t* tlv320_dev, uint8_t channel, tlv320_state_t state);

/**
 * @brief   General power control function. 
 *          Power control for ADC and PDM channels.
 *          Power control for the microphone bias output.
 *          Power control for the PLL
 * 
 * @param[inout]    tlv320_dev  TLV320 instance pointer.
 * @param[in]       state       Desired state. 
 *                              @arg @ref TLV320_DISABLE Power down 
 *                              @arg @ref TLV320_ENABLE Power up 
 * 
 * @note    All the programmable coefficient values must be
 *          written before powering up the respective channel.
 * @note    MicBias setting is ignored if the GPIO1 or GPIx pin is configured
 *          to set the microphone bias on or off
 *          @arg @ref TLV320_READ_ERROR Failed to read register value.
 *          @arg @ref TLV320_WRITE_ERROR Failed to write at least one register.
 *          @arg @ref TLV320_NO_ERROR No error occured.
 */
uint8_t pi_tlv320_power(pi_device_t* tlv320_dev, tlv320_state_t state);

/**
 * @brief   TLV320 clock configuration. Set the device's clock
 *          into slave mode based on internal PLL mechanism.
 * 
 * @param[inout] tlv320_dev TLV320 instance pointer.  
 */
void pi_tlv320_set_slave_clk_pll(pi_device_t* tlv320_dev);

/**
 * @brief   TLV320 clock configuration. Set the device's clock
 *          into slave mode based on Bit clock signal.
 * 
 * @param[inout] tlv320_dev TLV320 instance pointer.  
 */
void pi_tlv320_set_slave_clk_bclk(pi_device_t* tlv320_dev);

/**
 * @brief   TLV320 clock configuration. Set the device's clock
 *          into slave mode based on a ratio from FSYNC signal.
 * 
 * @param[inout]    tlv320_dev TLV320 instance pointer.  
 * @param[in]       ratio Ratio from FSYNC signal.
 *                  @arg See @ref TLV320_IS_VALID_MCLK_RATIO_SEL for accepted values.
 */
void pi_tlv320_set_slave_clk_mclk(pi_device_t* tlv320_dev, uint8_t ratio);

/**
 * @brief   TLV320 clock configuration. Set the device's clock
 *          into master mode
 * 
 * @param[inout]    tlv320_dev  TLV320 instance pointer. 
 * @param[in]       fs          ASI bus frame synchronization signal configuration
 * @param[in]       mclk        Master clock configuration structure
 */
void pi_tlv320_set_master_clk(pi_device_t* tlv320_dev, tlv320_fs_t fs, tlv320_mclk_t mclk);

/**
 * @brief   TLV320 clock configuration. This function allows to set all
 *          clocks parameters from main clock configuration structure.
 * 
 * @param[inout]    tlv320_dev  TLV320 instance pointer.  
 * @param[in]       clk_cfg     Clock configuration parameters structure.
 */
void pi_tlv320_set_clk(pi_device_t* tlv320_dev, tlv320_clk_cfg_t clk_cfg);

/**
 * @brief   Control TLV320 functional state by setting its SHDNZ pin to LOW (for shutdown state)
 *          or HIGH (for active state). 
 * 
 * @param tlv320_dev    TLV320 instance pointer
 * @param state         Enable or disable hardware shutdown mode
 *                      @arg @ref TLV320_SHUTDOWN   Set the device into shutdown mode
 *                      @arg @ref TLV320_ACTIVE     Set the device into active mode. 
 */
void pi_tlv320_set_shutdown_mode(pi_device_t* tlv320_dev, tlv320_hw_mode_t state);

/**
 * @}
*/

/**
 * @}
 */