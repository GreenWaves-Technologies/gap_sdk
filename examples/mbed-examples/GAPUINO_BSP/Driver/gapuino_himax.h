#ifndef __GAPUINO_HIMAX_H
#define __GAPUINO_HIMAX_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "gap_common.h"
#include "PinNames.h"
#include "../../Components/himax/himax.h"

typedef struct regval_list_ {
    uint16_t reg_num;
    uint8_t  value;
} regval_list_t;

uint8_t HIMAX_Open(void);
void HIMAX_Mode(uint8_t mode);


#ifdef __cplusplus
}
#endif

#endif /* __GAPUINO_HIMAX_H */
