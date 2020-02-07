/*
 * Copyright (C) 2019 GreenWaves Technologies
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


#ifndef __BSP__FS__HOSTFS_H__
#define __BSP__FS__HOSTFS_H__

#include "bsp/fs.h"

/**
 * @addtogroup FS
 * @{
 */

/**
 * @defgroup Hostfs Hostfs
 *
 */

/**
 * @addtogroup Hostfs
 * @{
 */

/**@{*/

/** \struct hostfs_conf
 * \brief Hostfs configuration structure.
 *
 * This structure is used to pass the desired Hyperflash configuration to the
 * runtime when opening the device.
 */
struct pi_hostfs_conf
{
  struct pi_fs_conf fs;  /*!< Generic flaFSsh configuration. */
};

/** \brief Initialize a Hostfs configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * FS device is opened.
 *
 * \param conf A pointer to the Hostfs configuration.
 */
void pi_hostfs_conf_init(struct pi_hostfs_conf *conf);

//!@}

/**
 * @} end of Hostfs
 */

/**
 * @} end of FS
 */

#endif 
