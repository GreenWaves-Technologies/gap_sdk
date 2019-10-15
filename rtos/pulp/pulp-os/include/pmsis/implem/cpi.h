/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifndef __PMSIS_IMPLEM_CPI_H__
#define __PMSIS_IMPLEM_CPI_H__

#ifdef ARCHI_UDMA_HAS_CAM

#include "pmsis/drivers/cpi.h"
#include "pmsis/data/cpi.h"
#include "archi/udma/cpi/udma_cpi_v1.h"

static inline void pi_cpi_control_start(struct pi_device *device)
{
  rt_cpi_t *cpi = (rt_cpi_t *)device->data;
  udma_cpi_cam_cfg_glob_t reg = { .raw =  udma_cpi_cam_cfg_glob_get(cpi->base) };
  reg.en = 1;
  udma_cpi_cam_cfg_glob_set(cpi->base, reg.raw);
}

static inline void pi_cpi_control_stop(struct pi_device *device)
{
  rt_cpi_t *cpi = (rt_cpi_t *)device->data;
  udma_cpi_cam_cfg_glob_t reg = { .raw =  udma_cpi_cam_cfg_glob_get(cpi->base) };
  reg.en = 0;
  udma_cpi_cam_cfg_glob_set(cpi->base, reg.raw);
}

static inline void pi_cpi_set_format(struct pi_device *device, pi_cpi_format_e format)
{
  rt_cpi_t *cpi = (rt_cpi_t *)device->data;
  udma_cpi_cam_cfg_glob_t reg = { .raw =  udma_cpi_cam_cfg_glob_get(cpi->base) };
  reg.format = format;
  udma_cpi_cam_cfg_glob_set(cpi->base, reg.raw);
}

static inline void pi_cpi_set_frame_drop(struct pi_device *device, uint32_t nb_frame_dropped)
{
  rt_cpi_t *cpi = (rt_cpi_t *)device->data;
  udma_cpi_cam_cfg_glob_t reg = { .raw =  udma_cpi_cam_cfg_glob_get(cpi->base) };
  if (nb_frame_dropped)
  {
    reg.framedrop_en = 1;
    reg.framedrop_val = nb_frame_dropped;
  }
  else
  {
    reg.framedrop_en = 0;
  }
  udma_cpi_cam_cfg_glob_set(cpi->base, reg.raw);
}

static inline void pi_cpi_set_filter(struct pi_device *device, uint32_t r_coeff, uint32_t g_coeff, uint32_t b_coeff, uint32_t shift)
{
  rt_cpi_t *cpi = (rt_cpi_t *)device->data;

  udma_cpi_cam_cfg_glob_t reg = { .raw =  udma_cpi_cam_cfg_glob_get(cpi->base) };
  reg.shift = shift;
  udma_cpi_cam_cfg_glob_set(cpi->base, reg.raw);

  udma_cpi_cam_cfg_filter_set(cpi->base,
    UDMA_CPI_CAM_CFG_FILTER_B_COEFF(b_coeff) |
    UDMA_CPI_CAM_CFG_FILTER_G_COEFF(g_coeff) |
    UDMA_CPI_CAM_CFG_FILTER_R_COEFF(r_coeff)
  );
}

static inline void pi_cpi_set_slice(struct pi_device *device, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
  rt_cpi_t *cpi = (rt_cpi_t *)device->data;

  udma_cpi_cam_cfg_glob_t reg = { .raw =  udma_cpi_cam_cfg_glob_get(cpi->base) };
  reg.frameslice_en = w != 0;
  udma_cpi_cam_cfg_glob_set(cpi->base, reg.raw);

  if (w)
  {
    udma_cpi_cam_cfg_ll_set(cpi->base,
      UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX(x) |
      UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY(y)
    );

    udma_cpi_cam_cfg_ur_set(cpi->base,
      UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX(x + w - 1) |
      UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY(y + h - 1)
    );

    udma_cpi_cam_cfg_size_set(cpi->base,
      UDMA_CPI_CAM_CFG_SIZE_ROWLEN(w - 1)
    );
  }
}

#endif

#endif