/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
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

#pragma once


#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/queue.hpp>
#include <queue>
#include "implem/mem_plug.hpp"

/**
 * @brief Model a memory plug
 * 
 * This memory plug is meant to be connected to a memory interface to provide high-bandwidth 
 * throughput through multiple outstanding requests.
 * It has several ports, and each port can send a single request to the memory interface.
 * It can accept multiple input requests, each request being served on one port, which means
 * multiple request has to be enqueued to increase the bandwidth.
 */
class Mem_plug : public Mem_plug_implem
{
public:
    /**
     * @brief Construct a new Mem_plug object
     * 
     * @param parent Parent block containing this block.
     * @param comp Component containing this block (used for tracing and events).
     * @param path Path of the block in the component (for trace declaration).
     * @param mem_itf Memory output interface.
     * @param nb_ports Number of ports to the memory interface.
     * @param output_latency Latency in cycles on each output port.
     */
    Mem_plug(vp::block *parent, vp::component *comp, std::string path, vp::io_master *mem_itf,
        int nb_ports, int output_latency);

    /**
     * @brief Enqueue an access request.
     * 
     * A request fully describes the access to be done to the memory port.
     * It is enqueued to the list of pending requests. A number of requests corresponding to the
     * number of ports are processed in parallel, the other are put on hold until one port is
     * available. Requests are processed in a FIFO way.
     * Once a requets has been fully processed, the caller is notified through a callback method
     * attached to the request.
     * 
     * @param request Request to be enqueued
     */
    void enqueue(Mem_plug_req *request);
};



/**
 * @brief Mem_plug input request
 * 
 * Structure holding information about the access to be done on the output memory interface.
 * ALso used to notify the termination of the access through a method callback.
 */
class Mem_plug_req : public vp::queue_elem
{
    friend class Mem_plug_port;
    friend class Mem_plug;

public:
    /**
     * @brief Construct a new Mem_plug_req object
     * 
     * @param addr Address of the access.
     * @param data Pointer the memory containing the data.
     * @param size Size of the access.
     * @param is_write True if the access is a write, false if it is a read.
     */
    Mem_plug_req(uint32_t addr, uint8_t *data, uint32_t size, bool is_write);

    /**
     * @brief End of request notification
     * 
     * This virtual method is called when the request has been fully processed and can thus be
     * overloaded by the caller to be notified.
     */
    virtual void handle_termination() {}

protected:
    uint32_t addr;  // Address of the access.
    uint8_t *data;  // Pointer the memory containing the data.
    uint32_t size;  // Size of the access.
    bool is_write;  // True if the access is a write, false if it is a read.
};
