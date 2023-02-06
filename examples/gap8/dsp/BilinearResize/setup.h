#ifndef __SETUP_H__
#define __SETUP_H__

#if defined(CAMERA)
#define HAVE_CAMERA
#else
#define HAVE_BRIDGE
#endif  /* CAMERA */
#define HAVE_DISPLAY
#define USE_BRIDGE
#if defined(AUTOTILER)
#define USE_AUTOTILER
#endif  /* AUTOTILER */

#if defined(USE_BRIDGE)
#include "ImgIO.h"
#if defined(__PULP_OS__)
#include "bridge_stubs.h"
#endif  /* __PULP_OS__ */
#endif  /* USE_BRIDGE */

#if defined(CAMERA)
#if defined(HIMAX)
#define CAMERA_WIDTH  ( 324 )
#define CAMERA_HEIGHT ( 244 )
#else
#define CAMERA_WIDTH  ( 320 )
#define CAMERA_HEIGHT ( 240 )
#endif  /* HIMAX */
#else
#define CAMERA_WIDTH  ( 324 )
#define CAMERA_HEIGHT ( 244 )
#endif  /* CAMERA */

#define LCD_OFF_X     ( 40 )
#define LCD_OFF_Y     ( 60 )

#define RESIZE_HEIGHT ( CAMERA_HEIGHT / 2 )
#define RESIZE_WIDTH  ( CAMERA_WIDTH / 2 )

#define STACK_SIZE    ( 2048 )

#endif  /* __SETUP_H__ */
