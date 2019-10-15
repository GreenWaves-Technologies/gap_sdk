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

#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <vp/vp.hpp>
#include <stdio.h>
#include "string.h"
#include <iostream>
#include <sstream>
#include <string>

#ifdef __VP_USE_SYSTEMC
#include <systemc.h>
#endif

char vp_error[VP_ERROR_SIZE];

vp::component::component(const char *config_string) : traces(*this), power(*this), reset_done_from_itf(false)
{
  this->set_config(config_string);
}


void vp::component::set_config(const char *config_string)
{
  comp_js_config = js::import_config_from_string(strdup(config_string));
}

void vp::component::reg_step_pre_start(std::function<void()> callback)
{
  this->pre_start_callbacks.push_back(callback);
}

void vp::component::post_post_build()
{
  traces.post_post_build();
  power.post_post_build();
}



void vp::component_clock::clk_reg(component *_this, component *clock)
{
  _this->clock = (clock_engine *)clock;
  for (auto& x: _this->childs)
  {
    x->clk_reg(x, clock);
  }
}

void vp::component::reset_all(bool active, bool from_itf)
{
  // Small hack to not propagate the reset from top level if the reset has
  // already been done through the interface from another component.
  // This should be all implemented with interfaces to better control
  // the reset propagation.
  this->reset_done_from_itf |= from_itf;

  if (from_itf || !this->reset_done_from_itf)
  {
    this->get_trace()->msg("Reset\n");

    this->pre_reset();
    
    for (auto reg: this->regs)
    {
      reg->reset(active);
    }

    this->reset(active);
    
    for (auto& x: this->childs)
    {
      x->reset_all(active);
    }
  }
}

void vp::component_clock::reset_sync(void *__this, bool active)
{
  component *_this = (component *)__this;
  _this->reset_done_from_itf = true;
  _this->reset_all(active, true);
}


void vp::component_clock::pre_build(component *comp) {
  clock_port.set_reg_meth((vp::clk_reg_meth_t *)&component_clock::clk_reg);
  comp->new_slave_port("clock", &clock_port);

  reset_port.set_sync_meth(&component_clock::reset_sync);
  comp->new_slave_port("reset", &reset_port);

  comp->traces.new_trace("comp", comp->get_trace(), vp::DEBUG);
  comp->traces.new_trace("warning", &comp->warning, vp::WARNING);
}

bool vp::time_engine::dequeue(time_engine_client *client)
{
  if (!client->is_enqueued) return false;

  client->is_enqueued = false;

  time_engine_client *current = this->first_client, *prev = NULL;
  while (current && current != client)
  {
    prev = current;
    current = current->next;
  }
  if (prev)
    prev->next = client->next;
  else
    this->first_client = client->next;

  return true;
}

bool vp::time_engine::enqueue(time_engine_client *client, int64_t time)
{
  vp_assert(time >= 0, NULL, "Time must be positive\n");

  int64_t full_time = this->get_time() + time;

#ifdef __VP_USE_SYSTEMC
  // Notify to the engine that something has been pushed in case it is done
  // by an external systemC component and the engine needs to be waken up
  if (started) sync_event.notify();
#endif

  if (client->is_running())
    return false;

  if (client->is_enqueued) 
  {
    if (client->next_event_time <= full_time)
      return false;
    this->dequeue(client);
  }

  client->is_enqueued = true;

  time_engine_client *current = first_client, *prev = NULL;
  client->next_event_time = full_time;
  while (current && current->next_event_time < client->next_event_time)
  {
    prev = current;
    current = current->next;
  }
  if (prev) prev->next = client;
  else first_client = client;
  client->next = current;

  return true;
}

bool vp::clock_engine::dequeue_from_engine()
{
  if (this->is_running() || !this->is_enqueued)
    return false;

  this->engine->dequeue(this);

  return true;
}

void vp::clock_engine::reenqueue_to_engine()
{
  this->engine->enqueue(this, this->next_event_time);
}

void vp::clock_engine::apply_frequency(int frequency)
{
  if (frequency > 0)
  {
    bool reenqueue = this->dequeue_from_engine();
    int64_t period = this->period;

    this->freq = frequency;
    this->period = 1e12 / this->freq;
    if (reenqueue && period > 0)
    {
      int64_t cycles = (this->next_event_time - this->get_time()) / period;
      this->next_event_time = cycles*this->period;
      this->reenqueue_to_engine();
    }
    else if (period == 0)
    {
      // Case where the clock engine was clock-gated
      // We need to reenqueue the engine in case it has pending events
      if (this->has_events())
      {
        // Compute the time of the next event based on the new frequency
        this->next_event_time = (this->get_next_event()->get_cycle() - this->get_cycles())*this->period;

        this->reenqueue_to_engine();
      }
    }
  }
  else if (frequency == 0)
  {
    this->dequeue_from_engine();
    this->period = 0;
  }
}


void vp::clock_engine::update()
{
  if (this->period == 0)
    return;

  int64_t diff = this->get_time() - this->stop_time;

#ifdef __VP_USE_SYSTEMC
  if ((int64_t)sc_time_stamp().to_double() > this->get_time())
    diff = (int64_t)sc_time_stamp().to_double() - this->stop_time;

  engine->update((int64_t)sc_time_stamp().to_double());
#endif

  if (diff > 0)
  {
    int64_t cycles = (diff + this->period - 1) / this->period;
    this->stop_time += cycles * this->period;
    this->cycles += cycles;
  }
}

vp::clock_event *vp::clock_engine::enqueue_other(vp::clock_event *event, int64_t cycle)
{
  // Slow case where the engine is not running or we must enqueue out of the
  // circular buffer.

  // First check if we have to enqueue it to the global time engine in case we
  // were not running.

  // Check if we can enqueue to the fast circular queue in case were not
  // running.
  bool can_enqueue_to_cycle = false;
  if (!this->is_running())
  {
    //this->current_cycle = (this->get_cycles() + 1) & CLOCK_EVENT_QUEUE_MASK;
    //can_enqueue_to_cycle = this->current_cycle + cycle - 1 < CLOCK_EVENT_QUEUE_SIZE;
  }

  if (can_enqueue_to_cycle)
  {
    //this->current_cycle = (this->get_cycles() + cycle) & CLOCK_EVENT_QUEUE_MASK;
    this->enqueue_to_cycle(event, cycle - 1);
    if (this->period != 0)
      enqueue_to_engine(period);
  }
  else
  {
    this->must_flush_delayed_queue = true;
    if (this->period != 0)
      enqueue_to_engine(cycle*period);

    vp::clock_event *current = delayed_queue, *prev = NULL;
    int64_t full_cycle = cycle + get_cycles();
    while (current && current->cycle < full_cycle)
    {
      prev = current;
      current = current->next;
    }
    if (prev) prev->next = event;
    else delayed_queue = event;
    event->next = current;
    event->cycle = full_cycle;
  }
  return event;
}

vp::clock_event *vp::clock_engine::get_next_event()
{
  // There is no quick way of getting the next event.
  // We have to first check if there is an event in the circular buffer
  // and if not in the delayed queue

  if (this->nb_enqueued_to_cycle)
  {
    for (int i=0; i<CLOCK_EVENT_QUEUE_SIZE; i++)
    {
      int cycle = (current_cycle + i) & CLOCK_EVENT_QUEUE_MASK;
      vp::clock_event *event = event_queue[cycle];
      if (event)
      {
        return event;
      }
    }
    vp_assert(false, 0, "Didn't find any event in circular buffer while it is not empty\n");
  }

  return this->delayed_queue;
}

void vp::clock_engine::cancel(vp::clock_event *event)
{
  if (!event->is_enqueued())
    return;

  // There is no way to know if the event is enqueued into the circular buffer
  // or in the delayed queue so first go through the delayed queue and if it is
  // not found, look in the circular buffer

  // First the delayed queue
  vp::clock_event *current = delayed_queue, *prev = NULL;
  while (current)
  {
    if (current == event)
    {
      if (prev)
        prev->next = event->next;
      else
        delayed_queue = event->next;

      goto end;
    }

    prev = current;
    current = current->next;
  }

  // Then in the circular buffer
  for (int i=0; i<CLOCK_EVENT_QUEUE_SIZE; i++)
  {
    vp::clock_event *current = event_queue[i], *prev = NULL;
    while (current)
    {
      if (current == event)
      {
        if (prev)
          prev->next = event->next;
        else
          event_queue[i] = event->next;

        this->nb_enqueued_to_cycle--;

        goto end;
      }

      prev = current;
      current = current->next;
    }
  }

  vp_assert(0, NULL, "Didn't find event in any queue while canceling event\n");

end:
  event->enqueued = false;

  if (!this->has_events())
    this->dequeue_from_engine();
}

void vp::clock_engine::flush_delayed_queue()
{
  clock_event *event = delayed_queue;
  this->must_flush_delayed_queue = false;
  while (event)
  {
    if (nb_enqueued_to_cycle == 0) cycles = event->cycle;

    uint64_t cycle_diff = event->cycle - get_cycles();
    if (cycle_diff >= CLOCK_EVENT_QUEUE_SIZE) break;

    clock_event *next = event->next;

    enqueue_to_cycle(event, cycle_diff);

    event = next;
    delayed_queue = event;
  }
}

int64_t vp::clock_engine::exec()
{
  vp_assert(this->has_events(), NULL, "Executing clock engine while it has no event\n");
  vp_assert(this->get_next_event(), NULL, "Executing clock engine while it has no next event\n");

  this->cycles_trace.event_real(this->cycles);

  // The clock engine has a circular buffer of events to be executed.
  // Events longer than the buffer as put temporarly in a queue.
  // Everytime we start again at the beginning of the buffer, we need
  // to check if events must be enqueued from the queue to the buffer
  // in case they fit the window.
  if (unlikely(this->must_flush_delayed_queue))
  {
    this->flush_delayed_queue();
  }

  vp_assert(this->get_next_event(), NULL, "Executing clock engine while it has no next event\n");

  // Now take all events available at the current cycle and execute them all without returning
  // to the main engine to execute them faster. 
  clock_event *current = event_queue[current_cycle];

  while (likely(current != NULL))
  {
    event_queue[current_cycle] = current->next;
    current->enqueued = false;
    nb_enqueued_to_cycle--;

    current->meth(current->_this, current);
    current = event_queue[current_cycle];
  }

  // Now we need to tell the time engine when is the next event.
  // The most likely is that there is an event in the circular buffer, 
  // in which case we just return the clock period, as we will go through
  // each element of the circular buffer, even if the next event is further in
  // the buffer.
  if (likely(nb_enqueued_to_cycle))
  {
    cycles++;
    current_cycle = (current_cycle + 1) & CLOCK_EVENT_QUEUE_MASK;
    if (unlikely(current_cycle == 0))
      this->must_flush_delayed_queue = true;

    return period;
  }
  else
  {
    // Otherwise if there is an event in the delayed queue, return the time
    // to this event.
    // In both cases, force the delayed queue flush so that the next event to be
    // executed is moved to the circular buffer.
    this->must_flush_delayed_queue = true;

    // Also remember the current time in order to resynchronize the clock engine
    // in case we enqueue and event from another engine.
    this->stop_time = this->get_time();

    if (delayed_queue)
    {
      return (delayed_queue->cycle - get_cycles()) * period;
    }
    else
    {
      // In case there is no more event to execute, returns -1 to tell the time
      // engine we are done.
      return -1;
    }
  }
}


vp::clock_event::clock_event(component_clock *comp, clock_event_meth_t *meth) 
: comp(comp), _this((void *)static_cast<vp::component *>((vp::component_clock *)(comp))), meth(meth), enqueued(false)
{

}

vp::time_engine *vp::component::get_time_engine()
{
  if (this->time_engine_ptr == NULL)
  {
    this->time_engine_ptr = this->parent->get_time_engine();
  }

  return this->time_engine_ptr;
}


void vp::component::new_master_port(std::string name, vp::master_port *port)
{
  port->set_owner(this);
  port->set_context(this);
  master_ports[name] = port;
}

void vp::component::new_master_port(void *comp, std::string name, vp::master_port *port)
{
  port->set_owner(this);
  port->set_context(comp);
  master_ports[name] = port;
}

void vp::component::new_slave_port(std::string name, vp::slave_port *port)
{
  port->set_owner(this);
  port->set_context(this);
  slave_ports[name] = port;
}

void vp::component::new_slave_port(void *comp, std::string name, vp::slave_port *port)
{
  port->set_owner(this);
  port->set_context(comp);
  slave_ports[name] = port;
}

void vp::component::new_service(std::string name, void *service)
{
  services[name] = service;
}


template<typename P> int vp::component::get_ports(std::map<std::string, P *> ports_map,
  int size, const char *names[], void *ports[])
{
  if (size != 0)
  {   
    int i = 0;    
    for (auto& x: ports_map) {
      names[i] = strdup(x.first.c_str());
      ports[i] = x.second;
      i++;
    }
  }
  return ports_map.size();
}

int vp::component::get_ports(bool master, int size, const char *names[], void *ports[])
{
  if (master)
  {
    return get_ports<vp::master_port>(master_ports, size, names, ports);
  }
  else
  {
    return get_ports<vp::slave_port>(slave_ports, size, names, ports);
  }
}

void *vp::component::get_service(string name)
{
  return all_services[name];
}

int vp::component::get_services(int size, const char *names[], void *res_services[])
{
  if (size != 0)
  {   
    int i = 0;    
    for (auto& x: services) {
      names[i] = x.first.c_str();
      res_services[i] = x.second;
      i++;
    }
  }
  return services.size();
}

std::vector<std::string> split_name(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

vp::config *vp::config::create_config(jsmntok_t *tokens, int *_size)
{
  jsmntok_t *current = tokens;
  config *config = NULL;

  switch (current->type)
  {
    case JSMN_PRIMITIVE:
      if (strcmp(current->str, "True") == 0 || strcmp(current->str, "False") == 0 || strcmp(current->str, "true") == 0 || strcmp(current->str, "false") == 0)
      {
        config = new config_bool(current);
      }
      else
      {
        config = new config_number(current);
      }
      current++;
      break;

    case JSMN_OBJECT: {
      int size;
      config = new config_object(current, &size);
      current += size;
      break;
    }

    case JSMN_ARRAY: {
      int size;
      config = new config_array(current, &size);
      current += size;
      break;
    }

    case JSMN_STRING:
      config = new config_string(current);
      current++;
      break;

    case JSMN_UNDEFINED:
      break;
  }

  if (_size) {
    *_size = current - tokens;
  }

  return config;
}

vp::config *vp::config_string::get_from_list(std::vector<std::string> name_list)
{
  if (name_list.size() == 0) return this;
  return NULL;
}

vp::config *vp::config_number::get_from_list(std::vector<std::string> name_list)
{
  if (name_list.size() == 0) return this;
  return NULL;
}

vp::config *vp::config_bool::get_from_list(std::vector<std::string> name_list)
{
  if (name_list.size() == 0) return this;
  return NULL;
}

vp::config *vp::config_array::get_from_list(std::vector<std::string> name_list)
{
  if (name_list.size() == 0) return this;
  return NULL;
}

vp::config *vp::config_object::get_from_list(std::vector<std::string> name_list)
{
  if (name_list.size() == 0) return this;

  vp::config *result = NULL;
  std::string name;
  int name_pos = 0;

  for (auto& x: name_list) {
    if (x != "*" && x != "**")
    {
      name = x;
      break;
    }
    name_pos++;
  }

  for (auto& x: childs) {

    if (name == x.first)
    {
      result = x.second->get_from_list(std::vector<std::string>(name_list.begin () + name_pos + 1, name_list.begin () + name_list.size()));
      if (name_pos == 0 || result != NULL) return result;

    }
    else if (name_list[0] == "*")
    {
      result = x.second->get_from_list(std::vector<std::string>(name_list.begin () + 1, name_list.begin () + name_list.size()));
      if (result != NULL) return result;
    }
    else if (name_list[0] == "**")
    {
      result = x.second->get_from_list(name_list);
      if (result != NULL) return result;
    }
  }

  return result;
}

vp::config *vp::config_object::get(std::string name)
{
  return get_from_list(split_name(name, '/'));
}

vp::config_string::config_string(jsmntok_t *tokens)
{
  value = tokens->str;
}

vp::config_number::config_number(jsmntok_t *tokens)
{
  value = atof(tokens->str);
}

vp::config_bool::config_bool(jsmntok_t *tokens)
{
  value = strcmp(tokens->str, "True") == 0 || strcmp(tokens->str, "true") == 0;
}

vp::config_array::config_array(jsmntok_t *tokens, int *_size)
{
  jsmntok_t *current = tokens;
  jsmntok_t *top = current++;
  
  for (int i=0; i<top->size; i++)
  {
    int child_size;
    elems.push_back(create_config(current, &child_size));
    current += child_size;
  }


  if (_size) {
    *_size = current - tokens;
  }
}

vp::config_object::config_object(jsmntok_t *tokens, int *_size)
{
  jsmntok_t *current = tokens;
  jsmntok_t *t = current++;

  for (int i=0; i<t->size; i++)
  {
    jsmntok_t *child_name = current++;
    int child_size;
    config *child_config = create_config(current, &child_size);
    current += child_size;

    if (child_config != NULL)
    {
      childs[child_name->str] = child_config;

    }
  }

  if (_size) {
    *_size = current - tokens;
  }
}

vp::config *vp::component::import_config(const char *config_string)
{
  if (config_string == NULL) return NULL;

  jsmn_parser parser;
  
  jsmn_init(&parser);
  int nb_tokens = jsmn_parse(&parser, config_string, strlen(config_string), NULL, 0);

  jsmntok_t tokens[nb_tokens];

  jsmn_init(&parser);
  nb_tokens = jsmn_parse(&parser, config_string, strlen(config_string), tokens, nb_tokens);

  char *str = strdup(config_string);
  for (int i=0; i<nb_tokens; i++)
  {
    jsmntok_t *tok = &tokens[i];
    tok->str = &str[tok->start];
    str[tok->end] = 0;
  }



  return new vp::config_object(tokens);
}

void vp::component::set_services(int nb_services, const char *names[], void *services[])
{
  for (int i=0; i<nb_services; i++)
  {
    all_services[names[i]] = services[i];
  }
}

void vp::component::conf(string path, vp::component *parent)
{
  this->parent = parent;
  this->path = path;
  if (parent != NULL)
  {
    parent->add_child(this);
  }
}

void vp::component::add_child(vp::component *child)
{
  this->childs.push_back(child);
}


void vp::component::elab()
{
  for (auto& x: this->childs)
  {
    x->elab();
  }
}

void vp::component::pre_start_all()
{
  this->pre_start();
  for (auto x: pre_start_callbacks)
  {
    x();
  }
}

void vp::component::new_reg_any(std::string name, vp::reg *reg, int bits, uint8_t *reset_val)
{
  reg->init(this, name, bits, NULL, reset_val);
  this->regs.push_back(reg);
}

void vp::component::new_reg(std::string name, vp::reg_1 *reg, uint8_t reset_val, bool reset)
{
  reg->init(this, name, reset ? (uint8_t *)&reset_val : NULL);
  this->regs.push_back(reg);
}

void vp::component::new_reg(std::string name, vp::reg_8 *reg, uint8_t reset_val, bool reset)
{
  reg->init(this, name, reset ? (uint8_t *)&reset_val : NULL);
  this->regs.push_back(reg);
}

void vp::component::new_reg(std::string name, vp::reg_16 *reg, uint16_t reset_val, bool reset)
{
  reg->init(this, name, reset ? (uint8_t *)&reset_val : NULL);
  this->regs.push_back(reg);
}

void vp::component::new_reg(std::string name, vp::reg_32 *reg, uint32_t reset_val, bool reset)
{
  reg->init(this, name, reset ? (uint8_t *)&reset_val : NULL);
  this->regs.push_back(reg);
}

void vp::component::new_reg(std::string name, vp::reg_64 *reg, uint64_t reset_val, bool reset)
{
  reg->init(this, name, reset ? (uint8_t *)&reset_val : NULL);
  this->regs.push_back(reg);
}


void vp::reg::init(vp::component *top, std::string name, int bits, uint8_t *value, uint8_t *reset_value)
{
  this->top = top;
  this->nb_bytes = (bits + 7) / 8;
  this->bits = bits;
  if (reset_value)
    this->reset_value_bytes = new uint8_t[this->nb_bytes];
  else
    this->reset_value_bytes = NULL;
  if (value)
    this->value_bytes = value;
  else
    this->value_bytes = new uint8_t[this->nb_bytes];
  this->name = name;
  if (reset_value)
    memcpy((void *)this->reset_value_bytes, (void *)reset_value, this->nb_bytes);
  top->traces.new_trace(name, &this->trace, vp::TRACE);
  top->traces.new_trace_event(name + "/vcd", &this->reg_event, bits);
}

void vp::reg::reset(bool active)
{
  if (active)
  {
    if (this->reset_value_bytes)
    {
      this->trace.msg("Resetting register\n");
      memcpy((void *)this->value_bytes, (void *)this->reset_value_bytes, this->nb_bytes);
    }
  }
}

void vp::reg_1::init(vp::component *top, std::string name, uint8_t *reset_val)
{
  reg::init(top, name, 1, (uint8_t *)&this->value, reset_val);
}

void vp::reg_8::init(vp::component *top, std::string name, uint8_t *reset_val)
{
  reg::init(top, name, 8, (uint8_t *)&this->value, reset_val);
}

void vp::reg_16::init(vp::component *top, std::string name, uint8_t *reset_val)
{
  reg::init(top, name, 16, (uint8_t *)&this->value, reset_val);
}

void vp::reg_32::init(vp::component *top, std::string name, uint8_t *reset_val)
{
  reg::init(top, name, 32, (uint8_t *)&this->value, reset_val);
}

void vp::reg_64::init(vp::component *top, std::string name, uint8_t *reset_val)
{
  reg::init(top, name, 64, (uint8_t *)&this->value, reset_val);
}


extern "C" int vp_comp_get_ports(void *comp, bool master, int size, const char *names[], void *ports[])
{
  return ((vp::component *)comp)->get_ports(master, size, names, ports);
}

extern "C" char *vp_get_error()
{
  return vp_error;
}

extern "C" int vp_comp_get_services(void *comp, int size, const char *names[], void *services[])
{
  return ((vp::component *)comp)->get_services(size, names, services);
}

extern "C" void vp_comp_set_services(void *comp, int nb_services, const char *names[], void *services[])
{
  ((vp::component *)comp)->set_services(nb_services, names, services);
}

extern "C" void vp_port_bind_to(void *_master, void *_slave, const char *config_str)
{
  vp::master_port *master = (vp::master_port *)_master;
  vp::slave_port *slave = (vp::slave_port *)_slave;

  vp::config *config = NULL;
  if (config_str != NULL)
  {
    config = master->get_comp()->import_config(config_str);
  }

  master->bind_to(slave, config);
  slave->bind_to(master, config);
}

extern "C" void vp_port_finalize(void *_master)
{
  vp::master_port *master = (vp::master_port *)_master;
  master->finalize();
}

extern "C" void vp_comp_set_config(void *comp, const char *config)
{
  ((vp::component *)comp)->set_config(config);
}

extern "C" void vp_comp_conf(void *comp, const char *path, void *parent)
{
  ((vp::component *)comp)->conf(path, (vp::component *)parent);
}

extern "C" void vp_pre_start(void *comp)
{
  ((vp::component *)comp)->pre_start_all();
}

extern "C" void vp_load(void *comp)
{
  ((vp::component *)comp)->load();
}

extern "C" void vp_start(void *comp)
{
  ((vp::component *)comp)->start();
}

extern "C" void vp_reset(void *comp, int active)
{
  ((vp::component *)comp)->reset_all(active);
}

extern "C" void vp_stop(void *comp)
{
  ((vp::component *)comp)->stop();
}

extern "C" const char *vp_run(void *comp)
{
  return ((vp::component *)comp)->run().c_str();
}

extern "C" int vp_run_status(void *comp)
{
  return ((vp::component *)comp)->run_status();
}

extern "C" void vp_post_post_build(void *comp)
{
  ((vp::component *)comp)->post_post_build();
}


extern "C" int vp_build(void *comp)
{
  ((vp::component *)comp)->pre_build();
  return ((vp::component *)comp)->build();
}

#ifdef __VP_USE_SYSTEMC

static void *(*sc_entry)(void *);
static void *sc_entry_arg;

void set_sc_main_entry(void *(*entry)(void *), void *arg)
{
  sc_entry = entry;
  sc_entry_arg = arg;
}

int sc_main(int argc, char* argv[])
{
  sc_entry(sc_entry_arg);
  return 0;
}
#endif
