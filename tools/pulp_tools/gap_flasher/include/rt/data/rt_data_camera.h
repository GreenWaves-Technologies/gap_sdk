/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __RT_DATA_RT_DATA_RT_DATA_CAMERA_H__
#define __RT_DATA_RT_DATA_RT_DATA_CAMERA_H__

#include "rt/rt_i2c.h"
#include "rt/data/rt_data_i2c.h"

/**
 * @addtogroup Camera
 * @{
 */

/** \enum rt_cam_resol_e
 * \brief Camera resolution.
 *
 * This enum defines the resolution of the image sensor.
 */
typedef enum{
    QVGA,       /*!< Resolution QVGA: 320*240 (pixels). */
    QQVGA,      /*!< Resolution QQVGA: 160*120 (pixels). */
}rt_cam_resol_e;

/** \enum rt_ov7670_format_e
 * \brief Output format of the ov7670 image sensor.
 *
 * This enum defines the output format of the OV7670 image sensor.
 * The simulator currently only supports RGB565.
 */
typedef enum{
    OV7670_RGB565 = 0,   /*!< Data format RGB565, 5bits for R, 6bits for G and 5bits for B. */
    OV7670_RGB555,       /*!< Data format RGB555, 5bits for R, 5bits for G and 5bits for B. */
    OV7670_RGB444,       /*!< Data format RGB444, 4bits for R, 4bits for G and 4bits for B. */
    OV7670_YUV422,       /*!< Data format YUV422, for each 4 bytes: Y,U,Y,V. */
    OV7670_MONO_COLOR,   /*!< Data format Grayscale, 1 pixel = 1 Bytes. */
}rt_ov7670_format_e;

/** \enum rt_himax_format_e
 * \brief Output format of the Himax image sensor.
 *
 * This enum defines the output format of the Himax image sensor.
 */
typedef enum{
    HIMAX_MONO_COLOR=0,     /*!< Data format Grayscale, 1 pixel = 1 Bytes. */
    HIMAX_RAW_BAYER,        /*!< Data format RAW Bayer. */
}rt_himax_format_e;

/** \enum rt_cam_fps_e
 * \brief Frame rate of the image sensor.
 *
 * This enum defines the frame rate of the Himax image sensor.
 * The simulator currently only supports fps30.
 */
typedef enum{
    fps5,            /*!< 5 frames per second. */
    fps10,           /*!< 10 frames per second. */
    fps30,           /*!< 30 frames per second. */
}rt_cam_fps_e;

/** \enum rt_cam_cmd_e
 * \brief Camera commands.
 *
 * This enum defines the commands which which can be used to control the image sensor.
 */
typedef enum{
  CMD_RESOL = 0,       /*!< Command for configuring the Resolution. */
  CMD_FORMAT,          /*!< Command for configuring the Output Format. */
  CMD_FPS,             /*!< Command for configuring the Frame Rates. */
  CMD_SLICE,           /*!< Command for configuring the area of interest. */
  CMD_FILTER,          /*!< Command for configuring the HW Filter. */
  CMD_SHIFT,           /*!< Command for configuring the HW Shifter. */
  CMD_FRAMEDROP,       /*!< Command for configuring frames drop. */
  CMD_START,           /*!< Command for enabling the camera interface. */
  CMD_STOP,            /*!< Command for disabling the camera interface and put camera on standby. */
  CMD_PAUSE,           /*!< Command for stop the camera interface. */
  CMD_INIT,            /*!< Command for init the camera interface and switch on camera. */
}rt_cam_cmd_e;


/** \struct rt_img_coordinate_t
 * \brief Coordinate of a pixel.
 *
 * This structure is used to describe the coordinate of a pixel in an image.
 */
typedef struct{
  unsigned int x:16;
  unsigned int y:16;
}rt_img_coordinate_t;


/** \struct rt_img_slice_t
 * \brief CPI interface parameter.
 *
 * This structure is used to define an area of interest in an image.
 */
typedef struct{
#ifdef ARCHI_UDMA_HAS_CAM
  rt_img_coordinate_t   slice_ll;   /*!< Structure contains the coordinates of lower-left corner. */
  rt_img_coordinate_t   slice_ur;   /*!< Structure contains the coordinates of upper-right corner. */
#endif
}rt_img_slice_t;

/** \struct rt_img_filter_t
 * \brief CPI interface parameter .
 *
 * This structure is used to define the HW filter coefficients (red, green, blue).
 * These coefficients are applied as follows:
 *          pixel = red*r_coeff + green*g_coeff + blue*b_coeff
 *
 * For example, with the RGB565 format, each pixel is coded in 2 bytes:
 *          red = pixel[15:11]
 *          green  = pixel[10:5]
 *          blue = pixel[4:0]
 * In the rx buffer, each pixel received would be the result of the above formula.
 */
typedef struct{
  unsigned char r_coeff;    /*!< Value of red coefficient */
  unsigned char g_coeff;    /*!< Value of green coefficient */
  unsigned char b_coeff;    /*!< Value of blue coefficient */
}rt_img_filter_t;


/** \enum rt_cam_type_e
 * \brief Camera identifier.
 *
 * This can be used to describe the type of camera connected to the chip.
 */
typedef enum {
  RT_CAM_TYPE_HIMAX,     /*!< Himax camera. */
  RT_CAM_TYPE_OV7670    /*!< Ov7670 camera. */
} rt_cam_type_e;



/** \struct rt_cam_conf_t
 * \brief CPI device configuration structure.
 *
 * This structure is used to pass the desired CPI configuration to the runtime when opening the device.
 */
typedef struct{
  unsigned int     resolution;            /*!< Resolution of the sensor. Please check the available values in enum rt_cam_resol_e */
  unsigned int     format;                /*!< Image format of the sensor. Please check the available values in enum rt_ov7670_format_e or rt_himax_format_e */
  unsigned int     fps;                   /*!< Frames per second of the sensor. Please check the available value in enum rt_cam_fps_e */
  unsigned int     slice_en;              /*!< Enables slicing mode */
  unsigned int     shift;                 /*!< Number of bits to shift pixels. For example, shift = 2, pixel received = (pixel >> 2) */
  unsigned int     frameDrop_en;          /*!< Enables frame dropping */
  unsigned int     frameDrop_value;       /*!< Define how many frames need to be dropped */
  unsigned int     cpiCfg;                /*!< Width of CPI interface*/
  int id;                                 /*!< If it is different from -1, this specifies on which CPI interface the device is connected. */
  int control_id;                                 /*!< If it is different from -1, this specifies on which interface the control interface of the device is connected. */
  rt_cam_type_e type;   /*!< Camera type. */
}rt_cam_conf_t;

/**
 * @}
 */


/// @cond IMPLEM

typedef struct rt_cam_s rt_camera_t;

typedef struct rt_cam_dev_s {
  rt_camera_t *(*open)(int channel, rt_cam_conf_t *cam, rt_event_t*event);
  void (*close)(rt_camera_t *dev_cam, rt_event_t *event);
  void (*control)(rt_camera_t *dev_cam, rt_cam_cmd_e cmd, void *arg);
  void (*capture)(rt_camera_t *dev_cam, void *buffer, size_t bufferlen, rt_event_t *event);
} rt_cam_dev_t;

typedef struct rt_cam_s {
  rt_cam_dev_t desc;
  int channel;
  rt_i2c_t          *i2c;
  rt_i2c_conf_t     i2c_conf;
  rt_cam_conf_t conf;
} rt_camera_t;

typedef struct rt_camera_req_s {
  rt_camera_t *handle;
  union {
    struct {
      void *buffer;
      size_t size;
    };
    struct {
      rt_cam_cmd_e cmd;
      void *arg;
    };
  };
  rt_event_t event;
  int done;
  unsigned char cid;
  unsigned char is_capture;
} rt_camera_req_t ;


/// @endcond



#endif
