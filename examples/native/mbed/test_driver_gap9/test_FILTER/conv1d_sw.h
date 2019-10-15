#ifndef __CONV1D_SW_H__
#define __CONV1D_SW_H__
#include "stdint.h"

void conv1d_uint16(uint16_t *data,
                   uint16_t *result,
                   uint16_t *kernel,
                   uint32_t data_size,
                   uint32_t kernel_size);

void conv1d_int16(short int *data,
                  short int *result,
                  short int *kernel,
                  uint32_t data_size,
                  uint32_t kernel_size);

void conv1d_uint32(uint32_t *data,
                   uint32_t *result,
                   uint32_t *kernel,
                   uint32_t data_size,
                   uint32_t kernel_size);

void conv1d_int32(int *data,
                  int *result,
                  int *kernel,
                  uint32_t data_size,
                  uint32_t kernel_size);

void vect_AxREGpB_uint16(uint16_t *vect_a,
                         uint16_t *vect_b,
                         uint16_t reg0,
                         uint16_t *result,
                         uint16_t size);

void vect_ApB_uint16(uint16_t *vect_a,
                     uint16_t *vect_b,
                     uint16_t *result,
                     uint16_t size);
#endif
