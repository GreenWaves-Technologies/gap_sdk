#ifndef __IMAGENET_H__
#define __IMAGENET_H__

#include "face_detection_frontKernels.h"
#include "face_detection_frontInfo.h"
#include "Gap.h"
#include "gaplib/ImgIO.h"

#ifdef __EMUL__
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/param.h>
#include <string.h>
#endif
extern AT_HYPERFLASH_FS_EXT_ADDR_TYPE face_detection_front_L3_Flash;


#endif