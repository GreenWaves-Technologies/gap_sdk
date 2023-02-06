
#ifndef __MNIST_H__
#define __MNIST_H__

#define __PREFIX(x) Mnist ## x

#include "Gap.h"

#ifdef __EMUL__
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/param.h>
#include <string.h>
#endif
extern AT_HYPERFLASH_FS_EXT_ADDR_TYPE mnist_L3_Flash;



#endif
