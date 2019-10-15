// srad.cpp : Defines the entry point for the console application.

#include <stdint.h>
#include "omp.h"
#include "common.h"

//NOTE Make your configuration here
#define NUM_THREADS  8U
#define ITERS        3U
#define CHECK

// #define VERBOSE
// #define OUTPUT

#include "data/input.h"

static inline int32_t
checkResults(float *output, uint32_t rows, uint32_t cols)
{
    float checksum = 0.0;
    uint64_t uchecksum;
    int32_t  ret = 1U; //1 == fail
    uint32_t i = 0, j = 0;

    #ifdef VERBOSE
    printf("[SRAD] Checksumming...\n");
    #endif

    for(i = 0 ; i < rows ; i++){
        for (j = 0 ; j < cols ; j++){
            #ifdef OUTPUT
            printf("%x ", *((uint32_t *) &output[i * cols + j]));
            #endif
            checksum += output[i * cols + j];
        }
        #ifdef OUTPUT
        printf("\n");
        #endif
    }

    /* Convert to ull */
    uchecksum = *((uint32_t *) &checksum);

    #ifdef VERBOSE
    printf("[SRAD] Checksum 0x%x\n", (int)uchecksum);
    #endif

    if(CHECKSUM)
    {
        int64_t diff = CHECKSUM - uchecksum;
        if (diff < 0) diff = -diff;

        if(diff < 100)
        {
            #ifdef VERBOSE
            printf("[SRAD] Check...[" ANSI_COLOR_GREEN "SUCCESS" ANSI_COLOR_RESET "]\n");
            #endif
            ret = 0;
        }
        else
        {
            printf("[SRAD] Check...[" ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET "]\n");
            printf("[SRAD] Checksum 0x%x\n", (int) uchecksum);
            ret = 1;
        }
    }
    return ret;
}

int main()
{
    int32_t  ret = -1; //1 == fail

    int i, j;
    int rows, cols, size_I, size_R, niter = ITERS, iter, k;
    float *I, q0sqr, sum, sum2, tmp, meanROI,varROI ;
    float Jc, G2, L, num, den, qsqr;
    int *iN,*iS,*jE,*jW;
    float *dN,*dS,*dW,*dE;
    int r1, r2, c1, c2;
    float cN,cS,cW,cE;
    float *c, D;
    float lambda;

    /* Job ID for DMA*/
    uint32_t job_id_read[2]    = {0U,0U};
    uint32_t job_id_write[2]   = {0U,0U};
    uint16_t wait_writeback[2] = {0U,0U};
    uint32_t bufID             = 0U;

    rows = DIM; //number of rows in the domain
    cols = DIM; //number of cols in the domain

    r1       = 0; //y1 position of the speckle
    r2       = 4; //y2 position of the speckle
    c1       = 0; //x1 position of the speckle
    c2       = 4; //x2 position of the speckle
    lambda   = 0.5; //Lambda value   NB: a float il defaullt sarebbe stato 0.5

    size_I = cols * rows;
    size_R = (r2-r1+1)*(c2-c1+1);

    I  = (float *)l2malloc( size_I * sizeof(float) );
    c  = (float *)l2malloc(sizeof(float)* size_I) ;

    iN = (int *)l1malloc(sizeof(unsigned int*) * rows) ;
    iS = (int *)l1malloc(sizeof(unsigned int*) * rows) ;
    jW = (int *)l1malloc(sizeof(unsigned int*) * cols) ;
    jE = (int *)l1malloc(sizeof(unsigned int*) * cols) ;

    dN = (float *)l2malloc(sizeof(float)* size_I) ;
    dS = (float *)l2malloc(sizeof(float)* size_I) ;
    dW = (float *)l2malloc(sizeof(float)* size_I) ;
    dE = (float *)l2malloc(sizeof(float)* size_I) ;


    for (i=0; i< rows; i++)
    {
        iN[i] = i-1;
        iS[i] = i+1;
    }
    for (j=0; j< cols; j++)
    {
        jW[j] = j-1;
        jE[j] = j+1;
    }
    iN[0]    = 0;
    iS[rows-1] = rows-1;
    jW[0]    = 0;
    jE[cols-1] = cols-1;


#ifdef VERBOSE
    printf("Start the SRAD main loop\n");
#endif

    for (iter=0; iter< niter; iter++)
    {
        profile_start(iter);

        sum=0; sum2=0;
        for (i=r1; i<=r2; i++) {
            for (j=c1; j<=c2; j++) {
                tmp   = J[i * cols + j];
                sum  += tmp ;
                sum2 += tmp*tmp;
            }
        }
        meanROI = sum / size_R;
        varROI  = (sum2 / size_R) - meanROI*meanROI;
        q0sqr   = varROI / (meanROI*meanROI);

#ifdef VERBOSE
        #pragma omp master
        printf("[SRAD] Step one...");
#endif

        #pragma omp parallel \
                    firstprivate(dN, dS, dW, dE, c, rows, cols, iN, iS, jW, jE)\
                    private(i, j, k, Jc, G2, L, num, den, qsqr)\
                    num_threads(NUM_THREADS)

        {
            for (i = 0 ; i < rows ; i++)
            {
                #pragma omp for
                for (j = 0; j < cols; j++)
                {

                    k = i * cols + j;
                    Jc = J[k];

                    // directional derivates
                    dN[k] = J[iN[i] * cols + j] - Jc;
                    dS[k] = J[iS[i] * cols + j] - Jc;
                    dW[k] = J[i * cols + jW[j]] - Jc;
                    dE[k] = J[i * cols + jE[j]] - Jc;

                    G2 = (dN[k]*dN[k] + dS[k]*dS[k]
                    + dW[k]*dW[k] + dE[k]*dE[k]) / (Jc*Jc);

                    L = (dN[k] + dS[k] + dW[k] + dE[k]) / Jc;

                    num  = (0.5f*G2) - ((1.0f/16.0f)*(L*L)) ;
                    den  = 1 + (.25f*L);
                    qsqr = num/(den*den);

                    // diffusion coefficent (equ 33)
                    den = (qsqr-q0sqr) / (q0sqr * (1+q0sqr)) ;
                    c[k] = 1.0f / (1.0f+den) ;

                    // saturate diffusion coefficent
                    if (c[k] < 0) {c[k] = 0;}
                    else if (c[k] > 1) {c[k] = 1;}

                }

            }
        }

#ifdef VERBOSE
        #pragma omp master
        printf("done\n");
#endif

#ifdef VERBOSE
        #pragma omp master
        printf("[SRAD] Step two...");
#endif

        #pragma omp parallel for  \
                    firstprivate(I, c, rows, cols, lambda) \
                    private(i, j, k, D, cS, cN, cW, cE) \
                    num_threads(NUM_THREADS)
        for (i = 0; i < rows; i++)
        {
            for (j = 0; j < cols; j++)
            {

                // current index
                k = i * cols + j;

                // diffusion coefficent
                cN = c[k];
                cS = c[iS[i] * cols + j];
                cW = c[k];
                cE = c[i * cols + jE[j]];

                // divergence (equ 58)
                D = cN * dN[k] + cS * dS[k] + cW * dW[k] + cE * dE[k];

                // image update (equ 61)
                I[k] = J[k] + 0.25f*lambda*D;
            }

        }

#ifdef VERBOSE
        #pragma omp master
        printf("done\n");
#endif

        profile_stop(iter);

#ifdef CHECK
        ret = checkResults(I, rows, cols);
        if(ret) break;
#endif
    }
    profile_show();

    l2free(I);
    l1free(iN); l1free(iS); l1free(jW); l1free(jE);
    l2free(dN); l2free(dS); l2free(dW); l2free(dE);

    l2free(c);

    return ret;
}
