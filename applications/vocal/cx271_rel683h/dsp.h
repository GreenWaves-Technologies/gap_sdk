
/* ************************************************************************* */
/*
 *  dsp.h
 *
 *	(C) 2016 VOCAL Technologies, Ltd.
 *
 *  ALL RIGHTS RESERVED.  PROPRIETARY AND CONFIDENTIAL.
 *
 *  VOCAL Technologies, Ltd.
 *  520 Lee Entrance, Suite 202
 *  Buffalo, NY  14228
 *
 *  Product:
 *
 *  Module:
 *
 *  This header contains the declarations for the DSP module
 *
 *  Revision Number:    $Revision$
 *  Revision Status:    $State$
 *  Last Modified:      $Date$
 *  Identification:     $Id$
 *
 *  Revision History:   $Log$
 *
 */
/* ************************************************************************* */

#ifndef _DSP_DSP_H
#define _DSP_DSP_H

/* ************************************************************************* */
typedef signed short fract;

void init_dsp_process(void);
int dsp_process_block(fract *input_mics, fract *input_spk, fract *output, int mics, int samples);

/* ************************************************************************* */

#endif /* _DSP_DSP_H */
