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

/*
 * Authors: Yao ZHANG, GreenWaves Technologies (yao.zhang@greenwaves-technologies.com)
 */

#include "rt/rt_api.h"

void _camera_drop_frame(rt_cam_conf_t *cam, unsigned int *arg){
  plpUdmaCamCustom_u _cpi;
  _cpi.raw = hal_cpi_glob_read(0);
  _cpi.cfg_glob.framedrop_enable = ENABLE & MASK_1BIT;
  _cpi.cfg_glob.framedrop_value =  *arg & MASK_6BITS;

  hal_cpi_glob_set(0, _cpi.raw);
}

void _camera_normlize(rt_cam_conf_t *cam, unsigned int *arg){
  cam->shift = *arg;
}

void _camera_filter(rt_cam_conf_t *cam, rt_img_filter_t *filter){
  plpUdmaCamCustom_u _cpi;
  _cpi.cfg_filter.r_coeff = filter->r_coeff;
  _cpi.cfg_filter.g_coeff = filter->g_coeff;
  _cpi.cfg_filter.b_coeff = filter->b_coeff;
  hal_cpi_filter_set(0, _cpi.raw);
}

void _camera_extract(rt_cam_conf_t *cam, rt_img_slice_t *slicer){
  plpUdmaCamCustom_u _cpi;
  // Write the coordinate of lower left corner
  _cpi.cfg_ll.frameslice_llx = (slicer->slice_ll.x/2) & MASK_16BITS;
  _cpi.cfg_ll.frameslice_lly = slicer->slice_ll.y & MASK_16BITS;
  hal_cpi_ll_set(0, _cpi.raw);

  _cpi.raw = 0;
  // Write the coordinate of upper right corner
  _cpi.cfg_ur.frameslice_urx = (slicer->slice_ur.x/2-1) & MASK_16BITS;
  _cpi.cfg_ur.frameslice_ury = (slicer->slice_ur.y-1) & MASK_16BITS;
  hal_cpi_ur_set(0, _cpi.raw);

  _cpi.raw = 0;
  _cpi.raw = hal_cpi_glob_read(0);
  _cpi.cfg_glob.frameslice_enable = ENABLE;
  hal_cpi_glob_set(0, _cpi.raw);
}

void _camera_stop(){
  plpUdmaCamCustom_u _cpi;
  _cpi.raw = hal_cpi_glob_read(0);
  _cpi.cfg_glob.enable = DISABLE;
  hal_cpi_glob_set(0, _cpi.raw);
}

void _camera_start(){
  plpUdmaCamCustom_u _cpi;
  _cpi.raw = hal_cpi_glob_read(0);
  _cpi.cfg_glob.enable = ENABLE;
  hal_cpi_glob_set(0, _cpi.raw);
}

rt_camera_t* rt_camera_open(char *dev_name, rt_cam_conf_t *conf, rt_event_t*event)
{
  int irq = rt_irq_disable();

  rt_trace(RT_TRACE_DEV_CTRL, "[CAM] Opening camera device (name: %s)\n", dev_name);

  rt_cam_dev_t *desc;
  int channel;

  if (dev_name)
  {
    rt_dev_t *dev = rt_dev_get(dev_name);
    if (dev == NULL) goto error;

    desc = (rt_cam_dev_t *)dev->desc;
    if (desc == NULL) goto error;

    if (dev->channel != -1)
        channel = dev->channel & 0xf;
    else
        channel = dev->itf + ARCHI_UDMA_CAM_ID(dev->itf);
  }
  else
  {
    channel = ARCHI_UDMA_CAM_ID(conf->id);
    if (conf->type == RT_CAM_TYPE_HIMAX)
      desc = &himax_desc;
    else
      goto error;
  }

  rt_camera_t *cam = desc->open(channel, conf, event);
  if (cam == NULL) goto error;

  memcpy((void *)&cam->desc, (void *)desc, sizeof(rt_cam_dev_t));

  rt_irq_restore(irq);

  return cam;

error:
  rt_irq_restore(irq);

  rt_warning("[CAM] Failed to open camera device\n");
  return NULL;
}


void rt_camera_conf_init(rt_cam_conf_t *conf)
{
  conf->resolution = QVGA;
  conf->format = 0;
  conf->fps = fps30;
  conf->slice_en = DISABLE;
  conf->shift = 0;
  conf->frameDrop_en = DISABLE;
  conf->frameDrop_value = 0;
  conf->cpiCfg = UDMA_CHANNEL_CFG_SIZE_16;
  conf->id = -1;
  conf->type = RT_CAM_TYPE_HIMAX;
  conf->control_id = -1;
}



#if defined(ARCHI_HAS_CLUSTER)

void __rt_camera_cluster_req_done(void *_req)
{
  rt_camera_req_t *req = (rt_camera_req_t *)_req;
  req->done = 1;
  __rt_cluster_notif_req_done(req->cid);
}

void __rt_camera_cluster_req(void *_req)
{
  rt_camera_req_t *req = (rt_camera_req_t *)_req;
  if (req->is_capture)
  {
    rt_event_t *event = &req->event;
    __rt_init_event(event, rt_event_internal_sched(), __rt_camera_cluster_req_done, (void *)req);
    __rt_event_set_pending(event);
    rt_camera_capture (req->handle, req->buffer, req->size, event);
  }
  else
  {
    rt_cam_control(req->handle, req->cmd, req->arg);
    req->done = 1;
    __rt_cluster_notif_req_done(req->cid);
  }
}

void rt_camera_cluster_capture (rt_camera_t *handle, void *buffer, size_t size, rt_camera_req_t *req)
{
  req->handle = handle;
  req->buffer = buffer;
  req->size = size;
  req->cid = rt_cluster_id();
  req->done = 0;
  req->is_capture = 1;
  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_camera_cluster_req, (void *)req);
  // Mark it as pending event so that it is not added to the list of free events
  // as it stands inside the event request
  __rt_event_set_pending(&req->event);
  __rt_cluster_push_fc_event(&req->event);
}

void rt_camera_cluster_control(rt_camera_t *handle, rt_cam_cmd_e cmd, void *arg, rt_camera_req_t *req)
{
  req->handle = handle;
  req->cmd = cmd;
  req->arg = arg;
  req->cid = rt_cluster_id();
  req->done = 0;
  req->is_capture = 0;
  __rt_init_event(&req->event, __rt_cluster_sched_get(), __rt_camera_cluster_req, (void *)req);
  // Mark it as pending event so that it is not added to the list of free events
  // as it stands inside the event request
  __rt_event_set_pending(&req->event);
  __rt_cluster_push_fc_event(&req->event);
}

#endif
