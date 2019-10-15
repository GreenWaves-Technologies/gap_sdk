// File GAPOC_BSP_General.h

#ifndef  __GAPOC_BSP_GENERAL_H
#define  __GAPOC_BSP_GENERAL_H  // prevent multiple inclusions


// This files combines include files that any GAPOC platform may use,
// independently of the peripherals present on the board 
// (and addressed by additional files in ..../GAPOC_BSP/GAPOC_BSP_xxxx

// Note the GAPOC_BSP_Board_Init.c (source file) is however dependent on the which specific GAPOC
// is being used, and is therefore stored under  ..../GAPOC_<X>/GAPOC_BSP_Board_Init.h   wher <x> can be A, B, C....
// Same for GAPOC_BSP_Platform_Defs.h


#include "GAPOC_BSP_gap8.h"

#include "GAPOC_BSP_Platform_Defs.h"

#include "GAPOC_BSP_ImgIO.h"

#include "GAPOC_BSP_Pin_Services.h"

#include "GAPOC_BSP_Board_Init.h"


#endif   //####  __GAPOC_BSP_GENERAL_H  #####################################################

