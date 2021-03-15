/*
 * copyright (c) 2020 greenwaves technologies
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

#ifndef __BSP_CAMERA_HM0360_HM0360_H__
#define __BSP_CAMERA_HM0360_HM0360_H__

/*
 *  HM0360 camera registers
 */

/* Sensor ID registers */
#define HM0360_MODEL_ID_H     (0x0000)
#define HM0360_MODEL_ID_L     (0x0001)
#define HM0360_SILICON_REV    (0x0002)
#define HM0360_FRAME_COUNT_H  (0x0005)
#define HM0360_FRAME_COUNT_L  (0x0006)
#define HM0360_PIXEL_ORDER    (0x0007)

/* Sensor mode control registers */
#define HM0360_MODE_SELECT       (0x0100)
#define HM0360_IMAGE_ORIENTATION (0x0101)
#define HM0360_EMBEDDED_LINE_EN  (0x0102)
#define HM0360_SW_RESET          (0x0103)
#define HM0360_COMMAND_UPDATE    (0x0104)

/* Sensor exposure gain control registers */

#define HM0360_INTEGRATION_H  (0x0202)
#define HM0360_INTEGRATION_L  (0x0203)
#define HM0360_ANALOG_GAIN    (0x0205)
#define HM0360_DIGITAL_GAIN_H (0x020E)
#define HM0360_DIGITAL_GAIN_L (0x020F)

/* Clock control registers */

#define HM0360_PLL1CFG (0x0300)
#define HM0360_PLL2CFG (0x0301)
#define HM0360_PLL3CFG (0x0302)

/* Frame timing control registers */

#define HM0360_FRAME_LENGTH_LINES_H (0x0340)
#define HM0360_FRAME_LENGTH_LINES_L (0x0341)
#define HM0360_FRAME_LENGTH_PCK_H   (0x0342)
#define HM0360_FRAME_LENGTH_PCK_L   (0x0343)

/* Monochrome programming registers */

#define HM0360_MONO_MODE     (0x0370)
#define HM0360_MONO_MODE_ISP (0x0371)
#define HM0360_MONO_MODE_SEL (0x0372)

/* Sub-sampling / binning control registers */

#define HM0360_H_SUB        (0x0380)
#define HM0360_L_SUB        (0x0381)
#define HM0360_BINNING_MODE (0x0381)

/* Test pattern control registers */

#define HM0360_TEST_PATTERN_MODE (0x0601)
#define HM0360_TEST_DATA_BLUE_H  (0x0602)
#define HM0360_TEST_DATA_BLUE_L  (0x0603)
#define HM0360_TEST_DATA_GB_H    (0x0604)
#define HM0360_TEST_DATA_GB_L    (0x0605)
#define HM0360_TEST_DATA_GR_H    (0x0606)
#define HM0360_TEST_DATA_GR_L    (0x0607)
#define HM0360_TEST_DATA_RED_H   (0x0608)
#define HM0360_TEST_DATA_RED_L   (0x0609)

/* Black level control registers */

#define HM0360_RESERVED_1000 (0x1000)
#define HM0360_RESERVED_1003 (0x1003)
#define HM0360_BLC_TGT       (0x1004)
#define HM0360_RESERVED_1007 (0x1007)
#define HM0360_RESERVED_1008 (0x1008)
#define HM0360_BLC2_TGT      (0x1009)

/* Monochrome programming registers */

#define HM0360_MONO_CTRL     (0x100A)

/* VSYNC / HSYNC / pixel shift registers */

#define HM0360_OPFM_CTRL     (0x1014)

/* Tone mapping registers */

#define HM0360_CMPRS_01 (0x1030)
#define HM0360_CMPRS_02 (0x1031)
#define HM0360_CMPRS_03 (0x1032)
#define HM0360_CMPRS_04 (0x1033)
#define HM0360_CMPRS_05 (0x1034)
#define HM0360_CMPRS_06 (0x1035)
#define HM0360_CMPRS_07 (0x1036)
#define HM0360_CMPRS_08 (0x1037)
#define HM0360_CMPRS_09 (0x1038)
#define HM0360_CMPRS_10 (0x1039)
#define HM0360_CMPRS_11 (0x103A)
#define HM0360_CMPRS_12 (0x103B)
#define HM0360_CMPRS_13 (0x103C)
#define HM0360_CMPRS_14 (0x103D)
#define HM0360_CMPRS_15 (0x103E)
#define HM0360_CMPRS_16 (0x103F)

/* Automatic exposure programming registers */

#define HM0360_AE_CTRL          (0x2000)
#define HM0360_AE_CTRL1         (0x2001)
#define HM0360_CNT_ORG_H1       (0x2002)
#define HM0360_CNT_ORG_H2       (0x2003)
#define HM0360_CNT_ORG_V1       (0x2004)
#define HM0360_CNT_ORG_V2       (0x2005)
#define HM0360_CNT_ST_H1        (0x2006)
#define HM0360_CNT_ST_H2        (0x2007)
#define HM0360_CNT_ST_V1        (0x2008)
#define HM0360_CNT_ST_V2        (0x2009)
#define HM0360_CTRL_PG_SKIPCNT  (0x200A)
#define HM0360_BV_WIN_WEIGHT_EN (0x200D)
#define HM0360_WINARRAY_1       (0x200E)
#define HM0360_WINARRAY_2       (0x200F)
#define HM0360_WINARRAY_3       (0x2010)
#define HM0360_WINARRAY_4       (0x2011)
#define HM0360_WINWEIGHT_1_12   (0x2012)
#define HM0360_WINWEIGHT_1_34   (0x2013)
#define HM0360_WINWEIGHT_1_5    (0x2014)
#define HM0360_WINWEIGHT_2_12   (0x2015)
#define HM0360_WINWEIGHT_2_34   (0x2016)
#define HM0360_WINWEIGHT_2_5    (0x2017)
#define HM0360_WINWEIGHT_3_12   (0x2018)
#define HM0360_WINWEIGHT_3_34   (0x2019)
#define HM0360_WINWEIGHT_3_5    (0x201A)
#define HM0360_WINWEIGHT_4_12   (0x201B)
#define HM0360_WINWEIGHT_4_34   (0x201C)
#define HM0360_WINWEIGHT_4_5    (0x201D)
#define HM0360_WINWEIGHT_5_12   (0x201E)
#define HM0360_WINWEIGHT_5_34   (0x201F)
#define HM0360_WINWEIGHT_5_5    (0x2020)
#define HM0360_MAX_INTG_H       (0x2029)
#define HM0360_MAX_INTG_L       (0x202A)
#define HM0360_MAX_AGAIN        (0x202B)
#define HM0360_MAX_DGAIN1       (0x202C)
#define HM0360_MAX_DGAIN2       (0x202D)
#define HM0360_MIN_INTG         (0x202E)
#define HM0360_MIN_AGAIN        (0x202F)
#define HM0360_MIN_DGAIN        (0x2030)
#define HM0360_T_DAMPING        (0x2031)
#define HM0360_N_DAMPING        (0x2032)
#define HM0360_ALC_TH           (0x2033)
#define HM0360_AE_TARGET        (0x2034)
#define HM0360_MIN_MEAN         (0x2035)
#define HM0360_AE_TARGETZONE    (0x2036)
#define HM0360_CONVERGE_IN_TH   (0x2037)
#define HM0360_CONVERGE_OUT_TH  (0x2038)
#define HM0360_RESERVED_2039    (0x2039)
#define HM0360_RESERVED_203A    (0x203A)
#define HM0360_FS_CTRL          (0x203B)
#define HM0360_FS_60HZ_H        (0x203C)
#define HM0360_FS_60HZ_L        (0x203D)
#define HM0360_FS_50HZ_H        (0x203E)
#define HM0360_FS_50HZ_L        (0x203F)
#define HM0360_FR_STAGE1_H      (0x2042)
#define HM0360_FR_STAGE1_L      (0x2043)
#define HM0360_FR_STAGE2_H      (0x2044)
#define HM0360_FR_STAGE2_L      (0x2045)
#define HM0360_FR_STAGE3_H      (0x2046)
#define HM0360_FR_STAGE3_L      (0x2047)
#define HM0360_FR_EGPTH12_H     (0x2048)
#define HM0360_FR_EGPTH12_M     (0x2049)
#define HM0360_FR_EGPTH12_L     (0x204A)
#define HM0360_FR_EGPTH21_H     (0x204B)
#define HM0360_FR_EGPTH21_M     (0x204C)
#define HM0360_FR_EGPTH21_L     (0x204D)
#define HM0360_FR_EGPTH23_H     (0x204E)
#define HM0360_FR_EGPTH23_M     (0x204F)
#define HM0360_FR_EGPTH23_L     (0x2050)
#define HM0360_FR_EGPTH32_H     (0x2051)
#define HM0360_FR_EGPTH32_M     (0x2052)
#define HM0360_FR_EGPTH32_L     (0x2053)
#define HM0360_RESERVED_2054    (0x2054)
#define HM0360_RESERVED_2055    (0x2055)
#define HM0360_RESERVED_2056    (0x2056)
#define HM0360_RESERVED_2057    (0x2057)
#define HM0360_RESERVED_2058    (0x2058)
#define HM0360_RESERVED_2059    (0x2059)
#define HM0360_FRAME_CNT_TH     (0x205B)
#define HM0360_AE_MEAN          (0x205D)
#define HM0360_AE_CONVERGE      (0x2060)
#define HM0360_RESERVED_206F    (0x206F)
#define HM0360_AE_BLI_TGT       (0x2070)
#define HM0360_AE_TARGET_TH0    (0x2071)
#define HM0360_AE_TARGET_TH1    (0x2072)

/* Interrupt programming registers */

#define HM0360_PULSE_MODE (0x2061)
#define HM0360_PULSE_TH_H (0x2062)
#define HM0360_PULSE_TH_L (0x2063)
#define HM0360_INT_INDIC  (0x2064)
#define HM0360_INT_CLEAR  (0x2065)

/* Motion detection control registers */

#define HM0360_MD_CTRL         (0x2080)
#define HM0360_ROI_START_END_V (0x2081)
#define HM0360_ROI_START_END_H (0x2082)
#define HM0360_MD_TH_MIN       (0x2083)
#define HM0360_MD_TH_STR       (0x2084)
#define HM0360_RESERVED_2085   (0x2085)
#define HM0360_MD_TH_COEF_0    (0x2086)
#define HM0360_MD_TH_COEF_1    (0x2087)
#define HM0360_MD_TH_COEF_2    (0x2088)
#define HM0360_MD_TH_COEF_3    (0x2089)
#define HM0360_MD_TH_COEF_4    (0x208A)
#define HM0360_MD_TH_COEF_5    (0x208B)
#define HM0360_RESERVED_208C   (0x208C)
#define HM0360_RESERVED_208D   (0x208D)
#define HM0360_RESERVED_208E   (0x208E)
#define HM0360_RESERVED_208F   (0x208F)
#define HM0360_RESERVED_2090   (0x2090)
#define HM0360_RESERVED_2091   (0x2091)
#define HM0360_RESERVED_2092   (0x2092)
#define HM0360_MD_TG_COEF_1    (0x2093)
#define HM0360_MD_TG_COEF_2    (0x2094)
#define HM0360_MD_TG_COEF_3    (0x2095)
#define HM0360_MD_TG_COEF_4    (0x2096)
#define HM0360_MD_TG_COEF_5    (0x2097)
#define HM0360_MD_TG_COEF_6    (0x2098)
#define HM0360_MD_LIGHT_COEF   (0x2099)
#define HM0360_RESERVED_209A   (0x209A)
#define HM0360_MD_BLOCK_TH     (0x209B)
#define HM0360_MD_LATENCY      (0x209C)
#define HM0360_MD_LATENCY_TH   (0x209D)
#define HM0360_MD_CTRL1        (0x209E)
#define HM0360_MD_ROI_OUT_0    (0x20A1)
#define HM0360_MD_ROI_OUT_1    (0x20A2)
#define HM0360_MD_ROI_OUT_2    (0x20A3)
#define HM0360_MD_ROI_OUT_3    (0x20A4)
#define HM0360_MD_ROI_OUT_4    (0x20A5)
#define HM0360_MD_ROI_OUT_5    (0x20A6)
#define HM0360_MD_ROI_OUT_6    (0x20A7)
#define HM0360_MD_ROI_OUT_7    (0x20A8)
#define HM0360_MD_ROI_OUT_8    (0x20A9)
#define HM0360_MD_ROI_OUT_9    (0x20AA)
#define HM0360_MD_ROI_OUT_10   (0x20AB)
#define HM0360_MD_ROI_OUT_11   (0x20AC)
#define HM0360_MD_ROI_OUT_12   (0x20AD)
#define HM0360_MD_ROI_OUT_13   (0x20AE)
#define HM0360_MD_ROI_OUT_14   (0x20AF)
#define HM0360_MD_ROI_OUT_15   (0x20B0)
#define HM0360_MD_ROI_OUT_16   (0x20B1)
#define HM0360_MD_ROI_OUT_17   (0x20B2)
#define HM0360_MD_ROI_OUT_18   (0x20B3)
#define HM0360_MD_ROI_OUT_19   (0x20B4)
#define HM0360_MD_ROI_OUT_20   (0x20B5)
#define HM0360_MD_ROI_OUT_21   (0x20B6)
#define HM0360_MD_ROI_OUT_22   (0x20B7)
#define HM0360_MD_ROI_OUT_23   (0x20B8)
#define HM0360_MD_ROI_OUT_24   (0x20B9)
#define HM0360_MD_ROI_OUT_25   (0x20BA)
#define HM0360_MD_ROI_OUT_26   (0x20BB)
#define HM0360_MD_ROI_OUT_27   (0x20BC)
#define HM0360_MD_ROI_OUT_28   (0x20BD)
#define HM0360_MD_ROI_OUT_29   (0x20BE)
#define HM0360_MD_ROI_OUT_30   (0x20BF)
#define HM0360_MD_ROI_OUT_31   (0x20C0)

/* OTP programming registers */

#define HM0360_PAGE_NUMBER    (0x2500)
#define HM0360_PAGE_OFFSET    (0x2501)
#define HM0360_OTP_LENGTH     (0x2502)
#define HM0360_OTP_COMMAND    (0x2503)
#define HM0360_OTP_STATUS     (0x2504)
#define HM0360_CP_STATUS      (0x2507)

/* MIPI programming registers */

#define HM0360_MIPI_EN      (0x2800)
#define HM0360_LANE_CFG     (0x2821)
#define HM0360_EMB_DATA_CFG (0x2822)

/* SYNC function control registers */

#define HM0360_EXP_SYNC_CFG    (0x3010)
#define HM0360_ERR_FLAG_CFG    (0x3013)
#define HM0360_OFFSET_RDSYNC_H (0x3019)
#define HM0360_OFFSET_RDSYNC_L (0x301A)
#define HM0360_RDSYNC_DEC_TH_H (0x301B)
#define HM0360_RDSYNC_DEC_TH_L (0x301C)

/* Context switch control registers */

#define HM0360_PMU_CFG_3 (0x3024)
#define HM0360_PMU_CFG_4 (0x3025)
#define HM0360_PMU_CFG_5 (0x3026)
#define HM0360_PMU_CFG_6 (0x3027)
#define HM0360_PMU_CFG_7 (0x3028)
#define HM0360_PMU_CFG_8 (0x3029)
#define HM0360_PMU_CFG_9 (0x302A)

/* ROI and sensor control registers */

#define HM0360_WIN_MODE       (0x3030)
#define HM0360_N_PLUS_MODE_EN (0x3032)
#define HM0360_RESERVED_3034  (0x3034)
#define HM0360_RESERVED_3035  (0x3035)
#define HM0360_ROI_CFG        (0x3060)
#define HM0360_ROI_WIN_NUMBER (0x3061)
#define HM0360_ROI_WIN_01_H   (0x3062)
#define HM0360_ROI_WIN_01_L   (0x3063)
#define HM0360_ROI_WIN_02_H   (0x3064)
#define HM0360_ROI_WIN_02_L   (0x3065)
#define HM0360_ROI_WIN_03_H   (0x3066)
#define HM0360_ROI_WIN_03_L   (0x3067)
#define HM0360_ROI_WIN_04_H   (0x3068)
#define HM0360_ROI_WIN_04_L   (0x3069)
#define HM0360_ROI_WIN_05_H   (0x306A)
#define HM0360_ROI_WIN_05_L   (0x306B)
#define HM0360_ROI_WIN_06_H   (0x306C)
#define HM0360_ROI_WIN_06_L   (0x306D)
#define HM0360_ROI_WIN_07_H   (0x306E)
#define HM0360_ROI_WIN_07_L   (0x306F)
#define HM0360_ROI_WIN_08_H   (0x3070)
#define HM0360_ROI_WIN_08_L   (0x3071)
#define HM0360_ROI_WIN_09_H   (0x3072)
#define HM0360_ROI_WIN_09_L   (0x3073)
#define HM0360_ROI_WIN_10_H   (0x3074)
#define HM0360_ROI_WIN_10_L   (0x3075)
#define HM0360_ROI_WIN_11_H   (0x3076)
#define HM0360_ROI_WIN_11_L   (0x3077)
#define HM0360_ROI_WIN_12_H   (0x3078)
#define HM0360_ROI_WIN_12_L   (0x3079)
#define HM0360_ROI_WIN_13_H   (0x307A)
#define HM0360_ROI_WIN_13_L   (0x307B)
#define HM0360_ROI_WIN_14_H   (0x307C)
#define HM0360_ROI_WIN_14_L   (0x307D)
#define HM0360_ROI_WIN_15_H   (0x307E)
#define HM0360_ROI_WIN_15_L   (0x307F)

/* Strobe control registers */

#define HM0360_STROBE_CFG     (0x3080)
#define HM0360_STROBE_SEL     (0x3081)
#define HM0360_STROBE_FRONT_H (0x3082)
#define HM0360_STROBE_FRONT_L (0x3083)
#define HM0360_STROBE_END_H   (0x3084)
#define HM0360_STROBE_END_L   (0x3085)
#define HM0360_STROBE_LINE_H  (0x3086)
#define HM0360_STROBE_LINE_L  (0x3087)
#define HM0360_STROBE_FRAME_H (0x3088)
#define HM0360_STROBE_FRAME_L (0x3089)

/* IO and clock control registers */

#define HM0360_VSYNC_FRONT         (0x3094)
#define HM0360_VSYNC_END           (0x3095)
#define HM0360_HSYNC_FRONT_H       (0x3096)
#define HM0360_HSYNC_FRONT_L       (0x3097)
#define HM0360_HSYNC_END_H         (0x3098)
#define HM0360_HSYNC_END_L         (0x3099)
#define HM0360_EARLY_INT_EN        (0x309C)
#define HM0360_PLKO_GATED_EN       (0x309E)
#define HM0360_PLKO_FRAME_FRONT    (0x309F)
#define HM0360_PLKO_FRAME_END      (0x30A0)
#define HM0360_PLKO_LINE_FRONT_H   (0x30A1)
#define HM0360_PLKO_LINE_FRONT_L   (0x30A2)
#define HM0360_PLKO_LINE_END_H     (0x30A3)
#define HM0360_PLKO_LINE_END_L     (0x30A4)
#define HM0360_OUTPUT_EN           (0x30A5)
#define HM0360_FRAME_OUTPUT_EN     (0x30A8)
#define HM0360_MULTI_CAMERA_CONFIG (0x30A9)
#define HM0360_MULTI_CAMERA_TUNE_H (0x30AA)
#define HM0360_MULTI_CAMERA_TUNE_L (0x30AB)
#define HM0360_ANA_REGISTER_03     (0x310E)
#define HM0360_ANA_REGISTER_04     (0x310F)
#define HM0360_ANA_REGISTER_05     (0x3110)
#define HM0360_ANA_REGISTER_06     (0x3111)
#define HM0360_ANA_REGISTER_07     (0x3112)
#define HM0360_PLL_POST_DIV_D      (0x3128)

/* I2C slave registers */

#define HM0360_I2C_ID_SEL          (0x3400)
#define HM0360_I2C_ID_REG          (0x3401)

/* Context switch A registers */

#define HM0360_CTX_A_PLL1CFG           (0x3500)
#define HM0360_CTX_A_PLL2CFG           (0x3501)
#define HM0360_CTX_A_PLL3CFG           (0x3502)
#define HM0360_CTX_A_FRAME_LENGTH_H    (0x3503)
#define HM0360_CTX_A_FRAME_LENGTH_L    (0x3504)
#define HM0360_CTX_A_LINE_LENGTH_H     (0x3505)
#define HM0360_CTX_A_LINE_LENGTH_L     (0x3506)
#define HM0360_CTX_A_H_SUB             (0x3507)
#define HM0360_CTX_A_V_SUB             (0x3508)
#define HM0360_CTX_A_BIN_MODE          (0x3509)
#define HM0360_RESERVED_350A           (0x350A)
#define HM0360_CTX_A_MONO_MODE_ISP     (0x350B)
#define HM0360_CTX_A_N_PLUS_MODE_EN    (0x350C)
#define HM0360_CTX_A_WIN_MODE          (0x350D)
#define HM0360_CTX_A_ROI_CFG           (0x350E)
#define HM0360_CTX_A_EARLY_INT_EN      (0x350F)
#define HM0360_CTX_A_FRAME_OUTPUT_EN   (0x3510)
#define HM0360_CTX_A_EMBEDDED_LINE_EN  (0x3511)
#define HM0360_CTX_A_AE_CTRL           (0x3512)
#define HM0360_CTX_A_AE_CTRL1          (0x3513)
#define HM0360_CTX_A_CNT_ORG_H1        (0x3514)
#define HM0360_CTX_A_CNT_ORG_H2        (0x3515)
#define HM0360_CTX_A_CNT_ORG_V1        (0x3516)
#define HM0360_CTX_A_CNT_ORG_V2        (0x3517)
#define HM0360_CTX_A_CNT_ST_H1         (0x3518)
#define HM0360_CTX_A_CNT_ST_H2         (0x3519)
#define HM0360_CTX_A_CNT_ST_V1         (0x351A)
#define HM0360_CTX_A_CNT_ST_V2         (0x351B)
#define HM0360_CTX_A_CTRL_PG_SKIPCNT   (0x351C)
#define HM0360_CTX_A_MAX_INTG_H        (0x351D)
#define HM0360_CTX_A_MAX_INTG_L        (0x351E)
#define HM0360_CTX_A_MAX_AGAIN         (0x351F)
#define HM0360_CTX_A_MAX_DGAIN_H       (0x3520)
#define HM0360_CTX_A_MAX_DGAIN_L       (0x3521)
#define HM0360_CTX_A_MIN_INTG          (0x3522)
#define HM0360_CTX_A_T_DAMPING         (0x3523)
#define HM0360_CTX_A_N_DAMPING         (0x3524)
#define HM0360_CTX_A_AE_TGT_ZONE       (0x3525)
#define HM0360_CTX_A_CONV_IN_TH        (0x3526)
#define HM0360_CTX_A_CONV_OUT_TH       (0x3527)
#define HM0360_RESERVED_3528           (0x3528)
#define HM0360_RESERVED_3529           (0x3529)
#define HM0360_CTX_A_FS_60HZ_H         (0x352A)
#define HM0360_CTX_A_FS_60HZ_L         (0x352B)
#define HM0360_CTX_A_FS_50HZ_H         (0x352C)
#define HM0360_CTX_A_FS_50HZ_L         (0x352D)
#define HM0360_CTX_A_FR_STAGE1_H       (0x352E)
#define HM0360_CTX_A_FR_STAGE1_L       (0x352F)
#define HM0360_CTX_A_FR_STAGE2_H       (0x3530)
#define HM0360_CTX_A_FR_STAGE2_L       (0x3531)
#define HM0360_CTX_A_FR_STAGE3_H       (0x3532)
#define HM0360_CTX_A_FR_STAGE3_L       (0x3533)
#define HM0360_CTX_A_MD_CTRL           (0x354B)
#define HM0360_CTX_A_MD_BLOCK_NUM_TH   (0x354C)
#define HM0360_CTX_A_ROI_START_END_V   (0x354D)
#define HM0360_CTX_A_ROI_START_END_H   (0x354E)
#define HM0360_RESERVED_354F           (0x354F)
#define HM0360_CTX_A_MD_TH_STR         (0x3550)
#define HM0360_RESERVED_3551           (0x3551)
#define HM0360_CTX_A_MD_FLICK_TH_ADJ_N (0x3552)
#define HM0360_CTX_A_MD_FLICK_TH_ADJ_P (0x3553)
#define HM0360_CTX_A_MD_TH_COEF_0      (0x3554)
#define HM0360_CTX_A_MD_TH_COEF_1      (0x3555)
#define HM0360_CTX_A_MD_TH_COEF_2      (0x3556)
#define HM0360_CTX_A_MD_TH_COEF_3      (0x3557)
#define HM0360_CTX_A_MD_TH_COEF_4      (0x3558)
#define HM0360_CTX_A_MD_TH_COEF_5      (0x3559)

/* Context switch B registers */

#define HM0360_CTX_B_PLL1CFG           (0x355A)
#define HM0360_CTX_B_PLL2CFG           (0x355B)
#define HM0360_CTX_B_PLL3CFG           (0x355C)
#define HM0360_CTX_B_FRAME_LENGTH_H    (0x355D)
#define HM0360_CTX_B_FRAME_LENGTH_L    (0x355E)
#define HM0360_CTX_B_LINE_LENGTH_H     (0x355F)
#define HM0360_CTX_B_LINE_LENGTH_L     (0x3560)
#define HM0360_CTX_B_H_SUB             (0x3561)
#define HM0360_CTX_B_V_SUB             (0x3562)
#define HM0360_CTX_B_BIN_MODE          (0x3563)
#define HM0360_RESERVED_3564           (0x3564)
#define HM0360_CTX_B_MONO_MODE_ISP     (0x3565)
#define HM0360_CTX_B_N_PLUS_MODE_EN    (0x3566)
#define HM0360_CTX_B_WIN_MODE          (0x3567)
#define HM0360_CTX_B_ROI_CFG           (0x3568)
#define HM0360_CTX_B_EARLY_INT_EN      (0x3569)
#define HM0360_CTX_B_FRAME_OUTPUT_EN   (0x356A)
#define HM0360_CTX_B_EMBEDDED_LINE_EN  (0x356B)
#define HM0360_CTX_B_AE_CTRL           (0x356C)
#define HM0360_CTX_B_AE_CTRL1          (0x356D)
#define HM0360_CTX_B_CNT_ORG_H1        (0x356E)
#define HM0360_CTX_B_CNT_ORG_H2        (0x356F)
#define HM0360_CTX_B_CNT_ORG_V1        (0x3570)
#define HM0360_CTX_B_CNT_ORG_V2        (0x3571)
#define HM0360_CTX_B_CNT_ST_H1         (0x3572)
#define HM0360_CTX_B_CNT_ST_H2         (0x3573)
#define HM0360_CTX_B_CNT_ST_V1         (0x3574)
#define HM0360_CTX_B_CNT_ST_V2         (0x3575)
#define HM0360_CTX_B_CTRL_PG_SKIPCNT   (0x3576)
#define HM0360_CTX_B_MAX_INTG_H        (0x3577)
#define HM0360_CTX_B_MAX_INTG_L        (0x3578)
#define HM0360_CTX_B_MAX_AGAIN         (0x3579)
#define HM0360_CTX_B_MAX_DGAIN_H       (0x357A)
#define HM0360_CTX_B_MAX_DGAIN_L       (0x357B)
#define HM0360_CTX_B_MIN_INTG          (0x357C)
#define HM0360_CTX_B_T_DAMPING         (0x357D)
#define HM0360_CTX_B_N_DAMPING         (0x357E)
#define HM0360_CTX_B_AE_TGT_ZONE       (0x357F)
#define HM0360_CTX_B_CONV_IN_TH        (0x3580)
#define HM0360_CTX_B_CONV_OUT_TH       (0x3581)
#define HM0360_RESERVED_3582           (0x3582)
#define HM0360_RESERVED_3583           (0x3583)
#define HM0360_CTX_B_FS_60HZ_H         (0x3584)
#define HM0360_CTX_B_FS_60HZ_L         (0x3585)
#define HM0360_CTX_B_FS_50HZ_H         (0x3586)
#define HM0360_CTX_B_FS_50HZ_L         (0x3587)
#define HM0360_CTX_B_FR_STAGE1_H       (0x3588)
#define HM0360_CTX_B_FR_STAGE1_L       (0x3589)
#define HM0360_CTX_B_FR_STAGE2_H       (0x358A)
#define HM0360_CTX_B_FR_STAGE2_L       (0x358B)
#define HM0360_CTX_B_FR_STAGE3_H       (0x358C)
#define HM0360_CTX_B_FR_STAGE3_L       (0x358D)
#define HM0360_CTX_B_MD_CTRL           (0x35A5)
#define HM0360_CTX_B_MD_BLOCK_NUM_TH   (0x35A6)
#define HM0360_CTX_B_ROI_START_END_V   (0x35A7)
#define HM0360_CTX_B_ROI_START_END_H   (0x35A8)
#define HM0360_RESERVED_35A9           (0x35A9)
#define HM0360_CTX_B_MD_TH_STR         (0x35AA)
#define HM0360_RESERVED_35AB           (0x35AB)
#define HM0360_CTX_B_MD_FLICK_TH_ADJ_N (0x35AC)
#define HM0360_CTX_B_MD_FLICK_TH_ADJ_P (0x35AD)
#define HM0360_CTX_B_MD_TH_COEF_0      (0x35AE)
#define HM0360_CTX_B_MD_TH_COEF_1      (0x35AF)
#define HM0360_CTX_B_MD_TH_COEF_2      (0x35B0)
#define HM0360_CTX_B_MD_TH_COEF_3      (0x35B1)
#define HM0360_CTX_B_MD_TH_COEF_4      (0x35B2)
#define HM0360_CTX_B_MD_TH_COEF_5      (0x35B3)

/*
 * Useful value of HM0360 registers TODO
 */
#define         HM0360_RESET             (0x01)
#define         HM0360_Pclk_rising_edge  (0x00)
#define         HM0360_Pclk_falling_edge (0x01)


enum {
    HM0360_STANDBY    = 0x0,  // Sleep
    HM0360_STREAMING1 = 0x1,  // SW I2C trigger: continuous streaming
    HM0360_STREAMING2 = 0x2,  // SW I2C trigger: automatic wakeup sleep cycles
    HM0360_STREAMING3 = 0x3,  // SW I2C trigger: snapshot with N frame output
    HM0360_STREAMING4 = 0x4,  // HW pin trigger: continuous streaming
    HM0360_STREAMING5 = 0x5,  // HW pin trigger: snapshot with N frame output
    HM0360_STREAMING6 = 0x6,  // HW pin trigger: automatic wakeup sleep cycles
};


#endif
