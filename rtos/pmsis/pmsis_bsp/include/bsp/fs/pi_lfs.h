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

/*
 * Created by Mathieu Barbe <mathieu.barbe@greenwaves-technologies.com>.
 * on 11/29/2019.
 */

#ifndef PMSIS_PI_LFS_H
#define PMSIS_PI_LFS_H

#include "stdint.h"

#include "pmsis.h"
#include "bsp/fs.h"
#include "bsp/fs/lfs.h"

#include "bsp/fs.h"

/**
 * @addtogroup FS
 * @{
 */

/**
 * @defgroup LFS LFS
 *
 */

/**
 * @addtogroup LFS
 * @{
 */

/**@{*/

/** \struct hostfs_conf
 * \brief LFS configuration structure.
 *
 * This structure is used to pass the desired LFS configuration to the
 * runtime when opening the device.
 */
struct pi_lfs_conf
{
  struct pi_fs_conf fs;  /*!< Generic flaFSsh configuration. */
};

/** \brief Initialize a LFS configuration with default values.
 *
 * The structure containing the configuration must be kept alive until the
 * FS device is opened.
 *
 * \param conf A pointer to the Hostfs configuration.
 */
void pi_lfs_conf_init(struct pi_lfs_conf *conf);

lfs_t *pi_lfs_get_native_lfs(pi_device_t *device);

//!@}

/**
 * @} end of Hostfs
 */

/**
 * @} end of FS
 */

#endif //PMSIS_PI_LFS_H
