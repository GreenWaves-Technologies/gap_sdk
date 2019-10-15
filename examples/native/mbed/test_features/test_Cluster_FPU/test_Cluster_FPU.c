#include <stdlib.h>
// PRINTF
#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "fpu_l1_data.h"

#define CORE_NUMBER      8
#define KERNEL_ITS       10

void FPU_Calculation(void *arg) {
    int coreid;

    int pass;
    int flag;
    int k;
    float y[100];
    int ix;

    float b_y;
    float xbar;
    int b_k;
    float r;
    float c_y;
    float check[2];
    float golden[4];

    coreid = __core_ID();

    printf("starting %d kernel iterations... (coreid = %d)\n",KERNEL_ITS,coreid);

    if (coreid>3)
        coreid=coreid-4;

    // Barrier
    CLUSTER_SynchBarrier();

    for(k = 0; k < KERNEL_ITS; k++)
    {
        // matlab kernel
        for (ix = 0; ix < 100; ix++) {
            y[ix] = fv2[coreid] * fv1[ix + 100 * coreid] + fv0[ix + 100 * coreid];

        }
    }

    // Barrier
    CLUSTER_SynchBarrier();

    /////////////////////////////////////////////////////////
    // check results
    /////////////////////////////////////////////////////////

    b_y = y[0];
    ix = 0;
    xbar = y[0];
    for (b_k = 0; b_k < 99; b_k++) {
        b_y += y[b_k + 1];
        ix++;
        xbar += y[ix];
    }

    xbar *= 1.0F/100.0F;
    ix = 0;
    r = y[0] - xbar;
    c_y = r * r;
    for (b_k = 0; b_k < 99; b_k++) {
        ix++;
        r = y[ix] - xbar;
        c_y += r * r;
    }

    // Barrier
    CLUSTER_SynchBarrier();

    c_y *= 1.0F/99.0F;
    check[0] = b_y;
    check[1] = c_y;
    pass = 1;
    flag = 1;

    for (ix = 0; ix < 2; ix++) {
        for (b_k = 0; b_k < 2; b_k++) {
            golden[b_k + (ix << 1)] = fv3[(b_k + (ix << 1)) + (coreid << 2)];
        }
        flag = 1;
        flag = (check[ix] <= golden[ix << 1]);
        flag = flag && (check[ix] >= golden[1 + (ix << 1)]);

        if(!flag)
        {
            printf("Error = %d, %d, %d, %d\n", ix, check[ix], golden[1 + (ix << 1)], golden[ix << 1]);
        }
        pass = pass && flag;
    }

    /////////////////////////////////////////////////////////
    // exit
    /////////////////////////////////////////////////////////

}

void Master_Entry(void *arg) {
    CLUSTER_CoresFork(FPU_Calculation, arg);
}

int main()
{
    printf("Fabric controller code execution for mbed_os Cluster Power On test\n");

    /* Cluster Start - Power on */
    CLUSTER_Start(0, CORE_NUMBER, 0);

    /* FC send a task to Cluster */
    CLUSTER_SendTask(0, Master_Entry, 0, 0);

    /* Cluster Stop - Power down */
    CLUSTER_Stop(0);

    /* Check read values */
    int error = 0;

    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

#ifdef JENKINS_TEST_FLAG
    exit(error);
#else
    return error;
#endif
}
