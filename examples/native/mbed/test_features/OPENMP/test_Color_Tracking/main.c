#include <stdint.h>
#include "omp.h"
#include "common.h"
#include "color-tracking.h"

//NOTE Make your configuration here
#define NUM_THREADS  8U
#define NB_FRAMES    3U
#define CHECK

//NOTE add here your input selection
//#define WIDTH        (320U)
//#define BAND_HEIGHT  (4U)
#include "images.h"

IMG_DATATYPE *in_frame = image; /* Input Frame */
IMG_DATATYPE out_frame[WIDTH*HEIGHT*3]; /* Output Frame (3ch)*/
IMG_DATATYPE track_frame[WIDTH*HEIGHT*3]; /* Tracking frame (3ch)*/

static inline int32_t
checkResults(uint32_t y, int32_t x, uint32_t *moments)
{
    #ifdef VERBOSE
    printf("[Color-Tracking] Checksumming...\n");
    #endif
    uint64_t cnt = 0;
    int32_t  ret = 1; //1 == fail

    cnt += moments[0] + moments[1] + moments[2];

    #ifdef PRINT_RESULTS
    printf("\n[Color-Tracking] Color Found at %d,%d (%d,%d,%d)\n", (int) y,
           (int) x,
           (int) moments[0],
           (int) moments[1],
           (int) moments[2]);
    #endif

    if(CHECKSUM)
    {
        if(CHECKSUM == cnt && CHECK_X==x && CHECK_Y==y)
        {
            #ifdef VERBOSE
            printf("[Color-Tracking] Check ...[" ANSI_COLOR_GREEN "SUCCESS" ANSI_COLOR_RESET "]\n");
            #endif
            ret = 0;
        }
        else
        {
            printf("[Color-Tracking] Check ...[" ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET "]\n");
            printf("\n[Color-Tracking] Color Found at %d,%d (%x)\n", (int) y,
                   (int) x,
                   (int) cnt);
            ret = 1;
        }

    }
    else
        printf("\n[Color-Tracking] Color Found at %d,%d (%x)\n", (int) y,
               (int) x,
               (int) cnt);

    return ret;
}
int
main()
{
    int32_t  ret = -1; //1 == fail

    uint32_t i = 0U;
    uint32_t moments[3]; /* Output color center of gravity */
    IMG_DATATYPE *l1_input[2]; // 2x 3ch input buffers
    IMG_DATATYPE *l1_input1ch;    // 1x 1ch input buffer

    /* Job ID for DMA*/
    dma_req_t job_id_read[2]    = {{0U},{0U}};
    dma_req_t job_id_write[2]   = {{0U},{0U}};
    uint16_t wait_writeback[2] = {0U,0U};

    uint32_t inputWidth    = WIDTH;
    uint32_t inputHeight   = HEIGHT;

    int32_t Y              = 0U;
    int32_t nextY          = 0U;

    uint32_t bandHeight    = BAND_HEIGHT;
    uint32_t nbands        = inputHeight / bandHeight;
    int32_t  odd           = ((int32_t) inputHeight - (int32_t)(bandHeight * nbands));
    uint32_t band1ch_size = (inputWidth*bandHeight);
    uint32_t band3h_size  = (band1ch_size*3U);
    nbands += odd > 0 ? 1 : 0;

    float moment10, moment01, area, posX, posY;

    #ifdef VERBOSE
    printf("[Color-Tracking] --------------------------------------\n");
    printf("[Color-Tracking] Configuration\n");
    printf("[Color-Tracking] --------------------------------------\n");
    printf("[Color-Tracking] inputWidth      %d\n", (int) inputWidth );
    printf("[Color-Tracking] inputHeight     %d\n", (int) inputHeight);
    printf("[Color-Tracking] bandHeight      %d\n", (int) bandHeight );
    printf("[Color-Tracking] nbands          %d\n", (int) nbands     );
    printf("[Color-Tracking] odd             %d\n", (int) odd        );
    printf("[Color-Tracking] --------------------------------------\n");
    #endif

    /* Allocate L1 Buffers for input */
    l1_input[0] = (IMG_DATATYPE *) l1malloc(band3h_size*sizeof(IMG_DATATYPE));
    l1_input[1] = (IMG_DATATYPE *) l1malloc(band3h_size*sizeof(IMG_DATATYPE));
    l1_input1ch = (IMG_DATATYPE *) l1malloc(band1ch_size*sizeof(IMG_DATATYPE));

    for(i = 0; i < NB_FRAMES; ++i)
    {
        profile_start(i);

        #ifdef VERBOSE
        printf("[Color-Tracking] Tracking on frame %d of %d...\n", (int) i, (int) NB_FRAMES);
        #endif

        /*Current Frame*/
        IMG_DATATYPE *current_frame_in = in_frame;

        /*Current Frame Out*/
        IMG_DATATYPE *current_frame_out = out_frame;

        #pragma omp parallel shared(moments, bandHeight, band1ch_size, band3h_size, Y, nextY) num_threads(NUM_THREADS)
        {
            uint32_t bufID      = 0U;
            uint32_t bandID     = 0U;
            uint32_t nextBandID = 1U;

            /* First load DMA */
            #pragma omp master
            {
                memcpy_async(l1_input[bufID],
                             &current_frame_in[0],
                             band3h_size*sizeof(IMG_DATATYPE),
                             &job_id_read[bufID]); /*  normal transfer */

                /* Reset shared variables */
                Y          = 0U;
                nextY      = 0U;
                bandHeight = BAND_HEIGHT;
                band1ch_size = inputWidth*bandHeight;
                band3h_size  = 3*band1ch_size;

                /* Reset Center of Gravity */
                moments[0] = moments[1] = moments[2] = 0U;
            }

            /*###################### Three-stages pipe ######################*/
            for(bandID = 0; bandID < nbands; bandID++)
            {
                /* swap buffer */
                if (bufID == 0U)
                    bufID = 1U;
                else
                    bufID = 0U;

                #pragma omp master
                {
                    /* Setup Y offset */
                    Y = nextY;

                    /* DMA Programming for next transfer */
                    nextBandID++;
                    if((nbands > nextBandID) || ((nbands == nextBandID) && !odd))
                    {
                        nextY+=bandHeight;
                        memcpy_async(l1_input[bufID],
                                     &current_frame_in[nextY*inputWidth*3],
                                     band3h_size*sizeof(IMG_DATATYPE),
                                     &job_id_read[bufID]); /*  normal transfer */
                    }
                    else if((nbands == nextBandID) && odd)
                    {
                        nextY+=bandHeight;
                        memcpy_async(l1_input[bufID],
                                     &current_frame_in[nextY*inputWidth*3],
                                     3*odd*inputWidth*sizeof(IMG_DATATYPE),
                                     &job_id_read[bufID]); /* odd transfer */
                    }
                    else if(odd)
                    {
                        bandHeight   = odd;
                        band1ch_size = inputWidth*bandHeight;
                        band3h_size  = 3*band1ch_size;
                    }

                    //DMA wait for input load
                    memcpy_wait(&job_id_read[!bufID]);

                    #ifdef PRINT_RESULTS
                    printf("[Color-Tracking] Working %d od %d, y=%d y+1=%d\n", (int) bandID,
                                                                               (int) nbands,
                                                                               (int) Y,
                                                                               (int) (Y+bandHeight));
                    #endif
                }
                #pragma omp barrier

                colorScaleConversion(l1_input[!bufID],
                                     l1_input[!bufID],
                                     band3h_size);

                threshold(l1_input[!bufID],
                          l1_input1ch,
                          band1ch_size);

                computeMoments(l1_input1ch,
                        moments,
                        Y,
                        inputWidth,
                        band1ch_size);
            }
        }

        #ifdef VERBOSE
        printf("[Color-Tracking] CSC+CVT+CVM end frame %d\n", (int) i);
        #endif

        /* Compute Center of Gravity */
        moment10 = moments[2];
        moment01 = moments[1];
        area     = moments[0];
        posX     = moment10 / area;
        posY     = moment01 / area;

        #ifdef VERBOSE
        printf("[Color-Tracking] Area: %d\n" ,     (int) moments[0]);
        printf("[Color-Tracking] Moment01: %d\n" , (int) moments[1]);
        printf("[Color-Tracking] Moment10: %d\n" , (int) moments[2]);
        #endif
        #ifdef VERBOSE
        printf("[Color-Tracking] end computation frame %d\n", (int) i);
        #endif

        profile_stop(i);
    }//for frame
    profile_show();

#ifdef CHECK
    checkResults(posY, posX, moments);
#endif

    l1free(l1_input[0]);
    l1free(l1_input[1]);
    l1free(l1_input1ch);

    return 0;
}
