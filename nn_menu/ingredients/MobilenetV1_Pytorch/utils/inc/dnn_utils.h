#ifndef __DNN_UTILS_H__
#define __DNN_UTILS_H__

#include <bsp/bsp.h>
#include <bsp/ram/hyperram.h>
#include "bsp/ram.h"
#include "bsp/fs.h"

//#include "setup.h"
//
//extern struct pi_device HyperRam;
//
#define IO_BUFF_SIZE 1024
//
//extern short memory_pool[MEMORY_POOL_SIZE];

void* loadLayerFromFsToL3(struct pi_device *fs, const char* file_name, struct pi_device *hyper, int* layer_size);

void loadLayerFromL3ToL2(struct pi_device *hyper, void* hyper_buff, void* base_addr, int layer_size);

void clusterLoadLayerFromL3ToL2(struct pi_device* hyper, void* hyper_buff, void* base_addr, int layer_size);

int loadLayerFromFsToL2(struct pi_device *fs, const char* file_name, void* buffer, int size);



#endif
