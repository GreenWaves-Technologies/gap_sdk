/* 
 * Copyright (C) 2015 ETH Zurich and University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Francesco Conti (f.conti@unibo.it)
 *          Germain Haugou (germain.haugou@gmail.com)
 */

#include "rt/rt_api.h"
#include "archi/hwce/v4/hwce.h"


#define HWCE_LBSIZE 32


static PI_CL_L1 int pos_hwce_width_rest;
static PI_CL_L1 int pos_hwce_nb_free_jobs;


void pi_hwce_open()
{
    *(volatile int*) (ARCHI_CLUSTER_CTRL_ADDR + (3 << 3)) |=  0xc00;
}


void pi_hwce_close()
{
    *(volatile int*) (ARCHI_CLUSTER_CTRL_ADDR + (3 << 3)) &= ~0xc00;
}


static inline void pos_hwce_enqueue_jobs(struct pi_hwce_conv_s *conv)
{

    int i;
    int maxJobs = 2;

    int irq = hal_irq_disable();

    int w = conv->width;
    int h = conv->height;
    int nif = conv->nif;
    int nof = conv->nof;
    int fix = conv->fix;
    void *y = conv->output;
    int filter_size = conv->filter_size;
    uint32_t W_ptr = (uint32_t)conv->weights;
    uint32_t x_ptr = (uint32_t)conv->input;
    uint32_t yout0_ptr = (uint32_t)conv->output;

    int ow = w - filter_size;
    int oh = h - filter_size;

    while (pos_hwce_nb_free_jobs)
    {
        int w_iter = w + fix;
        if (w_iter > HWCE_LBSIZE)
            w_iter = HWCE_LBSIZE;

        int ow_iter = w_iter - filter_size - fix;
            
        pos_hwce_nb_free_jobs--;

        if (conv->first)
        {
            uint32_t bias = conv->set_bias ? conv->bias : 0;
            char set_bias;

            conv->first = 0;

            if (nif > 1)
            {
                set_bias = 0;
            }
            else
            {
                set_bias = 1;
            }


            hwce_gen_config0_set(ARCHI_HWCE_ADDR,
                HWCE_GEN_CONFIG0_WSTRIDE(conv->weight_stride) |
                HWCE_GEN_CONFIG0_CONV(conv->type) |
                HWCE_GEN_CONFIG0_NF(1) |
                HWCE_GEN_CONFIG0_NY(set_bias) |
                HWCE_GEN_CONFIG0_QF(conv->norm) |
                HWCE_GEN_CONFIG0_RND(1)
            );

            hwce_gen_config1_set(ARCHI_HWCE_ADDR, 0);
            
            while (hwce_acquire_get(ARCHI_HWCE_ADDR) == (unsigned int)-3);

            // Now sets the fields that will remain the same for all jobs expect for the last one in case it is partial
            hwce_job_config1_set(ARCHI_HWCE_ADDR,
                HWCE_JOB_CONFIG1_LN(nif > 1 && nof > 1) |
                HWCE_JOB_CONFIG1_WOF_PARAM(nof) |
                HWCE_JOB_CONFIG1_WIF_PARAM(nif) |
                HWCE_JOB_CONFIG1_VECT_DISABLE_MASK(0x7)
            );

            hwce_job_config0_set(ARCHI_HWCE_ADDR,
                HWCE_JOB_CONFIG0_LBUFLEN(w_iter >> 1) |      // With 3x3 convolutions, the line must fix 5x5 as the engine can only fetch data for 5x5 convolutions
                HWCE_JOB_CONFIG0_NOYCONST(bias)
            );

            hwce_w_base_addr_set(ARCHI_HWCE_ADDR, W_ptr);


            hwce_x_trans_size_set(ARCHI_HWCE_ADDR, (nof*nif*h*(w_iter))>>1);

            hwce_x_line_stride_length_set(ARCHI_HWCE_ADDR,
                HWCE_X_LINE_STRIDE_LENGTH_LENGTH(w_iter >> 1) |
                HWCE_X_LINE_STRIDE_LENGTH_STRIDE(w*2)
            );

            hwce_x_feat_stride_length_set(ARCHI_HWCE_ADDR,
                HWCE_X_FEAT_STRIDE_LENGTH_LENGTH(h) |
                HWCE_X_FEAT_STRIDE_LENGTH_STRIDE(w*h*2)
            );


            hwce_y_trans_size_set(ARCHI_HWCE_ADDR, (nof*nif*oh*ow_iter)>>1);

            hwce_y_line_stride_length_set(ARCHI_HWCE_ADDR,
                HWCE_Y_LINE_STRIDE_LENGTH_LENGTH(ow_iter>>1) |
                HWCE_Y_LINE_STRIDE_LENGTH_STRIDE(ow*2)
            );

            hwce_y_feat_stride_length_set(ARCHI_HWCE_ADDR,
                HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH(oh) |
                HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE(ow*oh*2)
            );
        }
        else
        {

        }

        // Now the part always specific to this job
        hwce_x_in_base_addr_set(ARCHI_HWCE_ADDR, x_ptr);
        hwce_y_in_0_base_addr_set(ARCHI_HWCE_ADDR, yout0_ptr);
        hwce_y_out_0_base_addr_set(ARCHI_HWCE_ADDR, yout0_ptr);

        // Trigger HWCE
        hwce_trigger_set(ARCHI_HWCE_ADDR, 0);

        break;
        
        #if 0
        // Update remaining size and leave in case it fits 1 job
        wRem -= ow_hwce;

        if (wRem <= 0)
        {
            // We already reached the end of convolution, the interrupt handle won't have anything to enqueue
            pos_hwce_width_rest = 0;
            hal_irq_restore(irq);
            return;
        }
        #endif

        // Update pointers to prepare next iteration
        x_ptr += ow_iter*4;
        yout0_ptr += ow_iter*4;

#if 0
        if (vectSize > 1)
            yout1_ptr += ow_hwce*4;

        if (vectSize > 2)
        {
            yout2_ptr += ow_hwce*4;
            yout3_ptr += ow_hwce*4;
        }
#endif

#if 0
    // We seperate the HWCE registers in 3 groups
    //   - Global config: set only once for all jobs
    //   - Lines length and conv sizes: set once for all jobs and a second time in case we have a partial job
    //   - Conv pointers: set for each job

    // As we use interrupts, 2 jobs will be used and have these fields initialized
    // in case a job will be handled by the interrupt handler.
    // Thus other jobs enqueued by the interrupt handler will keep these values
    ''



#endif
}

#if 0
  // If we get there if means we enqueued 2 jobs and that there are still convolutions to enqueue
  // Note that we assume the hwce has not enough time to finish the convolution before we get there
  // otherwise the interrupt handler would see an inconsistent descriptor.
  // We can assume it because the hwce is the only one using an interrupt handler

  hwce_x_ptr = x_ptr;
  hwce_yout0_ptr = yout0_ptr;
  if (vectSize > 1) hwce_yout1_ptr = yout1_ptr;
  if (vectSize > 2) {
    hwce_yout2_ptr = yout2_ptr;
    hwce_yout3_ptr = yout3_ptr;
  }

  hwce_wTrigRem = wRem;
  hwce_yLines = nif*(h-4);
  hwce_xLines = nif*h;
  hwce_yLineStride = yLineStride;
  hwce_xLineStride = xLineStride;
#endif

  hal_irq_restore(irq);
}


void pi_hwce_wait(struct pi_hwce_conv_s *conv)
{
    do
    {
        eu_evt_maskWaitAndClr(1<<ARCHI_CL_EVT_ACC0);
    } while(*(volatile int *)&pos_hwce_width_rest);
}


void pi_hwce_3x3_16x16_feat_init(struct pi_hwce_conv_s *conv, void *weights, void *input, void *output, int width, int height)
{
    // We align weights on 8 bytes to avoid X propagation on RTL as the hwce is fetching
    // weights 8 by 8 bytes
    conv->weight_stride = 24;
    conv->nif = 1;
    conv->nof = 1;
    conv->norm = 0;
    conv->fix = 2;
    conv->first = 1;
    conv->type = 1;
    conv->set_bias = 0;
    conv->weights = weights;
    conv->input = input;
    conv->output = output;
    conv->width = width;
    conv->height = height;
    conv->filter_size = 3 - 1;

    pos_hwce_nb_free_jobs = 2;
}


void pi_hwce_3x3_16x16_feat_start(struct pi_hwce_conv_s *conv)
{
    pos_hwce_enqueue_jobs(conv);
}


void pi_hwce_5x5_16x16_feat_init(struct pi_hwce_conv_s *conv, void *weights, void *input, void *output, int width, int height)
{
    conv->weight_stride = 56;
    conv->nif = 1;
    conv->nof = 1;
    conv->norm = 0;
    conv->fix = 0;
    conv->first = 1;
    conv->type = 0;
    conv->set_bias = 0;
    conv->weights = weights;
    conv->input = input;
    conv->output = output;
    conv->width = width;
    conv->height = height;
    conv->filter_size = 5 - 1;

    pos_hwce_nb_free_jobs = 2;
}


void pi_hwce_5x5_16x16_feat_start(struct pi_hwce_conv_s *conv)
{
    pos_hwce_enqueue_jobs(conv);
}


void __attribute__((constructor)) pos_hwce_init()
{
    //plp_irq_setHandler(PLP_HWCE_EVENT, hwce_irqHandler);
    //eu_irq_maskSet(1 << PLP_HWCE_EVENT);
}
