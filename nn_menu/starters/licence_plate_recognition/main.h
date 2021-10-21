#ifndef __OCRSSD_H__
#define __OCRSSD_H__

#define __PREFIX1(x) ssdlite_ocr ## x
#define __PREFIX2(x) lprnet ## x

#include "Gap.h"

extern AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX1(_L3_Flash);
extern AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX2(_L3_Flash);

#endif
