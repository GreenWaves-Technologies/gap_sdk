#ifndef __SETUP_H__
#define __SETUP_H__

#define SILENT
#ifdef SILENT
#define PRINTF(...) ((void) 0)
#else
#define PRINTF printf
#endif  /* DEBUG */

//#define DISPLAY

#define QVGA 1
//#define QQVGA 1

#if defined(QVGA)
#if defined(HIMAX)
#define CAMERA_WIDTH  ( 324 )
#define CAMERA_HEIGHT ( 244 )
#else
#define CAMERA_WIDTH  ( 320 )
#define CAMERA_HEIGHT ( 240 )
#endif  /* HIMAX */
#else
#define CAMERA_WIDTH  ( 160 )
#define CAMERA_HEIGHT ( 120 )
#endif  /* QVGA */

#define LCD_WIDTH     ( 320 )
#define LCD_HEIGHT    ( 240 )

#endif /* __SETUP_H__ */
