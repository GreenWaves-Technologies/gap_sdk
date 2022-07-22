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

/*
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "pmsis.h"
#include "bsp/bsp.h"

/**
 * @brief TLV320 object definition. pi_device's data.
 */
typedef struct
{
    struct pi_device i2c_dev; // I2C device instance
} tlv320_t;

// Module related global variable 

//! List of all ASI_CH registers that match each channel. 
tlv320_config_register_t tlv320_asi_ch_reg[TLV320_CHANNEL_NB] = {TLV320_ASI_CH1, TLV320_ASI_CH2, TLV320_ASI_CH3, TLV320_ASI_CH4, TLV320_ASI_CH5, TLV320_ASI_CH6, TLV320_ASI_CH7, TLV320_ASI_CH8};
//! List of all CFG0 registers that match each channel.  
tlv320_config_register_t tlv320_ch_cfg0_reg[TLV320_ANALOG_CHANNEL_NB] = {TLV320_CH1_CFG0, TLV320_CH2_CFG0, TLV320_CH3_CFG0, TLV320_CH4_CFG0};
//! List of all CFG1 registers that match each channel.  
tlv320_config_register_t tlv320_ch_cfg1_reg[TLV320_ANALOG_CHANNEL_NB] = {TLV320_CH1_CFG1, TLV320_CH2_CFG1, TLV320_CH3_CFG1, TLV320_CH4_CFG1};
//! List of all CFG2 registers that match each channel.  
tlv320_config_register_t tlv320_ch_cfg2_reg[TLV320_CHANNEL_NB] = {TLV320_CH1_CFG2, TLV320_CH2_CFG2, TLV320_CH3_CFG2, TLV320_CH4_CFG2, TLV320_CH5_CFG2, TLV320_CH6_CFG2, TLV320_CH7_CFG2, TLV320_CH8_CFG2};
//! List of all CFG3 registers that match each channel.  
tlv320_config_register_t tlv320_ch_cfg3_reg[TLV320_CHANNEL_NB] = {TLV320_CH1_CFG3, TLV320_CH2_CFG3, TLV320_CH3_CFG3, TLV320_CH4_CFG3, TLV320_CH5_CFG3, TLV320_CH6_CFG3, TLV320_CH7_CFG3, TLV320_CH8_CFG3};
//! List of all CFG4 registers that match each channel.  
tlv320_config_register_t tlv320_ch_cfg4_reg[TLV320_CHANNEL_NB] = {TLV320_CH1_CFG4, TLV320_CH2_CFG4, TLV320_CH3_CFG4, TLV320_CH4_CFG4, TLV320_CH5_CFG4, TLV320_CH6_CFG4, TLV320_CH7_CFG4, TLV320_CH8_CFG4};
//! List of all GPO registers that match each channel.  
tlv320_config_register_t tlv320_gpo_cfg_reg[TLV320_ANALOG_CHANNEL_NB] = {TLV320_GPO_CFG0, TLV320_GPO_CFG1, TLV320_GPO_CFG2, TLV320_GPO_CFG3};
//! List of all GPI registers that match each channel.  
tlv320_config_register_t tlv320_gpi_cfg_reg[TLV320_ANALOG_CHANNEL_NB] = {TLV320_GPI_CFG0, TLV320_GPI_CFG0, TLV320_GPI_CFG1, TLV320_GPI_CFG1};

// ************************************************************************************************
//
//  Helpers
//
// ************************************************************************************************

/**
 * @brief Perform a write operation via I2C. 
 * 
 * @param[in] tlv320_dev  TLV320 data instance
 * @param[in] reg         Register to read
 * @param[in] value       Value to write into the register
 * @return    uint8_t     Error management. Returns @ref TLV320_OK if no error
 *                        occured. @ref TLV320_WRITE_ERROR otherwise
 * @note                  This function does not take into account the TLV320 
 *                        "page" concept. TODO redirect to right function. 
 */
uint8_t __pi_tlv320_write(tlv320_t *tlv320_data, uint8_t reg, uint8_t value)
{
  pi_assert(tlv320_data);
  uint8_t buf[TLV320_I2C_MSG_SIZE] = {(uint8_t)reg, value};
  uint8_t retval = TLV320_NO_ERROR;
  if (pi_i2c_write(&(tlv320_data->i2c_dev), buf, TLV320_I2C_MSG_SIZE, TLV320_I2C_WRITE_FLAGS))
  {
    retval = TLV320_WRITE_ERROR;
  }
  return retval;
}

/**
 * @brief Perform a read operation via I2C. 
 * 
 * @param tlv320_data TLV320 data instance
 * @param reg         Register to read
 * @return uint8_t    Register value.
 * @note              This function does not take into account the TLV320 "page"
 *                    concept. TODO redirect to right function. 
 */
uint8_t __pi_tlv320_read(tlv320_t *tlv320_data, uint8_t reg)
{
    pi_assert(tlv320_data);
    uint8_t reg_value = 0x00;
    pi_i2c_write_read(&(tlv320_data->i2c_dev), &reg, &reg_value, TLV320_I2C_READ_TX_SIZE, TLV320_I2C_READ_RX_SIZE);
    return reg_value;
}

/**
 * @brief Get the right slave address depending on how ADDRs pin are set. 
 * 
 * @param[in] conf  ADDR pin configuration 
 * @return uint8_t  TLV320's hardware based slave adress. 
 *                  @arg TLV320_SLAVE_ADDRESS_1 if ADDR1 = 0 & ADDR0 = 0. 
 *                  @arg TLV320_SLAVE_ADDRESS_2 if ADDR1 = 0 & ADDR0 = 1. 
 *                  @arg TLV320_SLAVE_ADDRESS_3 if ADDR1 = 1 & ADDR0 = 0. 
 *                  @arg TLV320_SLAVE_ADDRESS_4 if ADDR1 = 1 & ADDR0 = 1.
 *                  @arg TLV320_SLAVEADDRESS_ERROR if the configuration is not valid.
 */
static uint8_t __pi_tlv320_get_slave_address(struct pi_tlv320_conf *tlv320_conf)
{
  uint8_t retval = TLV320_SLAVE_ADDRESS_1;
  pi_assert(tlv320_conf);
  retval |= ((tlv320_conf->addr0_sclk) << TLV320_ADDR0_SLAVE_ADDR_BITSHIFT);
  retval |= ((tlv320_conf->addr1_miso) << TLV320_ADDR1_SLAVE_ADDR_BITSHIFT);
  return retval;
}

/**
 * @brief Generic function to encode TLV320 settings such as : 
 *          - Digital volume control (Register Chx_CFG2)
 *          - Gain calibration (Register Chx_CFG3)
 *        This function convert from dB unit to 8bit representation.
 * 
 * @param value Value to convert [dB]
 * @param base  Conversion base. Value that match 0dB
 * @param step  dB resolution per bit.
 * @return uint8_t Encoded value
 */
static uint8_t __tlv320_encode(float value, uint8_t base, float step)
{
  uint8_t encoded_value = base + (value / step); 
  return encoded_value;
}

/**
 * @brief Generic function to decode TLV320 settings such as : 
 *          - Digital volume control (Register Chx_CFG2)
 *          - Gain calibration (Register Chx_CFG3)
 *        This function convert from 8bit representation to dB unit.
 * 
 * @param value Value to decode [8bits]
 * @param base  Conversion base. Value that match 0dB
 * @param step  dB resolution per bit.
 * @return uint8_t Decoded value [dB]
 */
static float __tlv320_decode(uint8_t value, uint8_t base, float step)
{
  float decoded_value = (float)(value - base) * step;
  return decoded_value;
}

// ************************************************************************************************
//
//  API Implementation
//
// ************************************************************************************************

uint8_t pi_tlv320_write_config_register(pi_device_t *tlv320_dev, tlv320_config_register_t reg, uint8_t value)
{
    uint8_t retval = TLV320_NO_ERROR;
    
    pi_assert(tlv320_dev);
    pi_assert(tlv320_dev->data);
    pi_assert(TLV320_IS_VALID_CONFIG_REG(reg));

    if(!__pi_tlv320_write(tlv320_dev->data, TLV320_PAGE_CFG, TLV320_PAGE_CONFIG_REG))
    {
        retval = __pi_tlv320_write(tlv320_dev->data, (uint8_t)reg, value);
    }
    else
    {
      retval = TLV320_WRITE_ERROR;
    }
    return retval;
}

uint8_t pi_tlv320_read_config_register(pi_device_t *tlv320_dev, tlv320_config_register_t reg, uint8_t* read_value)
{
  uint8_t retval = TLV320_NO_ERROR;
  
  pi_assert(tlv320_dev);
  pi_assert(tlv320_dev->data);

  if(!__pi_tlv320_write(tlv320_dev->data, TLV320_PAGE_CFG, TLV320_PAGE_CONFIG_REG))
  {
    *read_value = __pi_tlv320_read(tlv320_dev->data, (uint8_t)reg);
  }
  else
  {
    retval = TLV320_READ_ERROR;
  }
  return retval;
}

int pi_tlv320_open(pi_device_t *tlv320_dev)
{   
    int retval = 0;
    tlv320_t *tlv320_data = (tlv320_t *)pmsis_l2_malloc(sizeof(tlv320_t));
    struct pi_tlv320_conf *tlv320_conf = (struct pi_tlv320_conf *)tlv320_dev->config; // Get device configuration
    uint8_t slave_adress = 0x00; // I2C slave addr. Will be identify from ADDR0 & ADDR1 from the configuration

    pi_assert(tlv320_dev);
    pi_assert(tlv320_data);

    // Store TLV320 instance into the device
    (tlv320_dev->data) = (void *)tlv320_data;

    // Configure TLV320 BSP 
    if (bsp_tlv320_open(tlv320_conf))
    {
        pmsis_l2_malloc_free(tlv320_data, sizeof(tlv320_t));
        retval = -2;
    }
    else
    {
      // Configure TLV320 PMSIS interface
      struct pi_i2c_conf i2c_conf;
      pi_i2c_conf_init(&i2c_conf);

      // Configure I2C interface according to tlv320 configuration
      i2c_conf.itf = tlv320_conf->i2c_itf;
      i2c_conf.max_baudrate = tlv320_conf->i2c_max_baudrate;
      slave_adress = __pi_tlv320_get_slave_address(tlv320_conf);

      // Set the slave address shifted to include the read/write bit.
      pi_i2c_conf_set_slave_addr(&i2c_conf, (slave_adress << 1) , 0);

      // Initialize TLV320 I2C communication
      pi_open_from_conf(&tlv320_data->i2c_dev, &i2c_conf);
      if (pi_i2c_open(&tlv320_data->i2c_dev))
      {
          pmsis_l2_malloc_free(tlv320_data, sizeof(tlv320_t));
          retval = -2;
      }
      else
      {
          pi_tlv320_reset(tlv320_dev);
          pi_tlv320_set_sleep_mode(tlv320_dev, TLV320_SLEEP_CFG_SLEEP_ENZ_DISABLED);
          pi_tlv320_set_bias_cfg(tlv320_dev, TLV320_MICBIAS_VREF, TLV320_ADC_FSCALE_VREF_2_75);
          pi_tlv320_set_asi(tlv320_dev, pi_tlv320_build_default_asi());
          pi_tlv320_set_slave_clk_pll(tlv320_dev);
      } 
    }
    /* 
        NOTE:
        In application

        tlv320_analog_ch_t channel2_cfg = pi_tlv320_build_default_analog_ch(TLV320_CHANNEL_2, TLV320_TDM_SLOT_DEFAULT)
        pi_tlv320_write_analog_ch(tlv320_dev, channel2_cfg);
        
        pi_tlv320_set_channel_input(tlv320_dev, TLV320_CHANNEL_2, TLV320_CHANNEL_ENABLE);
        pi_tlv320_set_channel_output(tlv320_dev, TLV320_CHANNEL_2, TLV320_CHANNEL_ENABLE);

        pi_tlv320_power(tlv320_dev, TLV320_ENABLE);
    */
    return retval;
}

void pi_tlv320_close(pi_device_t *tlv320_dev)
{
  tlv320_t *tlv320_data = (tlv320_t *)tlv320_dev->data;
  pmsis_l2_malloc_free(tlv320_data, sizeof(tlv320_t));
}

void pi_tlv320_reset(pi_device_t* tlv320_dev)
{
  pi_assert(tlv320_dev);
  tlv320_sw_reset_t reg;
  reg.raw = 0x00;
  reg.sw_reset = TLV320_SW_RESET_RESET;
  pi_tlv320_write_config_register(tlv320_dev, TLV320_SW_RESET, reg.raw);
}     

void pi_tlv320_set_sleep_mode(pi_device_t* tlv320_dev, uint8_t mode)
{
    pi_assert(tlv320_dev);
    pi_assert(TLV320_IS_VALID_SLEEP_MODE(mode));

    tlv320_sleep_cfg_t reg = {0};
    int delay = TLV320_SLEEPMODE_SLEEP_DELAY;

    reg.sleep_enz = mode;

    if(TLV320_SLEEP_CFG_SLEEP_ENZ_DISABLED == mode)
    {
        reg.areg_select = TLV320_SLEEP_CFG_AREG_SELECT_INTERNAL;
        delay = TLV320_SLEEPMODE_WAKEUP_DELAY;
    }
    pi_tlv320_write_config_register(tlv320_dev, TLV320_SLEEP_CFG, reg.raw);
    pi_time_wait_us(delay); // Wait a certain time according to the documentation
}

void pi_tlv320_set_asi(pi_device_t* tlv320_dev, tlv320_asi_t asi)
{
    tlv320_asi_cfg0_t reg_cfg0;
    tlv320_asi_cfg1_t reg_cfg1;

    pi_assert(tlv320_dev);
    pi_assert(TLV320_IS_VALID_ASI_FORMAT(asi.format));
    pi_assert(TLV320_IS_VALID_ASI_WLEN(asi.wlen));
    pi_assert(TLV320_IS_VALID_ASI_FSYNC_POL(asi.fsync_pol));
    pi_assert(TLV320_IS_VALID_ASI_BCLK_POL(asi.bclk_pol));
    pi_assert(TLV320_IS_VALID_ASI_TX_EDGE(asi.tx_edge));
    pi_assert(TLV320_IS_VALID_ASI_TX_FILL(asi.tx_fill));
    pi_assert(TLV320_IS_VALID_ASI_TX_LSB_CYCLE(asi.tx_lsb));
    pi_assert(TLV320_IS_VALID_ASI_TX_KEEPER(asi.tx_keeper));
    pi_assert(TLV320_IS_VALID_TX_OFFSET(asi.tx_offset));

    reg_cfg0.tx_fill    = asi.tx_fill;
    reg_cfg0.tx_edge    = asi.tx_edge;
    reg_cfg0.bclk_pol   = asi.bclk_pol;
    reg_cfg0.fsync_pol  = asi.fsync_pol;
    reg_cfg0.asi_wlen   = asi.wlen;
    reg_cfg0.asi_format = asi.format;
    
    reg_cfg1.tx_offset = asi.tx_offset;
    reg_cfg1.tx_keeper = asi.tx_keeper;
    reg_cfg1.tx_lsb    = asi.tx_lsb;

    pi_tlv320_write_config_register(tlv320_dev, TLV320_ASI_CFG0, reg_cfg0.raw);
    pi_tlv320_write_config_register(tlv320_dev, TLV320_ASI_CFG1, reg_cfg1.raw);
}

uint8_t pi_tlv320_get_asi(pi_device_t* tlv320_dev, tlv320_asi_t *asi)
{
    pi_assert(tlv320_dev);
    pi_assert(asi);

    uint8_t retval = TLV320_NO_ERROR;

    tlv320_asi_cfg0_t reg_cfg0;
    tlv320_asi_cfg1_t reg_cfg1;

    if( (pi_tlv320_read_config_register(tlv320_dev, TLV320_ASI_CFG0, &(reg_cfg0.raw)) == TLV320_NO_ERROR) &&
        (pi_tlv320_read_config_register(tlv320_dev, TLV320_ASI_CFG1, &(reg_cfg1.raw)) == TLV320_NO_ERROR))
    {
      asi->tx_fill   = reg_cfg0.tx_fill;
      asi->tx_edge   = reg_cfg0.tx_edge;
      asi->bclk_pol  = reg_cfg0.bclk_pol;
      asi->fsync_pol = reg_cfg0.fsync_pol;
      asi->wlen      = reg_cfg0.asi_wlen;
      asi->format    = reg_cfg0.asi_format;
      
      asi->tx_offset = reg_cfg1.tx_offset;
      asi->tx_keeper = reg_cfg1.tx_keeper;
      asi->tx_lsb    = reg_cfg1.tx_lsb;
    }
    else
    {
      retval = TLV320_READ_ERROR;
    }
    return retval;
}

tlv320_asi_t pi_tlv320_build_default_asi(void)
{
    tlv320_asi_t asi; 
    asi.format    = TLV320_TDM;
    asi.wlen      = TLV320_WLEN_32;
    asi.fsync_pol = TLV320_FSYNCPOL_NORMAL;
    asi.bclk_pol  = TLV320_BCLKPOL_NORMAL;
    asi.tx_edge   = TLV320_TXEDGE_NORMAL;
    asi.tx_fill   = TLV320_TXFILL_ZERO;
    asi.tx_lsb    = TLV320_TX_LSB_FULLCYCLE;
    asi.tx_keeper = TLV320_TX_KEEPER_DISABLED;
    asi.tx_offset = TLV320_ASI_CFG1_TX_OFFSET_DEFAULT;
    return asi;
}

tlv320_analog_ch_t pi_tlv320_build_default_analog_ch(uint8_t channel_id, uint8_t slot_id)
{
    tlv320_analog_ch_t channel;

    pi_assert(TLV320_IS_VALID_ANALOG_CHANNEL_ID(channel_id));
    pi_assert(TLV320_IS_VALID_TDM_SLOT_ID(slot_id));

    channel.id              = channel_id;
    channel.input.type      = TLV320_INTYP_MICRO; 
    channel.input.src       = TLV320_INSRC_ANALOG_SINGLE;
    channel.input.coupling  = TLV320_INPUT_AC_COUPLED;
    channel.input.z         = TLV320_INPUT_Z_2_5_KOHM;
    channel.input.agcdre    = TLV320_AGCDRE_DISABLED;
    channel.output.slot     = slot_id;
    channel.output.line     = TLV320_OUTPUT_PRIMARY;
    channel.gain            = TLV320_CHx_GAIN_DEFAULT;
    channel.volume          = TLV320_CHx_VOLUME_DEFAULT;
    channel.gain_calib      = TLV320_CHx_GAIN_CALIB_DEFAULT;
    channel.phase_calib     = TLV320_CHx_PHASE_CALIB_DEFAULT;

    return channel;
}

uint8_t pi_tlv320_write_analog_ch(pi_device_t* tlv320_dev, tlv320_analog_ch_t channel)
{
    uint8_t retval = TLV320_NO_ERROR;
    uint8_t error_cnt  = 0; 
    tlv320_ch_cfg0_t reg_ch_cfg0 = {0};
    tlv320_ch_cfg1_t reg_ch_cfg1 = {0};
    tlv320_ch_cfg2_t reg_ch_cfg2 = {0};
    tlv320_ch_cfg3_t reg_ch_cfg3 = {0};
    tlv320_ch_cfg4_t reg_ch_cfg4 = {0};
    tlv320_asi_ch_t  reg_asi_ch  = {0};
    tlv320_gpo_cfg_t reg_gpo_cfg = {0};
    tlv320_gpi_cfg_t reg_gpi_cfg = {0};

    pi_assert(tlv320_dev);
    pi_assert(TLV320_IS_VALID_ANALOG_CHANNEL_ID(channel.id));
    pi_assert(TLV320_IS_VALID_INTYP(channel.input.type));
    pi_assert(TLV320_IS_VALID_ANALOG_CH(channel.input.src));
    pi_assert(TLV320_IS_VALID_COUPLING(channel.input.coupling));
    pi_assert(TLV320_IS_VALID_INPUT_Z(channel.input.z));
    pi_assert(TLV320_IS_VALID_DRE_AGC(channel.input.agcdre));
    pi_assert(TLV320_IS_VALID_TDM_SLOT_ID(channel.output.slot));
    pi_assert(TLV320_IS_VALID_ASI_CHx_OUTPUT(channel.output.line));
    pi_assert(TLV320_IS_VALID_CH_GAIN(channel.gain));
    pi_assert(TLV320_IS_VALID_CH_VOLUME(channel.volume));
    pi_assert(TLV320_IS_VALID_CH_GAIN_CALIB(channel.gain_calib));
    pi_assert(TLV320_IS_VALID_CH_PHASE_CALIB(channel.phase_calib)); 

    // Disable GPIx and GPOx
    reg_gpo_cfg.cfg = TLV320_GPOx_CFG_DISABLED;
    reg_gpo_cfg.drv = TLV320_GPOx_DRV_HI_Z;

    // Need to read GPI register to avoid erasing other channel setting.
    if(pi_tlv320_read_config_register(tlv320_dev, tlv320_gpi_cfg_reg[channel.id], &(reg_gpi_cfg.raw)) == TLV320_NO_ERROR)
    {
      // Writting correct bits depending on the channel id;
      if(channel.id % 2)
      {
        reg_gpi_cfg.cfg_x = TLV320_GPIx_CFG_DISABLED;
      }
      else
      {
        reg_gpi_cfg.cfg_y = TLV320_GPIx_CFG_DISABLED;
      }
                          
      reg_asi_ch.output     = channel.output.line;
      reg_asi_ch.slot       = channel.output.slot;
      reg_ch_cfg0.ch_intyp  = channel.input.type;
      reg_ch_cfg0.ch_insrc  = channel.input.src;
      reg_ch_cfg0.ch_dc     = channel.input.coupling;
      reg_ch_cfg0.ch_imp    = channel.input.z;
      reg_ch_cfg0.ch_dreen  = channel.input.agcdre;
      reg_ch_cfg1.ch_gain   = channel.gain;
      
      // Convert gain and volume from dB to 8bit representation 
      reg_ch_cfg2.ch_dvol = __tlv320_encode(channel.volume, TLV320_CHx_VOLUME_CODE_O_DB, TLV320_CHx_VOLUME_STEP);
      reg_ch_cfg3.ch_gcal = __tlv320_encode(channel.gain_calib, TLV320_CHx_GAIN_CALIB_CODE_0_DB, TLV320_CHx_GAIN_CALIB_STEP);
      reg_ch_cfg4.ch_pcal = channel.phase_calib;

      error_cnt += pi_tlv320_write_config_register(tlv320_dev, tlv320_gpo_cfg_reg[channel.id], reg_gpo_cfg.raw);
      error_cnt += pi_tlv320_write_config_register(tlv320_dev, tlv320_gpi_cfg_reg[channel.id], reg_gpi_cfg.raw);
      error_cnt += pi_tlv320_write_config_register(tlv320_dev, tlv320_asi_ch_reg[channel.id],  reg_asi_ch.raw);
      error_cnt += pi_tlv320_write_config_register(tlv320_dev, tlv320_ch_cfg0_reg[channel.id], reg_ch_cfg0.raw);
      error_cnt += pi_tlv320_write_config_register(tlv320_dev, tlv320_ch_cfg1_reg[channel.id], reg_ch_cfg1.raw);
      error_cnt += pi_tlv320_write_config_register(tlv320_dev, tlv320_ch_cfg2_reg[channel.id], reg_ch_cfg2.raw);
      error_cnt += pi_tlv320_write_config_register(tlv320_dev, tlv320_ch_cfg3_reg[channel.id], reg_ch_cfg3.raw);
      error_cnt += pi_tlv320_write_config_register(tlv320_dev, tlv320_ch_cfg4_reg[channel.id], reg_ch_cfg4.raw);

      if(error_cnt)
      {
        retval = TLV320_WRITE_ERROR;
      }
    }
    else
    {
      retval = TLV320_READ_ERROR;
    }
    return retval;

}

uint8_t pi_tlv320_read_analog_ch(pi_device_t* tlv320_dev, uint8_t channel_id, tlv320_analog_ch_t *an_channel)
{
    uint8_t retval = TLV320_NO_ERROR;
    uint8_t error_cnt = 0;

    tlv320_ch_cfg0_t    reg_ch_cfg0 = {0};
    tlv320_ch_cfg1_t    reg_ch_cfg1 = {0};
    tlv320_ch_cfg2_t    reg_ch_cfg2 = {0};
    tlv320_ch_cfg3_t    reg_ch_cfg3 = {0};
    tlv320_ch_cfg4_t    reg_ch_cfg4 = {0};
    tlv320_asi_ch_t     reg_asi_ch  = {0};

    pi_assert(tlv320_dev);
    pi_assert(TLV320_IS_VALID_ANALOG_CHANNEL_ID(channel_id));

    error_cnt += pi_tlv320_read_config_register(tlv320_dev, tlv320_ch_cfg0_reg[channel_id], &(reg_ch_cfg0.raw));
    error_cnt += pi_tlv320_read_config_register(tlv320_dev, tlv320_ch_cfg1_reg[channel_id], &(reg_ch_cfg1.raw));
    error_cnt += pi_tlv320_read_config_register(tlv320_dev, tlv320_ch_cfg2_reg[channel_id], &(reg_ch_cfg2.raw));
    error_cnt += pi_tlv320_read_config_register(tlv320_dev, tlv320_ch_cfg3_reg[channel_id], &(reg_ch_cfg3.raw));
    error_cnt += pi_tlv320_read_config_register(tlv320_dev, tlv320_ch_cfg4_reg[channel_id], &(reg_ch_cfg4.raw));
    error_cnt += pi_tlv320_read_config_register(tlv320_dev, tlv320_asi_ch_reg[channel_id],  &(reg_asi_ch.raw));

    if(error_cnt)
    {
        retval = TLV320_READ_ERROR;
    }
    else
    {
        an_channel->output.line = reg_asi_ch.output;
        an_channel->output.slot = reg_asi_ch.slot;

        an_channel->input.type     = reg_ch_cfg0.ch_intyp;
        an_channel->input.src      = reg_ch_cfg0.ch_insrc;
        an_channel->input.coupling = reg_ch_cfg0.ch_dc;
        an_channel->input.z        = reg_ch_cfg0.ch_imp;
        an_channel->input.agcdre   = reg_ch_cfg0.ch_dreen;
        an_channel->gain           = reg_ch_cfg1.ch_gain;
      
        // Convert gain and volume from 8bit representation to dB; 
        an_channel->volume       = __tlv320_decode(reg_ch_cfg2.ch_dvol, TLV320_CHx_VOLUME_CODE_O_DB, TLV320_CHx_VOLUME_STEP);
        an_channel->gain_calib   = __tlv320_decode(reg_ch_cfg3.ch_gcal, TLV320_CHx_GAIN_CALIB_CODE_0_DB, TLV320_CHx_GAIN_CALIB_STEP);
        an_channel->phase_calib  = reg_ch_cfg4.ch_pcal;

        an_channel->id = channel_id;
    }
    return retval;
}

uint8_t pi_tlv320_set_channel_input(pi_device_t* tlv320_dev, uint8_t channel, tlv320_state_t state)
{
  pi_assert(tlv320_dev);
  pi_assert(TLV320_IS_VALID_CHANNEL_ID(channel));
  pi_assert(TLV320_IS_VALID_STATE(state));

  uint8_t retval = TLV320_NO_ERROR;
  tlv320_in_ch_en_t in_ch_en = {0};
  uint8_t mask = 1 << (7 - channel);
  
  if(pi_tlv320_read_config_register(tlv320_dev, TLV320_IN_CH_EN, &(in_ch_en.raw)) != TLV320_NO_ERROR)
  {
      retval = TLV320_READ_ERROR;
  }
  else
  {  
    in_ch_en.raw = (TLV320_ENABLE == state) ? (in_ch_en.raw | mask) : (in_ch_en.raw & ~mask);
    if(pi_tlv320_write_config_register(tlv320_dev, TLV320_IN_CH_EN, in_ch_en.raw) != TLV320_NO_ERROR)
    {
      retval = TLV320_WRITE_ERROR;
    }
  }
  return retval;
}

uint8_t pi_tlv320_set_channel_output(pi_device_t* tlv320_dev, uint8_t channel_id, tlv320_state_t state)
{
  pi_assert(tlv320_dev);
  pi_assert(TLV320_IS_VALID_CHANNEL_ID(channel_id));
  pi_assert(TLV320_IS_VALID_CHANNEL_STATE(state));
  
  uint8_t retval = TLV320_NO_ERROR;
  tlv320_asi_out_ch_en_t asi_out_reg = {0};
  uint8_t mask = 1 << (7 - channel_id);

  if(pi_tlv320_read_config_register(tlv320_dev, TLV320_ASI_OUT_CH_EN, &(asi_out_reg.raw)) != TLV320_NO_ERROR)
  {
      retval = TLV320_READ_ERROR;
  }
  else
  {  
      asi_out_reg.raw = (TLV320_ENABLE == state) ? (asi_out_reg.raw | mask) : (asi_out_reg.raw & ~mask);
      if(pi_tlv320_write_config_register(tlv320_dev, TLV320_ASI_OUT_CH_EN, asi_out_reg.raw))
      {
          retval = TLV320_WRITE_ERROR;
      }
  }
  return retval;
}

void pi_tlv320_set_bias_cfg(pi_device_t* tlv320_dev, tlv320_micbias_t micbias, tlv320_afc_fscale_t adc_fscale)
{
    pi_assert(tlv320_dev);
    pi_assert(TLV320_IS_VALID_MICBIAS(bias));
    pi_assert(TLV320_IS_VALID_MICBIAS(adcfscale));

    tlv320_bias_cfg_t bias_cfg_reg = {0};

    bias_cfg_reg.adc_fscale = adc_fscale;
    bias_cfg_reg.mbias_val = micbias;

    pi_tlv320_write_config_register(tlv320_dev, TLV320_BIAS_CFG, bias_cfg_reg.raw);
}

uint8_t pi_tlv320_power(pi_device_t* tlv320_dev, tlv320_state_t state)
{    
    pi_assert(tlv320_dev);
    pi_assert(TLV320_IS_VALID_CHANNEL_STATE(state));

    tlv320_pwr_cfg_t pwr_cfg_reg = {0}; 
    uint8_t retval = TLV320_NO_ERROR;

    if(pi_tlv320_read_config_register(tlv320_dev, TLV320_PWR_CFG, &(pwr_cfg_reg.raw)) != TLV320_NO_ERROR)
    {
        retval = TLV320_READ_ERROR;
    }
    else
    {
        pwr_cfg_reg.adc_pdz = (state == TLV320_ENABLE) ? 1 : 0; 
        pwr_cfg_reg.micbias_pdz = (state == TLV320_ENABLE) ? 1 : 0; 
        pwr_cfg_reg.pll_pdz = (state == TLV320_ENABLE) ? 1 : 0; 
        if(pi_tlv320_write_config_register(tlv320_dev, TLV320_PWR_CFG, pwr_cfg_reg.raw) != TLV320_NO_ERROR)
        {
          retval = TLV320_WRITE_ERROR;
        }
    }
    return retval;
}

void pi_tlv320_set_slave_clk_pll(pi_device_t* tlv320_dev)
{
    tlv320_clk_cfg_t clk = {0};

    pi_assert(tlv320_dev);

    clk.mode.master   = TLV320_MSTCFG0_SLAVE;
    clk.mode.auto_clk = TLV320_MSTCFG0_AUTOCLK_ENABLE;
    clk.mode.auto_pll = TLV320_MSTCFG0_PLLAUTOMODE_ENABLE;
    pi_tlv320_set_clk(tlv320_dev, clk);
}

void pi_tlv320_set_slave_clk_bclk(pi_device_t* tlv320_dev)
{
    tlv320_clk_cfg_t clk = {0};

    pi_assert(tlv320_dev);

    clk.mode.master   = TLV320_MSTCFG0_SLAVE;
    clk.mode.auto_clk = TLV320_MSTCFG0_AUTOCLK_ENABLE;
    clk.mode.auto_pll = TLV320_MSTCFG0_PLLAUTOMODE_DISABLE;
    clk.mode.clk_src  = TLV320_CLKSRC_AUDIOROOT_BCLK;
    pi_tlv320_set_clk(tlv320_dev, clk);
}

void pi_tlv320_set_slave_clk_mclk(pi_device_t* tlv320_dev, uint8_t ratio)
{
    tlv320_clk_cfg_t clk = {0};

    pi_assert(tlv320_dev);
    pi_assert(TLV320_IS_VALID_MCLK_RATIO_SEL(ratio));

    clk.mode.master   = TLV320_MSTCFG0_SLAVE;
    clk.mode.auto_clk = TLV320_MSTCFG0_AUTOCLK_ENABLE;
    clk.mode.auto_pll = TLV320_MSTCFG0_PLLAUTOMODE_DISABLE;
    clk.mode.clk_src  = TLV320_CLKSRC_AUDIOROOT_MCLK;
    clk.mclk.fs_ratio = ratio;

    pi_tlv320_set_clk(tlv320_dev, clk);
}

void pi_tlv320_set_master_clk(pi_device_t* tlv320_dev, tlv320_fs_t fs, tlv320_mclk_t mclk)
{
    tlv320_clk_cfg_t clk = {0};

    pi_assert(tlv320_dev);
    pi_assert(TLV320_IS_VALID_FS_CFG(fs));
    pi_assert(TLV320_IS_VALID_MCLK_CFG(mclk));

    clk.mode.master   = TLV320_MSTCFG0_MASTER;
    clk.mode.auto_clk = TLV320_MSTCFG0_AUTOCLK_ENABLE;
    clk.mode.auto_pll = TLV320_MSTCFG0_PLLAUTOMODE_DISABLE;
    clk.mode.clk_src  = TLV320_CLKSRC_AUDIOROOT_MCLK;

    clk.fs.mode       = fs.mode;
    clk.fs.rate       = fs.rate;
    clk.fs.gate       = fs.gate;
    clk.fs.bclk_ratio = fs.bclk_ratio;

    clk.mclk.mode     = mclk.mode;
    clk.mclk.fs_ratio = mclk.fs_ratio;
    clk.mclk.select   = mclk.select;

    pi_tlv320_set_clk(tlv320_dev, clk);
}

void pi_tlv320_set_clk(pi_device_t* tlv320_dev, tlv320_clk_cfg_t clk_cfg)
{
    pi_assert(tlv320_dev);
    pi_assert(TLV320_IS_VALID_CLK_CFG(clk_cfg));

    tlv320_mst_cfg0_t mst_cfg0_reg = {0};
    tlv320_mst_cfg1_t mst_cfg1_reg = {0};
    tlv320_clk_src_t  clk_src_reg  = {0};

    mst_cfg0_reg.mclk_freq_sel      = clk_cfg.mclk.select;
    mst_cfg0_reg.fs_mode            = clk_cfg.fs.mode;
    mst_cfg0_reg.bclk_fsync_gate    = clk_cfg.fs.gate;
    mst_cfg0_reg.auto_mode_pll_dis  = clk_cfg.mode.auto_pll;
    mst_cfg0_reg.auto_clk_cfg       = clk_cfg.mode.auto_clk;
    mst_cfg0_reg.mst_slv_cfg        = clk_cfg.mode.master;

    mst_cfg1_reg.fs_bclk_ratio      = clk_cfg.fs.bclk_ratio;
    mst_cfg1_reg.fs_rate            = clk_cfg.fs.rate;

    clk_src_reg.mclk_ratio_sel      = clk_cfg.mclk.fs_ratio;
    clk_src_reg.mclk_freq_sel_mode  = clk_cfg.mclk.mode;
    clk_src_reg.dis_pll_slv_clk_src = clk_cfg.mode.clk_src;

    pi_tlv320_write_config_register(tlv320_dev, TLV320_MST_CFG0, mst_cfg0_reg.raw);
    pi_tlv320_write_config_register(tlv320_dev, TLV320_MST_CFG1, mst_cfg1_reg.raw);
    pi_tlv320_write_config_register(tlv320_dev, TLV320_CLK_SRC,  clk_src_reg.raw);
}

void pi_tlv320_set_shutdown_mode(pi_device_t* tlv320_dev, tlv320_hw_mode_t mode)
{
    int retval = 0;
    struct pi_tlv320_conf* tlv320_conf = (struct pi_tlv320_conf*)tlv320_dev->config;

    pi_assert(tlv320_dev);
    pi_assert(TLV320_IS_VALID_HW_MODE(mode));

    if(mode == TLV320_SHUTDOWN)
    {
        retval = tlv320_conf->shdnz.api.reset((void*)&(tlv320_conf->shdnz));
    }
    else
    {
        retval = tlv320_conf->shdnz.api.set((void*)&(tlv320_conf->shdnz));
    }
}
