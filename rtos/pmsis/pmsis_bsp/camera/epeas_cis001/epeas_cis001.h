



#ifndef __BSP_CAMERA_EPEAS_CIS001_EPEAS_CIS001_H__
#define __BSP_CAMERA_EPEAS_CIS001_EPEAS_CIS001_H__


/*
 *  EPEAS_CIS001 camera macros
 */

// Register address
// Read only registers
#define 		EPEAS_CIS001_STATUS               0x06
#define 		EPEAS_CIS001_IRQ_FLAG             0x0E
#define 		EPEAS_CIS001_CAL_CLK_BUFF_B0      0x35
#define 		EPEAS_CIS001_CAL_CLK_BUFF_B1      0x36
#define 		EPEAS_CIS001_CAL_RAMP_COUNT_B0    0x37
#define 		EPEAS_CIS001_CAL_RAMP_COUNT_B1    0x38
#define 		EPEAS_CIS001_CAL_RAMP_COUNT_B2    0x39
#define 		EPEAS_CIS001_VERSION              0x7F
// Write only registers
#define 		EPEAS_CIS001_CAPTURE              0x00
#define 		EPEAS_CIS001_SOFT_RESET           0x07
#define 		EPEAS_CIS001_IRQCLR               0x0F
// R&W registers
// Sensor mode control
#define 		EPEAS_CIS001_MODE                 0x01
#define 		EPEAS_CIS001_PMODE                0x02
#define 		EPEAS_CIS001_PCLK_MODE            0x03
#define 		EPEAS_CIS001_FLIP                 0x1A
#define 		EPEAS_CIS001_RESOLUTION           0x15
// Sensor exposure and gain control
#define         EPEAS_CIS001_ANALOG_GAIN          0x30
#define         EPEAS_CIS001_EXPOSURE_B0          0x04
#define         EPEAS_CIS001_EXPOSURE_B1          0x05

//Calibration
#define         EPEAS_CIS001_CCLK_CAL             0x31

#define         EPEAS_CIS001_ABB_EN               0x3B

//useful register values

#endif