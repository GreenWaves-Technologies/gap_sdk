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
#include <dlfcn.h>
#include <algorithm>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <regex>
#include <gv/gvsoc_proxy.hpp>
#include <gv/gvsoc.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/prctl.h>


extern "C" long long int dpi_time_ps();


#ifdef __VP_USE_SYSTEMC
#include <systemc.h>
#endif


#ifdef __VP_USE_SYSTEMV
extern "C" void dpi_raise_event();
#endif

char vp_error[VP_ERROR_SIZE];



class Gv_proxy
{
  public:
    Gv_proxy(vp::component *top, int req_pipe, int reply_pipe): top(top), req_pipe(req_pipe), reply_pipe(reply_pipe) {}
    int open(int port, int *out_port);
    void stop();
    
  private:
 

    void listener(void);
    void proxy_loop(int);
    
    int telnet_socket;
    int socket_port;
    
    std::thread *loop_thread;
    std::thread *listener_thread;

    std::vector<int> sockets;

    vp::component *top;
    int req_pipe;
    int reply_pipe;
};


static Gv_proxy *proxy = NULL;


uint64_t vp::reg::get_field(int offset, int width)
{
    uint64_t value = 0;
    this->read(0, (offset + width + 7)/8, (uint8_t *)&value);
    return (value >> offset) & ((1UL<<width)-1);
}



bool vp::regmap::access(uint64_t offset, int size, uint8_t *value, bool is_write)
{
    for (auto x: this->get_registers())
    {
        if (offset >= x->offset && offset + size <= x->offset + (x->width+7)/8)
        {
            vp::reg *aliased_reg = x;

            if (x->alias)
            {
                x = x->alias();
            }

            x->access((offset - aliased_reg->offset), size, value, is_write);
            
            if (aliased_reg->trace.get_active(vp::trace::LEVEL_DEBUG))
            {
                std::string regfields_values = "";

                if (aliased_reg->regfields.size() != 0)
                {
                    for (auto y: aliased_reg->regfields)
                    {
                        char buff[256];
                        snprintf(buff, 256, "0x%lx", x->get_field(y->bit, y->width));

                        if (regfields_values != "")
                            regfields_values += ", ";

                        regfields_values += y->name + "=" + std::string(buff);
                    }
                    
                    regfields_values = "{ " + regfields_values + " }";
                }
                else
                {
                    char buff[256];
                    snprintf(buff, 256, "0x%lx", x->get_field(0, aliased_reg->width));
                    regfields_values = std::string(buff);
                }

                aliased_reg->trace.msg(vp::trace::LEVEL_DEBUG,
                    "Register access (name: %s, offset: 0x%x, size: 0x%x, is_write: 0x%x, value: %s)\n",
                    aliased_reg->get_name().c_str(), offset, size, is_write, regfields_values.c_str()
                );
            }

            return false;
        }
    }

    vp_warning_always(this->trace, "Accessing invalid register (offset: 0x%lx, size: 0x%x, is_write: %d)\n", offset, size, is_write);
    return true;
}



void vp::regmap::build(vp::component *comp, vp::trace *trace, std::string name)
{
    this->comp = comp;
    this->trace = trace;

    for (auto x: this->get_registers())
    {
        std::string reg_name = name;
        if (reg_name == "")
            reg_name = x->get_hw_name();
        else
            reg_name = reg_name + "/" + x->get_hw_name();

        x->build(comp, reg_name);
    }
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

void vp::component::final_bind()
{
    for (auto port : this->slave_ports)
    {
        port.second->final_bind();
    }

    for (auto port : this->master_ports)
    {
        port.second->final_bind();
    }

    for (auto &x : this->childs)
    {
        x->final_bind();
    }
}



void vp::component::set_vp_config(js::config *config)
{
    this->vp_config = config;
}



void vp::component::set_gv_conf(struct gv_conf *gv_conf)
{
    memcpy(&this->gv_conf, gv_conf, sizeof(struct gv_conf));
}



js::config *vp::component::get_vp_config()
{
    if (this->vp_config == NULL)
    {
        if (this->parent != NULL)
        {
            this->vp_config = this->parent->get_vp_config();
        }
    }

    vp_assert_always(this->vp_config != NULL, NULL, "No VP config found\n");

    return this->vp_config;
}



void vp::component::load_all()
{
    for (auto &x : this->childs)
    {
        x->load_all();
    }

    this->load();
}



int vp::component::build_new()
{
    this->bind_comps();

    this->post_post_build_all();

    this->pre_start_all();

    this->start_all();

    this->final_bind();

    this->reset_all(true);

    if (!this->get_js_config()->get_child_bool("**/gvsoc/use_external_bridge"))
    {
        this->reset_all(false);
    }

    return 0;
}



void vp::component::post_post_build_all()
{
    for (auto &x : this->childs)
    {
        x->post_post_build_all();
    }

    this->post_post_build();
}



void vp::component::start_all()
{
    for (auto &x : this->childs)
    {
        x->start_all();
    }

    this->start();
}



void vp::component::flush_all()
{
    for (auto &x : this->childs)
    {
        x->flush_all();
    }

    this->flush();
}



void vp::component::pre_start_all()
{
    for (auto &x : this->childs)
    {
        x->pre_start_all();
    }

    this->pre_start();
    for (auto x : pre_start_callbacks)
    {
        x();
    }
}



void vp::component_clock::clk_reg(component *_this, component *clock)
{
    _this->clock = (clock_engine *)clock;
    for (auto &x : _this->childs)
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

        for (auto reg : this->regs)
        {
            reg->reset(active);
        }

        this->reset(active);

        for (auto &x : this->childs)
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

void vp::component_clock::pre_build(component *comp)
{
    clock_port.set_reg_meth((vp::clk_reg_meth_t *)&component_clock::clk_reg);
    comp->new_slave_port("clock", &clock_port);

    reset_port.set_sync_meth(&component_clock::reset_sync);
    comp->new_slave_port("reset", &reset_port);

    comp->traces.new_trace("comp", comp->get_trace(), vp::DEBUG);
    comp->traces.new_trace("warning", &comp->warning, vp::WARNING);
}


int64_t vp::time_engine::get_next_event_time()
{
    if (this->first_client)
    {
        return this->first_client->next_event_time;
    }

    return this->time;
}


bool vp::time_engine::dequeue(time_engine_client *client)
{
    if (!client->is_enqueued)
        return false;

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
    if (started)
        sync_event.notify();
#endif

#ifdef __VP_USE_SYSTEMV
    dpi_raise_event();
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
    if (prev)
        prev->next = client;
    else
        first_client = client;
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
            this->next_event_time = cycles * this->period;
            this->reenqueue_to_engine();
        }
        else if (period == 0)
        {
            // Case where the clock engine was clock-gated
            // We need to reenqueue the engine in case it has pending events
            if (this->has_events())
            {
                // Compute the time of the next event based on the new frequency
                this->next_event_time = (this->get_next_event()->get_cycle() - this->get_cycles()) * this->period;

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
            enqueue_to_engine(cycle * period);

        vp::clock_event *current = delayed_queue, *prev = NULL;
        int64_t full_cycle = cycle + get_cycles();
        while (current && current->cycle < full_cycle)
        {
            prev = current;
            current = current->next;
        }
        if (prev)
            prev->next = event;
        else
            delayed_queue = event;
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
        for (int i = 0; i < CLOCK_EVENT_QUEUE_SIZE; i++)
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
    for (int i = 0; i < CLOCK_EVENT_QUEUE_SIZE; i++)
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
        if (nb_enqueued_to_cycle == 0)
            cycles = event->cycle;

        uint64_t cycle_diff = event->cycle - get_cycles();
        if (cycle_diff >= CLOCK_EVENT_QUEUE_SIZE)
            break;

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
        this->time_engine_ptr = (vp::time_engine*)this->get_service("time");
    }

    return this->time_engine_ptr;
}



vp::master_port::master_port(vp::component *owner)
    : vp::port(owner)
{
}



void vp::component::new_master_port(std::string name, vp::master_port *port)
{
    this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "New master port (name: %s, port: %p)\n", name.c_str(), port);

    port->set_owner(this);
    port->set_context(this);
    port->set_name(name);
    this->add_master_port(name, port);
}



void vp::component::new_master_port(void *comp, std::string name, vp::master_port *port)
{
    this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "New master port (name: %s, port: %p)\n", name.c_str(), port);

    port->set_owner(this);
    port->set_context(comp);
    port->set_name(name);
    this->add_master_port(name, port);
}



void vp::component::add_slave_port(std::string name, vp::slave_port *port)
{
    vp_assert_always(port != NULL, this->get_trace(), "Adding NULL port\n");
    //vp_assert_always(this->slave_ports[name] == NULL, this->get_trace(), "Adding already existing port\n");
    this->slave_ports[name] = port;
}



void vp::component::add_master_port(std::string name, vp::master_port *port)
{
    vp_assert_always(port != NULL, this->get_trace(), "Adding NULL port\n");
    //vp_assert_always(this->master_ports[name] == NULL, this->get_trace(), "Adding already existing port\n");
    this->master_ports[name] = port;
}



void vp::component::new_slave_port(std::string name, vp::slave_port *port)
{
    this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "New slave port (name: %s, port: %p)\n", name.c_str(), port);

    port->set_owner(this);
    port->set_context(this);
    port->set_name(name);
    this->add_slave_port(name, port);
}



void vp::component::new_slave_port(void *comp, std::string name, vp::slave_port *port)
{
    this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "New slave port (name: %s, port: %p)\n", name.c_str(), port);

    port->set_owner(this);
    port->set_context(comp);
    port->set_name(name);
    this->add_slave_port(name, port);
}



void vp::component::add_service(std::string name, void *service)
{
    if (this->parent)
        this->parent->add_service(name, service);
    else if (all_services[name] == NULL)
        all_services[name] = service;
}



void vp::component::new_service(std::string name, void *service)
{
    this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "New service (name: %s, service: %p)\n", name.c_str(), service);

    if (this->parent)
        this->parent->add_service(name, service);

    all_services[name] = service;
}

void *vp::component::get_service(string name)
{
    if (all_services[name])
        return all_services[name];

    if (this->parent)
        return this->parent->get_service(name);

    return NULL;
}

std::vector<std::string> split_name(const std::string &s, char delimiter)
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

    case JSMN_OBJECT:
    {
        int size;
        config = new config_object(current, &size);
        current += size;
        break;
    }

    case JSMN_ARRAY:
    {
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

    if (_size)
    {
        *_size = current - tokens;
    }

    return config;
}

vp::config *vp::config_string::get_from_list(std::vector<std::string> name_list)
{
    if (name_list.size() == 0)
        return this;
    return NULL;
}

vp::config *vp::config_number::get_from_list(std::vector<std::string> name_list)
{
    if (name_list.size() == 0)
        return this;
    return NULL;
}

vp::config *vp::config_bool::get_from_list(std::vector<std::string> name_list)
{
    if (name_list.size() == 0)
        return this;
    return NULL;
}

vp::config *vp::config_array::get_from_list(std::vector<std::string> name_list)
{
    if (name_list.size() == 0)
        return this;
    return NULL;
}

vp::config *vp::config_object::get_from_list(std::vector<std::string> name_list)
{
    if (name_list.size() == 0)
        return this;

    vp::config *result = NULL;
    std::string name;
    int name_pos = 0;

    for (auto &x : name_list)
    {
        if (x != "*" && x != "**")
        {
            name = x;
            break;
        }
        name_pos++;
    }

    for (auto &x : childs)
    {

        if (name == x.first)
        {
            result = x.second->get_from_list(std::vector<std::string>(name_list.begin() + name_pos + 1, name_list.begin() + name_list.size()));
            if (name_pos == 0 || result != NULL)
                return result;
        }
        else if (name_list[0] == "*")
        {
            result = x.second->get_from_list(std::vector<std::string>(name_list.begin() + 1, name_list.begin() + name_list.size()));
            if (result != NULL)
                return result;
        }
        else if (name_list[0] == "**")
        {
            result = x.second->get_from_list(name_list);
            if (result != NULL)
                return result;
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

    for (int i = 0; i < top->size; i++)
    {
        int child_size;
        elems.push_back(create_config(current, &child_size));
        current += child_size;
    }

    if (_size)
    {
        *_size = current - tokens;
    }
}

vp::config_object::config_object(jsmntok_t *tokens, int *_size)
{
    jsmntok_t *current = tokens;
    jsmntok_t *t = current++;

    for (int i = 0; i < t->size; i++)
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

    if (_size)
    {
        *_size = current - tokens;
    }
}

vp::config *vp::component::import_config(const char *config_string)
{
    if (config_string == NULL)
        return NULL;

    jsmn_parser parser;

    jsmn_init(&parser);
    int nb_tokens = jsmn_parse(&parser, config_string, strlen(config_string), NULL, 0);

    jsmntok_t tokens[nb_tokens];

    jsmn_init(&parser);
    nb_tokens = jsmn_parse(&parser, config_string, strlen(config_string), tokens, nb_tokens);

    char *str = strdup(config_string);
    for (int i = 0; i < nb_tokens; i++)
    {
        jsmntok_t *tok = &tokens[i];
        tok->str = &str[tok->start];
        str[tok->end] = 0;
    }

    return new vp::config_object(tokens);
}

void vp::component::conf(string name, string path, vp::component *parent)
{
    this->parent = parent;
    this->path = path;
    if (parent != NULL)
    {
        parent->add_child(name, this);
    }
}

void vp::component::add_child(std::string name, vp::component *child)
{
    this->childs.push_back(child);
    this->childs_dict[name] = child;
}

void vp::component::elab()
{
    for (auto &x : this->childs)
    {
        x->elab();
    }
}

void vp::component::new_reg_any(std::string name, vp::reg *reg, int bits, uint8_t *reset_val)
{
    reg->init(this, name, bits, NULL, reset_val);
    this->regs.push_back(reg);
}

void vp::component::new_reg(std::string name, vp::reg_1 *reg, uint8_t reset_val, bool reset)
{
    this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "New register (name: %s, width: %d, reset_val: 0x%x, reset: %d)\n",
        name.c_str(), 1, reset_val, reset
    );

    reg->init(this, name, reset ? (uint8_t *)&reset_val : NULL);
    this->regs.push_back(reg);
}

void vp::component::new_reg(std::string name, vp::reg_8 *reg, uint8_t reset_val, bool reset)
{
    this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "New register (name: %s, width: %d, reset_val: 0x%x, reset: %d)\n",
        name.c_str(), 8, reset_val, reset
    );

    reg->init(this, name, reset ? (uint8_t *)&reset_val : NULL);
    this->regs.push_back(reg);
}

void vp::component::new_reg(std::string name, vp::reg_16 *reg, uint16_t reset_val, bool reset)
{
    this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "New register (name: %s, width: %d, reset_val: 0x%x, reset: %d)\n",
        name.c_str(), 16, reset_val, reset
    );

    reg->init(this, name, reset ? (uint8_t *)&reset_val : NULL);
    this->regs.push_back(reg);
}

void vp::component::new_reg(std::string name, vp::reg_32 *reg, uint32_t reset_val, bool reset)
{
    this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "New register (name: %s, width: %d, reset_val: 0x%x, reset: %d)\n",
        name.c_str(), 32, reset_val, reset
    );

    reg->init(this, name, reset ? (uint8_t *)&reset_val : NULL);
    this->regs.push_back(reg);
}

void vp::component::new_reg(std::string name, vp::reg_64 *reg, uint64_t reset_val, bool reset)
{
    this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "New register (name: %s, width: %d, reset_val: 0x%x, reset: %d)\n",
        name.c_str(), 64, reset_val, reset
    );

    reg->init(this, name, reset ? (uint8_t *)&reset_val : NULL);
    this->regs.push_back(reg);
}

bool vp::reg::access_callback(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    if (this->callback != NULL)
        this->callback(reg_offset, size, value, is_write);

    return this->callback != NULL;
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

void vp::reg_1::build(vp::component *top, std::string name)
{
    top->new_reg(name, this, this->reset_val, this->do_reset);
}

void vp::reg_8::build(vp::component *top, std::string name)
{
    top->new_reg(name, this, this->reset_val, this->do_reset);
}

void vp::reg_16::build(vp::component *top, std::string name)
{
    top->new_reg(name, this, this->reset_val, this->do_reset);
}

void vp::reg_32::build(vp::component *top, std::string name)
{
    top->new_reg(name, this, this->reset_val, this->do_reset);
}

void vp::reg_64::build(vp::component *top, std::string name)
{
    top->new_reg(name, this, this->reset_val, this->do_reset);
}

void vp::master_port::final_bind()
{
    if (this->is_bound)
    {
        this->finalize();
    }
}

void vp::slave_port::final_bind()
{
    if (this->is_bound)
        this->finalize();
}

extern "C" char *vp_get_error()
{
    return vp_error;
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


void vp::component::throw_error(std::string error)
{
    throw std::invalid_argument("[\033[31m" + this->get_path() + "\033[0m] " + error);
}



vp::component *vp::component::new_component(std::string name, js::config *config, std::string module_name)
{
    if (module_name == "")
    {
        module_name = config->get_child_str("vp_component");
    }

    if (this->get_vp_config()->get_child_bool("sv-mode"))
    {
        module_name = "sv." + module_name;
    }
    else if (this->get_vp_config()->get_child_bool("debug-mode"))
    {
        module_name = "debug." + module_name;
    }

    this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "New component (name: %s, module: %s)\n", name.c_str(), module_name.c_str());

    std::replace(module_name.begin(), module_name.end(), '.', '/');

    std::string path = std::string(getenv("GVSOC_PATH")) + "/" + module_name + ".so";

    void *module = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND);
    if (module == NULL)
    {
        this->throw_error("ERROR, Failed to open periph model (module: " + module_name + ", error: " + std::string(dlerror()) + ")");
    }

    vp::component *(*constructor)(js::config *) = (vp::component * (*)(js::config *)) dlsym(module, "vp_constructor");
    if (constructor == NULL)
    {
        this->throw_error("ERROR, couldn't find vp_constructor in loaded module (module: " + module_name + ")");
    }

    vp::component *instance = constructor(config);

    std::string comp_path = this->get_path() != "" ? this->get_path() + "/" + name : name == "" ? "" : "/" + name;

    instance->conf(name, comp_path, this);
    instance->pre_pre_build();
    instance->pre_build();
    instance->build();

    return instance;
}

vp::component::component(js::config *config)
    : traces(*this), power(*this), reset_done_from_itf(false)
{
    this->comp_js_config = config;

    //this->conf(path, parent);
}

void vp::component::create_ports()
{
    js::config *config = this->get_js_config();
    js::config *ports = config->get("vp_ports");

    if (ports != NULL)
    {
        this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Creating ports\n");

        for (auto x : ports->get_elems())
        {
            std::string port_name = x->get_str();

            this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Creating port (name: %s)\n", port_name.c_str());

            if (this->get_master_port(port_name) == NULL && this->get_slave_port(port_name) == NULL)
                this->add_master_port(port_name, new vp::virtual_port(this));
        }
    }
}

void vp::component::create_bindings()
{
    js::config *config = this->get_js_config();
    js::config *bindings = config->get("vp_bindings");

    if (bindings != NULL)
    {
        this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Creating bindings\n");

        for (auto x : bindings->get_elems())
        {
            std::string master_binding = x->get_elem(0)->get_str();
            std::string slave_binding = x->get_elem(1)->get_str();
            int pos = master_binding.find_first_of("->");
            std::string master_comp_name = master_binding.substr(0, pos);
            std::string master_port_name = master_binding.substr(pos + 2);
            pos = slave_binding.find_first_of("->");
            std::string slave_comp_name = slave_binding.substr(0, pos);
            std::string slave_port_name = slave_binding.substr(pos + 2);

            this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Creating binding (%s:%s -> %s:%s)\n",
                master_comp_name.c_str(), master_port_name.c_str(),
                slave_comp_name.c_str(), slave_port_name.c_str()
            );

            vp::component *master_comp = master_comp_name == "self" ? this : this->get_childs_dict()[master_comp_name];
            vp::component *slave_comp = slave_comp_name == "self" ? this : this->get_childs_dict()[slave_comp_name];

            vp_assert_always(master_comp != NULL, this->get_trace(), "Binding from invalid master\n");
            vp_assert_always(slave_comp != NULL, this->get_trace(), "Binding from invalid slave\n");

            vp::port *master_port = master_comp->get_master_port(master_port_name);
            vp::port *slave_port = slave_comp->get_slave_port(slave_port_name);

            vp_assert_always(master_port != NULL, this->get_trace(), "Binding from invalid master port\n");
            vp_assert_always(slave_port != NULL, this->get_trace(), "Binding from invalid slave port\n");

            master_port->bind_to_virtual(slave_port);
        }
    }
}



std::vector<vp::slave_port *> vp::slave_port::get_final_ports()
{
    return { this };
}



std::vector<vp::slave_port *> vp::master_port::get_final_ports()
{
    std::vector<vp::slave_port *> result;

    for (auto x : this->slave_ports)
    {
        std::vector<vp::slave_port *> slave_ports = x->get_final_ports();
        result.insert(result.end(), slave_ports.begin(), slave_ports.end());
    }

    return result;
}



void vp::master_port::bind_to_slaves()
{
    for (auto x : this->slave_ports)
    {
        for (auto y : x->get_final_ports())
        {
            this->get_owner()->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Creating final binding (%s:%s -> %s:%s)\n",
                this->get_owner()->get_path().c_str(), this->get_name().c_str(),
                y->get_owner()->get_path().c_str(), y->get_name().c_str()
            );

            this->bind_to(y, NULL);
            y->bind_to(this, NULL);
        }
    }
}

void vp::component::bind_comps()
{
    this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Creating final bindings\n");

    for (auto x : this->get_childs())
    {
        x->bind_comps();
    }

    for (auto x : this->master_ports)
    {
        if (!x.second->is_virtual())
        {
            x.second->bind_to_slaves();
        }
    }
}


void *vp::component::external_bind(std::string name, int handle)
{
    for (auto &x : this->childs)
    {
        void *result = x->external_bind(name, handle);
        if (result != NULL)
            return result;
    }

    return NULL;
}


void vp::master_port::bind_to_virtual(vp::port *port)
{
    vp_assert_always(port != NULL, this->get_comp()->get_trace(), "Trying to bind master port to NULL\n");
    this->slave_ports.push_back(port);
}

void vp::component::create_comps()
{
    js::config *config = this->get_js_config();
    js::config *comps = config->get("vp_comps");

    if (comps != NULL)
    {
        this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Creating components\n");

        for (auto x : comps->get_elems())
        {
            std::string comp_name = x->get_str();

            js::config *comp_config = config->get(comp_name);

            std::string vp_component = comp_config->get_child_str("vp_component");

            if (vp_component == "")
                vp_component = "utils.composite_impl";

            this->new_component(comp_name, comp_config, vp_component);
        }
    }
}



void Gv_proxy::proxy_loop(int socket_fd)
{
    FILE *sock = fdopen(socket_fd, "r");

    while(1)
    {
        char line[1024];

        if (!fgets(line, 1024, sock)) 
            return ;

        std::string s = std::string(line);
        std::regex regex{R"([\s]+)"};
        std::sregex_token_iterator it{s.begin(), s.end(), regex, -1};
        std::vector<std::string> words{it, {}};

        if (words.size() > 0)
        {
            if (words[0] == "run")
            {
                int64_t timestamp = top->get_time();
                this->top->run();
                dprintf(this->reply_pipe, "running %ld\n", timestamp);
            }
            else if (words[0] == "stop")
            {
                this->top->pause();
                this->top->flush_all();
                fflush(NULL);
                dprintf(this->reply_pipe, "stopped %ld\n", top->get_time());
            }
            else if (words[0] == "quit")
            {
                this->top->quit();
            }
            else if (words[0] == "trace")
            {
                if (words.size() != 3)
                {
                    fprintf(stderr, "This command requires 2 arguments: trace [add|remove] regexp");
                }
                else
                {
                    if (words[1] == "add")
                    {
                        this->top->traces.get_trace_manager()->add_trace_path(0, words[2]);
                        this->top->traces.get_trace_manager()->check_traces();
                    }
                    else
                    {
                        this->top->traces.get_trace_manager()->add_exclude_trace_path(0, words[2]);
                        this->top->traces.get_trace_manager()->check_traces();
                    }
                }
            }
            else if (words[0] == "event")
            {
                if (words.size() != 3)
                {
                    fprintf(stderr, "This command requires 2 arguments: event [add|remove] regexp");
                }
                else
                {
                    if (words[1] == "add")
                    {
                        this->top->traces.get_trace_manager()->add_trace_path(1, words[2]);
                        this->top->traces.get_trace_manager()->check_traces();
                    }
                    else
                    {
                        this->top->traces.get_trace_manager()->add_exclude_trace_path(1, words[2]);
                        this->top->traces.get_trace_manager()->check_traces();
                    }
                }
            }
            else
            {
                printf("Ignoring invalid command: %s\n", words[0].c_str());
            }
        }
    }
}


void Gv_proxy::listener(void)
{
    int client_fd;
    while(1)
    {
        if ((client_fd = accept(this->telnet_socket, NULL, NULL)) == -1)
        {
            if(errno == EAGAIN) continue;
            fprintf(stderr, "Failed to accept connection: %s\n", strerror(errno));
            return;
        }

        this->sockets.push_back(client_fd);
        this->loop_thread = new std::thread(&Gv_proxy::proxy_loop, this, client_fd);
    }
}



int Gv_proxy::open(int port, int *out_port)
{
    if (this->req_pipe == -1)
    {
        struct sockaddr_in addr;
        int yes = 1;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;
        socklen_t size = sizeof(addr.sin_zero);
        memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

        this->telnet_socket = socket(PF_INET, SOCK_STREAM, 0);

        if (this->telnet_socket < 0)
        {
            fprintf(stderr, "Unable to create comm socket: %s\n", strerror(errno));
            return -1;
        }

        if (setsockopt(this->telnet_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            fprintf(stderr, "Unable to setsockopt on the socket: %s\n", strerror(errno));
            return -1;
        }

        if (bind(this->telnet_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            fprintf(stderr, "Unable to bind the socket: %s\n", strerror(errno));
            return -1;
        }

        if (listen(this->telnet_socket, 1) == -1) {
            fprintf(stderr, "Unable to listen: %s\n", strerror(errno));
            return -1;
        }

        getsockname(this->telnet_socket, (sockaddr*)&addr, &size);

        if (out_port)
        {
            *out_port = ntohs(addr.sin_port);
        }

        this->listener_thread = new std::thread(&Gv_proxy::listener, this);
    }
    else
    {
        this->loop_thread = new std::thread(&Gv_proxy::proxy_loop, this, this->req_pipe);
    }

    return 0;
}



void Gv_proxy::stop()
{
    for (auto x: this->sockets)
    {
        shutdown(x, SHUT_RDWR);
    }
}


extern "C" void *gv_create(const char *config_path, struct gv_conf *gv_conf)
{
    setenv("PULP_CONFIG_FILE", config_path, 1);

    js::config *js_config = js::import_config_from_file(config_path);
    if (js_config == NULL)
    {
        fprintf(stderr, "Invalid configuration.");
        return NULL;
    }

    js::config *gv_config = js_config->get("**/gvsoc");

    std::string module_name = "vp.trace_domain_impl";

    if (gv_config->get_child_bool("sv-mode"))
    {
        module_name = "sv." + module_name;
    }
    else if (gv_config->get_child_bool("debug-mode"))
    {
        module_name = "debug." + module_name;
    }


    std::replace(module_name.begin(), module_name.end(), '.', '/');

    std::string path = std::string(getenv("GVSOC_PATH")) + "/" + module_name + ".so";

    void *module = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND);
    if (module == NULL)
    {
        throw std::invalid_argument("ERROR, Failed to open periph model (module: " + module_name + ", error: " + std::string(dlerror()) + ")");
    }

    vp::component *(*constructor)(js::config *) = (vp::component * (*)(js::config *)) dlsym(module, "vp_constructor");
    if (constructor == NULL)
    {
        throw std::invalid_argument("ERROR, couldn't find vp_constructor in loaded module (module: " + module_name + ")");
    }

    vp::component *instance = constructor(js_config);

    instance->set_vp_config(gv_config);
    instance->set_gv_conf(gv_conf);

    return (void *)instance;
}


extern "C" void gv_destroy(void *arg)
{
}


extern "C" void gv_start(void *arg)
{
    vp::component *instance = (vp::component *)arg;

    instance->pre_pre_build();
    instance->pre_build();
    instance->build();
    instance->build_new();

    if (instance->gv_conf.open_proxy)
    {
        proxy = new Gv_proxy(instance, instance->gv_conf.req_pipe, instance->gv_conf.reply_pipe);
        proxy->open(0, instance->gv_conf.proxy_socket);
    }

}


extern "C" void gv_step(void *arg, int64_t timestamp)
{
    vp::component *instance = (vp::component *)arg;

    instance->step(timestamp);
}


extern "C" int64_t gv_time(void *arg)
{
    vp::component *instance = (vp::component *)arg;

    return instance->get_time_engine()->get_next_event_time();
}


extern "C" void *gv_open(const char *config_path, bool open_proxy, int *proxy_socket, int req_pipe, int reply_pipe)
{
    struct gv_conf gv_conf;

    gv_conf.open_proxy = open_proxy;
    gv_conf.proxy_socket = proxy_socket;
    gv_conf.req_pipe = req_pipe;
    gv_conf.reply_pipe = reply_pipe;

    void *instance = gv_create(config_path, &gv_conf);
    if (instance == NULL)
        return NULL;

    gv_start(instance);

    return instance;
}


Gvsoc_proxy::Gvsoc_proxy(std::string config_path)
    : config_path(config_path)
{

}



void Gvsoc_proxy::proxy_loop()
{
    FILE *sock = fdopen(this->reply_pipe[0], "r");

    while(1)
    {
        char line[1024];

        if (!fgets(line, 1024, sock)) 
            return ;

        std::string s = std::string(line);
        std::regex regex{R"([\s]+)"};
        std::sregex_token_iterator it{s.begin(), s.end(), regex, -1};
        std::vector<std::string> words{it, {}};


        for (auto x: words)
        {
            printf("%s\n", x.c_str());
        }

        if (words.size() > 0)
        {
            if (words[0] == "stopped")
            {
                int64_t timestamp = std::atoll(words[1].c_str());
                printf("GOT STOP AT %ld\n", timestamp);
                this->mutex.lock();
                this->stopped_timestamp = timestamp;
                this->running = false;
                this->cond.notify_all();
                this->mutex.unlock();
            }
            else if (words[0] == "running")
            {
                int64_t timestamp = std::atoll(words[1].c_str());
                printf("GOT RUN AT %ld\n", timestamp);
                this->mutex.lock();
                this->running = true;
                this->cond.notify_all();
                this->mutex.unlock();
            }
            else
            {
                printf("Ignoring invalid command: %s\n", words[0].c_str());
            }
        }
    }
}


int Gvsoc_proxy::open()
{
    pid_t ppid_before_fork = getpid();

    if (pipe(this->req_pipe) == -1)
        return -1;

    if (pipe(this->reply_pipe) == -1)
        return -1;

    pid_t child_id = fork();

    if(child_id == -1)
    {
        return -1;
    }
    else if (child_id == 0)
    {
        int r = prctl(PR_SET_PDEATHSIG, SIGTERM);
        if (r == -1) { perror(0); exit(1); }
        // test in case the original parent exited just
        // before the prctl() call
        if (getppid() != ppid_before_fork) exit(1);
        
        void *instance = gv_open(this->config_path.c_str(), true, NULL, this->req_pipe[0], this->reply_pipe[1]);

        int retval = gv_run(instance);

        gv_stop(instance);

        return retval;
    }
    else
    {
        this->running = false;
        this->loop_thread = new std::thread(&Gvsoc_proxy::proxy_loop, this);
    }

    return 0;
}



void Gvsoc_proxy::run()
{
    dprintf(this->req_pipe[1], "run\n");
}



int64_t Gvsoc_proxy::pause()
{
    int64_t result;
    dprintf(this->req_pipe[1], "stop\n");
    std::unique_lock<std::mutex> lock(this->mutex);
    while (this->running)
    {
        this->cond.wait(lock);
    }
    result = this->stopped_timestamp;
    lock.unlock();
    return result;
}



void Gvsoc_proxy::close()
{
    dprintf(this->req_pipe[1], "quit\n");
}



void Gvsoc_proxy::add_event_regex(std::string regex)
{
    dprintf(this->req_pipe[1], "event add %s\n", regex.c_str());

}



void Gvsoc_proxy::remove_event_regex(std::string regex)
{
    dprintf(this->req_pipe[1], "event remove %s\n", regex.c_str());
}



void Gvsoc_proxy::add_trace_regex(std::string regex)
{
    dprintf(this->req_pipe[1], "trace add %s\n", regex.c_str());
}



void Gvsoc_proxy::remove_trace_regex(std::string regex)
{
    dprintf(this->req_pipe[1], "trace remove %s\n", regex.c_str());
}




extern "C" int gv_run(void *_instance)
{
    vp::component *instance = (vp::component *)_instance;

    if (!proxy)
    {
        instance->run();
    }

    return instance->join();
}


extern "C" void gv_init(struct gv_conf *gv_conf)
{
    gv_conf->open_proxy = 0;
    gv_conf->open_proxy = NULL;
    gv_conf->req_pipe = 0;
    gv_conf->reply_pipe = 0;
}


extern "C" void gv_stop(void *_instance)
{
    vp::component *instance = (vp::component *)_instance;

    if (proxy)
    {
        proxy->stop();
    }

    instance->stop();
}



#ifdef __VP_USE_SYSTEMC

static void *(*sc_entry)(void *);
static void *sc_entry_arg;

void set_sc_main_entry(void *(*entry)(void *), void *arg)
{
    sc_entry = entry;
    sc_entry_arg = arg;
}

int sc_main(int argc, char *argv[])
{
    sc_entry(sc_entry_arg);
    return 0;
}
#endif


extern "C" void *gv_chip_pad_bind(void *handle, char *name, int ext_handle)
{
    vp::component *instance = (vp::component *)handle;
    return instance->external_bind(name, ext_handle);
}
