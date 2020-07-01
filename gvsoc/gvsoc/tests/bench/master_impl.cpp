/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <time.h>

#define ENQUEUE_ITER 100000000
#define CALL_ITER 100000000

class master : public vp::component
{

public:

  master(js::config *config);

  int build();

  void start();

  static void test_enqueue_1(void *_this, vp::clock_event *event);
  static void test_enqueue_1_dyn(void *_this, vp::clock_event *event);
  static void test_enqueue_1_multiple(void *_this, vp::clock_event *event);
  static void test_enqueue_10(void *_this, vp::clock_event *event);
  static void test_enqueue_100(void *_this, vp::clock_event *event);
  static void test_enqueue_var(void *_this, vp::clock_event *event);
  static void test_call(void *_this, vp::clock_event *event);
  static void test_call_sync(void *_this, vp::clock_event *event);

  static void test(void *_this, vp::clock_event *event);

  static void resp(void *_this, vp::io_req *req);

  vp::clock_event *event;

private:

  vp::trace trace;
  vp::io_master out;
  int step;
  int delay;
};

void master::test_enqueue_1(void *__this, vp::clock_event *event)
{
  master *_this = (master *)__this;

  static int count = 0;
  static clock_t start;

  if (count == 0)
  {
    start = ::clock();
  }

  count++;

  if (count == ENQUEUE_ITER)
  {
     clock_t end = ::clock();
     double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
     printf("%f\n", ENQUEUE_ITER / time_elapsed_in_seconds / 1000000);
    _this->event_enqueue(_this->event_new((vp::clock_event_meth_t *)master::test), 1);
  }
  else
  {
    _this->event_enqueue(_this->event, 1);
  }
}

void master::test_enqueue_1_dyn(void *__this, vp::clock_event *event)
{
  master *_this = (master *)__this;

  _this->event_del(event);

  static int count = 0;
  static clock_t start;

  if (count == 0)
  {
    start = ::clock();
  }

  count++;

  if (count == ENQUEUE_ITER)
  {
     clock_t end = ::clock();
     double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
     printf("%f\n", ENQUEUE_ITER / time_elapsed_in_seconds / 1000000);
    _this->event_enqueue(_this->event_new((vp::clock_event_meth_t *)master::test), 1);
  }
  else
  {
    _this->event_enqueue(_this->event_new((vp::clock_event_meth_t *)master::test_enqueue_1_dyn), 1);
  }
}

void master::test_enqueue_1_multiple(void *__this, vp::clock_event *event)
{
  master *_this = (master *)__this;

  static int count = 0;
  static clock_t start;

  if (count == 0)
  {
    start = ::clock();
  }

  count++;

  if (count == ENQUEUE_ITER)
  {
     clock_t end = ::clock();
     double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
     printf("%f\n", ENQUEUE_ITER / time_elapsed_in_seconds / 1000000);
    _this->event_enqueue(_this->event_new((vp::clock_event_meth_t *)master::test), 1);
  }
  else if (count <= ENQUEUE_ITER - 6)
  {
    _this->event_enqueue(event, 1);    
  }
}

void master::test_enqueue_10(void *__this, vp::clock_event *event)
{
  master *_this = (master *)__this;

  static int count = 0;
  static clock_t start;

  if (count == 0)
  {
    start = ::clock();
  }

  count++;

  if (count == ENQUEUE_ITER)
  {
     clock_t end = ::clock();
     double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
     printf("%f\n", ENQUEUE_ITER / time_elapsed_in_seconds / 1000000);
    _this->event_enqueue(_this->event_new((vp::clock_event_meth_t *)master::test), 1);
  }
  else
  {
    _this->event_enqueue(_this->event, 10);    
  }
}

void master::test_enqueue_100(void *__this, vp::clock_event *event)
{
  master *_this = (master *)__this;

  static int count = 0;
  static clock_t start;

  if (count == 0)
  {
    start = ::clock();
  }

  count++;

  if (count == ENQUEUE_ITER)
  {
     clock_t end = ::clock();
     double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
     printf("%f\n", ENQUEUE_ITER / time_elapsed_in_seconds / 1000000);
    _this->event_enqueue(_this->event_new((vp::clock_event_meth_t *)master::test), 1);
  }
  else
  {
    _this->event_enqueue(_this->event, 100);    
  }
}

void master::test_enqueue_var(void *__this, vp::clock_event *event)
{
  master *_this = (master *)__this;

  static int count = 0;
  static clock_t start;

  if (count == 0)
  {
    start = ::clock();
  }

  count++;

  if (count == ENQUEUE_ITER)
  {
     clock_t end = ::clock();
     double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
     printf("%f\n", ENQUEUE_ITER / time_elapsed_in_seconds / 1000000);
    _this->event_enqueue(_this->event_new((vp::clock_event_meth_t *)master::test), 1);
  }
  else
  {
    _this->event_enqueue(_this->event, _this->delay);    
  }
}

void master::test_call(void *__this, vp::clock_event *event)
{
  master *_this = (master *)__this;

  clock_t start = ::clock();

  vp::io_req *req = _this->out.req_new(0, NULL, 0, 0);

  for (int i=0; i<CALL_ITER; i++)
  {
    _this->out.req(req);
  }

  clock_t end = ::clock();
  double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
  printf("%f\n", CALL_ITER / time_elapsed_in_seconds / 1000000);
  _this->event_enqueue(_this->event_new((vp::clock_event_meth_t *)master::test), 1);
}

void master::test_call_sync(void *__this, vp::clock_event *event)
{
  master *_this = (master *)__this;

  clock_t start = ::clock();

  for (int i=0; i<CALL_ITER; i++)
  {
    vp::io_req *req = _this->out.req_new(0, NULL, 0, 0);
    _this->out.req(req);
    _this->out.req_del(req);
  }

  clock_t end = ::clock();
  double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
  printf("%f\n", CALL_ITER / time_elapsed_in_seconds / 1000000);
  _this->event_enqueue(_this->event_new((vp::clock_event_meth_t *)master::test), 1);
}

void master::test(void *__this, vp::clock_event *event)
{
  master *_this = (master *)__this;

  switch (_this->step)
  {
    case 0:
      printf("Benchmarking event enqueue with 1 cycle constant\n");
      _this->event = _this->event_new(master::test_enqueue_1);
      _this->event_enqueue(_this->event, 1);
      break;
    case 1:
      printf("Benchmarking event enqueue with 1 cycle constant with event allocation\n");
      _this->event = _this->event_new(master::test_enqueue_1_dyn);
      _this->event_enqueue(_this->event, 1);
      break;
    case 2:
      printf("Benchmarking event enqueue with 1 cycle constant with 6 events\n");
      _this->event_enqueue(_this->event_new(master::test_enqueue_1_multiple), 1);
      _this->event_enqueue(_this->event_new(master::test_enqueue_1_multiple), 1);
      _this->event_enqueue(_this->event_new(master::test_enqueue_1_multiple), 1);
      _this->event_enqueue(_this->event_new(master::test_enqueue_1_multiple), 1);
      _this->event_enqueue(_this->event_new(master::test_enqueue_1_multiple), 1);
      _this->event_enqueue(_this->event_new(master::test_enqueue_1_multiple), 1);
      break;
    case 3:
      printf("Benchmarking event enqueue with 10 cycle constant\n");
      _this->event = _this->event_new(master::test_enqueue_10);
      _this->event_enqueue(_this->event, 1);
      break;
    case 4:
      printf("Benchmarking event enqueue with 100 cycle constant\n");
      _this->event = _this->event_new(master::test_enqueue_100);
      _this->event_enqueue(_this->event, 1);
      break;
    case 5:
      printf("Benchmarking event enqueue with 1 cycle variable\n");
      _this->delay = 1;
      _this->event = _this->event_new(master::test_enqueue_var);
      _this->event_enqueue(_this->event, 1);
      break;
    case 6:
      printf("Benchmarking interface calls with no argument\n");
      _this->event = _this->event_new(master::test_call);
      _this->event_enqueue(_this->event, 1);
      break;
    case 7:
      printf("Benchmarking interface calls with synchronous io req\n");
      _this->event = _this->event_new(master::test_call_sync);
      _this->event_enqueue(_this->event, 1);
      break;
    default:
      exit(0);
  }

  _this->step++;
}

void master::resp(void *_this, vp::io_req *req)
{
}

int master::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);


  out.set_resp_meth(&master::resp);

  new_master_port("out", &out);

  return 0;
}

void master::start()
{
  step = 0;
  event_enqueue(event_new(master::test), 1);
}


master::master(js::config *config)
: vp::component(config)
{
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new master(config);
}
