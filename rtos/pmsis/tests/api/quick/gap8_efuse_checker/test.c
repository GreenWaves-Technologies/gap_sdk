/* 
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "pmsis.h"

int main()
{
    printf("Checking efuse\n");

    efuse_t conf;

    plp_efuse_startRead();
    conf.info.raw  = plp_efuse_readByte(GAP_EFUSE_INFO_REG);
    conf.info2.raw = plp_efuse_readByte(GAP_EFUSE_INFO2_REG);
    conf.info3.raw = plp_efuse_readByte(GAP_EFUSE_INFO3_REG);
    conf.info4.raw = plp_efuse_readByte(GAP_EFUSE_INFO4_REG);
    conf.info5.raw = plp_efuse_readByte(GAP_EFUSE_INFO5_REG);
    conf.info6.raw = plp_efuse_readByte(GAP_EFUSE_INFO6_REG);

    if (conf.info3.set_clkdiv == 0)
    {
        conf.info2.spim_clkdiv = 2;
    }

    if (conf.info.wait_xtal)
    {
        conf.wait_xtal_delta = plp_efuse_readShort(GAP_EFUSE_WAIT_XTAL_DELTA_REG_LSB, GAP_EFUSE_WAIT_XTAL_DELTA_REG_MSB);
        conf.wait_xtal_min = plp_efuse_readByte(GAP_EFUSE_WAIT_XTAL_MIN_REG);
        conf.wait_xtal_max = plp_efuse_readByte(GAP_EFUSE_WAIT_XTAL_MAX_REG);
    }

    conf.fll_assert_cycles = 0;
    conf.fll_lock_tolerance = 0;
    conf.fll_freq = 0;

    if (conf.info2.fll_freq_set)
        conf.fll_freq = plp_efuse_readByte(GAP_EFUSE_FLL_FREQ);

    if (conf.info2.ref_clk_wait)
        conf.ref_clk_wait_cycles = plp_efuse_readShort(GAP_EFUSE_REF_CLK_WAIT_CYCLES_LSB, GAP_EFUSE_REF_CLK_WAIT_CYCLES_MSB);

    if (conf.info3.ref_clk_wait_deep_sleep)
        conf.ref_clk_wait_cycles_deep_sleep = plp_efuse_readShort(GAP_EFUSE_REF_CLK_WAIT_CYCLES_DEEP_SLEEP_LSB, GAP_EFUSE_REF_CLK_WAIT_CYCLES_DEEP_SLEEP_MSB);

    if (conf.info2.fll_conf) {
        conf.fll_assert_cycles = plp_efuse_readByte(GAP_EFUSE_FLL_ASSERT_CYCLES);
        conf.fll_lock_tolerance = plp_efuse_readByte(GAP_EFUSE_FLL_TOLERANCE);
    }

    if (conf.info3.flash_type)
    {
        conf.hyper_rds_delay = plp_efuse_readByte(GAP_EFUSE_HYPER_RDS_DELAY); 
        conf.hyper_latency = plp_efuse_readByte(GAP_EFUSE_HYPER_LATENCY); 
    }

    if (conf.info6.flash_id)
        conf.flash_id = plp_efuse_readShort(GAP_EFUSE_FLASH_ID_LSB, GAP_EFUSE_FLASH_ID_MSB);
    else
    {
        // Default value for Hyperflash
        conf.flash_id = 0x1;
    }

    if (conf.info4.flash_cmd)
        conf.flash_cmd = plp_efuse_readByte(GAP_EFUSE_FLASH_CMD);

    if (conf.info4.flash_cmd2)
        conf.flash_cmd2 = plp_efuse_readByte(GAP_EFUSE_FLASH_CMD2);

    if (conf.info4.flash_cmd3)
        conf.flash_cmd3 = plp_efuse_readByte(GAP_EFUSE_FLASH_CMD3);

    if (conf.info4.flash_cmd4)
        conf.flash_cmd4 = plp_efuse_readByte(GAP_EFUSE_FLASH_CMD4);

    if (conf.info3.flash_wait)
        conf.flash_wait = plp_efuse_readByte(GAP_EFUSE_FLASH_WAIT);

    if (conf.info.encrypted)
    {
        for (int i=0; i<GAP_EFUSE_AES_KEY_NB_REGS; i++) {
        ((unsigned char *)conf.Key)[i] =  plp_efuse_readByte(GAP_EFUSE_AES_KEY_FIRST_REG + i);
        }
        for (int i=0; i<GAP_EFUSE_AES_IV_NB_REGS; i++) {
        ((unsigned char *)conf.IV)[i] = plp_efuse_readByte(GAP_EFUSE_AES_IV_FIRST_REG + i);
        }
    }

    if (conf.info6.i2c_loader)
    {
        conf.i2c_div = plp_efuse_readShort(GAP_EFUSE_I2C_DIV_LSB, GAP_EFUSE_I2C_DIV_MSB);
        conf.i2c_cs = plp_efuse_readByte(GAP_EFUSE_I2C_CS);
    }

    if (conf.info5.hyperchip_size)
        conf.hyperchip_size = plp_efuse_readByte(GAP_EFUSE_HYPERCHIP_SIZE);

    if (conf.info3.flash_reset_wait)
        conf.flash_reset_wait = plp_efuse_readByte(GAP_EFUSE_FLASH_RESET_WAIT);

    plp_efuse_sleep();
    

    printf("Info: raw: 0x%x, boot_mode: %d\n", conf.info.raw, conf.info.bootmode);

    printf("Info3: raw: 0x%x, flash_type: %d\n", conf.info3.raw, conf.info3.flash_type);

    plp_efuse_sleep();

    return 0;
}