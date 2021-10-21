
#ifndef __LYNRED_H__
#define __LYNRED_H__

#include "lynredInfo.h"

#if defined(QSPI)
extern AT_QSPIFLASH_FS_EXT_ADDR_TYPE lynred_L3_Flash;
#else
extern AT_HYPERFLASH_FS_EXT_ADDR_TYPE lynred_L3_Flash;
#endif
#define INPUT1_Q S0_Op_input_1_Q

#define OUTPUT1_Q S12_Op_output_1_Q
#define OUTPUT2_Q S24_Op_output_2_Q
#define OUTPUT3_Q S36_Op_output_3_Q
#define OUTPUT4_Q S47_Op_output_4_Q
#define OUTPUT5_Q S16_Op_output_5_Q
#define OUTPUT6_Q S28_Op_output_6_Q
#define OUTPUT7_Q S40_Op_output_7_Q
#define OUTPUT8_Q S51_Op_output_8_Q

#endif //__LYNRED_H__
