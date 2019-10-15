// File Identify_Image_ROI.h

#ifndef  __IDENTIFY_IMAGE_ROI_H
#define  __IDENTIFY_IMAGE_ROI_H  // prevent multiple inclusions


// ==  Includes relied upon in this header file   =================================
#include "GAPOC_BSP_General.h"


// ==  Custom Types  ==============================================================

typedef struct _ROI_Coord_t{
    uint32_t  ROI_TopLeft_X;
    uint32_t  ROI_TopLeft_Y;
    uint32_t  ROI_BotRight_X;
    uint32_t  ROI_BotRight_Y;   
} ROI_Coord_t;

#define     MAX_NUM_ROI     16            
typedef struct _ROI_Struct_t{
    uint32_t  Num_ROI;
    ROI_Coord_t  All_ROI_Coord[MAX_NUM_ROI];
} ROI_Struct_t;
    
    
// ==  Public Functions Prototypes        ============================================  
    
ROI_Struct_t    Identify_Image_ROI( uint8_t* image_buffer, uint32_t Picture_Width, uint32_t Picture_Height );



#endif  // __IDENTIFY_IMAGE_ROI_H
