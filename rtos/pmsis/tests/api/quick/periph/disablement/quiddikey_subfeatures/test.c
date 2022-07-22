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

int test_generate_random_operation(pi_device_t *qk, uint16_t data_length)
{
    /* Allocate space for random data maximum 4096 bits = 512 bytes */
    char *random_data = pi_malloc(data_length >> 3);

    pi_quiddikey_context_t context = {data_length, 0, 0, 0, 0};

    int ret = pi_quiddikey_generate_random(qk, (uint32_t *)random_data,
            &context);

#if DEBUG == 1
    printf("Generate Random : %x\n", random_data[0]);
#endif

    /* Release allocated space */
    pi_free(random_data);

    return ret;
}

int test_wrap_unwrap_operation_dor(pi_device_t *qk, uint16_t key_length,
        quiddikey_key_scope_type_t scope)
{
    int ret = 0;

    int key_bytes = (key_length >> 3);

    /* Allocate space for key maximum 4096 bits = 512 bytes */
    char *key      = pi_malloc(key_bytes);
    /* Allocate space for key code, use key code length table */
    char *key_code = pi_malloc(GAP_QUIDDIKEY_KEY_CODE_LENGTH_BYTE(key_length));

    /* Initialize key just for test */
    for (int i = 0; i < key_bytes; i++) {
        key[i] = i;
    }


    pi_quiddikey_context_t context = {0, 0, 0, 0, 0};

    // dor ok, so ok, no special context
    pi_quiddikey_set_key_context(&context, scope, key_length, 1, 1, 0);

    ret = pi_quiddikey_wrap(qk, (uint32_t *)key, (uint32_t *)key_code, &context);

    printf("wrap ret=%x\n",ret);
//#if DEBUG == 1
    printf("Wrap Key (FSM = %d) : %x\n", scope, key_code[0]);
//#endif

    /* Initialize key 0 for check */
    for (int i = 0; i < key_bytes; i++) {
        key[i] = 0;
    }
    ret |= !pi_quiddikey_unwrap(qk, (uint32_t *)key_code, (uint32_t *)key, context.key_length);

//#if DEBUG == 1
    printf("UnWrap Key Code (FSM = %d) : %x\n", scope, key[0]);
//#endif

    /* Release allocated space */
    pi_free(key_code);
    /* Release allocated space */
    pi_free(key);

    return ret;
}

static int test_entry()
{
    printf("test_entry\n");

    int error = 0;
    pi_device_t qk_device = {0};
    pi_quiddikey_conf_t conf = {0};
    conf.id = 0;
    conf.enroll = 1;
    conf.ac = pi_malloc(GAP_QUIDDIKEY_AC_BYTES);
    qk_device.config = &conf;
    int ret = pi_quiddikey_open(&qk_device);

    int qk_state = uQUIDDIKEY_Enrolled;

    pi_quiddikey_close(&qk_device);
    apb_soc_ctrl_feature_disablement_disable_quiddikey_enroll_set(apb_soc_ctrl,1);
    apb_soc_ctrl_feature_disablement_disable_quiddikey_unwrap_set(apb_soc_ctrl,1);
    
    ret = pi_quiddikey_open(&qk_device);
    printf("ret=%x\n",ret);


//#ifdef QUIDDIKEY_TEST_START
    qk_state = uQUIDDIKEY_Started;
    //pi_quiddikey_close(&qk_device);
    
    conf.id = 0;
    conf.enroll = 0;
    ret = pi_quiddikey_open(&qk_device);
//#endif


    for  (int i = 1024; i <= 4096; i += 1024) {
        error = test_generate_random_operation(&qk_device,i);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }
    }

    /* Wrap <-> UnWrap */
    for  (int i = 64; i < 1024; i += 64) {
        error = test_wrap_unwrap_operation_dor(&qk_device, i, qk_state);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }
    }
    for  (int i = 1024; i <= 4096; i += 1024) {
        error = test_wrap_unwrap_operation_dor(&qk_device, i, qk_state);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }
    }

    pi_quiddikey_close(&qk_device);
    printf("Test success\n");
    return 0;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
