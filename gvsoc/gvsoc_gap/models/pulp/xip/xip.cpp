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
 * Authors: Antoine Faravelon, GreenWaves Technologies (antoine.faravelon@greenwaves-technologies.com)
 */

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <string.h>
#include "vp/itf/wire.hpp"
#include <archi/chips/gap9_v2/xip/xip_regs.h>
#include <archi/chips/gap9_v2/xip/xip_regfields.h>
#include <archi/chips/gap9_v2/xip/xip_gvsoc.h>
#include "pulp/udma/udma_mem_refill.hpp"
#include <interco/mem_plug/mem_plug.hpp>

using namespace std::placeholders;

class Xip;


#define L2_PLUG_NB_PORTS 4
#define L2_PLUG_OUTPUT_LATENCY 5

#define XIP_PAGES_NB 16
#define XIP_AREAS_NB 3
// update as itfs (in the sense => l2 port)
#define XIP_INTERFACES_NB 2
// max number of queued requests, as per rtl
#define XIP_REFILL_REQ_DEPTH 2

typedef struct {
    int req;
} fetch_t;

typedef struct {
    // programeable data
    uint32_t l2_addr;
    uint32_t ext_addr;
    uint8_t per_id;
    bool active;
    // internal data
    bool valid; // page has been filled
    uint32_t page_offset;
} xip_page_t;

typedef struct {
    uint32_t page_size; // converted to one hot
    uint32_t mnt_size; // converted to one hot
    uint32_t start_addr;
    uint32_t end_addr; // in bytes, converted from entry
    uint32_t ext_addr; // in bytes, converted from entry
} xip_area_t;

class Xip_mem_plug_req : public Mem_plug_req
{
public:
     Xip_mem_plug_req(uint32_t addr, uint8_t *data, uint32_t size, bool is_write, Xip *comp, vp::io_req *req);

     void handle_termination();
private:
    Xip *comp;
    vp::io_req *req;
};

class Xip : public vp::component
{
    friend class Xip_mem_plug_req;

public:
    Xip(js::config *config);

    int build();
    void reset(bool active);

protected:
    void check_state();

private:
    static vp::io_req_status_e apb_req(void *__this, vp::io_req *req);
    static vp::io_req_status_e fc_data_req(void *__this, vp::io_req *req);
    static vp::io_req_status_e fc_fetch_req(void *__this, vp::io_req *req);
    static vp::io_req_status_e axi_req(void *__this, vp::io_req *req);
    static void refill_reply(void *__this, udma_refill_req_t*);
    int32_t match_per(uint32_t addr);
    int32_t match_page(uint32_t page_offset, uint32_t per_id);
    int32_t find_lru(uint32_t per_id);
    void cfg_page_req(uint64_t, int, uint8_t*, bool, int);

    bool refill_ongoing(uint32_t per_id);
    void cfg_page0_req(uint64_t, int, uint8_t*, bool);
    void cfg_page1_req(uint64_t, int, uint8_t*, bool);
    void cfg_page2_req(uint64_t, int, uint8_t*, bool);
    void cfg_page3_req(uint64_t, int, uint8_t*, bool);
    void cfg_page4_req(uint64_t, int, uint8_t*, bool);
    void cfg_page5_req(uint64_t, int, uint8_t*, bool);
    void cfg_page6_req(uint64_t, int, uint8_t*, bool);
    void cfg_page7_req(uint64_t, int, uint8_t*, bool);
    void cfg_page8_req(uint64_t, int, uint8_t*, bool);
    void cfg_page9_req(uint64_t, int, uint8_t*, bool);
    void cfg_page10_req(uint64_t, int, uint8_t*, bool);
    void cfg_page11_req(uint64_t, int, uint8_t*, bool);
    void cfg_page12_req(uint64_t, int, uint8_t*, bool);
    void cfg_page13_req(uint64_t, int, uint8_t*, bool);
    void cfg_page14_req(uint64_t, int, uint8_t*, bool);
    void cfg_page15_req(uint64_t, int, uint8_t*, bool);

    void cfg_virt_addr_req(uint64_t, int, uint8_t*, bool, int);
    void cfg_virt_addr0_req(uint64_t, int, uint8_t*, bool);
    void cfg_virt_addr1_req(uint64_t, int, uint8_t*, bool);
    void cfg_virt_addr2_req(uint64_t, int, uint8_t*, bool);

    void cfg_ext_addr_req(uint64_t, int, uint8_t*, bool, int);
    void cfg_ext_addr0_req(uint64_t, int, uint8_t*, bool);
    void cfg_ext_addr1_req(uint64_t, int, uint8_t*, bool);
    void cfg_ext_addr2_req(uint64_t, int, uint8_t*, bool);

    void cfg_mnt_size_req(uint64_t, int, uint8_t*, bool, int);
    void cfg_mnt_size0_req(uint64_t, int, uint8_t*, bool);
    void cfg_mnt_size1_req(uint64_t, int, uint8_t*, bool);
    void cfg_mnt_size2_req(uint64_t, int, uint8_t*, bool);

    void cfg_page_size_req(uint64_t, int, uint8_t*, bool, int);
    void cfg_page_size0_req(uint64_t, int, uint8_t*, bool);
    void cfg_page_size1_req(uint64_t, int, uint8_t*, bool);
    void cfg_page_size2_req(uint64_t, int, uint8_t*, bool);

    static void fc_fetch_handler(void *__this, vp::clock_event *event);
    static void fsm_handler(void *__this, vp::clock_event *event);

    vp::trace trace;
    vp::io_slave apb_itf;
    vp::io_slave fc_data_in_itf;
    vp::io_slave fc_fetch_in_itf;
    vp::io_slave axi_in_itf;
    vp::io_master out_itf;
    vp_regmap_xip regmap;
    udma_refill_req_t refill_req;
    vp::io_req *refill_pending_reqs_first = {0};
    vp::io_req *refill_pending_reqs_last;

    xip_page_t xip_pages[XIP_PAGES_NB] = {0};
    xip_area_t xip_areas[XIP_AREAS_NB] = {0};

    uint32_t page_available[XIP_AREAS_NB] = {0}; // page that are usable by per
    uint32_t page_mask = {0}; // pages that are not in refill
    uint32_t page_status[XIP_AREAS_NB] = {0}; // used or not used (for lru)
    uint32_t page_flag[XIP_AREAS_NB] = {0}; // To reverse search when full
    uint32_t page_last_avail[XIP_AREAS_NB] = {0}; // To reverse search when full
    uint32_t current_page[XIP_AREAS_NB] = {0}; // To reverse search when full

    uint32_t last_lru = 0;

    std::vector<vp::wire_master<udma_refill_req_t *> *>     refill_itfs;

    vp::io_req *fc_pending_req;

    Mem_plug *mem_plug;

    vp::clock_event *fc_fetch_event;
    vp::clock_event *fsm_event;

    bool pending_refill = false;
};


Xip::Xip(js::config *config)
    : vp::component(config)
{
}


vp::io_req_status_e Xip::apb_req(void *__this, vp::io_req *req)
{
    Xip *_this = (Xip *)__this;
    if (_this->regmap.access(req->get_addr(), req->get_size(), req->get_data(), req->get_is_write()))
    {
        return vp::IO_REQ_INVALID;
    }

    return vp::IO_REQ_OK;
}

void Xip::cfg_page_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int id)
{
    // Factorized handler
    if (is_write)
    {
        uint32_t cfg_page = *(uint32_t*)value;
        this->xip_pages[id].active   = (cfg_page >> 29) & 0x1;
        this->xip_pages[id].per_id  = (cfg_page >> 30) & 0x3;
        this->xip_pages[id].l2_addr = 0x1c000000 | (cfg_page & 0x1FFFFF);
        this->xip_pages[id].valid = false; // page is init
        this->page_available[this->xip_pages[id].per_id] |= 1 << id;
    }
}

void Xip::cfg_page0_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page0.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 0);
}

void Xip::cfg_page1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page1.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 1);
}

void Xip::cfg_page2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page2.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 2);
}

void Xip::cfg_page3_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page3.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 3);
}

void Xip::cfg_page4_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page4.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 4);
}

void Xip::cfg_page5_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page5.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 5);
}

void Xip::cfg_page6_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page6.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 6);
}

void Xip::cfg_page7_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page7.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 7);
}

void Xip::cfg_page8_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page8.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 8);
}

void Xip::cfg_page9_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page9.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 9);
}

void Xip::cfg_page10_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page10.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 10);
}

void Xip::cfg_page11_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page11.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 11);
}

void Xip::cfg_page12_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page12.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 12);
}

void Xip::cfg_page13_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page13.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 13);
}

void Xip::cfg_page14_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page14.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 14);
}

void Xip::cfg_page15_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
        this->regmap.cfg_page15.update(reg_offset, size, value, is_write);
        this->cfg_page_req(reg_offset, size, value, is_write, 15);
}

void Xip::cfg_virt_addr_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int id)
{
    if(is_write)
    {
        uint32_t cfg_virt_addr = *(uint32_t*)value;
        this->xip_areas[id].start_addr = cfg_virt_addr;
        this->xip_areas[id].end_addr = this->xip_areas[id].start_addr
            + this->xip_areas[id].mnt_size * this->xip_areas[id].page_size;
    }
}

void Xip::cfg_virt_addr0_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_virt_addr0.update(reg_offset, size, value, is_write);
    this->cfg_virt_addr_req(reg_offset, size, value, is_write, 0);
}

void Xip::cfg_virt_addr1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_virt_addr1.update(reg_offset, size, value, is_write);
    this->cfg_virt_addr_req(reg_offset, size, value, is_write, 1);
}

void Xip::cfg_virt_addr2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_virt_addr2.update(reg_offset, size, value, is_write);
    this->cfg_virt_addr_req(reg_offset, size, value, is_write, 2);
}


void Xip::cfg_ext_addr_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int id)
{
    if(is_write)
    {
        uint32_t cfg_ext_addr = *(uint32_t*)value;
        this->xip_areas[id].ext_addr = cfg_ext_addr;
    }
}

void Xip::cfg_ext_addr0_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_ext_addr0.update(reg_offset, size, value, is_write);
    this->cfg_ext_addr_req(reg_offset, size, value, is_write, 0);
}

void Xip::cfg_ext_addr1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_ext_addr1.update(reg_offset, size, value, is_write);
    this->cfg_ext_addr_req(reg_offset, size, value, is_write, 1);
}

void Xip::cfg_ext_addr2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_ext_addr2.update(reg_offset, size, value, is_write);
    this->cfg_ext_addr_req(reg_offset, size, value, is_write, 2);
}

void Xip::cfg_mnt_size_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int id)
{
    if(is_write)
    {
        uint32_t cfg_mnt_size = *(uint32_t*)value;
        this->xip_areas[id].mnt_size = cfg_mnt_size;
        this->xip_areas[id].end_addr = this->xip_areas[id].start_addr
            + this->xip_areas[id].mnt_size * this->xip_areas[id].page_size;
    }
}

void Xip::cfg_mnt_size0_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_mnt_size0.update(reg_offset, size, value, is_write);
    this->cfg_mnt_size_req(reg_offset, size, value, is_write, 0);
}

void Xip::cfg_mnt_size1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_mnt_size1.update(reg_offset, size, value, is_write);
    this->cfg_mnt_size_req(reg_offset, size, value, is_write, 1);
}

void Xip::cfg_mnt_size2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_mnt_size2.update(reg_offset, size, value, is_write);
    this->cfg_mnt_size_req(reg_offset, size, value, is_write, 2);
}

void Xip::cfg_page_size_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write, int id)
{
    if(is_write)
    {
        uint32_t cfg_page_size = *(uint32_t*)value;
        this->xip_areas[id].page_size = (1<<cfg_page_size) << 9;
        this->xip_areas[id].end_addr = this->xip_areas[id].start_addr
            + this->xip_areas[id].mnt_size * this->xip_areas[id].page_size;
    }
}

void Xip::cfg_page_size0_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_page_size0.update(reg_offset, size, value, is_write);
    this->cfg_page_size_req(reg_offset, size, value, is_write, 0);
}

void Xip::cfg_page_size1_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_page_size1.update(reg_offset, size, value, is_write);
    this->cfg_page_size_req(reg_offset, size, value, is_write, 1);
}

void Xip::cfg_page_size2_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.cfg_page_size2.update(reg_offset, size, value, is_write);
    this->cfg_page_size_req(reg_offset, size, value, is_write, 2);
}

vp::io_req_status_e Xip::fc_data_req(void *__this, vp::io_req *req)
{
    Xip *_this = (Xip *)__this;

    // L2 req addr: req->get_addr()
    // L2 req data: req->get_data()
    // L2 req size: req->get_size()
    // L2 req is write: req->req->get_is_write()
    uint32_t virt_addr = req->get_addr();
    int32_t per_id = _this->match_per(virt_addr);
    if(per_id < 0)
    {
        vp_warning_always(&_this->warning, "Accessing invalid peripheral (addr: 0x%x)\n", virt_addr);
        // TODO this should trigger an mcause exception on the core
        return vp::IO_REQ_INVALID;
    }
    uint32_t base = _this->xip_areas[per_id].ext_addr;
    uint32_t page_size = _this->xip_areas[per_id].page_size;
    uint32_t page_offset = virt_addr & ~((page_size-1)|(0x2<<28));
    uint32_t in_offset = virt_addr & (page_size-1);
    int32_t page_id = _this->match_page(page_offset, per_id);
    if(page_id < 0)
    {// miss

        vp::io_req *first = _this->refill_pending_reqs_first;

        if (first)
        {
            _this->refill_pending_reqs_last->set_next(req);
        }
        else
        {
            _this->refill_pending_reqs_first = req;
        }

        _this->refill_pending_reqs_last = req;
        req->set_next(NULL);

        if (!first)
        {
            page_id = _this->find_lru(per_id);
            
            udma_refill_req_t *refill_req = &refill_req[per_id];
            refill_req->ext_addr = in_offset + base;
            refill_req->l2_addr = _this->xip_pages[page_id].l2_addr;
            refill_req->size = page_size;
            refill_req->is_write = req->get_is_write();
            refill_req->req = req;
            refill_req->l2_itf = &(_this->out_itf);
            _this->refill_itfs[per_id]->sync(refill_req);
        }
    }
    else
    {// hit
        vp::io_req l2_req;
        l2_req.prepare();
        l2_req.set_addr(in_offset + _this->xip_pages[page_id].l2_addr);
        l2_req.set_size(req->get_size());
        int err = 0;
        if(req->get_is_write())
        {
            // TODO: fetch from L2
            //req->set_data(/*L2 DATA HERE*/); 
            // Prepare L2 request
            l2_req.set_is_write(true);
            err = _this->out_itf.req(&l2_req);
        }
        else
        {
            err = _this->out_itf.req(&l2_req);
            req->set_data(l2_req.get_data());
        }
        _this->trace.msg(vp::trace::LEVEL_TRACE, "Reading from memory (addr: 0x%x, size: %d)\n",
                l2_req.get_addr(), l2_req.get_size());

        if (err == vp::IO_REQ_OK)
        {
            return vp::IO_REQ_OK;
        }
        else
        {
            _this->trace.warning("UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
        }
    }

    return vp::IO_REQ_PENDING;
}


vp::io_req_status_e Xip::axi_req(void *__this, vp::io_req *req)
{
    Xip *_this = (Xip *)__this;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "AXI request (addr: 0x%x, size: 0x%x)\n", 
        req->get_addr(), req->get_size());

    return vp::IO_REQ_PENDING;
}

void Xip::fsm_handler(void *__this, vp::clock_event *event)
{
    Xip *_this = (Xip *)__this;
    if (!_this->pending_refill && _this->refill_pending_reqs_first)
    {
        vp::io_req *req = _this->refill_pending_reqs_first;
        _this->refill_pending_reqs_first = req->get_next();

        _this->fc_fetch_req(_this, req);
    }
}


void Xip::check_state()
{
    if (this->refill_pending_reqs_first && !this->pending_refill)
    {
        if (!this->fsm_event->is_enqueued())
        {
            this->event_enqueue(this->fsm_event, 1);
        }

    }
}


vp::io_req_status_e Xip::fc_fetch_req(void *__this, vp::io_req *req)
{
    Xip *_this = (Xip *)__this;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "FC fetch request (addr: 0x%x, size: 0x%x)\n", 
        req->get_addr(), req->get_size());

    uint32_t virt_addr = req->get_addr();
    int32_t per_id = _this->match_per(virt_addr);
    if(per_id < 0)
    {
        vp_warning_always(&_this->warning, "Accessing invalid peripheral (addr: 0x%x)\n", virt_addr);
        // TODO this should trigger an mcause exception on the core
        return vp::IO_REQ_INVALID;
    }

    uint32_t base = _this->xip_areas[per_id].ext_addr;
    uint32_t page_size = _this->xip_areas[per_id].page_size;
    uint32_t page_offset = virt_addr & ~((page_size-1)|(0x2<<28));
    uint32_t in_offset = virt_addr & (page_size-1);
    int32_t page_id = _this->match_page(page_offset, per_id);

    if(page_id < 0 || _this->pending_refill)
    {// miss
        if (_this->refill_pending_reqs_first)
        {
            _this->refill_pending_reqs_last->set_next(req);
        }
        else
        {
            _this->refill_pending_reqs_first = req;
        }

        _this->refill_pending_reqs_last = req;
        req->set_next(NULL);

        if (!_this->pending_refill)
        {
            udma_refill_req_t *refill_req = &_this->refill_req;

            page_id = _this->find_lru(per_id);
            _this->page_mask &= ~(1<<page_id); // clr page mask immediately

            _this->pending_refill = true;
            refill_req->ext_addr = page_offset + base;
            refill_req->l2_addr = _this->xip_pages[page_id].l2_addr;
            refill_req->size = page_size;
            refill_req->is_write = req->get_is_write();
            refill_req->req = req;
            refill_req->l2_itf = &(_this->out_itf);

            _this->xip_pages[page_id].per_id = per_id;
            _this->xip_pages[page_id].page_offset = page_offset;
            _this->xip_pages[page_id].valid = true;
            
            _this->trace.msg(vp::trace::LEVEL_DEBUG, "FC fetch miss, refilling page (addr: 0x%x, size: 0x%x, page: %d, refill_addr: 0x%x)\n", 
                req->get_addr(), req->get_size(), page_id, refill_req->l2_addr);

            _this->refill_itfs[per_id]->sync(refill_req);
        }
    }
    else
    {// hit
        // Now handle the request. Juste register it and the FSM will periodically access the L2

        Xip_mem_plug_req *plug_req = new Xip_mem_plug_req(
            _this->xip_pages[page_id].l2_addr + (req->get_addr() & (_this->xip_areas[per_id].page_size-1)),
            req->get_data(),
            req->get_size(),
            req->get_is_write(),
            _this,
            req
        );

        _this->mem_plug->enqueue(plug_req);
    }

    return vp::IO_REQ_PENDING;
}

int32_t Xip::find_lru(uint32_t per_id)
{
    #if 0
    uint32_t page_flag = this->page_flag[per_id];
    uint32_t page_avail = this->page_available[per_id];
    uint32_t page_status = this->page_status[per_id];
    int32_t lru = 0;

    while(((page_avail >> lru)&0x1) 
            && (((page_status >> lru)&0x1) != page_flag)
            && ((this->page_mask>>lru)&0x1))
    {
        lru++;
    }
    #else
    uint32_t page_avail = this->page_available[per_id];
    int32_t lru = this->last_lru;

    while(((page_avail >> lru) & 0x1) == 0)
    {
        lru++;
        if (lru == XIP_PAGES_NB)
        {
            lru = 0;
        }
    }

    this->last_lru = (lru + 1) % XIP_PAGES_NB;

    return lru;
    #endif
    return lru;
}



void Xip::fc_fetch_handler(void *__this, vp::clock_event *event)
{
    vp::io_req *req = (vp::io_req *)event->get_args()[0];
    req->get_resp_port()->resp(req);

#if 0
    // pop and exec next req
    udma_refill_req_t *req_next = _this->pop_req(per_id);
    if(req_next)
    {// push it in the pipe
        _this->refill_itfs[per_id]->sync(req_next);
    }
#endif
}


void Xip::refill_reply(void *__this, udma_refill_req_t *req)
{
    Xip *_this = (Xip *)__this;

    _this->trace.msg(vp::trace::LEVEL_TRACE, "Received refill reply\n");

    _this->pending_refill = false;

    _this->check_state();
}

// return matching area if there is on, -1 otherwise
int32_t Xip::match_per(uint32_t addr)
{
    for(int i = 0; i< XIP_AREAS_NB; i++)
    {
        if((addr >= this->xip_areas[i].start_addr) && (addr < this->xip_areas[i].end_addr))
        {
            return i;
        }
    }
    return -1;
}

// return matching area if there is on, -1 otherwise
int32_t Xip::match_page(uint32_t page_offset, uint32_t per_id)
{
    for(int i = 0; i< XIP_PAGES_NB; i++)
    {
        if(this->xip_pages[i].valid && (this->xip_pages[i].per_id == per_id) && (this->xip_pages[i].page_offset == page_offset))
        {
            return i;
        }
    }
    return -1;
}

int Xip::build()
{
    traces.new_trace("trace", &trace, vp::DEBUG);

    apb_itf.set_req_meth(&Xip::apb_req);
    new_slave_port("apb_input", &apb_itf);

    fc_data_in_itf.set_req_meth(&Xip::fc_data_req);
    new_slave_port("fc_data_input", &fc_data_in_itf);

    new_master_port("output", &out_itf);

    fc_fetch_in_itf.set_req_meth(&Xip::fc_fetch_req);
    new_slave_port("fc_fetch_input", &fc_fetch_in_itf);

    axi_in_itf.set_req_meth(&Xip::axi_req);
    new_slave_port("axi_input", &axi_in_itf);

    this->regmap.build(this, &this->trace, "xip");

    this->regmap.cfg_page0.register_callback(std::bind(&Xip::cfg_page0_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page1.register_callback(std::bind(&Xip::cfg_page1_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page2.register_callback(std::bind(&Xip::cfg_page2_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page3.register_callback(std::bind(&Xip::cfg_page3_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page4.register_callback(std::bind(&Xip::cfg_page4_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page5.register_callback(std::bind(&Xip::cfg_page5_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page6.register_callback(std::bind(&Xip::cfg_page6_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page7.register_callback(std::bind(&Xip::cfg_page7_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page8.register_callback(std::bind(&Xip::cfg_page8_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page9.register_callback(std::bind(&Xip::cfg_page9_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page10.register_callback(std::bind(&Xip::cfg_page10_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page11.register_callback(std::bind(&Xip::cfg_page11_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page12.register_callback(std::bind(&Xip::cfg_page12_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page13.register_callback(std::bind(&Xip::cfg_page13_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page14.register_callback(std::bind(&Xip::cfg_page14_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page15.register_callback(std::bind(&Xip::cfg_page15_req, this, _1, _2, _3, _4));

    
    this->regmap.cfg_virt_addr0.register_callback(std::bind(&Xip::cfg_virt_addr0_req, this, _1, _2, _3, _4));
    this->regmap.cfg_virt_addr1.register_callback(std::bind(&Xip::cfg_virt_addr1_req, this, _1, _2, _3, _4));
    this->regmap.cfg_virt_addr2.register_callback(std::bind(&Xip::cfg_virt_addr2_req, this, _1, _2, _3, _4));
    this->regmap.cfg_ext_addr0.register_callback(std::bind(&Xip::cfg_ext_addr0_req, this, _1, _2, _3, _4));
    this->regmap.cfg_ext_addr1.register_callback(std::bind(&Xip::cfg_ext_addr1_req, this, _1, _2, _3, _4));
    this->regmap.cfg_ext_addr2.register_callback(std::bind(&Xip::cfg_ext_addr2_req, this, _1, _2, _3, _4));
    this->regmap.cfg_mnt_size0.register_callback(std::bind(&Xip::cfg_mnt_size0_req, this, _1, _2, _3, _4));
    this->regmap.cfg_mnt_size1.register_callback(std::bind(&Xip::cfg_mnt_size1_req, this, _1, _2, _3, _4));
    this->regmap.cfg_mnt_size2.register_callback(std::bind(&Xip::cfg_mnt_size2_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page_size0.register_callback(std::bind(&Xip::cfg_page_size0_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page_size1.register_callback(std::bind(&Xip::cfg_page_size1_req, this, _1, _2, _3, _4));
    this->regmap.cfg_page_size2.register_callback(std::bind(&Xip::cfg_page_size2_req, this, _1, _2, _3, _4));

    for (int i=0; i<this->get_js_config()->get_child_int("nb_refill_itfs"); i++)
    {
        vp::wire_master<udma_refill_req_t *> *itf = new vp::wire_master<udma_refill_req_t *>();
        itf->set_sync_meth(&Xip::refill_reply);
        new_master_port("refill_" + std::to_string(i), itf);
        this->refill_itfs.push_back(itf);
    }

    this->mem_plug = new Mem_plug(this, this, "mem_plug", &this->out_itf, L2_PLUG_NB_PORTS,
        L2_PLUG_OUTPUT_LATENCY);

    this->fc_fetch_event = this->event_new(Xip::fc_fetch_handler);
    this->fsm_event = this->event_new(Xip::fsm_handler);

    return 0;
}

void Xip::reset(bool active)
{
}


Xip_mem_plug_req::Xip_mem_plug_req(uint32_t addr, uint8_t *data, uint32_t size, bool is_write,
    Xip *comp, vp::io_req *req)
    : Mem_plug_req(addr, data, size, is_write), comp(comp), req(req)
{
}

void Xip_mem_plug_req::handle_termination()
{
    this->req->get_resp_port()->resp(this->req);
    this->comp->check_state();
}

extern "C" vp::component *vp_constructor(js::config *config)
{
    return new Xip(config);
}
