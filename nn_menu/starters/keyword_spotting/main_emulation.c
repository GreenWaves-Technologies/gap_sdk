/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s 

#define pmsis_exit exit
#define pi_l2_malloc malloc
#define pi_l2_free(ptr,size) free(ptr)

/* Autotiler includes. */
#include "Gap.h"
#ifdef SMALL
	#include "KWS_ds_cnn_s_quantKernels.h"
    #include "MFCC_params.h"
#endif
#ifdef MEDIUM
    #include "KWS_ds_cnn_m_quantKernels.h"
    #include "MFCC_params.h"
#endif
#ifdef LARGE
	#include "KWS_ds_cnn_l_quantKernels.h"
    #include "MFCC_params.h"
#endif
#ifdef WITH_MFCC
    #include "gaplib/wavIO.h"
#else
    #include "gaplib/ImgIO.h"
#endif
#include "MFCCKernels.h"
#include "LUT.def"
#include "MFCC_FB.def"

#define  WAV_BUFFER_SIZE    16000 // Something more than 1sec@16kHz
#define  NUM_CLASSES        12
#define  N_FRAME            49
#define  NORM               3
#define  MFCC_Q             15-NORM-7

//DCT_NORMALIZATION        -> np.sqrt(2/(N_DCT))*0.5
//NNTOOL_INPUT_SCALE_FLOAT -> 1.9372712
// SCALE = DCT_NORMALIZATION*DCT_SCALE/NNTOOL_INPUT_SCALE_FLOAT
// DCT_SCALE = 2**(-MFCC_Q)
#define  INPUT_SCALE        236
#define  INPUT_SCALEN       17


static char *LABELS[NUM_CLASSES] = {"silence", "unknown", "yes", "no", "up", "down", "left", "right", "on", "off", "stop", "go"};
L2_MEM unsigned short int *ResOut;
char *WavName = NULL;
char *ImageName = NULL;
char *ImageIn;
int off_shift = 0;
AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;

short int *mfcc_features;
short int *inSig;
int rec_digit;


static void RunMFCC(){
    L1_Memory = __PREFIX(_L1_Memory);
    PRINTF("Runnning MFCC\n");
    #if (DATA_TYPE==1) //HIGH PRECISION 32BITS FFT
    MFCC(inSig, mfcc_features, TwiddlesLUTR2, SwapTableR2, WindowLUT, MFCC_FilterBank, MFCC_Coeffs, NORM, DCT_Coeff);
    #else //LOW PRECISION 16BITS FFT
    MFCC(inSig, mfcc_features, TwiddlesLUTR2, RFFTTwiddlesLUT, SwapTableR2, WindowLUT, MFCC_FilterBank, MFCC_Coeffs, NORM, DCT_Coeff);
    #endif
}

static void Runkws()
{
  PRINTF("Running on cluster\n");
  __PREFIX(CNN)(ImageIn, (short int *) ResOut);

  //Checki Results
  rec_digit = 0;
  int highest = ResOut[0];
  PRINTF("Results: \n");
  for(int i = 0; i < NUM_CLASSES; i++) {
    if(ResOut[i] > highest) {
      highest = ResOut[i];
      rec_digit = i;
    }
    PRINTF("class %d: %d\n", i, ResOut[i]);
  }
  printf("Recognized:\t%d\n", rec_digit);
}


void kws_ds_cnn(void)
{
    printf("Entering main controller\n");
    ResOut        = (unsigned short int *) pi_l2_malloc(NUM_CLASSES             * sizeof(short int));
    ImageIn       = (char *)      pi_l2_malloc(AT_INPUT_WIDTH * AT_INPUT_HEIGHT * sizeof(char));
    mfcc_features = (short int *) pi_l2_malloc(N_FRAME * N_DCT                  * sizeof(short int));
    inSig         = (short int *) pi_l2_malloc(WAV_BUFFER_SIZE                  * sizeof(short int));
    if (mfcc_features==NULL || ImageIn==NULL || inSig==NULL || ResOut==NULL){
        printf("Error allocating output\n");
        pmsis_exit(1);
    }

    printf("\n\nConstructor\n");
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    int err_construct = __PREFIX(CNN_Construct)();
    if (err_construct)
    {
        printf("Graph constructor exited with error: %d\n", err_construct);
        pmsis_exit(-5);
    }

    #ifdef WITH_MFCC
        header_struct header_info;
        if (ReadWavFromFile(WavName, inSig, WAV_BUFFER_SIZE*sizeof(short int), &header_info)){
            printf("Error reading wav file\n");
            pmsis_exit(1);
        }

        RunMFCC();
        // Normalize
        for (int i=0; i<N_FRAME; i++) {
            // Take only the first N_CEP that you need (in this case 10)
            for (int j=0; j<AT_INPUT_WIDTH; j++) {
                ImageIn[i*AT_INPUT_WIDTH+j] = (char) gap_roundnorm(mfcc_features[i*N_DCT+j]*INPUT_SCALE, INPUT_SCALEN);
            }
        }
    #else
        /* Input image size. */
        unsigned int W = AT_INPUT_WIDTH, H = AT_INPUT_HEIGHT;
        char *ImageFromFile = (char *) pi_l2_malloc(sizeof(char) * W * H);
        if (ImageFromFile == NULL){
            printf("Failed to allocate Memory for Image (%ld bytes)\n", sizeof(char) * W * H);
            pmsis_exit(-1);
        }
        printf("Reading image\n");
        if (ReadImageFromFile(ImageName, W, H, 1, ImageFromFile, W*H*sizeof(char), IMGIO_OUTPUT_CHAR, 0)){
            printf("Failed to load image %s or dimension mismatch Expects [%dx%d]\n", ImageName, W, H);
            pmsis_exit(-2);
        }
        printf("Finished reading image\n");

        ImageIn = (signed char *) ImageFromFile;
    #endif


    PRINTF("Call cluster\n");
    Runkws();
    __PREFIX(CNN_Destruct)();
    PRINTF("Ended\n");
    pmsis_exit(0);
}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        PRINTF("Usage: mnist [image_file]\n");
        exit(-1);
    }
    #ifdef WITH_MFCC
        WavName = argv[1];
    #else
        ImageName = argv[1];
    #endif
    PRINTF("\n\n\t *** IMAGENET EMUL ***\n\n");
    kws_ds_cnn();
    return 0;
}