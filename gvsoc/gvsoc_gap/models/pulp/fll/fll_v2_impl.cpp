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
#include <vp/itf/clock.hpp>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <archi/chips/gap9_v2/fll/fll_regs.h>
#include <archi/chips/gap9_v2/fll/fll_regfields.h>
#include <archi/chips/gap9_v2/fll/fll_gvsoc.h>


using namespace std::placeholders;


#define FLL_NB_DCOS    4


class Fll;


class Dco
{
public:
    Dco(Fll *top, int id, vp::reg_32 *fcr1, vp::reg_32 *fcr2);
    void handle_ref_clock_edge();
    void check_state();
    void set_dco_min_code(uint32_t code);
    void set_dco_max_code(uint32_t code);
    void update_fcr1();
    void update_fcr2();
    void reset();

private:

    int get_dco_frequency(int dco_input, int dith);
    void set_clamp_error(bool is_low);
    void set_lock(bool lock);

    vp::trace         trace;
    int               id;
    Fll              *top;
    bool              is_on;
    bool              is_open_loop;
    uint32_t          dco_input_code;
    int               dco_frequency;
    uint32_t          dco_min_code;
    uint32_t          dco_max_code;
    int64_t           dco_update_timestamp;
    int               integration_period_count;
    int               integrator;
    int64_t           actual_mult_factor;
    bool              is_locked;
    int               nb_stability_cycles;

    vp_fll_f0cr1      *fcr1;
    vp_fll_f0cr2      *fcr2;
};




class Fll : public vp::component
{
    friend class Dco;

public:

    Fll(js::config *config);

    int build();
    void reset(bool active);

    static vp::io_req_status_e req(void *__this, vp::io_req *req);
    static void ref_clock_sync(void *__this, bool value);
    void set_frequency(int dco, int dco_frequency, bool is_locked);

protected:

    vp_regmap_fll     regmap;

private:

    void check_state();
    void fcr1_handle_req(int dco, vp::reg_32 *reg, bool is_write);
    void drr_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void f0cr1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void f1cr1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void f2cr1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void f3cr1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void fcr2_handle_req(int dco, vp::reg_32 *reg, bool is_write);
    void f0cr2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void f1cr2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void f2cr2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void f3cr2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void drr_update_dcos();

    void handle_pending_requests();

    vp::clock_slave   ref_clock_itf;
    vp::clock_master  out_clock0_itf;
    vp::clock_master  out_clock1_itf;
    vp::clock_master  out_clock2_itf;
    vp::clock_master  out_clock3_itf;
    vp::io_slave      in;

    Dco              *dcos[FLL_NB_DCOS];

    int               req_wait_ref_clk_count;
    vp::io_req       *pending_reqs_first;
    vp::io_req       *pending_reqs_last;
};



Dco::Dco(Fll *top, int id, vp::reg_32 *fcr1, vp::reg_32 *fcr2)
{
    top->traces.new_trace("trace/dco_" + std::to_string(id), &this->trace, vp::DEBUG);

    this->id = id;
    this->top = top;
    this->is_on = false;
    this->is_open_loop = true;
    this->is_locked = false;
    this->fcr1 = (vp_fll_f0cr1 *)fcr1;
    this->fcr2 = (vp_fll_f0cr2 *)fcr2;
}


void Dco::reset()
{
    this->integration_period_count = 0;
    this->integrator = 0x44 << 10; // This is the initial value in the HW
}


int Dco::get_dco_frequency(int dco_input, int dith)
{
    return (int)((41.01f + 2.734f * (float)(dco_input + dith))*1000000);
}


void Dco::update_fcr1()
{
    this->is_on = this->fcr1->dco_en_get();

    int is_new_open_loop = this->fcr1->op_mode_get() == 0;
    if (is_new_open_loop != this->is_open_loop)
    {
        this->actual_mult_factor = -1;
        this->nb_stability_cycles = 0;
        this->dco_update_timestamp = -1;
    }
    this->is_open_loop = is_new_open_loop;
}


void Dco::update_fcr2()
{
    if (this->is_open_loop)
    {
        this->dco_input_code = this->fcr2->dco_code_get();
    }
}


void Dco::set_dco_min_code(uint32_t code)
{
    this->dco_min_code = code;
}


void Dco::set_dco_max_code(uint32_t code)
{
    this->dco_max_code = code;
}


void Dco::handle_ref_clock_edge()
{
    if (this->is_on)
    {
        if (!this->is_open_loop)
        {
            // We are in close loop mode, first check the integration period
            if (this->integration_period_count == 0)
            {
                // We reached the end of the period, update the DCO code with the feedback loop
                if (this->dco_update_timestamp != -1)
                {
                    // First we compute the number of edges of the DCO, to compare it with the mult factor
                    int64_t duration = this->top->get_time() - this->dco_update_timestamp;
                    
                    // The feeback loop is dividing the DCO frequency by 2
                    this->actual_mult_factor = (int64_t)this->dco_frequency / 2 * duration / 1000000000000;

                    // Compute the difference between expected and measured and adjust the integration using the gain
                    int delta = this->fcr2->mfi_get() - this->actual_mult_factor;
                    int delta_ext = delta << 10;
                    int gain = this->fcr1->loop_gain_get();
                    int delta_ext_amp = gain < 10 ? delta_ext >> gain : 0;
                    this->integrator = this->integrator + delta_ext_amp;
                }

                this->dco_update_timestamp = this->top->get_time();

                // Rearm the integration period from the register
                this->integration_period_count = this->fcr1->itg_per_get();

                this->check_state();
            }
            else
            {
                this->integration_period_count--;
            }
        }
    }
}


void Dco::set_clamp_error(bool is_low)
{
    if (is_low)
    {
        switch (this->id)
        {
            case 0:
                this->top->regmap.fsr.clmp_lo_err0_set(1);
                break;
            case 1:
                this->top->regmap.fsr.clmp_lo_err1_set(1);
                break;
            case 2:
                this->top->regmap.fsr.clmp_lo_err2_set(1);
                break;
            case 3:
                this->top->regmap.fsr.clmp_lo_err3_set(1);
                break;
        }
    }
    else
    {
        switch (this->id)
        {
            case 0:
                this->top->regmap.fsr.clmp_hi_err0_set(1);
                break;
            case 1:
                this->top->regmap.fsr.clmp_hi_err1_set(1);
                break;
            case 2:
                this->top->regmap.fsr.clmp_hi_err2_set(1);
                break;
            case 3:
                this->top->regmap.fsr.clmp_hi_err3_set(1);
                break;
        }   
    }
}


void Dco::set_lock(bool lock)
{
    this->is_locked = lock;
    switch(this->id)
    {
        case 0:
            this->top->regmap.fsr.lock0_set(lock);
            break;
        case 1:
            this->top->regmap.fsr.lock1_set(lock);
            break;
        case 2:
            this->top->regmap.fsr.lock2_set(lock);
            break;
        case 3:
            this->top->regmap.fsr.lock3_set(lock);
            break;
    }
}


void Dco::check_state()
{
    if (this->is_on)
    {
        int code = this->is_open_loop ? this->dco_input_code : this->integrator >> 10;

        if (this->dco_input_code < this->dco_min_code) 
        {
            code = this->dco_min_code;
            this->set_clamp_error(true);
        }

        if (this->dco_input_code > this->dco_max_code) 
        {
            code = this->dco_max_code;
            this->set_clamp_error(false);
        }

        if (!this->is_open_loop && this->actual_mult_factor != -1)
        {
            // Close loop mode, check if the dco must be locked or unlocked
            int delta = this->fcr2->mfi_get() - this->actual_mult_factor;
            if (delta < 0)
            {
                delta = -delta;
            }

            int tolerance = this->fcr1->lock_tol_get();
            if (this->is_locked)
            {
                if (delta >= tolerance)
                {
                    this->nb_stability_cycles++;
                    this->trace.msg(vp::trace::LEVEL_DEBUG, "DCO frequency outside tolerance, increasing unstable cycles (unstable_cycles: %d, deassert_cycles: %d)\n", this->nb_stability_cycles, this->fcr1->stbl_get());
                    if (this->nb_stability_cycles == this->fcr1->stbl_get())
                    {
                        this->trace.msg(vp::trace::LEVEL_INFO, "Unlocking DCO\n");
                        this->set_lock(false);
                        this->nb_stability_cycles = 0;
                    }
                }
            }
            else
            {
                if (delta < tolerance)
                {
                    this->nb_stability_cycles++;
                    this->trace.msg(vp::trace::LEVEL_DEBUG, "DCO frequency within tolerance, increasing stable cycles (stable_cycles: %d, assert_cycles: %d)\n", this->nb_stability_cycles, this->fcr1->stbl_get());
                    if (this->nb_stability_cycles == this->fcr1->stbl_get())
                    {
                    this->trace.msg(vp::trace::LEVEL_INFO, "Locking FLL\n");
                    this->set_lock(true);
                    this->nb_stability_cycles = 0;
                    }
                }
            }
        }

        this->dco_frequency = this->get_dco_frequency(code, 0);

        this->trace.msg(vp::trace::LEVEL_DEBUG, "Setting new DCO frequency (input_code: 0x%x, frequency: %d Hz)\n", code, this->dco_frequency);

        this->top->set_frequency(this->id, this->dco_frequency, this->is_open_loop || this->is_locked);
    }
}






Fll::Fll(js::config *config)
: vp::component(config)
{

}


void Fll::set_frequency(int dco, int dco_frequency, bool is_locked)
{
    if (this->regmap.ccr2.clk0_sel_get() == dco + 1)
    {
        int div = this->regmap.ccr1.clk0_div_get();
        int frequency = is_locked || !this->regmap.ccr2.ckg0_get() ? dco_frequency / (2 * (div == 0 ? 1 : div)) : 0;
        this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Setting new out clock frequency (clock: %d, frequency: %d Hz)\n", 0, frequency);
        this->out_clock0_itf.set_frequency(frequency);
    }

    if (this->regmap.ccr2.clk1_sel_get() == dco + 1)
    {
        int div = this->regmap.ccr1.clk1_div_get();
        int frequency = is_locked || !this->regmap.ccr2.ckg0_get() ? dco_frequency / (2 * (div == 0 ? 1 : div)) : 0;
        this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Setting new out clock frequency (clock: %d, frequency: %d Hz)\n", 1, frequency);
        this->out_clock1_itf.set_frequency(frequency);
    }

    if (this->regmap.ccr2.clk2_sel_get() == dco + 1)
    {
        int div = this->regmap.ccr1.clk2_div_get();
        int frequency = is_locked || !this->regmap.ccr2.ckg0_get() ? dco_frequency / (2 * (div == 0 ? 1 : div)) : 0;
        this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Setting new out clock frequency (clock: %d, frequency: %d Hz)\n", 2, frequency);
        this->out_clock2_itf.set_frequency(frequency);
    }

    if (this->regmap.ccr2.clk3_sel_get() == dco + 1)
    {
        int div = this->regmap.ccr1.clk3_div_get();
        int frequency = is_locked || !this->regmap.ccr2.ckg0_get() ? dco_frequency / (2 * (div == 0 ? 1 : div)) : 0;
        this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Setting new out clock frequency (clock: %d, frequency: %d Hz)\n", 3, frequency);
        this->out_clock3_itf.set_frequency(frequency);
    }
}


void Fll::ref_clock_sync(void *__this, bool value)
{
    Fll *_this = (Fll *)__this;

    _this->get_trace()->msg(vp::trace::LEVEL_TRACE, "Ref clock sync\n");
    
    if (value)
    {
        for (int i=0; i<FLL_NB_DCOS; i++)
        {
            _this->dcos[i]->handle_ref_clock_edge();
        }
    }

    _this->handle_pending_requests();
}


void Fll::check_state()
{
    for (int i=0; i<FLL_NB_DCOS; i++)
    {
        this->dcos[i]->check_state();
    }
}


void Fll::handle_pending_requests()
{
    if (this->req_wait_ref_clk_count == 0)
    {
        vp::io_req *req = this->pending_reqs_first;

        if (req)
        {
            // Once an access is acknowledged, it takes 3 more cycles to the FLL to accept another request
            this->req_wait_ref_clk_count = 3;

            this->pending_reqs_first = req->get_next();

            if (this->pending_reqs_first)
            {
                // In case another request is already pending, add the 3 cycles for the acknowledge which has not been accounted yet and another
                // cycle lost in between the 2 requests
                this->req_wait_ref_clk_count += 4;
            }

            this->regmap.access(req->get_addr(), req->get_size(), req->get_data(), req->get_is_write());
            req->resp_port->resp(req);
        }
    }
    else
    {
        this->req_wait_ref_clk_count--;
    }
}


vp::io_req_status_e Fll::req(void *__this, vp::io_req *req)
{
    Fll *_this = (Fll *)__this;

    _this->get_trace()->msg(vp::trace::LEVEL_DEBUG, "Received request, putting to wait list (addr: 0x%x, size: 0x%x, is_write: %d)\n", req->get_addr(), req->get_size(), req->get_is_write());

    if (_this->pending_reqs_first == NULL)
    {
        _this->pending_reqs_first = req;

        // In case the FLL is not already handling an access, it takes 3 ref cycles to the FLL to acknowledge the request and unblock the initiator
        if (_this->req_wait_ref_clk_count == 0)
        {
            _this->req_wait_ref_clk_count = 3;
        }
    }
    else
    {
        _this->pending_reqs_last->set_next(req);
    }

    _this->pending_reqs_last = req;
    req->set_next(NULL);

    return vp::IO_REQ_PENDING;
}


void Fll::drr_update_dcos()
{
    for (int i=0; i<FLL_NB_DCOS; i++)
    {
        this->dcos[i]->set_dco_min_code(this->regmap.drr.dco_min_get());
        this->dcos[i]->set_dco_max_code(this->regmap.drr.dco_max_get());
    }
}


void Fll::drr_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.drr.update(reg_offset, size, value, is_write);
    this->drr_update_dcos();
}


void Fll::fcr1_handle_req(int dco_id, vp::reg_32 *_reg, bool is_write)
{
    vp_fll_f0cr1 *reg = (vp_fll_f0cr1 *)_reg;
    Dco *dco = this->dcos[dco_id];
    dco->update_fcr1();
}


void Fll::f0cr1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.f0cr1.update(reg_offset, size, value, is_write);
    this->fcr1_handle_req(0, &this->regmap.f0cr1, is_write);
}


void Fll::f1cr1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.f1cr1.update(reg_offset, size, value, is_write);
    this->fcr1_handle_req(1, &this->regmap.f1cr1, is_write);
}


void Fll::f2cr1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.f2cr1.update(reg_offset, size, value, is_write);
    this->fcr1_handle_req(2, &this->regmap.f2cr1, is_write);
}


void Fll::f3cr1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.f3cr1.update(reg_offset, size, value, is_write);
    this->fcr1_handle_req(3, &this->regmap.f3cr1, is_write);
}


void Fll::fcr2_handle_req(int dco_id, vp::reg_32 *_reg, bool is_write)
{
    vp_fll_f0cr2 *reg = (vp_fll_f0cr2 *)_reg;
    Dco *dco = this->dcos[dco_id];
    dco->update_fcr2();
}


void Fll::f0cr2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.f0cr2.update(reg_offset, size, value, is_write);
    this->fcr2_handle_req(0, &this->regmap.f0cr2, is_write);
}


void Fll::f1cr2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.f1cr2.update(reg_offset, size, value, is_write);
    this->fcr2_handle_req(1, &this->regmap.f1cr2, is_write);
}


void Fll::f2cr2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.f2cr2.update(reg_offset, size, value, is_write);
    this->fcr2_handle_req(2, &this->regmap.f2cr2, is_write);
}


void Fll::f3cr2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.f3cr2.update(reg_offset, size, value, is_write);
    this->fcr2_handle_req(3, &this->regmap.f3cr2, is_write);
}


int Fll::build()
{
    in.set_req_meth(&Fll::req);
    new_slave_port("input", &in);

    ref_clock_itf.set_sync_meth(&Fll::ref_clock_sync);
    new_slave_port("ref_clock", &this->ref_clock_itf);

    new_master_port("clock_0", &this->out_clock0_itf);
    new_master_port("clock_1", &this->out_clock1_itf);
    new_master_port("clock_2", &this->out_clock2_itf);
    new_master_port("clock_3", &this->out_clock3_itf);


    this->dcos[0] = new Dco(this, 0, &this->regmap.f0cr1, &this->regmap.f0cr2);
    this->dcos[1] = new Dco(this, 1, &this->regmap.f1cr1, &this->regmap.f1cr2);
    this->dcos[2] = new Dco(this, 2, &this->regmap.f2cr1, &this->regmap.f2cr2);
    this->dcos[3] = new Dco(this, 3, &this->regmap.f3cr1, &this->regmap.f3cr2);

    this->regmap.build(this, this->get_trace());
    this->regmap.drr.register_callback(std::bind(&Fll::drr_req, this, _1, _2, _3, _4));
    this->regmap.f0cr1.register_callback(std::bind(&Fll::f0cr1_req, this, _1, _2, _3, _4));
    this->regmap.f1cr1.register_callback(std::bind(&Fll::f1cr1_req, this, _1, _2, _3, _4));
    this->regmap.f2cr1.register_callback(std::bind(&Fll::f2cr1_req, this, _1, _2, _3, _4));
    this->regmap.f3cr1.register_callback(std::bind(&Fll::f3cr1_req, this, _1, _2, _3, _4));
    this->regmap.f0cr2.register_callback(std::bind(&Fll::f0cr2_req, this, _1, _2, _3, _4));
    this->regmap.f1cr2.register_callback(std::bind(&Fll::f1cr2_req, this, _1, _2, _3, _4));
    this->regmap.f2cr2.register_callback(std::bind(&Fll::f2cr2_req, this, _1, _2, _3, _4));
    this->regmap.f3cr2.register_callback(std::bind(&Fll::f3cr2_req, this, _1, _2, _3, _4));

    return 0;
}


void Fll::reset(bool active)
{
    if (active)
    {
        this->req_wait_ref_clk_count = 0;
        this->pending_reqs_first = NULL;

        for (int i=0; i<FLL_NB_DCOS; i++)
        {
            this->dcos[i]->reset();
        }
    }
    else
    {
        this->drr_update_dcos();

        this->fcr1_handle_req(0, &this->regmap.f0cr1, true);
        this->fcr1_handle_req(1, &this->regmap.f1cr1, true);
        this->fcr1_handle_req(2, &this->regmap.f2cr1, true);
        this->fcr1_handle_req(3, &this->regmap.f3cr1, true);

        this->fcr2_handle_req(0, &this->regmap.f0cr2, true);
        this->fcr2_handle_req(1, &this->regmap.f1cr2, true);
        this->fcr2_handle_req(2, &this->regmap.f2cr2, true);
        this->fcr2_handle_req(3, &this->regmap.f3cr2, true);

        // Check the state now, this will have the effect of starting the DCOs in open loop
        this->check_state();
    }
}

extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Fll(config);
}
