// File CIS_PIXART.h

#ifndef __CIS_PIXART_H
#define __CIS_PIXART_H   // avoid recursive inclusions

#define PIXART_MCLK_FREQ                  6000000
#define PIXART_SPI_BAUDRATE               10000000

#define PIXART_TRIGGER_MODE               0
#define PIXART_CONTINUE_MODE              1

#define PIXART_CHIP_ID_REG_L              0x00
#define PIXART_CHIP_ID_REG_H              0x01
#define	PIXART_CHIP_ID                    0x6100

#define PIXART_UPDATE_REG                 0x00

#define PIXART_GLOBAL_RESET_REG           0x0C
#define PIXART_SOFT_RESET                 0x1D

#define PIXART_SWITCH_MODE_CTL1_REG       0x23
#define PIXART_SWITCH_MODE_CTL2_REG       0x24
#define PIXART_BANK_SELECT_REG            0x7F

#endif // ndef __CIS_PIXART_H


