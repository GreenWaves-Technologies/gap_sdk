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
 * Authors: Francesco Conti, University of Bologna & GreenWaves Technologies (f.conti@unibo.it)
 *          Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <ne16.hpp>

using namespace std::placeholders;

Ne16::Ne16(js::config *config)
    : vp::component(config)
{
    // FIXME these parameters might be settable through config, later...
    this->TP_IN           = 16;
    this->TP_OUT          = 32;
    this->QA_IN           = 8;
    this->QA_OUT          = 8;
    this->NR_COLUMN       = 9;
    this->COLUMN_SIZE     = 9;
    this->BLOCK_SIZE      = 16;
    this->F_BUFFER_SIZE   = 5;
    this->FILTER_SIZE     = 3;
    this->SHIFT_CYCLES    = 2;
    this->OVERHEAD_LD_1X1 = 19;
    this->OVERHEAD_LD_3X3 = 31;
    this->OVERHEAD_MV     = 17;
    this->QUANT_PER_CYCLE = 4;
}

void Ne16::reset(bool active)
{
    // All registers should be initialized here, so that cluster reset is properly working
    // This will be called first with active=1 and then with active=0
    this->psum_block      = xt::zeros<int64_t>({this->NR_COLUMN, this->COLUMN_SIZE});
    this->psum_column     = xt::zeros<int64_t>({this->NR_COLUMN});
    this->accum           = xt::zeros<int64_t>({this->TP_OUT, this->NR_COLUMN});
    this->x_buffer        = xt::zeros<uint8_t>({this->F_BUFFER_SIZE, this->F_BUFFER_SIZE, this->TP_IN});
    this->x_buffer_linear = xt::zeros<uint8_t>({32, this->TP_IN});
    this->x_array         = xt::zeros<uint8_t>({this->NR_COLUMN, this->COLUMN_SIZE, this->TP_IN}); 
    this->weight          = xt::zeros<uint8_t>({this->FILTER_SIZE*this->FILTER_SIZE, 2});
    this->nqs             = xt::zeros<uint8_t>({this->TP_OUT});
}

// The `hwpe_slave` member function models an access to the NE16 SLAVE interface
vp::io_req_status_e Ne16::hwpe_slave(void *__this, vp::io_req *req)
{
    Ne16 *_this = (Ne16 *)__this;

    _this->trace.msg(vp::trace::LEVEL_DEBUG, "Received request (addr: 0x%x, size: 0x%x, is_write: %d, data: %p\n", req->get_addr(), req->get_size(), req->get_is_write(), req->get_data());
    uint8_t *data = req->get_data(); // size depends on data get_size

    // Dispatch the register file access to the correct function
    if(req->get_is_write()) {
        if((req->get_addr() & 0x17f) == 0x0) {
            _this->commit();
            if (!_this->fsm_start_event->is_enqueued() && *(uint32_t *) data == 0) {
                _this->event_enqueue(_this->fsm_start_event, 1);
            }
        }
        else {
            _this->trace.msg(vp::trace::LEVEL_DEBUG, "offset: %d data: %08x\n", ((req->get_addr() & 0x17f) - 0x20) >> 2, *(uint32_t *) data);
            _this->regfile_wr(((req->get_addr() & 0x17f) - 0x20)>> 2, *(uint32_t *) data);
        }
    }
    else {
        if((req->get_addr() & 0x17f) == 0x4) {
            *(uint32_t *) data = _this->acquire();
            _this->trace.msg("Returning %x\n", *(uint32_t *) data);
        }
        else if((req->get_addr() & 0x17f) == 0xc) {
            *(uint32_t *) data = _this->status() ? 1 : 0;
            _this->trace.msg("Returning %x\n", *(uint32_t *) data);
        }
        else {
            *(uint32_t *) data = _this->regfile_rd(((req->get_addr() & 0x17f) - 0x20) >> 2);
            _this->trace.msg("Returning %x\n", *(uint32_t *) data);
        }
    }

    return vp::IO_REQ_OK;
}

void Ne16::data_access_test_handler(void *__this, vp::clock_event *event)
{
    Ne16 *_this = (Ne16 *)__this;

    // uint32_t data[4];
    uint8_t data[16];

    // Several requests can be done on the same port at same cycle, the reported latency will just be cumulated, then
    // just take the bigger one
    int max_latency = 0;

    int size = 4;
    for (int i=0; i<(16/size); i++)
    {
        _this->io_req.init();
        _this->io_req.set_addr((_this->infeat_ptr+i*size) & 0x0fffffff);
        _this->io_req.set_size(size);
        _this->io_req.set_data((uint8_t *) (data + i*size));
        _this->io_req.set_is_write(false);
        _this->trace.msg(vp::trace::LEVEL_DEBUG, "Issuing read requests (infeat_ptr=0x%08x)\n", (_this->infeat_ptr+i*size) & 0x0fffffff);

        int err = _this->out.req(&_this->io_req);
        if (err == vp::IO_REQ_OK)
        {
            int64_t latency = _this->io_req.get_latency();
            if (latency > max_latency)
            {
                max_latency = latency;
            }

            // DO something with the latency
        }
        else
        {
            // This should never happen when ne16 is accessing L1
            _this->trace.fatal("Unsupported asynchronous reply\n");
        }
    }
    
    for (int i=0; i<(16/size); i++)
    {
        _this->io_req.init();
        _this->io_req.set_addr((_this->outfeat_ptr+i*size) & 0x0fffffff);
        _this->io_req.set_size(size);
        _this->io_req.set_data((uint8_t *) (data + i*size));
        _this->io_req.set_is_write(true);
        _this->trace.msg(vp::trace::LEVEL_DEBUG, "Issuing write requests (outfeat_ptr=0x%08x, data=0x%08x)\n", (_this->outfeat_ptr+i*size) & 0x0fffffff, data[i]);

        int err = _this->out.req(&_this->io_req);
        if (err == vp::IO_REQ_OK)
        {
            int64_t latency = _this->io_req.get_latency();
            if (latency > max_latency)
            {
                max_latency = latency;
            }

            // DO something with the latency
        }
        else
        {
            // This should never happen when ne16 is accessing L1
            _this->trace.fatal("Unsupported asynchronous reply\n");
        }
    }

    _this->trace.msg(vp::trace::LEVEL_DEBUG, "Got response (latency: %ld)\n", max_latency);

}


int Ne16::build()
{
    this->traces.new_trace("trace", &this->trace, vp::DEBUG);

    this->new_master_port("out", &this->out);

    this->new_master_port("irq", &this->irq);

    this->in.set_req_meth(&Ne16::hwpe_slave);
    this->new_slave_port("input", &this->in); // how to change this name?

    this->fsm_start_event = this->event_new(&Ne16::fsm_start_handler);
    this->fsm_event = this->event_new(&Ne16::fsm_handler);
    this->fsm_end_event = this->event_new(&Ne16::fsm_end_handler);
    this->data_access_test_event = this->event_new(&Ne16::data_access_test_handler);

    this->trace.msg(vp::trace::LEVEL_DEBUG, "Is there anybody out there?\n");

    return 0;
}

extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Ne16(config);
}
