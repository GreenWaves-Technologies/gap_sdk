#ifndef __IMAGES_H__
#define __IMAGES_H__

//NOTE Image and offset import (default 32x32)
#ifndef WIDTH
# define WIDTH         32U
#endif

#if WIDTH == 32U
# include "images/chess32.h" //NOTE BW Chess square image

# ifndef HEIGHT
#  define HEIGHT        WIDTH
# endif

#ifndef BAND_HEIGHT
#define BAND_HEIGHT  (32U)
#endif

# define CHECK_CORNERS  245U
# define CHECKSUM       70658ULL

#elif WIDTH == 64U
# include "images/chess64.h" //NOTE BW Chess square image

# ifndef HEIGHT
#  define HEIGHT        WIDTH
# endif

#ifndef BAND_HEIGHT
#define BAND_HEIGHT  (32U)
#endif

# define CHECK_CORNERS  1158U
# define CHECKSUM       371283ULL

#elif WIDTH == 128U
# include "images/chess128.h" //NOTE BW Chess square image

# ifndef HEIGHT
#  define HEIGHT        WIDTH
# endif

#ifndef BAND_HEIGHT
#define BAND_HEIGHT  (12U)
#endif

# define CHECK_CORNERS  5092U
# define CHECKSUM       0x1de612ULL

//NOTE: following images are too big...
// #elif WIDTH == 240U
// # include "images/real-240x160bw.h" //NOTE BW Chess square image
//
// # ifndef HEIGHT
// #  define HEIGHT        160
// # endif
//
// # define CHECK_CORNERS  5092U
// # define CHECKSUM       0x1de612ULL

#else
# error WIDTH not supported
#endif

#ifndef CHECK_CORNERS
#define CHECK_CORNERS  0U
#define CHECKSUM       0x0ULL
#endif


#endif /* __IMAGES_H__*/
