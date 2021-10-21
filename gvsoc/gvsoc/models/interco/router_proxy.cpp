/*
 * Copyright (C) 2021 GreenWaves Technologies, SAS
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

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <gv/gvsoc.hpp>


class Router_proxy : public vp::component, public gv::Io_binding
{

public:

    Router_proxy(js::config *config);

    int build();

    void grant(gv::Io_request *req);
    void reply(gv::Io_request *req);
    void access(gv::Io_request *req);

    void *external_bind(std::string comp_name, std::string itf_name, void *handle);

    static vp::io_req_status_e req(void *__this, vp::io_req *req);

    static void grant(void *_this, vp::io_req *req);

    static void response(void *_this, vp::io_req *req);

private:
    vp::trace     trace;
    vp::io_slave  in;
    vp::io_master out;
    gv::Io_user   *user;
};

Router_proxy::Router_proxy(js::config *config)
: vp::component(config)
{

}

vp::io_req_status_e Router_proxy::req(void *__this, vp::io_req *req)
{
    Router_proxy *_this = (Router_proxy *)__this;
    gv::Io_request *io_req = new gv::Io_request();
    io_req->addr = req->get_addr();
    io_req->size = req->get_size();
    io_req->data = req->get_data();
    io_req->is_write = req->get_is_write();
    io_req->handle = (void *)req;
    _this->user->access(io_req);
    return vp::IO_REQ_PENDING;
}


void *Router_proxy::external_bind(std::string comp_name, std::string itf_name, void *handle)
{
    if (comp_name == this->get_path())
    {
        this->user = (gv::Io_user *)handle;
        return static_cast<gv::Io_binding *>(this);
    }
    else
    {
        return NULL;
    }
}


void Router_proxy::grant(void *__this, vp::io_req *req)
{
    Router_proxy *_this = (Router_proxy *)__this;

    gv::Io_request *io_req = (gv::Io_request *)req->arg_pop();

    _this->user->reply(io_req);
}

void Router_proxy::response(void *__this, vp::io_req *req)
{
    Router_proxy *_this = (Router_proxy *)__this;

    gv::Io_request *io_req = (gv::Io_request *)req->arg_pop();
    io_req->retval = req->status == vp::IO_REQ_INVALID ? gv::Io_request_ko : gv::Io_request_ok;

    _this->user->reply(io_req);
}

void Router_proxy::grant(gv::Io_request *io_req)
{
    this->get_time_engine()->lock();
    vp::io_req *req = (vp::io_req *)io_req->handle;
    req->get_resp_port()->grant(req);
    this->get_time_engine()->unlock();
}

void Router_proxy::reply(gv::Io_request *io_req)
{
    this->get_time_engine()->lock();
    vp::io_req *req = (vp::io_req *)io_req->handle;
    req->get_resp_port()->resp(req);
    this->get_time_engine()->unlock();
    delete io_req;
}

void Router_proxy::access(gv::Io_request *io_req)
{
    this->get_time_engine()->lock();
    vp::io_req *req = new vp::io_req();
    req->init();
    req->set_addr(io_req->addr);
    req->set_size(io_req->size);
    req->set_is_write(io_req->is_write);
    req->set_data(io_req->data);
    req->arg_push(io_req);

    int err = this->out.req(req);
    if (err == vp::IO_REQ_OK || err == vp::IO_REQ_INVALID)
    {
        this->response(this, req);
    }
    this->get_time_engine()->unlock();
}



int Router_proxy::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    in.set_req_meth(&Router_proxy::req);
    new_slave_port("input", &in);

    out.set_resp_meth(&Router_proxy::response);
    out.set_grant_meth(&Router_proxy::grant);
    new_master_port("out", &out);

    return 0;
}

extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Router_proxy(config);
}
