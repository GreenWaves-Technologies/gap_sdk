#ifndef __EMUL__
	#include "pmsis.h"
#else
	#include <stdlib.h>
	#include <stdio.h>
	#define pmsis_exit(a)	exit(a)		
#endif
#ifdef USE_GPIO
    struct pi_device gpio_port;
    pi_gpio_e gpio_pin_o;
    #define GPIO PI_GPIO_A89
    #define WRITE_GPIO(x) pi_gpio_pin_write(&gpio_port, gpio_pin_o, x)
#else
    #define WRITE_GPIO(x)
#endif
#include "gaplib/wavIO.h"
#include "MFCC_params.h"
#include "MfccKernels.h"
#include "WindowLUT.def"
#include "RFFTTwiddles.def"
#include "FFTTwiddles.def"
#include "SwapTable.def"
#include "DCTTwiddles.def"
#include "MelFBCoeff.def"
#include "MelFBSparsity.def"

#define  BUF_SIZE 	16500
#define  STACK_SIZE     2048
#define  NORM           6
#define  N_FRAME        49

#if (DATA_TYPE==2)
typedef F16_DSP INOUT_TYPE;
#elif (DATA_TYPE==3)
typedef float INOUT_TYPE;
#else
typedef short int INOUT_TYPE;
#endif

INOUT_TYPE *out_feat;
INOUT_TYPE *out_fft;
INOUT_TYPE *MfccInSig;
short int *inWav;
int num_samples;
char *FileName;


#ifdef __EMUL__
char *FileName1 = "GapMel.bin";
#else
char *FileName1 = "../../../GapMel.bin";
#endif

#ifdef TEST
#include "ground_truth.h"
#include "math.h"
#if (DATA_TYPE==2) || (DATA_TYPE==3)
float QSNR_THR = 40;
#else
float QSNR_THR = 38;
#endif
#endif

static void RunMFCC()
{
        printf("run mfcc\n");
        #ifdef PERF
        gap_cl_starttimer();
        gap_cl_resethwtimer();
        int start = gap_cl_readhwtimer();
        #endif

        WRITE_GPIO(1);
        #if (N_DCT == 0)
                #if (DATA_TYPE==2) || (DATA_TYPE==3)
                Librosa_MFCC(MfccInSig, out_feat, FFTTwiddles, RFFTTwiddles, SwapTable, WindowLUT, MelFBSparsity, MelFBCoeff);
                #elif (DATA_TYPE==1)
                Librosa_MFCC(MfccInSig, out_feat, FFTTwiddles, SwapTable, WindowLUT, MelFBSparsity, MelFBCoeff, NORM);
                #else
                Librosa_MFCC(MfccInSig, out_feat, FFTTwiddles, RFFTTwiddles, SwapTable, WindowLUT, MelFBSparsity, MelFBCoeff, NORM);
                #endif
        #else
                #if (DATA_TYPE==2) || (DATA_TYPE==3)
                Librosa_MFCC(MfccInSig, out_feat, FFTTwiddles, RFFTTwiddles, SwapTable, WindowLUT, MelFBSparsity, MelFBCoeff, DCTTwiddles);
                #elif (DATA_TYPE==1)
                Librosa_MFCC(MfccInSig, out_feat, FFTTwiddles, SwapTable, WindowLUT, MelFBSparsity, MelFBCoeff, NORM, DCTTwiddles);
                #else
                Librosa_MFCC(MfccInSig, out_feat, FFTTwiddles, RFFTTwiddles, SwapTable, WindowLUT, MelFBSparsity, MelFBCoeff, NORM, DCTTwiddles);
                #endif
        #endif
        WRITE_GPIO(0);
        #ifdef PERF
	int elapsed = gap_cl_readhwtimer() - start;
	printf("Total Cycles: %d over %d Frames %d Cyc/Frame\n", elapsed, N_FRAME, elapsed / N_FRAME);
        #endif
}

void test_kickoff(void *arg)
{
	#ifndef __EMUL__
	struct pi_device cluster_dev;
	struct pi_cluster_conf cl_conf;
	pi_cluster_conf_init(&cl_conf);
	cl_conf.id = 0;
    cl_conf.cc_stack_size = STACK_SIZE;

	pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
	if (pi_cluster_open(&cluster_dev))
	{
		printf("Cluster open failed !\n");
		pmsis_exit(-4);
	}
    pi_freq_set(PI_FREQ_DOMAIN_FC, FREQ_FC*1000*1000);
    pi_freq_set(PI_FREQ_DOMAIN_CL, FREQ_CL*1000*1000);
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, FREQ_PE*1000*1000);
    printf("Set FC Frequency as %d MHz, CL Frequency = %d MHz, PERIIPH Frequency = %d\n",
            pi_freq_get(PI_FREQ_DOMAIN_FC), pi_freq_get(PI_FREQ_DOMAIN_CL), pi_freq_get(PI_FREQ_DOMAIN_PERIPH));

    #ifdef USE_GPIO
        struct pi_gpio_conf gpio_conf = {0};
        gpio_pin_o = GPIO; /* PI_GPIO_A02-PI_GPIO_A05 */
        pi_gpio_conf_init(&gpio_conf);
        pi_open_from_conf(&gpio_port, &gpio_conf);
        gpio_conf.port = (gpio_pin_o & PI_GPIO_NUM_MASK) / 32;
        int errors = pi_gpio_open(&gpio_port);
        if (errors)
        {
          printf("Error opening GPIO %d\n", errors);
          pmsis_exit(errors);
        }
        pi_gpio_pin_configure(&gpio_port, gpio_pin_o, PI_GPIO_OUTPUT);
        WRITE_GPIO(0);
    #endif
	#endif
	
	L1_Memory = (AT_L1_POINTER) AT_L1_ALLOC(0, _L1_Memory_SIZE);
	if (L1_Memory==NULL){
		printf("Error allocating L1\n");
		pmsis_exit(-1);
	}

    int frame_size;
    if (N_DCT > 0) frame_size = N_DCT;
    else           frame_size = N_MELS;
	out_feat = (INOUT_TYPE *) AT_L2_ALLOC(0, N_FRAME * frame_size * sizeof(INOUT_TYPE));
	out_fft  = (INOUT_TYPE *) AT_L2_ALLOC(0, N_FRAME * N_FFT * 2 * sizeof(INOUT_TYPE));
	inWav    = (short int *) AT_L2_ALLOC(0, BUF_SIZE * sizeof(short));
	MfccInSig = (INOUT_TYPE *) AT_L2_ALLOC(0, BUF_SIZE * sizeof(INOUT_TYPE));
	if (inWav==NULL || MfccInSig==NULL){
		printf("Error allocating\n");
		pmsis_exit(1);
	}

	header_struct header_info;
        if (ReadWavFromFile(FileName, inWav, BUF_SIZE*sizeof(short), &header_info)){
        	printf("Error reading wav file\n");
        	pmsis_exit(1);
        }
        num_samples = header_info.DataSize * 8 / (header_info.NumChannels * header_info.BitsPerSample);

        #if (DATA_TYPE==2) || (DATA_TYPE==3)
        	printf("FLOAT %d\n", (DATA_TYPE==2)?16:32);
        	for (int i=0; i<num_samples; i++) {
        		MfccInSig[i] = (INOUT_TYPE) inWav[i] / (1<<15);
        	}
        #else
            	printf("FIXED POINT\n");
                for (int i=0; i<num_samples; i++) {
                        MfccInSig[i] = (INOUT_TYPE) gap_clip(((int) inWav[i]), 15);
                }
        #endif

	#ifdef __EMUL__
		RunMFCC();
	#else
		struct pi_cluster_task task;
                pi_cluster_task(&task, RunMFCC, NULL);
		pi_cluster_send_task(&cluster_dev, &task);
	#endif

        switch_fs_t fs;
        __FS_INIT(fs);
        void *File;
        int ret = 0;

        File = __OPEN_WRITE(fs, FileName1);
        ret = __WRITE(File, out_feat, N_FRAME * frame_size * sizeof(INOUT_TYPE));
        __CLOSE(File);

        __FS_DEINIT(fs);

        #ifdef TEST
            float MSE = 0.0, SUM = 0.0;
            int idx = 0;
            for (int i=0; i<N_FRAME; i++) {
                for (int j=0; j<frame_size; j++) {
                    #if (DATA_TYPE==2) || (DATA_TYPE==3)
                          MSE += (ground_truth[i*frame_size+j] - out_feat[i*frame_size+j])*(ground_truth[i*frame_size+j] - out_feat[i*frame_size+j]);
                    #else
                          int QMFCC = 15 - NORM - 7;
                          MSE += (ground_truth[i*frame_size+j] - FIX2FP(out_feat[i*frame_size+j], QMFCC)) * (ground_truth[i*frame_size+j] - FIX2FP(out_feat[i*frame_size+j], QMFCC));
                    #endif
                    SUM += (ground_truth[i*frame_size+j])*(ground_truth[i*frame_size+j]);
                }
            }
            float QSNR = 10*log10(SUM / MSE);
            printf("QSNR: %f (thr: %f) --> ", QSNR, QSNR_THR);
            if (QSNR < QSNR_THR) {
                printf("Test NOT PASSED\n");
                pmsis_exit(-1);
            } else {
                printf("Test PASSED\n");
            }
        #endif

    pmsis_exit(0);
}

int main(int argc, char *argv[])
{
	printf("\n\n\t *** MFCC Test ***\n\n");

    #ifndef __EMUL__
        #define __XSTR(__s) __STR(__s)
        #define __STR(__s) #__s
        FileName = __XSTR(AT_WAV);
        return pmsis_kickoff((void *) test_kickoff);
    #else
        if (argc < 2) {
                printf("Usage: mnist [image_file]\n");
                exit(-1);
        }
        FileName = argv[1];
        test_kickoff(NULL);
        return 0;
    #endif
}
