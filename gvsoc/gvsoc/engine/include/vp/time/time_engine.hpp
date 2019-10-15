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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __VP_TIME_ENGINE_HPP__
#define __VP_TIME_ENGINE_HPP__

#include "vp/vp_data.hpp"
#include "vp/component.hpp"

#ifdef __VP_USE_SYSTEMC
#include <systemc.h>
#endif



namespace vp {

  class time_engine_client;

  class time_engine : public component {
  public:
    time_engine(const char *config);

    void start();

    void run_loop();

    string run();

    int run_status() { return stop_status; }

    inline void lock_step();

    inline void lock_step_cancel();

    inline void lock();

    inline void wait_running();

    inline void unlock();

    inline void stop_engine(bool force=false);

    inline void stop_engine(int status);

    inline vp::time_engine *get_time_engine() { return this; }

    bool dequeue(time_engine_client *client);

    bool enqueue(time_engine_client *client, int64_t time);

    int64_t get_time() { return time; }

    inline void retain() { retain_count++; }
    inline void release() { retain_count--; }

    inline void fatal(const char *fmt, ...);

    inline void update(int64_t time);

    void wait_ready();
    
  private:
    time_engine_client *first_client = NULL;
    bool locked = false;
    bool locked_run_req;
    bool run_req;
    bool stop_req;
    bool finished = false;
    bool init = false;





    bool running;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t run_thread;

    int64_t time = 0;
    int stop_status = -1;
    int retain_count = 0;
    bool no_exit;

#ifdef __VP_USE_SYSTEMC
    sc_event sync_event;
    bool started = false;
#endif
  };

  class time_engine_client : public component {

    friend class time_engine;

  public:
    time_engine_client(const char *config)
      : vp::component(config)
    {
    }

    inline bool is_running() { return running; }

    inline bool enqueue_to_engine(int64_t time)
    {
      return engine->enqueue(this, time);
    }

    inline int64_t get_time() { return engine->get_time(); }

    virtual int64_t exec() = 0;

  protected:
    time_engine_client *next;

    // This gives the time of the next event.
    // It is only valid when the client is not the currently active one,
    // and is then updated either when the client is not the active one
    // anymore or when the client is enqueued to the engine.
    int64_t next_event_time = 0;

    vp::time_engine *engine;
    bool running = false;
    bool is_enqueued = false;
  };


  // This can be called from anywhere so just propagate the stop request
  // to the main python thread which will take care of stopping the engine.
  inline void vp::time_engine::stop_engine(bool force)
  {
    if (force || !this->no_exit)
    {
      // In case the vp is connected to an external bridge, prevent the platform
      // from exiting unless a ctrl-c is hit
      pthread_mutex_lock(&mutex);
      stop_req = true;
      pthread_cond_broadcast(&cond);
      pthread_mutex_unlock(&mutex);
    }
  }

  inline void vp::time_engine::stop_engine(int status)
  {
    stop_status = status;
#ifdef __VP_USE_SYSTEMC
    sync_event.notify();
#endif
    stop_engine();
  }

  inline void vp::time_engine::wait_running()
  {
    pthread_mutex_lock(&mutex);
    while (!init) pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
  }

  inline void vp::time_engine::lock_step()
  {
    if (!locked)
    {
      locked = true;
      locked_run_req = run_req;
      run_req = false;
    }
  }

  inline void vp::time_engine::lock_step_cancel()
  {
    pthread_mutex_lock(&mutex);
    if (locked)
    {
      run_req = locked_run_req;
      locked = false;
    }
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
  }

  inline void vp::time_engine::lock()
  {
    pthread_mutex_lock(&mutex);
    if (!locked)
    {
      locked_run_req = run_req;
      run_req = false;
      locked = true;
    }
    pthread_cond_broadcast(&cond);
    while (running) pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
  }

  inline void vp::time_engine::unlock()
  {
    pthread_mutex_lock(&mutex);
    run_req = locked_run_req;
    locked = false;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
  }

  inline void vp::time_engine::fatal(const char *fmt, ...)
  {
    fprintf(stdout, "[\033[31mFATAL\033[0m] ");
    va_list ap;
    va_start(ap, fmt);
    if (vfprintf(stdout, fmt, ap) < 0) {}
    va_end(ap);
    stop_engine(-1);
  }


  inline void vp::time_engine::update(int64_t time)
  {
    if (time > this->time)
      this->time = time;
  }


};

#endif
