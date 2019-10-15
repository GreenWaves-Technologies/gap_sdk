// File CIS_MT9V034.h

#ifndef __CIS_MT9V034_H
#define __CIS_MT9V034_H   // avoid recursive inclusions

#define MT9V034_DEV_ADDR_WR  0x90   // 8-bit device address for WRITE (0xB8 7-bit I2C device address)
#define MT9V034_DEV_ADDR_RD  0x91   // 8-bit device address for READ


// **** REGISTER DEFINITION :     ***************************************************

/* The first four rows are black rows. The active area spans 753x481 pixels. */
#define MT9V034_PIXEL_ARRAY_HEIGHT			485
#define MT9V034_PIXEL_ARRAY_WIDTH			753

#define MT9V034_SYSCLK_FREQ_DEF				26600000

#define MT9V034_CHIP_ID_REG  			0x00
#define		MT9V034_CHIP_ID			        0x1324

#define MT9V034_COLUMN_START_A			0x01    // _A for Context A
#define		MT9V034_COLUMN_START_MIN		1
#define		MT9V034_COLUMN_START_DEF		1
#define		MT9V034_COLUMN_START_MAX		752

#define MT9V034_ROW_START_A				0x02
#define		MT9V034_ROW_START_MIN			4
#define		MT9V034_ROW_START_DEF			4   // was 5?
#define		MT9V034_ROW_START_MAX			482

#define MT9V034_WINDOW_HEIGHT_A			0x03
#define		MT9V034_WINDOW_HEIGHT_MIN		1
#define		MT9V034_WINDOW_HEIGHT_DEF		480
#define		MT9V034_WINDOW_HEIGHT_MAX		480

#define MT9V034_WINDOW_WIDTH_A			0x04
#define		MT9V034_WINDOW_WIDTH_MIN		1
#define		MT9V034_WINDOW_WIDTH_DEF		752
#define		MT9V034_WINDOW_WIDTH_MAX		752

#define MT9V034_HORIZONTAL_BLANKING_A	0x05

#define		MT9V034_HORIZONTAL_BLANKING_MIN		61
#define		MT9V034_HORIZONTAL_BLANKING_DEF		94
#define		MT9V034_HORIZONTAL_BLANKING_MAX		1023

#define MT9V034_VERTICAL_BLANKING_A		0x06
#define		MT9V034_VERTICAL_BLANKING_MIN		2   // was 4?
#define		MT9V034_VERTICAL_BLANKING_DEF		45
#define		MT9V034_VERTICAL_BLANKING_MAX		32288


#define MT9V034_CHIP_CONTROL			0x07
#define		MT9V034_CHIP_CONTROL_MASTER_MODE	(1 << 3)
#define		MT9V034_CHIP_CONTROL_SNAPSHOT_MODE	(3 << 3)
#define		MT9V034_CHIP_CONTROL_DVP_ON	        (1 << 7)
#define		MT9V034_CHIP_CONTROL_DVP_OFF	    (0 << 7)
#define		MT9V034_CHIP_CONTROL_SIMULTANEOUS	(1 << 8)
#define		MT9V034_CHIP_CONTROL_SEQUENTIAL     (0 << 8)
// Some fields missing here incl. bit[15] = Context A/B switch
// !! CAUTION -- spec says bit [9] is reserved and =0 means normal (recommended) operation ...but says default value is 1 ("reserved, do not use"?!?
//  --> Rev.D says its Defective Pixel Correction Enable

#define MT9V034_SHUTTER_WIDTH1_A		0x08

#define MT9V034_SHUTTER_WIDTH2_A		0x09

#define MT9V034_SHUTTER_WIDTH_CONTROL_A	0x0a
#define		SHUTTER_WIDTH_T2_RATIO_MASK         (0xF << 0)  // 4 bits
#define		SHUTTER_WIDTH_T2_RATIO_SHIFT         0
#define		SHUTTER_WIDTH_T3_RATIO_MASK         (0xF << 4) // 4 bits
#define		SHUTTER_WIDTH_T3_RATIO_SHIFT         4
#define		SHUTTER_WIDTH_HDR_AUTO_ADJUST_MASK  (1 << 8)
#define		SHUTTER_WIDTH_HDR_AUTO_ADJUST_SHIFT  8
#define		SHUTTER_WIDTH_SINGLE_KNEE_MASK		(1 << 9)
#define		SHUTTER_WIDTH_SINGLE_KNEE_SHIFT		 9




#define MT9V034_COARSE_SHUTTER_WIDTH_A	0x0b
#define		MT9V034_COARSE_SHUTTER_WIDTH_MIN		0
#define		MT9V034_COARSE_SHUTTER_WIDTH_DEF		480
#define		MT9V034_COARSE_SHUTTER_WIDTH_MAX		32765

#define MT9V034_RESET					0x0c
#define		MT9V034_SOFT_RESET          		0x01
#define		MT9V034_GAIN_EXP_RESET          	0x02

#define MT9V034_READ_MODE_A				0x0d
#define		MT9V034_READ_MODE_ROW_BIN_MASK		(3 << 0)
#define		MT9V034_READ_MODE_ROW_BIN_SHIFT		0
#define		MT9V034_READ_MODE_COLUMN_BIN_MASK	(3 << 2)
#define		MT9V034_READ_MODE_COLUMN_BIN_SHIFT	2
#define		MT9V034_READ_MODE_ROW_FLIP_MASK		(1 << 4)
#define		MT9V034_READ_MODE_ROW_FLIP_SHIFT	4
#define		MT9V034_READ_MODE_COLUMN_FLIP_MASK	(1 << 5)
#define		MT9V034_READ_MODE_COLUMN_FLIP_SHIFT	5
#define		MT9V034_READ_MODE_DARK_ROWS_MASK	(1 << 6)
#define		MT9V034_READ_MODE_DARK_ROWS_SHIFT	6           //was at posn 7?
#define		MT9V034_READ_MODE_DARK_COLUMNS_MASK	(1 << 7)
#define		MT9V034_READ_MODE_DARK_COLUMNS_SHIFT    7           // was at posn 6?
#define		MT9V034_READ_MODE_RESERVED          0x300

// Register @0x0E = Read Mode Context B...

#define MT9V034_PIXEL_OPERATION_MODE	0x0f
#define		MT9V034_PIXEL_OPERATION_MODE_HDR_A	(1 << 0)  // context A
#define		MT9V034_PIXEL_OPERATION_MODE_COLOR	(1 << 1)
#define		MT9V034_PIXEL_OPERATION_MODE_HDR_B	(1 << 8)  // Context B   // was (1 << 6) ??

// added XC:
#define MT9V034_LED_OUT_CTRL			0x1B
#define     MT9V034_LED_OUT_CTRL_DISABLE		(1 << 0)
#define     MT9V034_LED_OUT_CTRL_INVERT		    (1 << 1)

#define MT9V034_ADC_COMPANDING			0x1C
#define     MT9V034_LINEAR_AB            		(2 << 0) | (2<<8)
#define     MT9V034_12TO10_COMPANDING_AB 	    (3 << 0) | (3<<8)


// Missing misc. ADC controls @0x2C, 0x31, 0x32, 0x33, 0x34

#define MT9V034_RESERVED_REG13			0x13
#define MT9V034_RESERVED_REG20			0x20
#define MT9V034_RESERVED_REG24			0x24
#define MT9V034_RESERVED_REG2B			0x2B
#define MT9V034_RESERVED_REG2F			0x2F

#define MT9V034_VREF_ADC                0x2C
#define     MT9V034_VREF_1V0                0   // for ADC input range = 0.71V
#define     MT9V034_VREF_1V1                1   // for ADC input range = 0.79V
#define     MT9V034_VREF_1V2                2   // for ADC input range = 0.86V
#define     MT9V034_VREF_1V3                3   // for ADC input range = 0.93V
#define     MT9V034_VREF_1V4                4   // for ADC input range = 1.0V  (default)
#define     MT9V034_VREF_1V5                5   // for ADC input range = 1.07V
#define     MT9V034_VREF_1V6                6   // for ADC input range = 1.14V
#define     MT9V034_VREF_2V1                7   // for ADC input range = 1.5V

#define MT9V034_ANALOG_GAIN_A           0x35
#define		MT9V034_ANALOG_GAIN_MIN			16
#define		MT9V034_ANALOG_GAIN_DEF			16
#define		MT9V034_ANALOG_GAIN_MAX			64
#define		MT9V034_ANALOG_GAIN_GLOBAL		(1<<15)

// @0x36 was MT9V034_MAX_ANALOG_GAIN  ?
#define MT9V034_ANALOG_GAIN_B			0x36

// @0x39, 0x3A, 0x3B, 0x3C : misc. voltage controls for Context B

#define MT9V034_FRAME_DARK_AVERAGE		0x42

#define MT9V034_DARK_AVG_THRESH			0x46
#define		MT9V034_DARK_AVG_LOW_THRESH_MASK	(255 << 0)
#define		MT9V034_DARK_AVG_LOW_THRESH_SHIFT	0
#define		MT9V034_DARK_AVG_HIGH_THRESH_MASK	(255 << 8)
#define		MT9V034_DARK_AVG_HIGH_THRESH_SHIFT	8

#define MT9V034_BLACK_LEVEL_CTRL        0x47
#define		MT9V034_BLACK_LEVEL_OVERRIDE        (1 << 0)
#define		MT9V034_BLACK_LEVEL_AUTO            (0 << 0)
#define		MT9V034_BLACK_LEVEL_AVG_FRAMES_MASK (7 << 5)
#define		MT9V034_BLACK_LEVEL_AVG_FRAMES_SHIFT    5

#define MT9V034_BLACK_LEVEL_VALUE_A     0x48

#define MT9V034_ROW_NOISE_CORR_CONTROL	0x70
#define		MT9V034_ROW_NOISE_CORR_ENABLE_A		(1 << 0)
#define		MT9V034_ROW_NOISE_CORR_USE_BLK_AVG_A	(1 << 1)
#define		MT9V034_ROW_NOISE_CORR_ENABLE_B		(1 << 8)    // was 5
#define		MT9V034_ROW_NOISE_CORR_USE_BLK_AVG_B	(1 << 9)    // was 7

// @0x71: Row noise constant

#define MT9V034_PIXEL_CLOCK				0x72
#define		MT9V034_PIXEL_CLOCK_INV_LINE		(1 << 0)
#define		MT9V034_PIXEL_CLOCK_INV_FRAME		(1 << 1)
#define		MT9V034_PIXEL_CLOCK_XOR_LINE		(1 << 2)
#define		MT9V034_PIXEL_CLOCK_CONT_LINE		(1 << 3)
#define		MT9V034_PIXEL_CLOCK_INV_PXL_CLK		(1 << 4)

#define MT9V034_HDR 					0x0F
#define		MT9V034_HDR_OFF						(0x00)
#define		MT9V034_HDR_ON 						(0x01)

#define MT9V034_TEST_PATTERN			0x7f
#define		MT9V034_TEST_PATTERN_DATA_MASK		(1023 << 0)
#define		MT9V034_TEST_PATTERN_DATA_SHIFT		0
#define		MT9V034_TEST_PATTERN_USE_DATA		(1 << 10)
#define		MT9V034_TEST_PATTERN_GREY_MASK		(3 << 11)
#define		MT9V034_TEST_PATTERN_GREY_NONE		(0 << 11)
#define		MT9V034_TEST_PATTERN_GREY_VERTICAL	(1 << 11)
#define		MT9V034_TEST_PATTERN_GREY_HORIZONTAL	(2 << 11)
#define		MT9V034_TEST_PATTERN_GREY_DIAGONAL	(3 << 11)
#define		MT9V034_TEST_PATTERN_ENABLE		(1 << 13)
#define		MT9V034_TEST_PATTERN_FLIP		(1 << 14)

// @0x80-0x98 -- Tiled digital gains
// 0x99-0xA4 - digtial tiles coordinates

#define MT9V034_AEC_AGC_DESIRED_BIN		0xa5
#define     MT9V034_AEC_AGC_DESIRED_BIN_DEF      45

#define MT9V034_AEC_UPDATE_FREQUENCY	0xa6

#define MT9V034_AEC_LPF					0xa8

#define MT9V034_AGC_UPDATE_FREQUENCY	0xa9

#define MT9V034_AGC_LPF					0xaa

// missing  @0xAB, 0xAC, 0xAE -- analog gain and coarse shutter widths settings
#define MT9V034_AEC_MAX_SHUTTER_WIDTH		0xad

#define MT9V034_AEC_AGC_ENABLE			0xaf
#define		MT9V034_AEC_ENABLE_A			(1 << 0)
#define		MT9V034_AGC_ENABLE_A			(1 << 1)
#define		MT9V034_AEC_ENABLE_B			(1 << 8)
#define		MT9V034_AGC_ENABLE_B			(1 << 9)

// Missing @0xB0 (AGC/AEC pixel count),
// Missing 0xB1-0xB9 (LVDS & stereoscopy settings)
// Missing 0xBA, 0xBB, 0xBC -- AEC/AGC status info

// #define MT9V034_AEC_MAX_SHUTTER_WIDTH	0xbd  // does not exist ?!
// #define MT9V034_THERMAL_INFO				0xc1  // does not exist ?!

// Missing regsiters @0x!BF, 0xC0-D9, 0xF0, 0xFE
#define MT9V034_HISTOGRAM_PIXCOUNT      0xB0
#define     MT9V034_HISTOGRAM_PIXCOUNT_DEF      44000


#define MT9V034_AGC_OUTPUT              0xBA

#define MT9V034_AEC_OUTPUT              0xBB

#define MT9V034_AGC_AEC_CURRENT_BIN     0xBC

#define MT9V034_MONITOR_NUM_FRAMES      0xC0
#define     MT9V034_MONITOR_DEFAULT_NUM     10  // default number of frames in register

#define MT9V034_FINE_SHUTTER_WIDTH_A	0xd5
#define		MT9V034_FINE_SHUTTER_WIDTH_MIN		0
#define		MT9V034_FINE_SHUTTER_WIDTH_DEF		0
#define		MT9V034_FINE_SHUTTER_WIDTH_MAX		1774

#define MT9V034_MONITOR_MODE        	0xd8
#define		MT9V034_MONITOR_EN  		(1<<0)
#define		MT9V034_MONITOR_DIS  		(0<<0)


#endif // ndef __CIS_MT9V034_H


