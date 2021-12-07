/*
 *  EPEAS camera macros
 */

// Register address
// Read only registers
#define 		ADDR_STATUS_REGISTER              0x06
#define 		ADDR_IRQ_FLAG_REGISTER            0x0E
#define 		ADDR_CAL_CLK_BUFF_B0_REGISTER     0x35
#define 		ADDR_CAL_CLK_BUFF_B1_REGISTER     0x36
#define 		ADDR_CAL_RAMP_COUNT_B0_REGISTER   0x37
#define 		ADDR_CAL_RAMP_COUNT_B1_REGISTER   0x38
#define 		ADDR_CAL_RAMP_COUNT_B2_REGISTER   0x39
#define 		ADDR_VERSION_REGISTER             0x7F
// Write only registers
#define 		ADDR_CAPTURE_REGISTER             0x00
#define 		ADDR_SOFT_RESET_REGISTER          0x07
#define 		ADDRE_IRQCLR_REGISTER             0x0F
// R&W registers
// Sensor mode control
#define 		ADDR_MODE_REGISTER                0x01
#define 		ADDR_PMODE_REGISTER               0x02
#define 		ADDR_PCLK_MODE_REGISTER           0x03
#define 		ADDR_FLIP_REGISTER                0x1A
#define 		ADDR_RESOLUTION_REGISTER          0x15
// Sensor exposure and gain control
#define         ADDR_ANALOG_GAIN_REGISTER         0x30
#define         ADDR_EXPOSURE_B0_REGISTER         0x04
#define         ADDR_EXPOSURE_B1_REGISTER         0x05
//Sensor ROI mode
#define         ADDR_ROI_EN_REGISTER              0x10
#define         ADDR_ROI_X_START_REGISTER         0x11
#define         ADDR_ROI_X_END_REGISTER           0x12
#define         ADDR_ROI_Y_START_REGISTER         0x13
#define         ADDR_ROI_Y_END_REGISTER           0x14


//Calibration
#define         ADDR_CCLK_CAL_REGISTER            0x31

//useful register values

