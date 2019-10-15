#ifndef __IMAGES_H__
#define __IMAGES_H__

//NOTE Input Images
#ifndef WIDTH
# define WIDTH         160U
#endif

#if WIDTH == 160U
#include "images/image160x80.h"

# ifndef HEIGHT
#  define HEIGHT        80U
# endif

#ifndef BAND_HEIGHT
#define BAND_HEIGHT     (20U)
#endif

#define CHECKSUM        0x8a6eULL
#define CHECK_X         82U
#define CHECK_Y         39U

#elif WIDTH == 320U
#include "images/image320x240.h"

# ifndef HEIGHT
#  define HEIGHT        240U
# endif

#ifndef BAND_HEIGHT
#define BAND_HEIGHT     (12U)
#endif

#define CHECKSUM        0x538d6ULL
#define CHECK_X         238U
#define CHECK_Y         62U

#elif WIDTH == 640U
#include "images/image640x480.h"

# ifndef HEIGHT
#  define HEIGHT        480U
# endif

// # define CHECKSUM       0x1de612ULL

#else
# error WIDTH not supported
#endif

#ifndef CHECKSUM
#define CHECKSUM        0x0ULL
#define CHECK_X         0x0U
#define CHECK_Y         0x0U
#endif

#endif /*__IMAGES_H__*/
