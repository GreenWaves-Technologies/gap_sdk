/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "vp/launcher.h"
#include "vp/launcher_internal.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <string>
#include <sys/types.h>
#include <signal.h>
#include <sys/prctl.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>


typedef struct {
  char **opts;
  int nb_opt;
  char *config_file;
} gv_launcher_t;

typedef struct {
  int rcv_pipe[2];
  int snd_pipe[2];
  pthread_t thread;
  gv_launcher_t *gv;
  FILE *snd_file;
  gv_ioreq_request_t callback;
  void *context;
} gv_ioreq_binding_t;

static pid_t child_id = -1;

void gv_init(gv_conf_t *gv_conf)
{
  gv_conf->timing = GV_CONF_NO_TIMING;
}

static void add_option(gv_launcher_t *gv, char *opt) {
  gv->nb_opt++;
  gv->opts = (char **)realloc(gv->opts, sizeof(char *)*gv->nb_opt);
  gv->opts[gv->nb_opt-1] = opt;
}

void *gv_create(gv_conf_t *gv_conf, char *config_file)
{
  gv_launcher_t *gv = (gv_launcher_t *)malloc(sizeof(gv_launcher_t));
  if (gv == NULL) return NULL;

  gv->opts = NULL;
  gv->nb_opt = 0;
  gv->config_file = strdup(config_file);

  add_option(gv, (char *)"pulp-run");

  return (void *)gv;
}

static void gv_process(gv_launcher_t *gv)
{
  add_option(gv, (char *)"--platform=gvsoc");
  add_option(gv, (char *)"--config-file");
  add_option(gv, gv->config_file);
  add_option(gv, NULL);
  //close(gv->snd_pipe[1]);
  //close(gv->rcv_pipe[0]);

  if (execvp("pulp-run", gv->opts)) {
    perror("FATAL ERROR while starting virtual platform");
    exit(-1);
  }
}

int gv_launch(void *handle)
{
  gv_launcher_t *gv = (gv_launcher_t *)handle;

  pid_t ppid_before_fork = getpid();
  child_id = fork();
  if(child_id == -1) {
    return -1;
  } else if (child_id == 0) {
    int r = prctl(PR_SET_PDEATHSIG, SIGTERM);
    if (r == -1) { perror(0); exit(1); }
    // test in case the original parent exited just
    // before the prctl() call
    if (getppid() != ppid_before_fork) exit(1);
    gv_process(gv);
    return 0;
  } else {
    //replyFile = fdopen(replyPipe[0], "r");
  }


  return 0;
}

void gv_destroy(void *handle)
{
  if (child_id != -1) {
    kill(-child_id, SIGTERM);
    sleep(1);
    kill(-child_id, SIGKILL);
  }
}

static void ioreq_response(void *context, gv_ioreq_t *req)
{
  gv_ioreq_desc_t *desc = (gv_ioreq_desc_t *)context;
  gv_ioreq_binding_t *binding = (gv_ioreq_binding_t *)desc->binding;
  desc->type = GV_IOREQ_DESC_TYPE_RESPONSE;
  desc->latency = req->latency;
  desc->timestamp += req->latency;
  if (fwrite((void *)desc, sizeof(*desc), 1, binding->snd_file) != 1) return;
  if (!desc->is_write && fwrite(desc->data, desc->size, 1, binding->snd_file) != 1) return;
  fflush(binding->snd_file);
  free(desc->data);
  free((void *)desc);
}

static void *ioreq_routine(void *arg)
{
  gv_ioreq_binding_t *binding = (gv_ioreq_binding_t *)arg;

  FILE *f = fdopen(binding->rcv_pipe[0], "r");
  if (f == NULL) return NULL;

  gv_ioreq_desc_t *req = NULL;

  while(1) {
    if (req == NULL) req = (gv_ioreq_desc_t *)malloc(sizeof(gv_ioreq_desc_t));
    if (req == NULL) return NULL;

    if (fread((void *)req, sizeof(*req), 1, f) != 1) return NULL;

    if (req->type == GV_IOREQ_DESC_TYPE_RESPONSE)
    {
      // Fabric side sent us a response fro a host->fabric request
      if (!req->is_write && fread(req->data, req->size, 1, f) != 1) return NULL;
      if (req->response_cb != NULL) {
        req->response_cb(req->response_context, &req->user_req);
      }
    }
    else
    {
      req->data = malloc(req->size);
      if (req->data == NULL) return NULL;

      if (req->is_write && fread(req->data, req->size, 1, f) != 1) return NULL;
      gv_ioreq_binding_t *binding = (gv_ioreq_binding_t *)req->binding;
      if (binding->callback != NULL) {
        binding->callback(binding->context, (void *)req->data, (void *)req->addr, req->size, req->is_write,
          ioreq_response, (void *)req);
        req = NULL;
      }
    }
  }
}


void *gv_ioreq_binding(void *handle, char *path, void *base, size_t size, gv_ioreq_request_t callback, void *context)
{
  gv_launcher_t *gv = (gv_launcher_t *)handle;

  gv_ioreq_binding_t *binding = (gv_ioreq_binding_t *)malloc(sizeof(gv_ioreq_binding_t));
  if (binding == NULL) return NULL;

  binding->callback = callback;
  binding->context = context;

  if(pipe(binding->snd_pipe) == -1) return NULL;
  if(pipe(binding->rcv_pipe) == -1) return NULL;

  std::string str = "--config-opt=**/" + std::string(path) + "/rcv_fd=" + std::to_string(binding->snd_pipe[0]);
  add_option(gv, strdup((char *)str.c_str()));

  str = "--config-opt=**/" + std::string(path) + "/snd_fd=" + std::to_string(binding->rcv_pipe[1]);
  add_option(gv, strdup((char *)str.c_str()));

  str = "--config-opt=**/" + std::string(path) + "/external_binding/base=" + std::to_string((int64_t)base);
  add_option(gv, strdup((char *)str.c_str()));

  str = "--config-opt=**/" + std::string(path) + "/external_binding/size=" + std::to_string(size);
  add_option(gv, strdup((char *)str.c_str()));

  str = "--config-opt=**/" + std::string(path) + "/context=" + std::to_string((int64_t)binding);
  add_option(gv, strdup((char *)str.c_str()));

  binding->gv = (gv_launcher_t *)handle;
  binding->snd_file = fdopen(binding->snd_pipe[1], "w");
  if (binding->snd_file == NULL) return NULL;

  pthread_create(&binding->thread, NULL, ioreq_routine, (void *)binding);
  
  return (gv_ioreq_binding_t *)binding;
}

int gv_ioreq(void *_binding, int64_t timestamp, void *data, void *addr, size_t size, int is_write, gv_ioreq_response_t callback, void *context)
{
  gv_ioreq_binding_t *binding = (gv_ioreq_binding_t *)_binding;

  gv_ioreq_desc_t desc = { .type=GV_IOREQ_DESC_TYPE_REQUEST, .addr=(uint64_t)addr, .size=(uint64_t)size, .is_write=(int64_t)is_write, .timestamp=timestamp,
    .response_cb=callback, .response_context=context
  };
  desc.data = data;
  if (fwrite((void *)&desc, sizeof(desc), 1, binding->snd_file) != 1) return -1;
  if (is_write && fwrite(data, size, 1, binding->snd_file) != 1) return -1;

  fflush(binding->snd_file);

  return 0;
}
