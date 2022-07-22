/* 
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "pmsis.h"
#include "stdio.h"

#define MAX_ERROR 0.05
#define NB_ITER 128
#define NB_CORES 9
#define FADD_0_1 1.051563
#define FADD_0_5 2.039844
#define FADD_0_8 2.026562
#define FADD_0_9 3.021875
#define FADD_1_1 1.037500
#define FADD_1_5 2.021094
#define FADD_1_8 2.025000
#define FADD_1_9 3.025000
#define FADD_2_1 1.023438
#define FADD_2_5 1.024219
#define FADD_2_8 1.026562
#define FADD_2_9 1.522656
#define FMUL_0_1 1.025000
#define FMUL_0_5 2.023438
#define FMUL_0_8 2.024219
#define FMUL_0_9 3.024219
#define FMUL_1_1 1.024219
#define FMUL_1_5 2.021875
#define FMUL_1_8 2.025781
#define FMUL_1_9 3.024219
#define FMUL_2_1 1.023438
#define FMUL_2_5 1.025781
#define FMUL_2_8 1.027344
#define FMUL_2_9 1.523438
#define FSGNJ_0_1 1.023438
#define FSGNJ_0_5 2.023438
#define FSGNJ_0_8 2.024219
#define FSGNJ_0_9 3.025000
#define FSGNJ_1_1 1.024219
#define FSGNJ_1_5 2.021094
#define FSGNJ_1_8 2.026562
#define FSGNJ_1_9 3.025000
#define FSGNJ_2_1 1.023438
#define FSGNJ_2_5 1.025781
#define FSGNJ_2_8 1.032031
#define FSGNJ_2_9 1.524219
#define FMIN_0_1 1.038281
#define FMIN_0_5 2.022656
#define FMIN_0_8 2.024219
#define FMIN_0_9 3.025000
#define FMIN_1_1 1.024219
#define FMIN_1_5 2.023438
#define FMIN_1_8 2.023438
#define FMIN_1_9 3.023438
#define FMIN_2_1 1.023438
#define FMIN_2_5 1.027344
#define FMIN_2_8 1.026562
#define FMIN_2_9 1.525000
#define FDIV_0_1 14.022656
#define FDIV_0_5 70.053909
#define FDIV_0_8 112.085938
#define FDIV_0_9 126.097656
#define FDIV_1_1 14.023438
#define FDIV_1_5 70.053909
#define FDIV_1_8 112.085938
#define FDIV_1_9 126.096878
#define FDIV_2_1 7.550000
#define FDIV_2_5 35.253906
#define FDIV_2_8 56.087502
#define FDIV_2_9 63.138283
#define FSQRT_0_1 14.022656
#define FSQRT_0_5 70.052345
#define FSQRT_0_8 112.086716
#define FSQRT_0_9 126.096878
#define FSQRT_1_1 14.023438
#define FSQRT_1_5 70.053123
#define FSQRT_1_8 112.086716
#define FSQRT_1_9 126.096878
#define FSQRT_2_1 7.523438
#define FSQRT_2_5 35.253906
#define FSQRT_2_8 56.086720
#define FSQRT_2_9 63.137501
#define FDIV_H_0_1 10.022656
#define FDIV_H_0_5 50.052345
#define FDIV_H_0_8 80.085159
#define FDIV_H_0_9 90.096092
#define FDIV_H_1_1 10.036718
#define FDIV_H_1_5 50.070313
#define FDIV_H_1_8 80.085938
#define FDIV_H_1_9 90.097656
#define FDIV_H_2_1 5.536719
#define FDIV_H_2_5 25.253124
#define FDIV_H_2_8 40.087502
#define FDIV_H_2_9 45.137501
#define FCVT_H_0_1 1.023438
#define FCVT_H_0_5 2.022656
#define FCVT_H_0_8 2.022656
#define FCVT_H_0_9 3.023438
#define FCVT_H_1_1 1.024219
#define FCVT_H_1_5 2.025000
#define FCVT_H_1_8 2.023438
#define FCVT_H_1_9 3.022656
#define FCVT_H_2_1 1.023438
#define FCVT_H_2_5 1.027344
#define FCVT_H_2_8 1.027344
#define FCVT_H_2_9 1.526562
#define FMADD_0_1 1.025000
#define FMADD_0_5 2.023438
#define FMADD_0_8 2.024219
#define FMADD_0_9 3.021875
#define FMADD_1_1 1.028125
#define FMADD_1_5 2.023438
#define FMADD_1_8 2.023438
#define FMADD_1_9 3.025000
#define FMADD_2_1 1.023438
#define FMADD_2_5 1.026562
#define FMADD_2_8 1.026562
#define FMADD_2_9 1.523438
#define FNMADD_0_1 1.023438
#define FNMADD_0_5 2.020313
#define FNMADD_0_8 2.024219
#define FNMADD_0_9 3.022656
#define FNMADD_1_1 1.024219
#define FNMADD_1_5 2.024219
#define FNMADD_1_8 2.023438
#define FNMADD_1_9 3.025000
#define FNMADD_2_1 1.036719
#define FNMADD_2_5 1.026562
#define FNMADD_2_8 1.026562
#define FNMADD_2_9 1.524219
#define ADD_D_0_1 1.025000
#define ADD_D_0_5 2.020313
#define ADD_D_0_8 2.023438
#define ADD_D_0_9 3.021875
#define ADD_D_1_1 1.024219
#define ADD_D_1_5 2.023438
#define ADD_D_1_8 2.023438
#define ADD_D_1_9 3.023438
#define ADD_D_2_1 1.023438
#define ADD_D_2_5 1.026562
#define ADD_D_2_8 1.026562
#define ADD_D_2_9 1.521094
#define P_MULS_D_0_1 1.036719
#define P_MULS_D_0_5 2.020313
#define P_MULS_D_0_8 2.024219
#define P_MULS_D_0_9 3.024219
#define P_MULS_D_1_1 1.024219
#define P_MULS_D_1_5 2.022656
#define P_MULS_D_1_8 2.025781
#define P_MULS_D_1_9 3.023438
#define P_MULS_D_2_1 1.023438
#define P_MULS_D_2_5 1.036719
#define P_MULS_D_2_8 1.028906
#define P_MULS_D_2_9 1.521094
#define P_MULHU_D_0_1 1.026562
#define P_MULHU_D_0_5 2.023438
#define P_MULHU_D_0_8 2.026562
#define P_MULHU_D_0_9 3.023438
#define P_MULHU_D_1_1 1.024219
#define P_MULHU_D_1_5 2.023438
#define P_MULHU_D_1_8 2.025000
#define P_MULHU_D_1_9 3.025000
#define P_MULHU_D_2_1 1.023438
#define P_MULHU_D_2_5 1.026562
#define P_MULHU_D_2_8 1.026562
#define P_MULHU_D_2_9 1.522656
#define P_MAC_D_0_1 1.023438
#define P_MAC_D_0_5 2.021875
#define P_MAC_D_0_8 2.025000
#define P_MAC_D_0_9 3.021875
#define P_MAC_D_1_1 1.024219
#define P_MAC_D_1_5 2.021094
#define P_MAC_D_1_8 2.024219
#define P_MAC_D_1_9 3.023438
#define P_MAC_D_2_1 1.023438
#define P_MAC_D_2_5 1.025781
#define P_MAC_D_2_8 1.027344
#define P_MAC_D_2_9 1.522656
#define P_CNT_D_0_1 1.023438
#define P_CNT_D_0_5 2.020313
#define P_CNT_D_0_8 2.026562
#define P_CNT_D_0_9 3.021094
#define P_CNT_D_1_1 1.026562
#define P_CNT_D_1_5 2.023438
#define P_CNT_D_1_8 2.025000
#define P_CNT_D_1_9 3.021094
#define P_CNT_D_2_1 1.036719
#define P_CNT_D_2_5 1.024219
#define P_CNT_D_2_8 1.028125
#define P_CNT_D_2_9 1.524219
#define P_EXTHS_D_0_1 1.023438
#define P_EXTHS_D_0_5 2.023438
#define P_EXTHS_D_0_8 2.024219
#define P_EXTHS_D_0_9 3.021094
#define P_EXTHS_D_1_1 1.024219
#define P_EXTHS_D_1_5 2.023438
#define P_EXTHS_D_1_8 2.025000
#define P_EXTHS_D_1_9 3.022656
#define P_EXTHS_D_2_1 1.023438
#define P_EXTHS_D_2_5 1.024219
#define P_EXTHS_D_2_8 1.027344
#define P_EXTHS_D_2_9 1.522656
#define VFADD_H_0_1 1.023438
#define VFADD_H_0_5 2.020313
#define VFADD_H_0_8 2.025000
#define VFADD_H_0_9 3.023438
#define VFADD_H_1_1 1.024219
#define VFADD_H_1_5 2.024219
#define VFADD_H_1_8 2.023438
#define VFADD_H_1_9 3.024219
#define VFADD_H_2_1 1.023438
#define VFADD_H_2_5 1.026562
#define VFADD_H_2_8 1.027344
#define VFADD_H_2_9 1.522656
#define VFMUL_H_0_1 1.025000
#define VFMUL_H_0_5 2.021875
#define VFMUL_H_0_8 2.024219
#define VFMUL_H_0_9 3.024219
#define VFMUL_H_1_1 1.037500
#define VFMUL_H_1_5 2.021094
#define VFMUL_H_1_8 2.025000
#define VFMUL_H_1_9 3.022656
#define VFMUL_H_2_1 1.023438
#define VFMUL_H_2_5 1.027344
#define VFMUL_H_2_8 1.028125
#define VFMUL_H_2_9 1.525000
#define VFMAC_H_0_1 1.023438
#define VFMAC_H_0_5 2.022656
#define VFMAC_H_0_8 2.026562
#define VFMAC_H_0_9 3.024219
#define VFMAC_H_1_1 1.026562
#define VFMAC_H_1_5 2.022656
#define VFMAC_H_1_8 2.026562
#define VFMAC_H_1_9 3.025000
#define VFMAC_H_2_1 1.023438
#define VFMAC_H_2_5 1.024219
#define VFMAC_H_2_8 1.024219
#define VFMAC_H_2_9 1.522656
#define FDIV_FDIV_H_4_4 96.085159
#define FDIV_FDIV_H_2_6 88.085159
#define FDIV_FADD_1_7 14.114062
#define ADD_D_FADD_4_4 1.041406



void fadd_0(int a, int b, int c, int d, int e);
void fadd_1(int a, int b, int c, int d, int e);
void fadd_2(int a, int b, int c, int d, int e);

void fmul_0(int a, int b, int c, int d, int e);
void fmul_1(int a, int b, int c, int d, int e);
void fmul_2(int a, int b, int c, int d, int e);

void fsgnj_0(int a, int b, int c, int d, int e);
void fsgnj_1(int a, int b, int c, int d, int e);
void fsgnj_2(int a, int b, int c, int d, int e);

void fmin_0(int a, int b, int c, int d, int e);
void fmin_1(int a, int b, int c, int d, int e);
void fmin_2(int a, int b, int c, int d, int e);

void fdiv_0(int a, int b, int c, int d, int e);
void fdiv_1(int a, int b, int c, int d, int e);
void fdiv_2(int a, int b, int c, int d, int e);

void fmadd_0(int a, int b, int c, int d, int e);
void fmadd_1(int a, int b, int c, int d, int e);
void fmadd_2(int a, int b, int c, int d, int e);

void fnmadd_0(int a, int b, int c, int d, int e);
void fnmadd_1(int a, int b, int c, int d, int e);
void fnmadd_2(int a, int b, int c, int d, int e);

void fsqrt_0(int a, int b, int c, int d, int e);
void fsqrt_1(int a, int b, int c, int d, int e);
void fsqrt_2(int a, int b, int c, int d, int e);

void fdiv_h_0(int a, int b, int c, int d, int e);
void fdiv_h_1(int a, int b, int c, int d, int e);
void fdiv_h_2(int a, int b, int c, int d, int e);

void vfadd_h_0(int a, int b, int c, int d, int e);
void vfadd_h_1(int a, int b, int c, int d, int e);
void vfadd_h_2(int a, int b, int c, int d, int e);

void vfmul_h_0(int a, int b, int c, int d, int e);
void vfmul_h_1(int a, int b, int c, int d, int e);
void vfmul_h_2(int a, int b, int c, int d, int e);

void vfmac_h_0(int a, int b, int c, int d, int e);
void vfmac_h_1(int a, int b, int c, int d, int e);
void vfmac_h_2(int a, int b, int c, int d, int e);

void fcvt_h_0(int a, int b, int c, int d, int e);
void fcvt_h_1(int a, int b, int c, int d, int e);
void fcvt_h_2(int a, int b, int c, int d, int e);


void add_d_0(int a, int b, int c, int d, int e);
void add_d_1(int a, int b, int c, int d, int e);
void add_d_2(int a, int b, int c, int d, int e);

void p_muls_d_0(int a, int b, int c, int d, int e);
void p_muls_d_1(int a, int b, int c, int d, int e);
void p_muls_d_2(int a, int b, int c, int d, int e);

void p_mulhu_d_0(int a, int b, int c, int d, int e);
void p_mulhu_d_1(int a, int b, int c, int d, int e);
void p_mulhu_d_2(int a, int b, int c, int d, int e);

void p_mac_d_0(int a, int b, int c, int d, int e);
void p_mac_d_1(int a, int b, int c, int d, int e);
void p_mac_d_2(int a, int b, int c, int d, int e);

void p_cnt_d_0(int a, int b, int c, int d, int e);
void p_cnt_d_1(int a, int b, int c, int d, int e);
void p_cnt_d_2(int a, int b, int c, int d, int e);

void p_exths_d_0(int a, int b, int c, int d, int e);
void p_exths_d_1(int a, int b, int c, int d, int e);
void p_exths_d_2(int a, int b, int c, int d, int e);



PI_CL_L1 static float results[NB_CORES];
PI_CL_L1 static int nb_errors;

PI_CL_L1 static void (*entry_0)(int, int, int, int, int);
PI_CL_L1 static int nb_cores_0;
PI_CL_L1 static void (*entry_1)(int, int, int, int, int);
PI_CL_L1 static int nb_cores_1;
PI_CL_L1 static char *desc;
PI_CL_L1 static float expected;
PI_CL_L1 static char *define;


static void pe_entry(void *arg)
{
    if (pi_core_id() == 0)
    {
#ifdef VERBOSE
        printf("Starting use case: %s\n", desc);
#endif
        for (int i=0; i<NB_CORES; i++)
        {
            results[i] = 0;
        }
    }

    int a = pi_core_id()*7+1138;
    int b = 7*a;
    int c = 13*a;
    int d = 3*a;

    for (int i=0; i<3; i++)
    {
        pi_cl_team_barrier();

        pi_perf_conf(1<<PI_PERF_ACTIVE_CYCLES);
        pi_perf_reset();
        pi_perf_start();

        if (pi_core_id() < nb_cores_0)
        {
            entry_0(i <= 1 ? 1 : NB_ITER, a, b, c, d);
        }
        else
        {
            entry_1(i <= 1 ? 1 : NB_ITER, a, b, c, d);
        }

        pi_perf_stop();
    }

    int cycles = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
    results[pi_core_id()] = ((float)cycles) / NB_ITER / 10;

    pi_cl_team_barrier();

    if (pi_core_id() == 0)
    {
        float result = 0;
        for (int i=0; i<NB_CORES; i++)
        {
            if (results[i] > result)
            {
                result = results[i];
            }
        }

        float error = (result - expected)/ result;
        if (error < 0)
        {
            error = -error;
        }

#ifdef VERBOSE
        printf("    Got result: %f, error: %f, max_error: %f\n", result, error, MAX_ERROR);
#else
        printf("#define %s %f\n", define, result);
#endif

        if (error > MAX_ERROR)
        {
#ifdef VERBOSE
            printf("    ERROR DETECTED !!!!\n");
#endif
            nb_errors++;
        }

#ifdef VERBOSE
        printf("\n");
#endif
    }
}


static void check_testcase(void (*_entry_0)(int, int, int, int, int), int _nb_cores_0, void (*_entry_1)(int, int, int, int, int), int _nb_cores_1, char *_desc, float _expected, char *_define)
{
    entry_0 = _entry_0;
    nb_cores_0 = _nb_cores_0;
    entry_1 = _entry_1;
    nb_cores_1 = _nb_cores_1;
    desc = _desc;
    expected = _expected;
    define = _define;
    int nb_cores = _nb_cores_0 + _nb_cores_1;

    if (nb_cores == pi_cl_cluster_nb_cores_with_cc())
    {
        pi_cl_team_fork_cc(nb_cores, pe_entry, NULL);
    }
    else
    {
        pi_cl_team_fork(nb_cores, pe_entry, NULL);
    }
}


static void cluster_entry(void *arg)
{
    check_testcase(fadd_0, 1, NULL, 0, "fadd at each cycle with no dependency on 1 core"           , FADD_0_1, "FADD_0_1");
    check_testcase(fadd_0, 5, NULL, 0, "fadd at each cycle with no dependency on 5 cores"          , FADD_0_5, "FADD_0_5");
    check_testcase(fadd_0, 8, NULL, 0, "fadd at each cycle with no dependency on 8 cores"          , FADD_0_8, "FADD_0_8");
    check_testcase(fadd_0, 9, NULL, 0, "fadd at each cycle with no dependency on 9 cores"          , FADD_0_9, "FADD_0_9");
    check_testcase(fadd_1, 1, NULL, 0, "fadd at each cycle with dependency on 1 core"              , FADD_1_1, "FADD_1_1");
    check_testcase(fadd_1, 5, NULL, 0, "fadd at each cycle with dependency on 5 cores"             , FADD_1_5, "FADD_1_5");
    check_testcase(fadd_1, 8, NULL, 0, "fadd at each cycle with dependency on 8 cores"             , FADD_1_8, "FADD_1_8");
    check_testcase(fadd_1, 9, NULL, 0, "fadd at each cycle with dependency on 9 cores"             , FADD_1_9, "FADD_1_9");
    check_testcase(fadd_2, 1, NULL, 0, "fadd at each cycle with no dependency and 1 nop on 1 core" , FADD_2_1, "FADD_2_1");
    check_testcase(fadd_2, 5, NULL, 0, "fadd at each cycle with no dependency and 1 nop on 5 cores", FADD_2_5, "FADD_2_5");
    check_testcase(fadd_2, 8, NULL, 0, "fadd at each cycle with no dependency and 1 nop on 8 cores", FADD_2_8, "FADD_2_8");
    check_testcase(fadd_2, 9, NULL, 0, "fadd at each cycle with no dependency and 1 nop on 9 cores", FADD_2_9, "FADD_2_9");

    check_testcase(fmul_0, 1, NULL, 0, "fmul at each cycle with no dependency on 1 core"           , FMUL_0_1, "FMUL_0_1");
    check_testcase(fmul_0, 5, NULL, 0, "fmul at each cycle with no dependency on 5 cores"          , FMUL_0_5, "FMUL_0_5");
    check_testcase(fmul_0, 8, NULL, 0, "fmul at each cycle with no dependency on 8 cores"          , FMUL_0_8, "FMUL_0_8");
    check_testcase(fmul_0, 9, NULL, 0, "fmul at each cycle with no dependency on 9 cores"          , FMUL_0_9, "FMUL_0_9");
    check_testcase(fmul_1, 1, NULL, 0, "fmul at each cycle with dependency on 1 core"              , FMUL_1_1, "FMUL_1_1");
    check_testcase(fmul_1, 5, NULL, 0, "fmul at each cycle with dependency on 5 cores"             , FMUL_1_5, "FMUL_1_5");
    check_testcase(fmul_1, 8, NULL, 0, "fmul at each cycle with dependency on 8 cores"             , FMUL_1_8, "FMUL_1_8");
    check_testcase(fmul_1, 9, NULL, 0, "fmul at each cycle with dependency on 9 cores"             , FMUL_1_9, "FMUL_1_9");
    check_testcase(fmul_2, 1, NULL, 0, "fmul at each cycle with no dependency and 1 nop on 1 core" , FMUL_2_1, "FMUL_2_1");
    check_testcase(fmul_2, 5, NULL, 0, "fmul at each cycle with no dependency and 1 nop on 5 cores", FMUL_2_5, "FMUL_2_5");
    check_testcase(fmul_2, 8, NULL, 0, "fmul at each cycle with no dependency and 1 nop on 8 cores", FMUL_2_8, "FMUL_2_8");
    check_testcase(fmul_2, 9, NULL, 0, "fmul at each cycle with no dependency and 1 nop on 9 cores", FMUL_2_9, "FMUL_2_9");

    check_testcase(fsgnj_0, 1, NULL, 0, "fsgnj at each cycle with no dependency on 1 core"           , FSGNJ_0_1, "FSGNJ_0_1");
    check_testcase(fsgnj_0, 5, NULL, 0, "fsgnj at each cycle with no dependency on 5 cores"          , FSGNJ_0_5, "FSGNJ_0_5");
    check_testcase(fsgnj_0, 8, NULL, 0, "fsgnj at each cycle with no dependency on 8 cores"          , FSGNJ_0_8, "FSGNJ_0_8");
    check_testcase(fsgnj_0, 9, NULL, 0, "fsgnj at each cycle with no dependency on 9 cores"          , FSGNJ_0_9, "FSGNJ_0_9");
    check_testcase(fsgnj_1, 1, NULL, 0, "fsgnj at each cycle with dependency on 1 core"              , FSGNJ_1_1, "FSGNJ_1_1");
    check_testcase(fsgnj_1, 5, NULL, 0, "fsgnj at each cycle with dependency on 5 cores"             , FSGNJ_1_5, "FSGNJ_1_5");
    check_testcase(fsgnj_1, 8, NULL, 0, "fsgnj at each cycle with dependency on 8 cores"             , FSGNJ_1_8, "FSGNJ_1_8");
    check_testcase(fsgnj_1, 9, NULL, 0, "fsgnj at each cycle with dependency on 9 cores"             , FSGNJ_1_9, "FSGNJ_1_9");
    check_testcase(fsgnj_2, 1, NULL, 0, "fsgnj at each cycle with no dependency and 1 nop on 1 core" , FSGNJ_2_1, "FSGNJ_2_1");
    check_testcase(fsgnj_2, 5, NULL, 0, "fsgnj at each cycle with no dependency and 1 nop on 5 cores", FSGNJ_2_5, "FSGNJ_2_5");
    check_testcase(fsgnj_2, 8, NULL, 0, "fsgnj at each cycle with no dependency and 1 nop on 8 cores", FSGNJ_2_8, "FSGNJ_2_8");
    check_testcase(fsgnj_2, 9, NULL, 0, "fsgnj at each cycle with no dependency and 1 nop on 9 cores", FSGNJ_2_9, "FSGNJ_2_9");

    check_testcase(fmin_0, 1, NULL, 0, "fmin at each cycle with no dependency on 1 core"           , FMIN_0_1, "FMIN_0_1");
    check_testcase(fmin_0, 5, NULL, 0, "fmin at each cycle with no dependency on 5 cores"          , FMIN_0_5, "FMIN_0_5");
    check_testcase(fmin_0, 8, NULL, 0, "fmin at each cycle with no dependency on 8 cores"          , FMIN_0_8, "FMIN_0_8");
    check_testcase(fmin_0, 9, NULL, 0, "fmin at each cycle with no dependency on 9 cores"          , FMIN_0_9, "FMIN_0_9");
    check_testcase(fmin_1, 1, NULL, 0, "fmin at each cycle with dependency on 1 core"              , FMIN_1_1, "FMIN_1_1");
    check_testcase(fmin_1, 5, NULL, 0, "fmin at each cycle with dependency on 5 cores"             , FMIN_1_5, "FMIN_1_5");
    check_testcase(fmin_1, 8, NULL, 0, "fmin at each cycle with dependency on 8 cores"             , FMIN_1_8, "FMIN_1_8");
    check_testcase(fmin_1, 9, NULL, 0, "fmin at each cycle with dependency on 9 cores"             , FMIN_1_9, "FMIN_1_9");
    check_testcase(fmin_2, 1, NULL, 0, "fmin at each cycle with no dependency and 1 nop on 1 core" , FMIN_2_1, "FMIN_2_1");
    check_testcase(fmin_2, 5, NULL, 0, "fmin at each cycle with no dependency and 1 nop on 5 cores", FMIN_2_5, "FMIN_2_5");
    check_testcase(fmin_2, 8, NULL, 0, "fmin at each cycle with no dependency and 1 nop on 8 cores", FMIN_2_8, "FMIN_2_8");
    check_testcase(fmin_2, 9, NULL, 0, "fmin at each cycle with no dependency and 1 nop on 9 cores", FMIN_2_9, "FMIN_2_9");

    check_testcase(fdiv_0, 1, NULL, 0, "fdiv at each cycle with no dependency on 1 core"           , FDIV_0_1, "FDIV_0_1");
    check_testcase(fdiv_0, 5, NULL, 0, "fdiv at each cycle with no dependency on 5 cores"          , FDIV_0_5, "FDIV_0_5");
    check_testcase(fdiv_0, 8, NULL, 0, "fdiv at each cycle with no dependency on 8 cores"          , FDIV_0_8, "FDIV_0_8");
    check_testcase(fdiv_0, 9, NULL, 0, "fdiv at each cycle with no dependency on 9 cores"          , FDIV_0_9, "FDIV_0_9");
    check_testcase(fdiv_1, 1, NULL, 0, "fdiv at each cycle with dependency on 1 core"              , FDIV_1_1, "FDIV_1_1");
    check_testcase(fdiv_1, 5, NULL, 0, "fdiv at each cycle with dependency on 5 cores"             , FDIV_1_5, "FDIV_1_5");
    check_testcase(fdiv_1, 8, NULL, 0, "fdiv at each cycle with dependency on 8 cores"             , FDIV_1_8, "FDIV_1_8");
    check_testcase(fdiv_1, 9, NULL, 0, "fdiv at each cycle with dependency on 9 cores"             , FDIV_1_9, "FDIV_1_9");
    check_testcase(fdiv_2, 1, NULL, 0, "fdiv at each cycle with no dependency and 1 nop on 1 core" , FDIV_2_1, "FDIV_2_1");
    check_testcase(fdiv_2, 5, NULL, 0, "fdiv at each cycle with no dependency and 1 nop on 5 cores", FDIV_2_5, "FDIV_2_5");
    check_testcase(fdiv_2, 8, NULL, 0, "fdiv at each cycle with no dependency and 1 nop on 8 cores", FDIV_2_8, "FDIV_2_8");
    check_testcase(fdiv_2, 9, NULL, 0, "fdiv at each cycle with no dependency and 1 nop on 9 cores", FDIV_2_9, "FDIV_2_9");

    check_testcase(fsqrt_0, 1, NULL, 0, "fsqrt at each cycle with no dependency on 1 core"           , FSQRT_0_1, "FSQRT_0_1");
    check_testcase(fsqrt_0, 5, NULL, 0, "fsqrt at each cycle with no dependency on 5 cores"          , FSQRT_0_5, "FSQRT_0_5");
    check_testcase(fsqrt_0, 8, NULL, 0, "fsqrt at each cycle with no dependency on 8 cores"          , FSQRT_0_8, "FSQRT_0_8");
    check_testcase(fsqrt_0, 9, NULL, 0, "fsqrt at each cycle with no dependency on 9 cores"          , FSQRT_0_9, "FSQRT_0_9");
    check_testcase(fsqrt_1, 1, NULL, 0, "fsqrt at each cycle with dependency on 1 core"              , FSQRT_1_1, "FSQRT_1_1");
    check_testcase(fsqrt_1, 5, NULL, 0, "fsqrt at each cycle with dependency on 5 cores"             , FSQRT_1_5, "FSQRT_1_5");
    check_testcase(fsqrt_1, 8, NULL, 0, "fsqrt at each cycle with dependency on 8 cores"             , FSQRT_1_8, "FSQRT_1_8");
    check_testcase(fsqrt_1, 9, NULL, 0, "fsqrt at each cycle with dependency on 9 cores"             , FSQRT_1_9, "FSQRT_1_9");
    check_testcase(fsqrt_2, 1, NULL, 0, "fsqrt at each cycle with no dependency and 1 nop on 1 core" , FSQRT_2_1, "FSQRT_2_1");
    check_testcase(fsqrt_2, 5, NULL, 0, "fsqrt at each cycle with no dependency and 1 nop on 5 cores", FSQRT_2_5, "FSQRT_2_5");
    check_testcase(fsqrt_2, 8, NULL, 0, "fsqrt at each cycle with no dependency and 1 nop on 8 cores", FSQRT_2_8, "FSQRT_2_8");
    check_testcase(fsqrt_2, 9, NULL, 0, "fsqrt at each cycle with no dependency and 1 nop on 9 cores", FSQRT_2_9, "FSQRT_2_9");

    check_testcase(fdiv_h_0, 1, NULL, 0, "fdiv.h at each cycle with no dependency on 1 core"           , FDIV_H_0_1, "FDIV_H_0_1");
    check_testcase(fdiv_h_0, 5, NULL, 0, "fdiv.h at each cycle with no dependency on 5 cores"          , FDIV_H_0_5, "FDIV_H_0_5");
    check_testcase(fdiv_h_0, 8, NULL, 0, "fdiv.h at each cycle with no dependency on 8 cores"          , FDIV_H_0_8, "FDIV_H_0_8");
    check_testcase(fdiv_h_0, 9, NULL, 0, "fdiv.h at each cycle with no dependency on 9 cores"          , FDIV_H_0_9, "FDIV_H_0_9");
    check_testcase(fdiv_h_1, 1, NULL, 0, "fdiv.h at each cycle with dependency on 1 core"              , FDIV_H_1_1, "FDIV_H_1_1");
    check_testcase(fdiv_h_1, 5, NULL, 0, "fdiv.h at each cycle with dependency on 5 cores"             , FDIV_H_1_5, "FDIV_H_1_5");
    check_testcase(fdiv_h_1, 8, NULL, 0, "fdiv.h at each cycle with dependency on 8 cores"             , FDIV_H_1_8, "FDIV_H_1_8");
    check_testcase(fdiv_h_1, 9, NULL, 0, "fdiv.h at each cycle with dependency on 9 cores"             , FDIV_H_1_9, "FDIV_H_1_9");
    check_testcase(fdiv_h_2, 1, NULL, 0, "fdiv.h at each cycle with no dependency and 1 nop on 1 core" , FDIV_H_2_1, "FDIV_H_2_1");
    check_testcase(fdiv_h_2, 5, NULL, 0, "fdiv.h at each cycle with no dependency and 1 nop on 5 cores", FDIV_H_2_5, "FDIV_H_2_5");
    check_testcase(fdiv_h_2, 8, NULL, 0, "fdiv.h at each cycle with no dependency and 1 nop on 8 cores", FDIV_H_2_8, "FDIV_H_2_8");
    check_testcase(fdiv_h_2, 9, NULL, 0, "fdiv.h at each cycle with no dependency and 1 nop on 9 cores", FDIV_H_2_9, "FDIV_H_2_9");

    check_testcase(fcvt_h_0, 1, NULL, 0, "fcvt.h at each cycle with no dependency on 1 core"           , FCVT_H_0_1, "FCVT_H_0_1");
    check_testcase(fcvt_h_0, 5, NULL, 0, "fcvt.h at each cycle with no dependency on 5 cores"          , FCVT_H_0_5, "FCVT_H_0_5");
    check_testcase(fcvt_h_0, 8, NULL, 0, "fcvt.h at each cycle with no dependency on 8 cores"          , FCVT_H_0_8, "FCVT_H_0_8");
    check_testcase(fcvt_h_0, 9, NULL, 0, "fcvt.h at each cycle with no dependency on 9 cores"          , FCVT_H_0_9, "FCVT_H_0_9");
    check_testcase(fcvt_h_1, 1, NULL, 0, "fcvt.h at each cycle with dependency on 1 core"              , FCVT_H_1_1, "FCVT_H_1_1");
    check_testcase(fcvt_h_1, 5, NULL, 0, "fcvt.h at each cycle with dependency on 5 cores"             , FCVT_H_1_5, "FCVT_H_1_5");
    check_testcase(fcvt_h_1, 8, NULL, 0, "fcvt.h at each cycle with dependency on 8 cores"             , FCVT_H_1_8, "FCVT_H_1_8");
    check_testcase(fcvt_h_1, 9, NULL, 0, "fcvt.h at each cycle with dependency on 9 cores"             , FCVT_H_1_9, "FCVT_H_1_9");
    check_testcase(fcvt_h_2, 1, NULL, 0, "fcvt.h at each cycle with no dependency and 1 nop on 1 core" , FCVT_H_2_1, "FCVT_H_2_1");
    check_testcase(fcvt_h_2, 5, NULL, 0, "fcvt.h at each cycle with no dependency and 1 nop on 5 cores", FCVT_H_2_5, "FCVT_H_2_5");
    check_testcase(fcvt_h_2, 8, NULL, 0, "fcvt.h at each cycle with no dependency and 1 nop on 8 cores", FCVT_H_2_8, "FCVT_H_2_8");
    check_testcase(fcvt_h_2, 9, NULL, 0, "fcvt.h at each cycle with no dependency and 1 nop on 9 cores", FCVT_H_2_9, "FCVT_H_2_9");

    check_testcase(fmadd_0, 1, NULL, 0, "fmadd at each cycle with no dependency on 1 core"           , FMADD_0_1, "FMADD_0_1");
    check_testcase(fmadd_0, 5, NULL, 0, "fmadd at each cycle with no dependency on 5 cores"          , FMADD_0_5, "FMADD_0_5");
    check_testcase(fmadd_0, 8, NULL, 0, "fmadd at each cycle with no dependency on 8 cores"          , FMADD_0_8, "FMADD_0_8");
    check_testcase(fmadd_0, 9, NULL, 0, "fmadd at each cycle with no dependency on 9 cores"          , FMADD_0_9, "FMADD_0_9");
    check_testcase(fmadd_1, 1, NULL, 0, "fmadd at each cycle with dependency on 1 core"              , FMADD_1_1, "FMADD_1_1");
    check_testcase(fmadd_1, 5, NULL, 0, "fmadd at each cycle with dependency on 5 cores"             , FMADD_1_5, "FMADD_1_5");
    check_testcase(fmadd_1, 8, NULL, 0, "fmadd at each cycle with dependency on 8 cores"             , FMADD_1_8, "FMADD_1_8");
    check_testcase(fmadd_1, 9, NULL, 0, "fmadd at each cycle with dependency on 9 cores"             , FMADD_1_9, "FMADD_1_9");
    check_testcase(fmadd_2, 1, NULL, 0, "fmadd at each cycle with no dependency and 1 nop on 1 core" , FMADD_2_1, "FMADD_2_1");
    check_testcase(fmadd_2, 5, NULL, 0, "fmadd at each cycle with no dependency and 1 nop on 5 cores", FMADD_2_5, "FMADD_2_5");
    check_testcase(fmadd_2, 8, NULL, 0, "fmadd at each cycle with no dependency and 1 nop on 8 cores", FMADD_2_8, "FMADD_2_8");
    check_testcase(fmadd_2, 9, NULL, 0, "fmadd at each cycle with no dependency and 1 nop on 9 cores", FMADD_2_9, "FMADD_2_9");

    check_testcase(fnmadd_0, 1, NULL, 0, "fnmadd at each cycle with no dependency on 1 core"           , FNMADD_0_1, "FNMADD_0_1");
    check_testcase(fnmadd_0, 5, NULL, 0, "fnmadd at each cycle with no dependency on 5 cores"          , FNMADD_0_5, "FNMADD_0_5");
    check_testcase(fnmadd_0, 8, NULL, 0, "fnmadd at each cycle with no dependency on 8 cores"          , FNMADD_0_8, "FNMADD_0_8");
    check_testcase(fnmadd_0, 9, NULL, 0, "fnmadd at each cycle with no dependency on 9 cores"          , FNMADD_0_9, "FNMADD_0_9");
    check_testcase(fnmadd_1, 1, NULL, 0, "fnmadd at each cycle with dependency on 1 core"              , FNMADD_1_1, "FNMADD_1_1");
    check_testcase(fnmadd_1, 5, NULL, 0, "fnmadd at each cycle with dependency on 5 cores"             , FNMADD_1_5, "FNMADD_1_5");
    check_testcase(fnmadd_1, 8, NULL, 0, "fnmadd at each cycle with dependency on 8 cores"             , FNMADD_1_8, "FNMADD_1_8");
    check_testcase(fnmadd_1, 9, NULL, 0, "fnmadd at each cycle with dependency on 9 cores"             , FNMADD_1_9, "FNMADD_1_9");
    check_testcase(fnmadd_2, 1, NULL, 0, "fnmadd at each cycle with no dependency and 1 nop on 1 core" , FNMADD_2_1, "FNMADD_2_1");
    check_testcase(fnmadd_2, 5, NULL, 0, "fnmadd at each cycle with no dependency and 1 nop on 5 cores", FNMADD_2_5, "FNMADD_2_5");
    check_testcase(fnmadd_2, 8, NULL, 0, "fnmadd at each cycle with no dependency and 1 nop on 8 cores", FNMADD_2_8, "FNMADD_2_8");
    check_testcase(fnmadd_2, 9, NULL, 0, "fnmadd at each cycle with no dependency and 1 nop on 9 cores", FNMADD_2_9, "FNMADD_2_9");


    check_testcase(add_d_0, 1, NULL, 0, "add.d at each cycle with no dependency on 1 core"           , ADD_D_0_1, "ADD_D_0_1");
    check_testcase(add_d_0, 5, NULL, 0, "add.d at each cycle with no dependency on 5 cores"          , ADD_D_0_5, "ADD_D_0_5");
    check_testcase(add_d_0, 8, NULL, 0, "add.d at each cycle with no dependency on 8 cores"          , ADD_D_0_8, "ADD_D_0_8");
    check_testcase(add_d_0, 9, NULL, 0, "add.d at each cycle with no dependency on 9 cores"          , ADD_D_0_9, "ADD_D_0_9");
    check_testcase(add_d_1, 1, NULL, 0, "add.d at each cycle with dependency on 1 core"              , ADD_D_1_1, "ADD_D_1_1");
    check_testcase(add_d_1, 5, NULL, 0, "add.d at each cycle with dependency on 5 cores"             , ADD_D_1_5, "ADD_D_1_5");
    check_testcase(add_d_1, 8, NULL, 0, "add.d at each cycle with dependency on 8 cores"             , ADD_D_1_8, "ADD_D_1_8");
    check_testcase(add_d_1, 9, NULL, 0, "add.d at each cycle with dependency on 9 cores"             , ADD_D_1_9, "ADD_D_1_9");
    check_testcase(add_d_2, 1, NULL, 0, "add.d at each cycle with no dependency and 1 nop on 1 core" , ADD_D_2_1, "ADD_D_2_1");
    check_testcase(add_d_2, 5, NULL, 0, "add.d at each cycle with no dependency and 1 nop on 5 cores", ADD_D_2_5, "ADD_D_2_5");
    check_testcase(add_d_2, 8, NULL, 0, "add.d at each cycle with no dependency and 1 nop on 8 cores", ADD_D_2_8, "ADD_D_2_8");
    check_testcase(add_d_2, 9, NULL, 0, "add.d at each cycle with no dependency and 1 nop on 9 cores", ADD_D_2_9, "ADD_D_2_9");

    check_testcase(p_muls_d_0, 1, NULL, 0, "p.muls.d at each cycle with no dependency on 1 core"           , P_MULS_D_0_1, "P_MULS_D_0_1");
    check_testcase(p_muls_d_0, 5, NULL, 0, "p.muls.d at each cycle with no dependency on 5 cores"          , P_MULS_D_0_5, "P_MULS_D_0_5");
    check_testcase(p_muls_d_0, 8, NULL, 0, "p.muls.d at each cycle with no dependency on 8 cores"          , P_MULS_D_0_8, "P_MULS_D_0_8");
    check_testcase(p_muls_d_0, 9, NULL, 0, "p.muls.d at each cycle with no dependency on 9 cores"          , P_MULS_D_0_9, "P_MULS_D_0_9");
    check_testcase(p_muls_d_1, 1, NULL, 0, "p.muls.d at each cycle with dependency on 1 core"              , P_MULS_D_1_1, "P_MULS_D_1_1");
    check_testcase(p_muls_d_1, 5, NULL, 0, "p.muls.d at each cycle with dependency on 5 cores"             , P_MULS_D_1_5, "P_MULS_D_1_5");
    check_testcase(p_muls_d_1, 8, NULL, 0, "p.muls.d at each cycle with dependency on 8 cores"             , P_MULS_D_1_8, "P_MULS_D_1_8");
    check_testcase(p_muls_d_1, 9, NULL, 0, "p.muls.d at each cycle with dependency on 9 cores"             , P_MULS_D_1_9, "P_MULS_D_1_9");
    check_testcase(p_muls_d_2, 1, NULL, 0, "p.muls.d at each cycle with no dependency and 1 nop on 1 core" , P_MULS_D_2_1, "P_MULS_D_2_1");
    check_testcase(p_muls_d_2, 5, NULL, 0, "p.muls.d at each cycle with no dependency and 1 nop on 5 cores", P_MULS_D_2_5, "P_MULS_D_2_5");
    check_testcase(p_muls_d_2, 8, NULL, 0, "p.muls.d at each cycle with no dependency and 1 nop on 8 cores", P_MULS_D_2_8, "P_MULS_D_2_8");
    check_testcase(p_muls_d_2, 9, NULL, 0, "p.muls.d at each cycle with no dependency and 1 nop on 9 cores", P_MULS_D_2_9, "P_MULS_D_2_9");

    check_testcase(p_mulhu_d_0, 1, NULL, 0, "p.mulhu.d at each cycle with no dependency on 1 core"           , P_MULHU_D_0_1, "P_MULHU_D_0_1");
    check_testcase(p_mulhu_d_0, 5, NULL, 0, "p.mulhu.d at each cycle with no dependency on 5 cores"          , P_MULHU_D_0_5, "P_MULHU_D_0_5");
    check_testcase(p_mulhu_d_0, 8, NULL, 0, "p.mulhu.d at each cycle with no dependency on 8 cores"          , P_MULHU_D_0_8, "P_MULHU_D_0_8");
    check_testcase(p_mulhu_d_0, 9, NULL, 0, "p.mulhu.d at each cycle with no dependency on 9 cores"          , P_MULHU_D_0_9, "P_MULHU_D_0_9");
    check_testcase(p_mulhu_d_1, 1, NULL, 0, "p.mulhu.d at each cycle with dependency on 1 core"              , P_MULHU_D_1_1, "P_MULHU_D_1_1");
    check_testcase(p_mulhu_d_1, 5, NULL, 0, "p.mulhu.d at each cycle with dependency on 5 cores"             , P_MULHU_D_1_5, "P_MULHU_D_1_5");
    check_testcase(p_mulhu_d_1, 8, NULL, 0, "p.mulhu.d at each cycle with dependency on 8 cores"             , P_MULHU_D_1_8, "P_MULHU_D_1_8");
    check_testcase(p_mulhu_d_1, 9, NULL, 0, "p.mulhu.d at each cycle with dependency on 9 cores"             , P_MULHU_D_1_9, "P_MULHU_D_1_9");
    check_testcase(p_mulhu_d_2, 1, NULL, 0, "p.mulhu.d at each cycle with no dependency and 1 nop on 1 core" , P_MULHU_D_2_1, "P_MULHU_D_2_1");
    check_testcase(p_mulhu_d_2, 5, NULL, 0, "p.mulhu.d at each cycle with no dependency and 1 nop on 5 cores", P_MULHU_D_2_5, "P_MULHU_D_2_5");
    check_testcase(p_mulhu_d_2, 8, NULL, 0, "p.mulhu.d at each cycle with no dependency and 1 nop on 8 cores", P_MULHU_D_2_8, "P_MULHU_D_2_8");
    check_testcase(p_mulhu_d_2, 9, NULL, 0, "p.mulhu.d at each cycle with no dependency and 1 nop on 9 cores", P_MULHU_D_2_9, "P_MULHU_D_2_9");

    check_testcase(p_mac_d_0, 1, NULL, 0, "p.mac.d at each cycle with no dependency on 1 core"           , P_MAC_D_0_1, "P_MAC_D_0_1");
    check_testcase(p_mac_d_0, 5, NULL, 0, "p.mac.d at each cycle with no dependency on 5 cores"          , P_MAC_D_0_5, "P_MAC_D_0_5");
    check_testcase(p_mac_d_0, 8, NULL, 0, "p.mac.d at each cycle with no dependency on 8 cores"          , P_MAC_D_0_8, "P_MAC_D_0_8");
    check_testcase(p_mac_d_0, 9, NULL, 0, "p.mac.d at each cycle with no dependency on 9 cores"          , P_MAC_D_0_9, "P_MAC_D_0_9");
    check_testcase(p_mac_d_1, 1, NULL, 0, "p.mac.d at each cycle with dependency on 1 core"              , P_MAC_D_1_1, "P_MAC_D_1_1");
    check_testcase(p_mac_d_1, 5, NULL, 0, "p.mac.d at each cycle with dependency on 5 cores"             , P_MAC_D_1_5, "P_MAC_D_1_5");
    check_testcase(p_mac_d_1, 8, NULL, 0, "p.mac.d at each cycle with dependency on 8 cores"             , P_MAC_D_1_8, "P_MAC_D_1_8");
    check_testcase(p_mac_d_1, 9, NULL, 0, "p.mac.d at each cycle with dependency on 9 cores"             , P_MAC_D_1_9, "P_MAC_D_1_9");
    check_testcase(p_mac_d_2, 1, NULL, 0, "p.mac.d at each cycle with no dependency and 1 nop on 1 core" , P_MAC_D_2_1, "P_MAC_D_2_1");
    check_testcase(p_mac_d_2, 5, NULL, 0, "p.mac.d at each cycle with no dependency and 1 nop on 5 cores", P_MAC_D_2_5, "P_MAC_D_2_5");
    check_testcase(p_mac_d_2, 8, NULL, 0, "p.mac.d at each cycle with no dependency and 1 nop on 8 cores", P_MAC_D_2_8, "P_MAC_D_2_8");
    check_testcase(p_mac_d_2, 9, NULL, 0, "p.mac.d at each cycle with no dependency and 1 nop on 9 cores", P_MAC_D_2_9, "P_MAC_D_2_9");

    check_testcase(p_cnt_d_0, 1, NULL, 0, "p.cnt.d at each cycle with no dependency on 1 core"           , P_CNT_D_0_1, "P_CNT_D_0_1");
    check_testcase(p_cnt_d_0, 5, NULL, 0, "p.cnt.d at each cycle with no dependency on 5 cores"          , P_CNT_D_0_5, "P_CNT_D_0_5");
    check_testcase(p_cnt_d_0, 8, NULL, 0, "p.cnt.d at each cycle with no dependency on 8 cores"          , P_CNT_D_0_8, "P_CNT_D_0_8");
    check_testcase(p_cnt_d_0, 9, NULL, 0, "p.cnt.d at each cycle with no dependency on 9 cores"          , P_CNT_D_0_9, "P_CNT_D_0_9");
    check_testcase(p_cnt_d_1, 1, NULL, 0, "p.cnt.d at each cycle with dependency on 1 core"              , P_CNT_D_1_1, "P_CNT_D_1_1");
    check_testcase(p_cnt_d_1, 5, NULL, 0, "p.cnt.d at each cycle with dependency on 5 cores"             , P_CNT_D_1_5, "P_CNT_D_1_5");
    check_testcase(p_cnt_d_1, 8, NULL, 0, "p.cnt.d at each cycle with dependency on 8 cores"             , P_CNT_D_1_8, "P_CNT_D_1_8");
    check_testcase(p_cnt_d_1, 9, NULL, 0, "p.cnt.d at each cycle with dependency on 9 cores"             , P_CNT_D_1_9, "P_CNT_D_1_9");
    check_testcase(p_cnt_d_2, 1, NULL, 0, "p.cnt.d at each cycle with no dependency and 1 nop on 1 core" , P_CNT_D_2_1, "P_CNT_D_2_1");
    check_testcase(p_cnt_d_2, 5, NULL, 0, "p.cnt.d at each cycle with no dependency and 1 nop on 5 cores", P_CNT_D_2_5, "P_CNT_D_2_5");
    check_testcase(p_cnt_d_2, 8, NULL, 0, "p.cnt.d at each cycle with no dependency and 1 nop on 8 cores", P_CNT_D_2_8, "P_CNT_D_2_8");
    check_testcase(p_cnt_d_2, 9, NULL, 0, "p.cnt.d at each cycle with no dependency and 1 nop on 9 cores", P_CNT_D_2_9, "P_CNT_D_2_9");

    check_testcase(p_exths_d_0, 1, NULL, 0, "p.exths.d at each cycle with no dependency on 1 core"           , P_EXTHS_D_0_1, "P_EXTHS_D_0_1");
    check_testcase(p_exths_d_0, 5, NULL, 0, "p.exths.d at each cycle with no dependency on 5 cores"          , P_EXTHS_D_0_5, "P_EXTHS_D_0_5");
    check_testcase(p_exths_d_0, 8, NULL, 0, "p.exths.d at each cycle with no dependency on 8 cores"          , P_EXTHS_D_0_8, "P_EXTHS_D_0_8");
    check_testcase(p_exths_d_0, 9, NULL, 0, "p.exths.d at each cycle with no dependency on 9 cores"          , P_EXTHS_D_0_9, "P_EXTHS_D_0_9");
    check_testcase(p_exths_d_1, 1, NULL, 0, "p.exths.d at each cycle with dependency on 1 core"              , P_EXTHS_D_1_1, "P_EXTHS_D_1_1");
    check_testcase(p_exths_d_1, 5, NULL, 0, "p.exths.d at each cycle with dependency on 5 cores"             , P_EXTHS_D_1_5, "P_EXTHS_D_1_5");
    check_testcase(p_exths_d_1, 8, NULL, 0, "p.exths.d at each cycle with dependency on 8 cores"             , P_EXTHS_D_1_8, "P_EXTHS_D_1_8");
    check_testcase(p_exths_d_1, 9, NULL, 0, "p.exths.d at each cycle with dependency on 9 cores"             , P_EXTHS_D_1_9, "P_EXTHS_D_1_9");
    check_testcase(p_exths_d_2, 1, NULL, 0, "p.exths.d at each cycle with no dependency and 1 nop on 1 core" , P_EXTHS_D_2_1, "P_EXTHS_D_2_1");
    check_testcase(p_exths_d_2, 5, NULL, 0, "p.exths.d at each cycle with no dependency and 1 nop on 5 cores", P_EXTHS_D_2_5, "P_EXTHS_D_2_5");
    check_testcase(p_exths_d_2, 8, NULL, 0, "p.exths.d at each cycle with no dependency and 1 nop on 8 cores", P_EXTHS_D_2_8, "P_EXTHS_D_2_8");
    check_testcase(p_exths_d_2, 9, NULL, 0, "p.exths.d at each cycle with no dependency and 1 nop on 9 cores", P_EXTHS_D_2_9, "P_EXTHS_D_2_9");


    check_testcase(vfadd_h_0, 1, NULL, 0, "vfadd.h at each cycle with no dependency on 1 core"           , VFADD_H_0_1, "VFADD_H_0_1");
    check_testcase(vfadd_h_0, 5, NULL, 0, "vfadd.h at each cycle with no dependency on 5 cores"          , VFADD_H_0_5, "VFADD_H_0_5");
    check_testcase(vfadd_h_0, 8, NULL, 0, "vfadd.h at each cycle with no dependency on 8 cores"          , VFADD_H_0_8, "VFADD_H_0_8");
    check_testcase(vfadd_h_0, 9, NULL, 0, "vfadd.h at each cycle with no dependency on 9 cores"          , VFADD_H_0_9, "VFADD_H_0_9");
    check_testcase(vfadd_h_1, 1, NULL, 0, "vfadd.h at each cycle with dependency on 1 core"              , VFADD_H_1_1, "VFADD_H_1_1");
    check_testcase(vfadd_h_1, 5, NULL, 0, "vfadd.h at each cycle with dependency on 5 cores"             , VFADD_H_1_5, "VFADD_H_1_5");
    check_testcase(vfadd_h_1, 8, NULL, 0, "vfadd.h at each cycle with dependency on 8 cores"             , VFADD_H_1_8, "VFADD_H_1_8");
    check_testcase(vfadd_h_1, 9, NULL, 0, "vfadd.h at each cycle with dependency on 9 cores"             , VFADD_H_1_9, "VFADD_H_1_9");
    check_testcase(vfadd_h_2, 1, NULL, 0, "vfadd.h at each cycle with no dependency and 1 nop on 1 core" , VFADD_H_2_1, "VFADD_H_2_1");
    check_testcase(vfadd_h_2, 5, NULL, 0, "vfadd.h at each cycle with no dependency and 1 nop on 5 cores", VFADD_H_2_5, "VFADD_H_2_5");
    check_testcase(vfadd_h_2, 8, NULL, 0, "vfadd.h at each cycle with no dependency and 1 nop on 8 cores", VFADD_H_2_8, "VFADD_H_2_8");
    check_testcase(vfadd_h_2, 9, NULL, 0, "vfadd.h at each cycle with no dependency and 1 nop on 9 cores", VFADD_H_2_9, "VFADD_H_2_9");

    check_testcase(vfmul_h_0, 1, NULL, 0, "vfmul.h at each cycle with no dependency on 1 core"           , VFMUL_H_0_1, "VFMUL_H_0_1");
    check_testcase(vfmul_h_0, 5, NULL, 0, "vfmul.h at each cycle with no dependency on 5 cores"          , VFMUL_H_0_5, "VFMUL_H_0_5");
    check_testcase(vfmul_h_0, 8, NULL, 0, "vfmul.h at each cycle with no dependency on 8 cores"          , VFMUL_H_0_8, "VFMUL_H_0_8");
    check_testcase(vfmul_h_0, 9, NULL, 0, "vfmul.h at each cycle with no dependency on 9 cores"          , VFMUL_H_0_9, "VFMUL_H_0_9");
    check_testcase(vfmul_h_1, 1, NULL, 0, "vfmul.h at each cycle with dependency on 1 core"              , VFMUL_H_1_1, "VFMUL_H_1_1");
    check_testcase(vfmul_h_1, 5, NULL, 0, "vfmul.h at each cycle with dependency on 5 cores"             , VFMUL_H_1_5, "VFMUL_H_1_5");
    check_testcase(vfmul_h_1, 8, NULL, 0, "vfmul.h at each cycle with dependency on 8 cores"             , VFMUL_H_1_8, "VFMUL_H_1_8");
    check_testcase(vfmul_h_1, 9, NULL, 0, "vfmul.h at each cycle with dependency on 9 cores"             , VFMUL_H_1_9, "VFMUL_H_1_9");
    check_testcase(vfmul_h_2, 1, NULL, 0, "vfmul.h at each cycle with no dependency and 1 nop on 1 core" , VFMUL_H_2_1, "VFMUL_H_2_1");
    check_testcase(vfmul_h_2, 5, NULL, 0, "vfmul.h at each cycle with no dependency and 1 nop on 5 cores", VFMUL_H_2_5, "VFMUL_H_2_5");
    check_testcase(vfmul_h_2, 8, NULL, 0, "vfmul.h at each cycle with no dependency and 1 nop on 8 cores", VFMUL_H_2_8, "VFMUL_H_2_8");
    check_testcase(vfmul_h_2, 9, NULL, 0, "vfmul.h at each cycle with no dependency and 1 nop on 9 cores", VFMUL_H_2_9, "VFMUL_H_2_9");

    check_testcase(vfmac_h_0, 1, NULL, 0, "vfmac.h at each cycle with no dependency on 1 core"           , VFMAC_H_0_1, "VFMAC_H_0_1");
    check_testcase(vfmac_h_0, 5, NULL, 0, "vfmac.h at each cycle with no dependency on 5 cores"          , VFMAC_H_0_5, "VFMAC_H_0_5");
    check_testcase(vfmac_h_0, 8, NULL, 0, "vfmac.h at each cycle with no dependency on 8 cores"          , VFMAC_H_0_8, "VFMAC_H_0_8");
    check_testcase(vfmac_h_0, 9, NULL, 0, "vfmac.h at each cycle with no dependency on 9 cores"          , VFMAC_H_0_9, "VFMAC_H_0_9");
    check_testcase(vfmac_h_1, 1, NULL, 0, "vfmac.h at each cycle with dependency on 1 core"              , VFMAC_H_1_1, "VFMAC_H_1_1");
    check_testcase(vfmac_h_1, 5, NULL, 0, "vfmac.h at each cycle with dependency on 5 cores"             , VFMAC_H_1_5, "VFMAC_H_1_5");
    check_testcase(vfmac_h_1, 8, NULL, 0, "vfmac.h at each cycle with dependency on 8 cores"             , VFMAC_H_1_8, "VFMAC_H_1_8");
    check_testcase(vfmac_h_1, 9, NULL, 0, "vfmac.h at each cycle with dependency on 9 cores"             , VFMAC_H_1_9, "VFMAC_H_1_9");
    check_testcase(vfmac_h_2, 1, NULL, 0, "vfmac.h at each cycle with no dependency and 1 nop on 1 core" , VFMAC_H_2_1, "VFMAC_H_2_1");
    check_testcase(vfmac_h_2, 5, NULL, 0, "vfmac.h at each cycle with no dependency and 1 nop on 5 cores", VFMAC_H_2_5, "VFMAC_H_2_5");
    check_testcase(vfmac_h_2, 8, NULL, 0, "vfmac.h at each cycle with no dependency and 1 nop on 8 cores", VFMAC_H_2_8, "VFMAC_H_2_8");
    check_testcase(vfmac_h_2, 9, NULL, 0, "vfmac.h at each cycle with no dependency and 1 nop on 9 cores", VFMAC_H_2_9, "VFMAC_H_2_9");

    check_testcase(fdiv_0, 4, fdiv_h_0, 4, "fdiv on 4 cores mixed with fdiv.h on 4 cores", FDIV_FDIV_H_4_4, "FDIV_FDIV_H_4_4");
    check_testcase(fdiv_0, 2, fdiv_h_0, 6, "fdiv on 2 cores mixed with fdiv.h on 6 cores", FDIV_FDIV_H_2_6, "FDIV_FDIV_H_2_6");
    check_testcase(fdiv_0, 1, fadd_0, 7, "fdiv on 1 cores mixed with fadd on 7 cores", FDIV_FADD_1_7, "FDIV_FADD_1_7");
    check_testcase(add_d_0, 4, fadd_0, 4, "add.d on 4 cores mixed with fadd on 4 cores", ADD_D_FADD_4_4, "ADD_D_FADD_4_4");
}

static int test_task_sync()
{
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;
    struct pi_cluster_task task;

    pi_cluster_conf_init(&conf);
    conf.id = 0;

    pi_open_from_conf(&cluster_dev, &conf);
      
    pi_cluster_open(&cluster_dev);

    nb_errors = 0;

    pi_cluster_task(&task, &cluster_entry, NULL);
    pi_cluster_send_task_to_cl(&cluster_dev, &task);
    pi_cluster_close(&cluster_dev);

    return nb_errors;
}

int test_entry()
{
    int errors = 0;

    printf("Starting test\n");

    errors += test_task_sync();

    if (errors)
      printf("Test failure\n");
    else
      printf("Test success\n");

    return errors;
}

void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
