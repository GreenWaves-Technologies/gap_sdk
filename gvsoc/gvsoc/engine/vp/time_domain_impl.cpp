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

#include <vp/vp.hpp>
#include "vp/time/time_engine.hpp"
#include <pthread.h>
#include <signal.h>

static pthread_t sigint_thread;



#ifdef __VP_USE_SYSTEMC

SC_MODULE(my_module)
{

  SC_HAS_PROCESS(my_module);
  my_module(sc_module_name nm, vp::time_engine *engine) : sc_module(nm), engine(engine)
  {
    SC_THREAD(run);
  }

  void run()
  {
    engine->run_loop();
  }

  vp::time_engine *engine;
};

#endif



class time_domain : public vp::time_engine
{

public:

  time_domain(const char *config);


};


time_domain::time_domain(const char *config)
: vp::time_engine(config)
{
}

// Global signal handler to catch sigint when we are in C world and after
// the engine has started.
// Just few pthread functions are signal-safe so just forward the signal to
// the sigint thread so that he can properly stop the engine
static void sigint_handler(int s) {
  pthread_kill(sigint_thread, SIGINT);
}

// This thread takes care of properly stopping the engine when ctrl C is hit
// so that the python world can properly close everything
static void *signal_routine(void *arg) {
  vp::time_engine *engine = (vp::time_engine *)arg;
  sigset_t sigs_to_catch;
  int caught;
  sigemptyset(&sigs_to_catch);
  sigaddset(&sigs_to_catch, SIGINT);
  do {
    sigwait(&sigs_to_catch, &caught);
    engine->stop_engine(true);
  } while (1);
  return NULL;
}

#ifdef __VP_USE_SYSTEMC
static void *engine_routine_sc_stub(void *arg) {
  vp::time_engine *engine = (vp::time_engine *)arg;
  my_module module("Hello", engine);
  engine->elab();
  while(1){
    sc_start();
    engine->wait_ready();
  }
  return NULL;
}
#endif

// Routine executed by the thread running the global time engine.
// Just switch to C++ world.

void set_sc_main_entry(void *(*entry)(void *), void *arg);

static void *engine_routine(void *arg) {
  vp::time_engine *engine = (vp::time_engine *)arg;
#ifdef __VP_USE_SYSTEMC
  set_sc_main_entry(&engine_routine_sc_stub, arg);
  sc_core::sc_elab_and_sim(0, NULL);
#else
  engine->run_loop();
#endif
  return NULL;
}

vp::time_engine::time_engine(const char *config)
  : vp::component(config), first_client(NULL)
{
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
  pthread_mutex_lock(&mutex);

  run_req = false;
  stop_req = false;
}


// This is called by the python thread once he wants to start the time engine.
// This for now just takes care of stopping the engine when it is asked
// and in this case returns to python world so that everything is closed.
// This function can be called several time in case the python world decides
// to continue running the engine.
string vp::time_engine::run()
{
  string result = "stopped";

  pthread_mutex_lock(&mutex);

  while(locked)
  {
    pthread_cond_wait(&cond, &mutex);
  }

  // First run the engine
  run_req = true;
  pthread_cond_broadcast(&cond);

  // And wait until we get a stop request
  while(!stop_req && !finished)
  {
    pthread_cond_wait(&cond, &mutex);    
  }

  if (finished) return "finished";

  // In case we get a stop request, first try to kindly stop the engine.
  // Then if it is still running after 100ms, we kill it. This can happen
  // because this is a cooperative engine.
  if (stop_req)
  {
    run_req = false;

    if (running)
    {
      pthread_cond_broadcast(&cond);

      int rc = 0;
      struct timespec ts;
      clock_gettime(CLOCK_REALTIME, &ts);
      ts.tv_sec += 1;

      while (running && rc == 0)
      {
        rc = pthread_cond_timedwait(&cond, &mutex, &ts);
      }

      if (running)
      {

        running = false;
        pthread_cancel(run_thread);

        result = "killed";
      }
    }
  }

  pthread_mutex_unlock(&mutex);

  return result;
}

void vp::time_engine::start()
{

  js::config *item_conf = this->get_js_config()->get("**/gvsoc/no_exit");
  this->no_exit = item_conf != NULL && item_conf->get_bool();

  if (this->no_exit)
  {
    // In case the vp is connected to an external bridge, prevent the platform
    // from exiting in case there is no more events.
    retain_count++;
  }
  pthread_create(&run_thread, NULL, engine_routine, (void *)this);
}

void vp::time_engine::wait_ready()
{
  while (!first_client)
  {
  }
}

void vp::time_engine::run_loop()
{
#ifdef __VP_USE_SYSTEMC
  started = true;
#endif

  pthread_mutex_unlock(&mutex);

  while(1)
  {
    pthread_mutex_lock(&mutex);

    while(!run_req)
    {
      running = false;
      pthread_cond_broadcast(&cond);
      pthread_cond_wait(&cond, &mutex);
    }
    running = true;

    if (!init)
    {
      init = true;
      pthread_cond_broadcast(&cond);
      // Now that the engine is starting, we can register the final sigint handler
      // and create the sigint thread so that we can properly close simulation
      // in case ctrl C is hit.
      sigset_t sigs_to_block;
      sigemptyset(&sigs_to_block);
      sigaddset(&sigs_to_block, SIGINT);
      pthread_sigmask(SIG_BLOCK, &sigs_to_block, NULL);
      pthread_create(&sigint_thread, NULL, signal_routine, (void *)this);

      signal (SIGINT, sigint_handler);
    }

    pthread_mutex_unlock(&mutex);

    time_engine_client *current = first_client;

    if (current)
    {
      first_client = current->next;
      current->is_enqueued = false;

      // Update the global engine time with the current event time
      this->time = current->next_event_time;

      while (1)
      {
        current->running = true;

  #ifdef __VP_USE_SYSTEMC

        // Update the global engine time with the current event time
        this->time = current->next_event_time;

        // Execute the events for the next engine
        int64_t time = current->exec();

        current->is_enqueued = false;
        current->running = false;

        // And reenqueue it in case it has events in the future
        if (time > 0)
        {
          time += this->time;
          current->next_event_time = time;
          time_engine_client *client = first_client, *prev = NULL;

          while (client && client->next_event_time < time)
          {
            prev = client;
            client = client->next;
          }
          if (prev)
            prev->next = current;
          else
            first_client = current;

          current->next = client;
          current->is_enqueued = true;
        }

        if (!run_req) break;

        // Now loop until the systemC times reaches the time of out next event.
        // We can get back control before the next event in case the systemC part
        // enqueues a new event.
        while(1)
        {
          if (!first_client)
          {
            if (stop_req || locked) {
              break;
            }

            // In case we don't have any event to schedule, just wait until the systemc part
            // enqueues something on our side
            wait(sync_event);
          }
          else
          {
            // Otherwise, either wait until we can schedule our event
            // or wait unil the systemC part enqueues something before

            vp_assert(first_client->next_event_time >= (int64_t)sc_time_stamp().to_double(), NULL, "SystemC time is after vp time\n");
            wait(first_client->next_event_time - (int64_t)sc_time_stamp().to_double(), SC_PS, sync_event);

            int64_t current_sc_time = (int64_t)sc_time_stamp().to_double();
            if (current_sc_time == first_client->next_event_time) break;
          }
        }

        current = first_client;
        if (current)
        {
          vp_assert(first_client->next_event_time >= get_time(), NULL, "event time is before vp time\n");

          first_client = current->next;
          current->is_enqueued = false;
        }

  #else
    
        int64_t time = current->exec();

        time_engine_client *next = first_client;

        // Shortcut to quickly continue with the same client
        if (likely(time > 0))
        {
          time += this->time;
          if (likely((!next || next->next_event_time >= time)))
          {
            if (likely(run_req))
            {
              this->time = time;
              continue;
            }
            else
            {
              current->next = first_client;
              first_client = current;
              current->next_event_time = time;
              current->is_enqueued = true;
              current->running = false;
              break;
            }
          }
        }

        // Otherwise remove it, reenqueue it and continue with the next one.
        // We can optimize a bit the operation as we already know
        // who to schedule next.

        if (time > 0)
        {
          current->next_event_time = time;
          time_engine_client *client = next->next, *prev = next;
          while (client && client->next_event_time < time)
          {
            prev = client;
            client = client->next;
          }
          current->next = client;
          prev->next = current;
          current->is_enqueued = true;
        }

        current->running = false;

        if (!run_req) break;

        current = first_client;
        if (current)
        {
          vp_assert(first_client->next_event_time >= get_time(), NULL, "event time is before vp time\n");

          first_client = current->next;
          current->is_enqueued = false;
        }

  #endif

        if (!current) break;

        // Update the global engine time with the current event time
        this->time = current->next_event_time;

      }
    }

    pthread_mutex_lock(&mutex);

    running = false;

    while(!first_client && retain_count && !locked)
    {
#ifdef __VP_USE_SYSTEMC
      pthread_mutex_unlock(&mutex);
      wait(SC_ZERO_TIME);
      pthread_mutex_lock(&mutex);
#else
      pthread_cond_wait(&cond, &mutex);
#endif
    }

    current = first_client;

    if (first_client == NULL && !locked && !retain_count)
    {
#ifdef __VP_USE_SYSTEMC
      sc_stop();
#endif
      finished = true;
    }
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
  }
}




static void init_sigint_handler(int s) {
  raise(SIGTERM);
}


extern "C" void *vp_constructor(const char *config)
{
  // This should be the first C method called by python.
  // As python is not catching SIGINT where we are in C world, we have to
  // setup a temporary sigint handler to exit in case control+C is hit
  // until the engine is started and we can better handle it.
  signal (SIGINT, init_sigint_handler);

  return (void *)new time_domain(config);
}
