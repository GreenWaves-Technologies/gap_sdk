#include <stdint.h>
#include "omp.h"
#include "common.h"

//NOTE Make your configuration here
#define NUM_THREADS  8U
#define ITERS        3U
#define CHECK
// #define VERBOSE

#include "data/input.h"

static inline int32_t
checkResults(int32_t *output, uint32_t nbNodes)
{
    uint32_t checksum = 0;
    int32_t  ret = 1U; //1 == fail
    uint32_t i = 0, j = 0;

    #ifdef VERBOSE
    printf("[bfs] Checksumming...\n");
    #endif

    for(i = 0 ; i < nbNodes ; i++)
    {
        #ifdef OUTPUT
        printf("%x \n", output[i]);
        #endif

        checksum += output[i];
    }

    #ifdef VERBOSE
    printf("[bfs] Checksum 0x%x\n", (int)checksum);
    #endif

    if(CHECKSUM)
    {
        if(CHECKSUM == checksum)
        {
            #ifdef VERBOSE
            printf("[bfs] Check...[" ANSI_COLOR_GREEN "SUCCESS" ANSI_COLOR_RESET "]\n");
            #endif
            ret = 0;
        }
        else
        {
            printf("[bfs] Check...[" ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET "]\n");
            printf("[bfs] Checksum 0x%x\n", (int) checksum);
            ret = 1;
        }
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
//Apply BFS on a Graph using CUDA
////////////////////////////////////////////////////////////////////////////////
int main() {
    uint32_t iter;
    int32_t  ret = -1; //1 == fail

    // allocate host memory
    uint8_t *h_graph_mask = (uint8_t*) l1malloc(sizeof(uint8_t)*no_of_nodes);
    uint8_t *h_updating_graph_mask = (uint8_t*) l1malloc(sizeof(uint8_t)*no_of_nodes);
    uint8_t *h_graph_visited = (uint8_t*) l1malloc(sizeof(uint8_t)*no_of_nodes);

    // allocate mem for the result on host side
    int32_t* h_cost = (int32_t*)l1malloc( sizeof(int32_t)*no_of_nodes);

    int start, edgeno, i;

    for(iter = 0; iter < ITERS; ++iter)
    {
        // initalize the memory
        for(i = 0; i < no_of_nodes; i++)
        {
            h_graph_mask[i]=false;
            h_updating_graph_mask[i]=false;
            h_graph_visited[i]=false;
        }

        //set the source node as true in the mask
        h_graph_mask[source]=true;
        h_graph_visited[source]=true;

        for(i=0;i<no_of_nodes;i++)
            h_cost[i]=-1;
        h_cost[source]=0;

        #ifdef VERBOSE
        printf("[bfs] start traversing the tree size=%d...\n", DIM);
        #endif

        profile_start(iter);

        int k=0;
        uint8_t stop;
        do
        {
            int tid;
            //if no thread changes this value then the loop stops
            stop=false;

            #pragma omp parallel for private(tid) private(i)
            for(tid = 0; tid < no_of_nodes; tid++ )
            {
                if (h_graph_mask[tid] == true){
                    h_graph_mask[tid]=false;
                    for(i=h_graph_nodes[tid].starting; i<(h_graph_nodes[tid].no_of_edges + h_graph_nodes[tid].starting); i++)
                    {
                        int id = h_graph_edges[i];
                        if(!h_graph_visited[id])
                        {
                            h_cost[id]=h_cost[tid]+1;
                            h_updating_graph_mask[id]=true;
                        }
                    }
                }
            }

            for(tid=0; tid< no_of_nodes ; tid++ )
            {
                if (h_updating_graph_mask[tid] == true){
                    h_graph_mask[tid]=true;
                    h_graph_visited[tid]=true;
                    stop=true;
                    h_updating_graph_mask[tid]=false;
                }
            }
            k++;
        }
        while(stop);

        #ifdef VERBOSE
        printf("[bfs] start traversing done!\n");
        #endif

        profile_stop(iter);

#ifdef CHECK
        ret = checkResults(h_cost, no_of_nodes);
        if(ret) break;
#endif
    }
    profile_show();

    // cleanup memory
    l1free( h_graph_mask);
    l1free( h_updating_graph_mask);
    l1free( h_graph_visited);
    l1free( h_cost);

    return ret;
}
