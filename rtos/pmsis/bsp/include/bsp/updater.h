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
 * on 12/20/2019.
 */

#ifndef SSBL_UPDATER_H
#define SSBL_UPDATER_H

#include "pmsis.h"
#include "bsp/fs.h"
#include "bsp/fs/readfs.h"

pi_err_t update_from_fs(pi_device_t *flash, pi_device_t *fs, const char *binary_path);

pi_err_t update_from_readfs(pi_device_t *flash, const char *binary_path);


#endif //SSBL_UPDATER_H
