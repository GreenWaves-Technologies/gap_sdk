// File K80_PROXY_REGISTERS.h

#ifndef __K80_PROXY_REGISTERS_H
#define __K80_PROXY_REGISTERS_H   // avoid recursive inclusions



#define IRPROXY_TINT    0x1     // Integration Time (in number of cycles -- Range 1:255)
#define     IRPROXY_TINT_DFLT   0x010

#define IRPROXY_GFID    0x2     // 12-bit ADC value for GFID - Range 0 for OV to 4095 for 3.6V
#define     IRPROXY_GFID_DFLT   0xBB0

#define IRPROXY_GSK     0x3     // 12-bit ADC value for VSK - Range 0 for OV to 4095 for 3.6V
#define     IRPROXY_GSK_DFLT    0x540

#define IRPROXY_CKDIV   0x4     // (USE WITH CAUTION) 
#define     IRPROXY_CKDIV_DEFLT   0x002    
#define     IRPROXY_CKDIV_INV_PIXCLK_MASK       0x1
#define     IRPROXY_CKDIV_INV_PIXCLK_SHIFT      11
#define     IRPROXY_CKDIV_DATA_PACKING_MASK     0x1
#define     IRPROXY_CKDIV_DATA_PACKING_SHIFT    10
#define     IRPROXY_CKDIV_VSYNC_POL_MASK        0x1
#define     IRPROXY_CKDIV_VSYNC_POL_SHIFT       9
#define     IRPROXY_CKDIV_HSYNC_POL_MASK        0x1
#define     IRPROXY_CKDIV_HSYNC_POL_SHIFT       8   // !!! BEWARE !!! Looks like inverting HSYNC pol doesnt work; VSYNC is lost if trying to use it
#define     IRPROXY_CKDIV_SENSOR_CLKDIV_MASK    0x1F
#define     IRPROXY_CKDIV_SENSOR_CLKDIV_SHIFT   0

#define IRPROXY_GMS    0x5     // Sensor Gain / Mirror / Size settings -- XXXXX|Gain3|Gain2|Gain1|X|0|UpRow|UpCol
#define     IRPROXY_GMS_DFLT   0x053

#define IRPROXY_TRIGGER_ADDR    0xE     // Trigger address parameter (send 0x010, then use Trigger Value register to set working mode
#define     IRPROXY_TRIGGER_ADDR_MAGIC_NUM  0x010

#define IRPROXY_SW_TRIGGER      0xE     // Software trigger command. If s/w trigger is enabled, send 0x011 to get 1 frame
#define     IRPROXY_SW_TRIGGER_MAGICNUM     0x011

#define IRPROXY_TRIGGER_VAL     0xF    // Trigger Mode Selection
#define     IRPROXY_TRIGGER_VAL_DFLT      0x0
#define     IRPROXY_TRIGGER_VAL_FREERUN     0x000  // free running mode
#define     IRPROXY_TRIGGER_VAL_TRIGMODE_A  0x001  // 'Ulis' trigger mode
#define     IRPROXY_TRIGGER_VAL_TRIGMODE_B  0x002  // 'FullScale' trigger mode
#define     IRPROXY_TRIGGER_VAL_SWTRIG      0x003  // software trigger mode


#endif //  __K80_PROXY_REGISTERS_H  ========================================= End of File ================================

