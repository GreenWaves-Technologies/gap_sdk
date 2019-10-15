#include <stdint.h>
#include "omp.h"
#include "common.h"
#include "fast.h"

//NOTE Make your configuration here
#define NUM_THREADS  8U

#define ITERS        3U
#define CHECK
//#define VERBOSE

//NOTE add here your input selection
// #define WIDTH        (32U)
// #define BAND_HEIGHT  (32U)
#include "images.h"

/*results array*/
uint32_t corners = 0;
uint32_t corners_map[WIDTH*HEIGHT*3];

static inline int32_t
checkResults(uint32_t corners_found, uint32_t *corners_coordinates)
{
#ifdef VERBOSE
    printf("[FAST] Checksumming...\n");
#endif
    uint32_t i   = 0;
    uint64_t cnt = 0;
    int32_t  ret = 1; //1 == fail

    for(i = 0; i < corners_found; i++)
    {
#ifdef VERBOSE
        printf("%d,%d,%d\n", (int)corners_coordinates[i*3],
               (int)corners_coordinates[i*3+1],
               (int)corners_coordinates[i*3+2]);
#endif
        cnt += corners_coordinates[i*3] + corners_coordinates[i*3 + 1] + corners_coordinates[i*3 + 2];
    }

#ifdef VERBOSE
    printf("[FAST] #Corners: %d (Checksum 0x%x)\n", (int)corners_found, (int)cnt);
#endif

    if(CHECK_CORNERS)
    {
        if(CHECK_CORNERS == corners_found && CHECKSUM == cnt)
        {
#ifdef VERBOSE
            printf("[FAST] Check #Corner...[" ANSI_COLOR_GREEN "SUCCESS" ANSI_COLOR_RESET "]\n");
#endif
            ret = 0;
        }
        else
        {
            printf("[FAST] Check #Corner...[" ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET "]\n");
            printf("[FAST] #Corners: %d (Checksum 0x%x)\n", (int)corners_found, (int)cnt);
            ret = 1;
        }
    }
    return ret;
}

int main()
{
    uint32_t iter;
    int32_t  ret = -1; //1 == fail

    for(iter = 0; iter < ITERS; ++iter)
    {
        profile_start(iter);

        uint32_t band_size_in;
        uint32_t local_corners;

        /* L1 Buffers for inputs */
        IMG_DATATYPE *inBuf[2];
        uint32_t     *outBuf[2];
        int32_t      *offsets;

        /* Job ID for DMA*/
        dma_req_t job_id_read[2] = {{0},{0}};
        dma_req_t job_id_write[2] = {{0},{0}};
        uint8_t  wait_writeback[2] = {0,0};

        uint32_t inputWidth = WIDTH;
        uint32_t inputHeight= HEIGHT;
        uint32_t netHeight  = inputHeight - 2*BORDER_FAST9; /* Height - 2 x 3rows */

        int32_t Y          = 0U;

        uint32_t bandHeight = BAND_HEIGHT;
        uint32_t nbands     = netHeight / bandHeight;
        int32_t  odd        = ((int32_t) netHeight - (int32_t)(bandHeight * nbands));
        nbands += odd > 0 ? 1 : 0;

#ifdef VERBOSE
        printf("[FAST] ------------------\n");
        printf("[FAST] Configuration\n\n");
        printf("[FAST] ------------------\n");
        printf("[FAST] Image WIDTH: %d\n", (int) inputWidth);
        printf("[FAST] Image HEIGHT: %d\n", (int)inputHeight);
        printf("[FAST] Image size [byte]: %d\n", (int) (inputWidth*inputHeight));
        printf("[FAST] Image #STRIPES: %d\n", (int) nbands);
        printf("[FAST] ------------------\n");
#endif

        /* Reset output */
        corners           = 0U;

        /* Create IDx offsets for circular detection */
        offsets = (int32_t *) l1malloc(16U*sizeof(int32_t));
        make_offsets(offsets, inputWidth);

        /* Input Buffer - Image Intensity */
        inBuf[0] = (IMG_DATATYPE *) l1malloc(inputWidth*(bandHeight+2*BORDER_FAST9)*sizeof(IMG_DATATYPE));
        inBuf[1] = (IMG_DATATYPE *) l1malloc(inputWidth*(bandHeight+2*BORDER_FAST9)*sizeof(IMG_DATATYPE));

        /* Output Buffers - Corneres Coordinate */
        outBuf[0] = (uint32_t *) l1malloc((3*inputWidth*bandHeight)*sizeof(uint32_t));
        outBuf[1] = (uint32_t *) l1malloc((3*inputWidth*bandHeight)*sizeof(uint32_t));

        /* Compute transfer sizes */
        band_size_in = ((bandHeight + 2*BORDER_FAST9) * inputWidth)*sizeof(IMG_DATATYPE);

#ifdef VERBOSE
        printf("[FAST] offsets   = %x\n", (unsigned int)offsets);
        printf("[FAST] inBuf[0]  = %x\n", (unsigned int)inBuf[0]);
        printf("[FAST] inBuf[1]  = %x\n", (unsigned int)inBuf[1]);
        printf("[FAST] outBuf[0] = %x\n", (unsigned int)outBuf[0]);
        printf("[FAST] outBuf[1] = %x\n", (unsigned int)outBuf[1]);
#endif

        /* First DMA buffer in-load */
        memcpy_async(inBuf[0], &inputImage[0], band_size_in, &job_id_read[0]);

        #pragma omp parallel shared(local_corners, bandHeight, Y) num_threads(NUM_THREADS)
        {
            uint32_t bufID      = 0U;
            uint32_t bandID     = 0U;
            uint32_t nextBandID = 1U;
            int32_t  nextY      = BORDER_FAST9;


            for(bandID = 0U; bandID < nbands; bandID++)
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

                    /* Reset local corners counter */
                    local_corners = 0;

                    /* DMA Programming for next transfer */
                    nextBandID++;
                    if((nbands > nextBandID) || ((nbands == nextBandID) && !odd))
                    {
                        nextY+=bandHeight;
                        memcpy_async(inBuf[bufID],
                                     &inputImage[(nextY-BORDER_FAST9) * inputWidth],
                                     band_size_in,
                                     &job_id_read[bufID]); /*  normal transfer */
                    }
                    else if((nbands == nextBandID) && odd)
                    {
                        nextY+=bandHeight;
                        memcpy_async(inBuf[bufID],
                                     &inputImage[(nextY-BORDER_FAST9) * inputWidth],
                                     (odd + 2*BORDER_FAST9)*inputWidth*sizeof(IMG_DATATYPE),
                                     &job_id_read[bufID]); /* odd transfer */
                    }
                    else if(odd)
                        bandHeight   = odd;

                    //DMA wait for input load
                    memcpy_wait(&job_id_read[!bufID]);

                    //DMA wait for output offload
                    if(wait_writeback[!bufID])
                    {
                        memcpy_wait(&job_id_write[!bufID]);
                        wait_writeback[!bufID] = 0U;
                    }
                }
                #pragma omp barrier

#ifdef VERBOSE
                #pragma omp master
                printf("[FAST] Computing band %d [%d-%d] of %d (%d)...",
                       (int)bandID,
                       (int)(Y+BORDER_FAST9),
                       (int)(Y+bandHeight+BORDER_FAST9),
                       (int)nbands,
                       (int)checksum_vector(inBuf[!bufID], (int)((bandHeight + 2*BORDER_FAST9) * inputWidth), (int)sizeof(IMG_DATATYPE)));
#endif

                FAST_kernel(&local_corners, outBuf[!bufID], inBuf[!bufID], inputWidth, bandHeight, offsets, Y);

#ifdef VERBOSE
                #pragma omp master
                printf("%d corners found.\n", (int)local_corners);
#endif

                #pragma omp master
                //DMA writeback of results
                if(local_corners)
                {
                    memcpy_async(&corners_map[corners*3],
                                 outBuf[!bufID],
                                 3*local_corners*sizeof(uint32_t),
                                 &job_id_write[!bufID]);
                    wait_writeback[!bufID] = 1U;
                    corners += local_corners; //direct writeback
                }
            } //N_BANDS
        }
        if(wait_writeback[0])
            memcpy_wait(&job_id_write[0]); //Wait for last job!

        if(wait_writeback[1])
            memcpy_wait(&job_id_write[1]);  //Wait for last job!

        l1free(offsets);
        l1free(inBuf[0]);
        l1free(inBuf[1]);
        l1free(outBuf[0]);
        l1free(outBuf[1]);

        profile_stop(iter);

#ifdef CHECK
        ret = checkResults(corners, corners_map);
        if(ret) break;
#endif

    }//iter
    profile_show();

    return ret;
}
