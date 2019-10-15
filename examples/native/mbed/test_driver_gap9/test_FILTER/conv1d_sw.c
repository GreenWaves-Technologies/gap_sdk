#include "conv1d_sw.h"

void conv1d_uint16(uint16_t *data,uint16_t *result,uint16_t *kernel,uint32_t data_size, uint32_t kernel_size) {
    uint16_t acc;
    uint32_t num_windows = data_size - kernel_size + 1;
    for(uint32_t i=0;i<num_windows;i++) {
        acc=0;
        //printf("Window %d/%d\n",i,num_windows);
        for(uint32_t j=0;j<kernel_size;j++) {
            acc=acc+data[i+j]*kernel[j];
        }
        result[i]=acc;
    }
}

void conv1d_int16(short int *data,short int *result,short int *kernel,uint32_t data_size, uint32_t kernel_size) {
    short int acc;
    uint32_t num_windows = data_size - kernel_size + 1;
    for(uint32_t i=0;i<num_windows;i++) {
        acc=0;
        for(uint32_t j=0;j<kernel_size;j++) {
            acc=acc+data[i+j]*kernel[j];
        }
        result[i]=acc;
    }
}

void conv1d_uint32(uint32_t *data,uint32_t *result,uint32_t *kernel,uint32_t data_size, uint32_t kernel_size) {
    uint32_t acc;
    uint32_t num_windows = data_size - kernel_size + 1;
    for(uint32_t i=0;i<num_windows;i++) {
        acc=0;
        for(uint32_t j=0;j<kernel_size;j++) {
            acc=acc+data[i+j]*kernel[j];
        }
        result[i]=acc;
    }
}

void conv1d_int32(int *data,int *result,int *kernel,uint32_t data_size, uint32_t kernel_size) {
    int acc;
    uint32_t num_windows = data_size - kernel_size + 1;
    for(uint32_t i=0;i<num_windows;i++) {
        acc=0;
        for(uint32_t j=0;j<kernel_size;j++) {
            acc=acc+data[i+j]*kernel[j];
        }
        result[i]=acc;
    }
}

void vect_AxREGpB_uint16(uint16_t *vect_a,uint16_t *vect_b,uint16_t reg0,uint16_t *result,uint16_t size) {
    for(uint32_t i=0;i<size;i++) {
        result[i]=reg0*vect_a[i]+vect_b[i];
    }
}

void vect_ApB_uint16(uint16_t *vect_a,uint16_t *vect_b,uint16_t *result,uint16_t size) {
    for(uint32_t i=0;i<size;i++) {
        result[i]=vect_a[i]+vect_b[i];
    }
}

