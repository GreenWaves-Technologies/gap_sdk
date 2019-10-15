#ifndef __FAST_H__
#define __FAST_H__

#include <stdint.h>

#define IMG_DATATYPE uint8_t
#define BORDER_FAST9 (3U)

void make_offsets(int32_t *, uint32_t);
// void circular_detection(uint32_t *, uint32_t, uint32_t, uint32_t, uint32_t, IMG_DATATYPE *, uint32_t *, uint32_t *, uint32_t);
void FAST_kernel(uint32_t *, uint32_t *, IMG_DATATYPE *, uint32_t , uint32_t , int32_t *, uint32_t);

#endif /* __FAST_H__ */
