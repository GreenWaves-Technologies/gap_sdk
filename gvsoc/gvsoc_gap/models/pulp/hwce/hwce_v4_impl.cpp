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
#include <string.h>
#include <archi/hwce/v4/hwce_regfields.h>
#include <archi/hwce/v4/hwce_gvsoc.h>

using namespace std::placeholders;

#define NB_MASTER_PORTS 4

#define HWCE_STATE_IDLE 0
#define HWCE_STATE_CONTEXT_COPY 1
#define HWCE_STATE_LOCKED 2

#define HWCE_JOBQUEUE_IDLE 0
#define HWCE_JOBQUEUE_FETCH_WEIGHTS 1
#define HWCE_JOBQUEUE_EXEC_CONV 3

#define HWCE_ACQUIRE_CONTEXT_COPY -3
#define HWCE_ACQUIRE_LOCKED -2
#define HWCE_ACQUIRE_QUEUE_FULL -1
#define HWCE_ACQUIRE_READY 0

#define HWCE_GEN_CONFIG0_CONV_5x5 0
#define HWCE_GEN_CONFIG0_CONV_3x3 1
#define HWCE_GEN_CONFIG0_CONV_4x7 2

#define HWCE_GEN_CONFIG0_VECT_1 0
#define HWCE_GEN_CONFIG0_VECT_2 1
#define HWCE_GEN_CONFIG0_VECT_4 2

#define HWCE_NB_IO_REGS 19

static string get_state_name(int state)
{
    switch (state)
    {
    case HWCE_STATE_IDLE:
        return "idle";
    case HWCE_STATE_CONTEXT_COPY:
        return "contextCopy";
    case HWCE_STATE_LOCKED:
        return "locked";
    }
    return "unknown";
}

static int getSignedValue(unsigned int val, int bits)
{
    return ((int)val) << (sizeof(int) * 8 - bits) >> (sizeof(int) * 8 - bits);
}

static int getCoeff(unsigned int coeff, int sel, int size, int signExtend)
{
    unsigned int value = (coeff >> (sel * size)) & ((1 << size) - 1);
    if (signExtend)
        return getSignedValue(value, size);
    else
        return value;
}

class hwce;

class hwce_job_t
{

public:
    hwce_job_t *next;
    int id;
    int run_id;

    int wstride;

    vp_hwce_y_trans_size *y_trans_size;
    vp_hwce_y_line_stride_length *y_line_stride_length;
    vp_hwce_y_feat_stride_length *y_feat_stride_length;
    vp_hwce_y_out_3_base_addr *y_out_3_base_addr;
    vp_hwce_y_out_2_base_addr *y_out_2_base_addr;
    vp_hwce_y_out_1_base_addr *y_out_1_base_addr;
    vp_hwce_y_out_0_base_addr *y_out_0_base_addr;
    vp_hwce_y_in_3_base_addr *y_in_3_base_addr;
    vp_hwce_y_in_2_base_addr *y_in_2_base_addr;
    vp_hwce_y_in_1_base_addr *y_in_1_base_addr;
    vp_hwce_y_in_0_base_addr *y_in_0_base_addr;
    vp_hwce_x_trans_size *x_trans_size;
    vp_hwce_x_line_stride_length *x_line_stride_length;
    vp_hwce_x_feat_stride_length *x_feat_stride_length;
    vp_hwce_x_in_base_addr *x_in_base_addr;
    vp_hwce_w_base_addr *w_base_addr;
    vp_hwce_job_config0 *job_config0;
    vp_hwce_job_config1 *job_config1;
};

class Hwce_base
{
public:
    Hwce_base(hwce *top, string name);
    void init(uint32_t base, int lineLen, int lineStride, int featLen, int featStride, int nbFeatures, int featLoop);
    uint32_t get();
    void update();
    bool reachedEof();
    void startFeature();

    int wordCount;

private:
    hwce *top;
    string name;
    uint32_t baseStart;
    uint32_t baseLoop;
    uint32_t base;
    int lineLen;
    int lineStride;
    int featLen;
    int featStride;
    int lineCount;
    int featCount;
    int featLoopCount;
    int nbFeatures;
    int featLoop;
    bool pendingEof;
};

class hwce : public vp::component
{
    friend class Hwce_base;

public:
    hwce(js::config *config);

    int build();
    void start();
    void reset(bool active);

private:
    static vp::io_req_status_e req(void *__this, vp::io_req *req);

    void set_state(int new_state);
    void enqueue_job();
    int alloc_job();
    void closeJob();
    void youtFlush();
    void execConvolution();
    void fetchXin();
    void fetchYin();
    int64_t getSaturated(int sat, int sign, int size, int64_t value, int *isSat);
    bool portAccess(int port, uint32_t addr, uint8_t *data, int size, bool isRead, int64_t *latency);

    static void job_queue_handle(void *__this, vp::clock_event *event);
    static void ctrl_handle(void *__this, vp::clock_event *event);
    void acquire_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void trigger_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
    void gen_config0_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write);

    vp::reg *y_trans_size_alias();
    vp::reg *y_line_stride_length_alias();
    vp::reg *y_feat_stride_length_alias();
    vp::reg *y_out_3_base_addr_alias();
    vp::reg *y_out_2_base_addr_alias();
    vp::reg *y_out_1_base_addr_alias();
    vp::reg *y_out_0_base_addr_alias();
    vp::reg *y_in_3_base_addr_alias();
    vp::reg *y_in_2_base_addr_alias();
    vp::reg *y_in_1_base_addr_alias();
    vp::reg *y_in_0_base_addr_alias();
    vp::reg *x_trans_size_alias();
    vp::reg *x_line_stride_length_alias();
    vp::reg *x_feat_stride_length_alias();
    vp::reg *x_in_base_addr_alias();
    vp::reg *w_base_addr_alias();
    vp::reg *job_config0_alias();
    vp::reg *job_config1_alias();


    void start_job(hwce_job_t *job);
    int allocPort();

    vp::trace trace;

    vp::io_slave in;
    vp::io_master out[NB_MASTER_PORTS];
    vp::wire_master<bool> irq;
    vp::io_req reqs[NB_MASTER_PORTS];
    uint32_t data[NB_MASTER_PORTS];

    vp::clock_event *job_queue_event;
    vp::clock_event *ctrl_event;

    hwce_job_t jobs[2];

    int16_t weights[26]; // for conv 5x5 rounded to next word to simplify weights fetch
    uint16_t xin[30];    // 5x6 to simplify fetch

    vp_regmap_hwce regmap;

    int current_job;
    int state;
    int job_queue_state;
    unsigned int free_jobs;
    int job_id;
    int nbYoutValue;
    int youtValid;
    int lineBufferCurrentWidth;
    int nbReadyLines;
    int nbReadyWords;
    int convCurrentPosition;
    int nbValidWeights;
    int expectedWeights;
    int filterSizeX;
    int filterSizeY;
    int nbValidXin;
    int nbAccess;
    int64_t lastAccessTime;
    uint32_t youtValue[4];
    uint32_t yinValue[4];
    int nbYinValid;
    int yinValid;
    uint32_t *lineBuffer;
    int nbFinished;
    bool evtEnable;

    uint32_t weights_base;
    uint32_t weights_base_start;
    uint32_t x_in_size;
    uint32_t y_in_size;
    uint32_t x_out_size;
    uint32_t x_size;
    uint32_t x_base;
    int y_in_take_first;
    int y_out_take_first;

    hwce_job_t *first_job;
    hwce_job_t *last_job;

    hwce_job_t *pending_job;
    hwce_job_t *prev_job;
    hwce_job_t *allocated_job;

    Hwce_base *xinBase;
    Hwce_base *youtBase[4];
    Hwce_base *yinBase[4];

    int lineBufferWidth = 32;
    int lineBufferHeight = 5;
    int nbMasterPorts = 4;

    uint32_t addr_mask = 0x0FFFFFFF;

    vp::trace event_busy;
    vp::trace event_conv;
    vp::trace event_conv_value;
};

hwce::hwce(js::config *config)
    : vp::component(config)
{
}

void hwce::reset(bool active)
{
    if (active)
    {
        this->current_job = 0;
        this->state = HWCE_STATE_IDLE;

        this->first_job = NULL;
        this->last_job = NULL;
        this->prev_job = NULL;
        this->pending_job = NULL;
        this->allocated_job = NULL;

        this->free_jobs = 0x3;
        this->job_id = 0;
        this->nbValidWeights = 0;
        this->nbAccess = 0;
        this->lastAccessTime = 0;

        this->nbYinValid = 0;
        this->yinValid = 0;
        this->nbFinished = 0;
        this->evtEnable = true;

        this->event_conv_value.event(NULL);
        this->event_conv.event(NULL);
        this->event_busy.event(NULL);
    }
    else
    {
        int32_t zero = 0;
        this->event_conv_value.event((uint8_t *)&zero);
        this->event_conv.event((uint8_t *)&zero);
        this->event_busy.event((uint8_t *)&zero);
    }
}

int hwce::allocPort()
{
    if (this->get_cycles() > this->lastAccessTime)
    {
        this->lastAccessTime = this->get_cycles();
        this->nbAccess = 0;
    }
    if (this->nbAccess == this->nbMasterPorts)
        return -1;
    return this->nbAccess++;
}

int hwce::alloc_job()
{
    if (this->free_jobs == 0)
        return -1;

    for (int i = 0; i < 32; i++)
    {
        if ((this->free_jobs >> i) & 1)
        {
            this->trace.msg("Allocated job (jobId: %d)\n", i);
            this->free_jobs &= ~(1 << i);
            this->current_job = i;
            this->jobs[i].run_id = this->job_id++;
            if (this->job_id == 256)
                this->job_id = 0;
            return i;
        }
    }
    return -1;
}

void hwce::start_job(hwce_job_t *job)
{
    uint8_t one = 1;
    this->event_busy.event(&one);

    job->wstride = this->regmap.gen_config0.wstride_get();

    this->pending_job = job;
    this->job_queue_state = HWCE_JOBQUEUE_FETCH_WEIGHTS;

    this->weights_base = job->w_base_addr->addr_get();
    this->weights_base_start = this->weights_base;

    this->x_in_size = job->x_trans_size->size_get();
    this->x_out_size = job->y_trans_size->size_get() * (1 << this->regmap.gen_config0.vect_get());
    this->y_in_size = job->y_trans_size->size_get() * (1 << this->regmap.gen_config0.vect_get());
    this->y_in_take_first = 1;
    this->y_out_take_first = 1;

    this->xinBase->init(job->x_in_base_addr->get(), job->x_line_stride_length->length_get(), job->x_line_stride_length->stride_get() / 4, job->x_feat_stride_length->length_get(), job->x_feat_stride_length->stride_get() / 4, job->job_config1->wif_param_get(), 0);

    for (int i = 0; i < (1 << this->regmap.gen_config0.vect_get()); i++)
    {
        uint32_t yin_base = i == 0 ? job->y_in_0_base_addr->get() : i == 1 ? job->y_in_1_base_addr->get() : i == 2 ? job->y_in_2_base_addr->get() : job->y_in_3_base_addr->get();
        uint32_t yout_base = i == 0 ? job->y_out_0_base_addr->get() : i == 1 ? job->y_out_1_base_addr->get() : i == 2 ? job->y_out_2_base_addr->get() : job->y_out_3_base_addr->get();

        this->yinBase[i]->init(yin_base, job->y_line_stride_length->length_get(), job->y_line_stride_length->stride_get() / 4, job->y_feat_stride_length->length_get(), job->y_feat_stride_length->stride_get() / 4, job->job_config1->wof_param_get(), job->job_config1->wif_param_get());
        this->youtBase[i]->init(yout_base, job->y_line_stride_length->length_get(), job->y_line_stride_length->stride_get() / 4, job->y_feat_stride_length->length_get(), job->y_feat_stride_length->stride_get() / 4, job->job_config1->wof_param_get(), job->job_config1->wif_param_get());
    }

    this->nbYoutValue = 0;
    this->youtValid = 0;

    this->lineBufferCurrentWidth = this->lineBufferWidth;
    if (this->lineBufferCurrentWidth > job->x_line_stride_length->length_get())
        this->lineBufferCurrentWidth = job->x_line_stride_length->length_get();
    this->nbReadyLines = 0;
    this->nbReadyWords = 0;
    this->convCurrentPosition = 0;
    this->trace.msg("Starting job (xoutSize: 0x%x, xinBase: 0x%x, youtBase: 0x%x, xinSize: 0x%x, xinLineStride: 0x%x, xinLineLen: 0x%x, xinFeatStride: 0x%x, xinFeatLen: 0x%x, )\n", this->x_out_size, this->xinBase->get(), this->youtBase[0]->get(), this->x_in_size, job->x_line_stride_length->stride_get() / 4, job->x_line_stride_length->length_get(), job->x_feat_stride_length->stride_get() / 4, job->x_feat_stride_length->length_get());

    if (this->x_out_size == 0)
        this->warning.force_warning("Trying to start job with 0 output size\n");
    if (this->x_in_size == 0)
        this->warning.force_warning("Trying to start job with 0 input size\n");

    if (!this->job_queue_event->is_enqueued())
        this->event_enqueue(this->job_queue_event, 1);
}


void hwce::set_state(int new_state)
{
    this->trace.msg("Setting new state (new_state: %s)\n", get_state_name(new_state).c_str());
    this->state = new_state;
}


void hwce::trigger_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    if (is_write)
    {
        if (this->state != HWCE_STATE_LOCKED)
        {
            this->warning.force_warning("Trying to trigger job while it is not locked (current_state: %s)\n", get_state_name(state).c_str());
            return;
        }

        this->enqueue_job();
        this->set_state(HWCE_STATE_IDLE);
    }

}


void hwce::acquire_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    if (!is_write)
    {
        switch (this->state)
        {
        case HWCE_STATE_IDLE:
        {
            int job = this->alloc_job();
            if (job == -1)
            {
                *data = HWCE_ACQUIRE_QUEUE_FULL;
                this->warning.force_warning("Trying to acquire job while job queue is full\n");
            }
            else
            {
                *data = jobs[job].run_id;
                this->set_state(HWCE_STATE_LOCKED);
                this->prev_job = this->allocated_job;
                this->allocated_job = &this->jobs[job];
                // If this is enabled perform the context copy from the old one to the new one
                // This will change the state bacl to LOCKED in a few cycles (one register copy per cycle)
                if (!this->regmap.gen_config0.ncp_get())
                {
                    this->set_state(HWCE_STATE_CONTEXT_COPY);
                    this->event_enqueue(this->ctrl_event, HWCE_NB_IO_REGS + 1);
                }
            }
        }
        break;

        case HWCE_STATE_CONTEXT_COPY:
            *data = HWCE_ACQUIRE_CONTEXT_COPY;
            break;

        case HWCE_STATE_LOCKED:
            *data = HWCE_ACQUIRE_LOCKED;
            break;
        }
    }
}


void hwce::gen_config0_req(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
{
    this->regmap.gen_config0.update(reg_offset, size, value, is_write);

    if (is_write)
    {
        switch (this->regmap.gen_config0.conv_get())
        {
        case HWCE_GEN_CONFIG0_CONV_5x5:
            this->filterSizeX = 5;
            this->filterSizeY = 5;
            this->expectedWeights = 26;
            break;

        case HWCE_GEN_CONFIG0_CONV_4x7:
            this->filterSizeX = 7;
            this->filterSizeY = 4;
            this->expectedWeights = 28;
            break;

        case HWCE_GEN_CONFIG0_CONV_3x3:
            this->filterSizeX = 3;
            this->filterSizeY = 3;
            if (this->regmap.gen_config0.vect_get() == HWCE_GEN_CONFIG0_VECT_1)
                this->expectedWeights = 10;
            else if (this->regmap.gen_config0.vect_get() == HWCE_GEN_CONFIG0_VECT_2)
                this->expectedWeights = 18;
            else if (this->regmap.gen_config0.vect_get() == HWCE_GEN_CONFIG0_VECT_4)
                this->expectedWeights = 28;
            break;
        }
    }
}


vp::io_req_status_e hwce::req(void *__this, vp::io_req *req)
{
    hwce *_this = (hwce *)__this;

    if (_this->regmap.access(req->get_addr(), req->get_size(), req->get_data(), req->get_is_write()))
    {
        return vp::IO_REQ_INVALID;
    }

    return vp::IO_REQ_OK;
}


void hwce::youtFlush()
{
    if (this->nbYoutValue == 0)
        return;

    for (int i = 0; i < (1 << this->regmap.gen_config0.vect_get()); i++)
    {
        if ((this->youtValid >> i) & 1)
        {
            int port = allocPort();
            if (port == -1)
                return;

            this->trace.msg("Flushing yout%d (addr: 0x%x, value: 0x%x, port: %d, reaminingSize: %d)\n", i, this->youtBase[i]->get(), this->youtValue[i], port, this->x_out_size);
            int64_t latency;
            bool err = portAccess(port, youtBase[i]->get(), (uint8_t *)&youtValue[i], 4, 0, &latency);
            if (err)
            {
                this->trace.msg("Got bus error while storing yout\n");
            }
            this->youtBase[i]->update();

            if (this->youtBase[i]->wordCount == 0)
            {
                this->trace.msg("Finished processing one buffer line, freeing the line\n");
                this->convCurrentPosition = 0;
                this->nbReadyLines--;
                uint32_t temp[lineBufferWidth * 4];
                memcpy((void *)temp, (void *)&this->lineBuffer[lineBufferWidth], this->lineBufferWidth * 4 * 4);
                memcpy((void *)this->lineBuffer, (void *)temp, this->lineBufferWidth * 4 * 4);
            }

            this->youtValid &= ~(1 << i);
            this->nbYoutValue--;
            this->x_out_size--;
            if (this->x_out_size == 0)
            {
                this->trace.msg("Finished convolution, freeing job\n");
                this->closeJob();
            }
        }
    }
}


void hwce::closeJob()
{
    this->trace.msg("Freeing job (jobId: %d)\n", this->pending_job->id);
    this->nbFinished++;
    if (this->evtEnable)
        this->irq.sync(true);
    this->job_queue_state = HWCE_JOBQUEUE_IDLE;
    this->free_jobs |= 1 << (this->pending_job->id);

    uint8_t zero = 0;
    this->event_busy.event(&zero);
    this->event_conv.event(&zero);
}

int64_t hwce::getSaturated(int sat, int sign, int size, int64_t value, int *isSat)
{
    if (!sat)
        return value;
    else
    {
        int64_t maxVal;
        int64_t minVal;
        if (sign)
        {
            maxVal = (1 << (size - 1)) - 1;
            minVal = -(1 << (size - 1));
        }
        else
        {
            maxVal = (1 << size) - 1;
            minVal = 0;
        }
        if (value > maxVal)
        {
            *isSat = 1;
            this->trace.msg("Saturating to max value (value: 0x%x, maxValue: 0x%x)\n", value, maxVal);
            return maxVal;
        }
        if (value < minVal)
        {
            *isSat = 1;
            this->trace.msg("Saturating to min value (value: 0x%x, minValue: 0x%x)\n", value, minVal);
            return minVal;
        }
        return value;
    }
}


void hwce::execConvolution()
{
    // Execute the convolution only if:
    // - We have number of lines equal to filter size
    // - Or equal to filter size - 1 with enough words on last line to feed convolution
    if (this->nbReadyLines < this->filterSizeY && (this->nbReadyLines < this->filterSizeY - 1 || this->nbReadyWords - this->convCurrentPosition < this->filterSizeX))
        return;

    if ((!this->regmap.gen_config0.ny_get() && this->nbYinValid < (1 << this->regmap.gen_config0.vect_get())) || this->nbYoutValue != 0)
        return;

    this->trace.msg("Executing convolution (position: 0x%x)\n", this->convCurrentPosition);

    int64_t result[4] = {0, 0, 0, 0};
    int16_t *xin = (int16_t *)this->lineBuffer;
    xin += this->convCurrentPosition;
    for (int i = 0; i < this->filterSizeY; i++)
    {
        for (int j = 0; j < this->filterSizeX; j++)
        {
            uint32_t weight = this->weights[i * this->filterSizeX + j];
            for (int k = 0; k < (1 << this->regmap.gen_config0.vect_get()); k++)
            {
                int16_t coeff = getCoeff(weight, k, 16 / (1 << this->regmap.gen_config0.vect_get()), !this->regmap.gen_config0.uns_get());
                if (this->regmap.gen_config0.uns_get())
                {
                    result[k] += (uint16_t)coeff * (uint16_t)xin[j];
                }
                else
                {
                    result[k] += coeff * xin[j];
                }
            }
        }
        xin += lineBufferWidth * 2;
    }

    for (int i = 0; i < (1 << this->regmap.gen_config0.vect_get()); i++)
    {
        // Convolution computes on 16bits, choose the right part
        int64_t data;
        int sat = 0;

        if (this->regmap.gen_config0.ny_get())
            data = (int16_t)this->pending_job->job_config0->noyconst_get();
        else
            data = this->y_in_take_first ? getSignedValue(this->yinValue[i] & 0xffff, 16) : getSignedValue(this->yinValue[i] >> 16, 16);

        data = (data << this->regmap.gen_config0.qf_get()) + result[i];

        if (this->regmap.gen_config0.qf_get() > 0)
        {
            data = (data + (1 << (this->regmap.gen_config0.qf_get() - 1))) >> this->regmap.gen_config0.qf_get();
        }
        int64_t outDataSat = getSaturated(1, !this->regmap.gen_config0.uns_get(), 16, data, &sat);

        this->trace.msg("Computed convolution yout%d (value: 0x%4.4x, yin%d/const: 0x%4.4x, conv: 0x%8.8x)\n", i, outDataSat, i, data, result[i]);
        if (this->y_out_take_first)
            this->youtValue[i] = outDataSat & 0xffff;
        else
            this->youtValue[i] |= outDataSat << 16;

        uint32_t conv_value = outDataSat & 0xffff;
        this->event_conv_value.event((uint8_t *)&conv_value);
        uint8_t one = 1, zero = 0;
        this->event_conv.event_pulse(this->get_period(), &one, &zero);
        //gv_vcd_dump(&vcdConv[i], (uint8_t *)&outDataSat);
        //gv_vcd_enqueue(&vcdConv[i], NULL, getEngine()->getCycles() + 1);
        //gv_vcd_dump(&vcdSat[i], (uint8_t *)&sat);
        //gv_vcd_enqueue(&vcdSat[i], NULL, getEngine()->getCycles() + 1);
    }

    this->convCurrentPosition++;
    if (this->convCurrentPosition / 2 == this->lineBufferCurrentWidth - this->filterSizeX / 2)
    {
    }

    this->y_out_take_first ^= 1;
    if (this->y_out_take_first)
    {
        this->youtValid = (1 << (1 << this->regmap.gen_config0.vect_get())) - 1;
        this->nbYoutValue += 1 << this->regmap.gen_config0.vect_get();
    }

    // Select next 16bits part for next time or invalidate word if it is fully processed
    this->y_in_take_first ^= 1;
    if (y_in_take_first)
    {
        this->nbYinValid = 0;
        this->yinValid = 0;
    }
}


bool hwce::portAccess(int port, uint32_t addr, uint8_t *data, int size, bool isRead, int64_t *latency)
{
    vp::io_req *req = &this->reqs[port];

    req->set_addr(addr & this->addr_mask);
    req->set_size(size);
    req->set_data(data);
    req->set_is_write(!isRead);

    vp::io_req_status_e err = this->out[port].req(req);

    if (err != vp::IO_REQ_OK)
    {
        this->warning.force_warning("Got bus error while getting data\n");
    }

    //gv_vcd_dump(&vcdPortAddr[port], (uint8_t *)&addr);
    // TODO once the access latency is taken into account by the model, add it to the enqueue
    //gv_vcd_enqueue(&vcdPortAddr[port], NULL, getEngine()->getCycles() + /* req.getLatency() + */ 1);

    //gv_vcd_dump(&vcdPortRead[port], (uint8_t *)&isRead);
    //gv_vcd_enqueue(&vcdPortRead[port], NULL, getEngine()->getCycles() + /* req.getLatency() + */ 1);

    //uint32_t value = 0;
    //memcpy((void *)&value, data, size);
    //gv_vcd_dump(&vcdPortData[port], (uint8_t *)&value);
    //gv_vcd_enqueue(&vcdPortData[port], NULL, getEngine()->getCycles() + /* req.getLatency() + */ 1);

    *latency = req->get_latency();

    return err;
}


void hwce::fetchXin()
{
    int port;
    if (this->x_in_size == 0 || this->nbReadyLines == this->lineBufferHeight || this->xinBase->reachedEof())
        return;
    if ((port = this->allocPort()) == -1)
        return;

    this->trace.msg("Fetching xin (addr: 0x%x, remainingSize: 0x%x, port: %d)\n", this->xinBase->get(), this->x_in_size, port);

    uint32_t data;
    int64_t latency;
    bool err = this->portAccess(port, this->xinBase->get(), (uint8_t *)&data, 4, 1, &latency);
    if (err)
    {
        this->warning.force_warning("Got bus error while fetching filter input\n");
    }
    int bufferIndex = this->nbReadyLines * this->lineBufferWidth + this->nbReadyWords;
    this->lineBuffer[bufferIndex] = data;
    this->trace.msg("Fetched filter input (value: 0x%x, latency: %d, bufferIndex: %d)\n", data, latency, bufferIndex);

    // Decrease global size, this will deactivate Xin fetching when it is over
    this->x_in_size--;

    // Update number of ready words / lines used to detect when a convolution can be executed
    this->nbReadyWords++;
    if (this->nbReadyWords == this->lineBufferCurrentWidth)
    {
        this->nbReadyLines++;
        this->nbReadyWords = 0;
        this->trace.msg("Finished filling-in one buffer line (nbReadyLines: %d)\n", this->nbReadyLines);
        if (this->nbReadyLines == this->lineBufferHeight)
            this->trace.msg("Finished filling-in whole buffer line\n");
    }

    this->xinBase->update();
}


void hwce::fetchYin()
{
    if (this->regmap.gen_config0.ny_get())
        return;
    if (this->y_in_size == 0 || this->nbYinValid == (1 << this->regmap.gen_config0.vect_get()))
        return;

    for (int i = 0; i < (1 << this->regmap.gen_config0.vect_get()); i++)
    {
        // Only get data if we have room for it and we are not waiting for a feature flush or we still have data to fetch for the current feature
        if (((this->yinValid >> i) & 1) == 0 && !this->yinBase[i]->reachedEof())
        {
            int port;
            if ((port = this->allocPort()) == -1)
                return;
            uint32_t data;
            this->trace.msg("Fetching yin%d (addr: 0x%x, remainingSize: 0x%x, port: %d)\n", i, this->yinBase[i]->get(), 0, port);

            int64_t latency;
            bool err = this->portAccess(port, this->yinBase[i]->get(), (uint8_t *)&data, 4, 1, &latency);
            if (err)
            {
                this->trace.msg("Got bus error while fetching yin\n");
            }
            this->yinValue[i] = data;
            this->y_in_size--;
            this->yinValid |= 1 << i;
            this->nbYinValid++;
            this->trace.msg("Fetched yin%d (value: 0x%x, latency: %d)\n", i, data, latency);

            this->yinBase[i]->update();
        }
    }
}


void hwce::job_queue_handle(void *__this, vp::clock_event *event)
{
    hwce *_this = (hwce *)__this;

    if (_this->job_queue_state == HWCE_JOBQUEUE_EXEC_CONV)
    {
        // The order is important as the FIFOs are not modeled
        // The output is first flushed to let convolution executes and stores a new result
        _this->youtFlush();

        // The convolution is executed before fetching data to compute only the cycle after the data has been received
        _this->execConvolution();

        _this->fetchXin();
        _this->fetchYin();

        bool reachedEof = true;
        for (int i = 0; i < (1 << _this->regmap.gen_config0.vect_get()); i++)
        {
            reachedEof &= _this->youtBase[i]->reachedEof();
        }

        if (reachedEof && _this->x_out_size)
        {
            // In case we are waiting for convolotion termination, change the state once there is nothing
            _this->trace.msg("Detected end of feature, switching to weight fetch\n");
            // to execute and everything is flushed
            for (int i = 0; i < (1 << _this->regmap.gen_config0.vect_get()); i++)
            {
                _this->youtBase[i]->startFeature();
                _this->yinBase[i]->startFeature();
            }
            _this->xinBase->startFeature();
            _this->job_queue_state = HWCE_JOBQUEUE_FETCH_WEIGHTS;
            _this->nbReadyLines = 0;

            _this->weights_base = _this->weights_base_start + _this->pending_job->wstride;
            _this->weights_base_start = _this->weights_base;
        }

        // TODO include latency from access
        if (!_this->job_queue_event->is_enqueued())
            _this->event_enqueue(_this->job_queue_event, 1);
    }
    else if (_this->job_queue_state == HWCE_JOBQUEUE_FETCH_WEIGHTS)
    {
        _this->trace.msg("Fetching weight (addr: 0x%x)\n", _this->weights_base);

        vp::io_req *req = &_this->reqs[0];

        req->set_addr(_this->weights_base & _this->addr_mask);
        req->set_data((uint8_t *)&_this->data[0]);
        req->set_size(4);
        req->set_is_write(false);

        vp::io_req_status_e err = _this->out[0].req(req);

        if (err != vp::IO_REQ_OK)
        {
            _this->warning.force_warning("Got bus error while fetching weights\n");
        }

        ((uint32_t *)_this->weights)[_this->nbValidWeights / 2] = _this->data[0];
        _this->trace.msg("Fetched weight (value: 0x%x, latency: %d)\n", _this->data[0], req->get_latency());

        _this->nbValidWeights += 2;
        _this->weights_base += 4;
        if (_this->nbValidWeights == _this->expectedWeights)
        {
            // The HWCE is using flipped kernels, do it now to keep clean Convolution
            if (!_this->regmap.gen_config0.nf_get())
            {
                int16_t tmp[_this->filterSizeX * _this->filterSizeY];
                memcpy(tmp, _this->weights, _this->filterSizeX * _this->filterSizeY * 2);
                for (int i = 0; i < _this->filterSizeX * _this->filterSizeY; i++)
                {
                    _this->weights[i] = tmp[_this->filterSizeX * _this->filterSizeY - 1 - i];
                }
            }
            _this->nbValidWeights = 0;
            _this->nbValidXin = 0;
            _this->job_queue_state = HWCE_JOBQUEUE_EXEC_CONV;
        }
        // TODO include latency from access
        if (!_this->job_queue_event->is_enqueued())
            _this->event_enqueue(_this->job_queue_event, 1);
    }
    else if (_this->job_queue_state == HWCE_JOBQUEUE_IDLE && _this->first_job)
    {
        hwce_job_t *job = _this->first_job;
        _this->trace.msg("Popping job from queue (job: %d)\n", job->id);
        _this->first_job = job->next;
        if (_this->first_job == NULL)
            _this->last_job = NULL;
        _this->start_job(job);
    }
}

void hwce::enqueue_job()
{
    hwce_job_t *job = &jobs[this->current_job];
    this->trace.msg("Enqueuing job (job_id: %d)\n", job->id);

    if (this->last_job)
        this->last_job->next = job;
    else
        this->first_job = job;
    job->next = NULL;
    if (!this->job_queue_event->is_enqueued())
        this->event_enqueue(this->job_queue_event, 1);
}

void hwce::ctrl_handle(void *__this, vp::clock_event *event)
{
    hwce *_this = (hwce *)__this;

    switch (_this->state)
    {
        case HWCE_STATE_CONTEXT_COPY:
            _this->trace.msg("Finished context copy\n");
            _this->set_state(HWCE_STATE_LOCKED);
            if (_this->prev_job)
            {
                _this->allocated_job->y_trans_size->set_32(_this->prev_job->y_trans_size->get_32());
                _this->allocated_job->y_line_stride_length->set_32(_this->prev_job->y_line_stride_length->get_32());
                _this->allocated_job->y_feat_stride_length->set_32(_this->prev_job->y_feat_stride_length->get_32());
                _this->allocated_job->y_out_3_base_addr->set_32(_this->prev_job->y_out_3_base_addr->get_32());
                _this->allocated_job->y_out_2_base_addr->set_32(_this->prev_job->y_out_2_base_addr->get_32());
                _this->allocated_job->y_out_1_base_addr->set_32(_this->prev_job->y_out_1_base_addr->get_32());
                _this->allocated_job->y_out_0_base_addr->set_32(_this->prev_job->y_out_0_base_addr->get_32());
                _this->allocated_job->y_in_3_base_addr->set_32(_this->prev_job->y_in_3_base_addr->get_32());
                _this->allocated_job->y_in_2_base_addr->set_32(_this->prev_job->y_in_2_base_addr->get_32());
                _this->allocated_job->y_in_1_base_addr->set_32(_this->prev_job->y_in_1_base_addr->get_32());
                _this->allocated_job->y_in_0_base_addr->set_32(_this->prev_job->y_in_0_base_addr->get_32());
                _this->allocated_job->x_trans_size->set_32(_this->prev_job->x_trans_size->get_32());
                _this->allocated_job->x_line_stride_length->set_32(_this->prev_job->x_line_stride_length->get_32());
                _this->allocated_job->x_feat_stride_length->set_32(_this->prev_job->x_feat_stride_length->get_32());
                _this->allocated_job->x_in_base_addr->set_32(_this->prev_job->x_in_base_addr->get_32());
                _this->allocated_job->w_base_addr->set_32(_this->prev_job->w_base_addr->get_32());
                _this->allocated_job->job_config0->set_32(_this->prev_job->job_config0->get_32());
                _this->allocated_job->job_config1->set_32(_this->prev_job->job_config1->get_32());
            }
            break;
    }
}

int hwce::build()
{
    this->traces.new_trace("trace", &this->trace, vp::DEBUG);

    this->in.set_req_meth(&hwce::req);
    this->new_slave_port("input", &this->in);

    for (int i = 0; i < NB_MASTER_PORTS; i++)
    {
        this->new_master_port("out_" + std::to_string(i), &this->out[i]);
    }

    this->new_master_port("irq", &this->irq);

    this->ctrl_event = this->event_new(&hwce::ctrl_handle);
    this->job_queue_event = this->event_new(&hwce::job_queue_handle);

    this->regmap.build(this, &this->trace, "");

    this->regmap.y_trans_size.register_alias(std::bind(&hwce::y_trans_size_alias, this));
    this->regmap.y_line_stride_length.register_alias(std::bind(&hwce::y_line_stride_length_alias, this));
    this->regmap.y_feat_stride_length.register_alias(std::bind(&hwce::y_feat_stride_length_alias, this));
    this->regmap.y_out_3_base_addr.register_alias(std::bind(&hwce::y_out_3_base_addr_alias, this));
    this->regmap.y_out_2_base_addr.register_alias(std::bind(&hwce::y_out_2_base_addr_alias, this));
    this->regmap.y_out_1_base_addr.register_alias(std::bind(&hwce::y_out_1_base_addr_alias, this));
    this->regmap.y_out_0_base_addr.register_alias(std::bind(&hwce::y_out_0_base_addr_alias, this));
    this->regmap.y_in_3_base_addr.register_alias(std::bind(&hwce::y_in_3_base_addr_alias, this));
    this->regmap.y_in_2_base_addr.register_alias(std::bind(&hwce::y_in_2_base_addr_alias, this));
    this->regmap.y_in_1_base_addr.register_alias(std::bind(&hwce::y_in_1_base_addr_alias, this));
    this->regmap.y_in_0_base_addr.register_alias(std::bind(&hwce::y_in_0_base_addr_alias, this));
    this->regmap.x_trans_size.register_alias(std::bind(&hwce::x_trans_size_alias, this));
    this->regmap.x_line_stride_length.register_alias(std::bind(&hwce::x_line_stride_length_alias, this));
    this->regmap.x_feat_stride_length.register_alias(std::bind(&hwce::x_feat_stride_length_alias, this));
    this->regmap.x_in_base_addr.register_alias(std::bind(&hwce::x_in_base_addr_alias, this));
    this->regmap.w_base_addr.register_alias(std::bind(&hwce::w_base_addr_alias, this));
    this->regmap.job_config0.register_alias(std::bind(&hwce::job_config0_alias, this));
    this->regmap.job_config1.register_alias(std::bind(&hwce::job_config1_alias, this));

    this->regmap.acquire.register_callback(std::bind(&hwce::acquire_req, this, _1, _2, _3, _4));
    this->regmap.trigger.register_callback(std::bind(&hwce::trigger_req, this, _1, _2, _3, _4));
    this->regmap.gen_config0.register_callback(std::bind(&hwce::gen_config0_req, this, _1, _2, _3, _4));

    for (int i = 0; i < 4; i++)
    {
        this->yinBase[i] = new Hwce_base(this, "yin_" + std::to_string(i));
        this->youtBase[i] = new Hwce_base(this, "yout_" + std::to_string(i));
    }

    this->xinBase = new Hwce_base(this, "xin");

    for (int i = 0; i < 2; i++)
    {
        jobs[i].id = i;
    }

    jobs[0].y_trans_size = &this->regmap.y_trans_size_ctx0;
    jobs[0].y_line_stride_length = &this->regmap.y_line_stride_length_ctx0;
    jobs[0].y_feat_stride_length = &this->regmap.y_feat_stride_length_ctx0;
    jobs[0].y_out_3_base_addr = &this->regmap.y_out_3_base_addr_ctx0;
    jobs[0].y_out_2_base_addr = &this->regmap.y_out_2_base_addr_ctx0;
    jobs[0].y_out_1_base_addr = &this->regmap.y_out_1_base_addr_ctx0;
    jobs[0].y_out_0_base_addr = &this->regmap.y_out_0_base_addr_ctx0;
    jobs[0].y_in_3_base_addr = &this->regmap.y_in_3_base_addr_ctx0;
    jobs[0].y_in_2_base_addr = &this->regmap.y_in_2_base_addr_ctx0;
    jobs[0].y_in_1_base_addr = &this->regmap.y_in_1_base_addr_ctx0;
    jobs[0].y_in_0_base_addr = &this->regmap.y_in_0_base_addr_ctx0;
    jobs[0].x_trans_size = &this->regmap.x_trans_size_ctx0;
    jobs[0].x_line_stride_length = &this->regmap.x_line_stride_length_ctx0;
    jobs[0].x_feat_stride_length = &this->regmap.x_feat_stride_length_ctx0;
    jobs[0].x_in_base_addr = &this->regmap.x_in_base_addr_ctx0;
    jobs[0].w_base_addr = &this->regmap.w_base_addr_ctx0;
    jobs[0].job_config0 = &this->regmap.job_config0_ctx0;
    jobs[0].job_config1 = &this->regmap.job_config1_ctx0;

    jobs[1].y_trans_size = &this->regmap.y_trans_size_ctx1;
    jobs[1].y_line_stride_length = &this->regmap.y_line_stride_length_ctx1;
    jobs[1].y_feat_stride_length = &this->regmap.y_feat_stride_length_ctx1;
    jobs[1].y_out_3_base_addr = &this->regmap.y_out_3_base_addr_ctx1;
    jobs[1].y_out_2_base_addr = &this->regmap.y_out_2_base_addr_ctx1;
    jobs[1].y_out_1_base_addr = &this->regmap.y_out_1_base_addr_ctx1;
    jobs[1].y_out_0_base_addr = &this->regmap.y_out_0_base_addr_ctx1;
    jobs[1].y_in_3_base_addr = &this->regmap.y_in_3_base_addr_ctx1;
    jobs[1].y_in_2_base_addr = &this->regmap.y_in_2_base_addr_ctx1;
    jobs[1].y_in_1_base_addr = &this->regmap.y_in_1_base_addr_ctx1;
    jobs[1].y_in_0_base_addr = &this->regmap.y_in_0_base_addr_ctx1;
    jobs[1].x_trans_size = &this->regmap.x_trans_size_ctx1;
    jobs[1].x_line_stride_length = &this->regmap.x_line_stride_length_ctx1;
    jobs[1].x_feat_stride_length = &this->regmap.x_feat_stride_length_ctx1;
    jobs[1].x_in_base_addr = &this->regmap.x_in_base_addr_ctx1;
    jobs[1].w_base_addr = &this->regmap.w_base_addr_ctx1;
    jobs[1].job_config0 = &this->regmap.job_config0_ctx1;
    jobs[1].job_config1 = &this->regmap.job_config1_ctx1;

    this->lineBuffer = new uint32_t[this->lineBufferWidth * this->lineBufferHeight];

    traces.new_trace_event("busy", &this->event_busy, 1);
    traces.new_trace_event("conv_exec", &this->event_conv, 1);
    traces.new_trace_event("conv_value", &this->event_conv_value, 32);


    return 0;
}

void hwce::start()
{
}

void Hwce_base::init(uint32_t base, int lineLen, int lineStride, int featLen, int featStride, int nbFeatures, int featLoop)
{
    this->baseStart = base;
    this->baseLoop = base;
    this->base = base;
    this->lineLen = lineLen;
    this->lineStride = lineStride;
    this->featLen = featLen;
    this->featStride = featStride;
    wordCount = 0;
    lineCount = 0;
    featCount = 0;
    featLoopCount = 0;
    this->nbFeatures = nbFeatures;
    this->featLoop = featLoop;
    pendingEof = false;
}

uint32_t Hwce_base::get()
{
    return base;
}

Hwce_base::Hwce_base(hwce *top, string name) : top(top), name(name) {}

void Hwce_base::update()
{
    // Update address pointer
    wordCount++;

    // Check end of line for stride
    base += 4;
    if (wordCount == lineLen)
    {
        // End of line reached
        this->top->trace.msg("Reached end of line (name: %s, stride: 0x%x)\n", this->name.c_str(), lineStride);
        wordCount = 0;
        lineCount++;
        base += (lineStride - lineLen) * 4;
        // Now check end of feature for stride
        if (lineCount == featLen)
        {
            // End of feature reached
            this->top->trace.msg("Reached end of feature (name: %s, stride: 0x%x)\n", this->name.c_str(), featStride);
            lineCount = 0;
            base += (featStride - lineStride * featLen) * 4;
            featLoopCount++;
            pendingEof = true;
            // TODO seems completly wrong
            if (featLoopCount < featLoop)
            {
                this->top->trace.msg("Feature loop (name: %s, feature: %d, nbFeaturesInLoop: %d)\n", this->name.c_str(), featLoopCount, featLoop);
                base = baseLoop;
            }
            else
            {
                featCount++;
                featLoopCount = 0;
                baseLoop = base;
                if (featCount == nbFeatures)
                {
                    this->top->trace.msg("Finished all features (name: %s)\n", this->name.c_str());
                    featCount = 0;
                    base = baseStart;
                }
            }
        }
    }
}

void Hwce_base::startFeature()
{
    pendingEof = false;
}

bool Hwce_base::reachedEof()
{
    return pendingEof;
}


vp::reg *hwce::y_trans_size_alias()
{
    if (current_job == 0)
        return &this->regmap.y_trans_size_ctx0;
    else
        return &this->regmap.y_trans_size_ctx1;
}


vp::reg *hwce::y_line_stride_length_alias()
{
    if (current_job == 0)
        return &this->regmap.y_line_stride_length_ctx0;
    else
        return &this->regmap.y_line_stride_length_ctx1;
}


vp::reg *hwce::y_feat_stride_length_alias()
{
    if (current_job == 0)
        return &this->regmap.y_feat_stride_length_ctx0;
    else
        return &this->regmap.y_feat_stride_length_ctx1;
}


vp::reg *hwce::y_out_3_base_addr_alias()
{
    if (current_job == 0)
        return &this->regmap.y_out_3_base_addr_ctx0;
    else
        return &this->regmap.y_out_3_base_addr_ctx1;
}


vp::reg *hwce::y_out_2_base_addr_alias()
{
    if (current_job == 0)
        return &this->regmap.y_out_2_base_addr_ctx0;
    else
        return &this->regmap.y_out_2_base_addr_ctx1;
}


vp::reg *hwce::y_out_1_base_addr_alias()
{
    if (current_job == 0)
        return &this->regmap.y_out_1_base_addr_ctx0;
    else
        return &this->regmap.y_out_1_base_addr_ctx1;
}


vp::reg *hwce::y_out_0_base_addr_alias()
{
    if (current_job == 0)
        return &this->regmap.y_out_0_base_addr_ctx0;
    else
        return &this->regmap.y_out_0_base_addr_ctx1;
}


vp::reg *hwce::y_in_3_base_addr_alias()
{
    if (current_job == 0)
        return &this->regmap.y_in_3_base_addr_ctx0;
    else
        return &this->regmap.y_in_3_base_addr_ctx1;
}


vp::reg *hwce::y_in_2_base_addr_alias()
{
    if (current_job == 0)
        return &this->regmap.y_in_2_base_addr_ctx0;
    else
        return &this->regmap.y_in_2_base_addr_ctx1;
}


vp::reg *hwce::y_in_1_base_addr_alias()
{
    if (current_job == 0)
        return &this->regmap.y_in_1_base_addr_ctx0;
    else
        return &this->regmap.y_in_1_base_addr_ctx1;
}


vp::reg *hwce::y_in_0_base_addr_alias()
{
    if (current_job == 0)
        return &this->regmap.y_in_0_base_addr_ctx0;
    else
        return &this->regmap.y_in_0_base_addr_ctx1;
}


vp::reg *hwce::x_trans_size_alias()
{
    if (current_job == 0)
        return &this->regmap.x_trans_size_ctx0;
    else
        return &this->regmap.x_trans_size_ctx1;
}


vp::reg *hwce::x_line_stride_length_alias()
{
    if (current_job == 0)
        return &this->regmap.x_line_stride_length_ctx0;
    else
        return &this->regmap.x_line_stride_length_ctx1;
}


vp::reg *hwce::x_feat_stride_length_alias()
{
    if (current_job == 0)
        return &this->regmap.x_feat_stride_length_ctx0;
    else
        return &this->regmap.x_feat_stride_length_ctx1;
}


vp::reg *hwce::x_in_base_addr_alias()
{
    if (current_job == 0)
        return &this->regmap.x_in_base_addr_ctx0;
    else
        return &this->regmap.x_in_base_addr_ctx1;
}


vp::reg *hwce::w_base_addr_alias()
{
    if (current_job == 0)
        return &this->regmap.w_base_addr_ctx0;
    else
        return &this->regmap.w_base_addr_ctx1;
}


vp::reg *hwce::job_config0_alias()
{
    if (current_job == 0)
        return &this->regmap.job_config0_ctx0;
    else
        return &this->regmap.job_config0_ctx1;
}


vp::reg *hwce::job_config1_alias()
{
    if (current_job == 0)
        return &this->regmap.job_config1_ctx0;
    else
        return &this->regmap.job_config1_ctx1;
}

    
extern "C" vp::component *vp_constructor(js::config *config)
{
    return new hwce(config);
}
