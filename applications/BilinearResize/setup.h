/*
 * Copyright 2019 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SETUP_H__
#define __SETUP_H__

#if defined(CAMERA)
#define HAVE_CAMERA
//#define HAVE_DISPLAY
#else
#define HAVE_BRIDGE
#endif  /* CAMERA */

#if defined(AUTOTILER)
#define USE_AUTOTILER
#endif  /* AUTOTILER */

#if defined(HAVE_BRIDGE)
#include "ImgIO.h"
#if defined(__PULP_OS__)
#include "bridge_stubs.h"
#endif  /* __PULP_OS__ */
#endif  /* HAVE_BRIDGE */

#define IMG_WIDTH  ( 320 )
#define IMG_HEIGHT ( 240 )

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
