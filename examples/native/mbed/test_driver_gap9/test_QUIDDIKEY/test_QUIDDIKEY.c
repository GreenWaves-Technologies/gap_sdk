// MBED OS
#include <stdlib.h>

#include "cmsis_os2.h"
#include "gap_common.h"

int test_enroll_operation(uint32_t* active_code)
{
    int ret = QUIDDIKEY_Enroll(QUIDDIKEY, active_code);

#if DEBUG == 1
    printf("Eroll : %x\n", active_code[0]);
#endif

    return ret;
}

int test_start_operation(uint32_t* active_code)
{
    int ret = QUIDDIKEY_Start(QUIDDIKEY, active_code);

#if DEBUG == 1
    printf("Start : %x\n", active_code[0]);
#endif

    return ret;
}

int test_stop_operation()
{
    int ret = QUIDDIKEY_Stop(QUIDDIKEY);

#if DEBUG == 1
    printf("Stop :\n");
#endif

    return ret;
}

int test_generate_random_operation(uint16_t data_length)
{
    /* Allocate space for random data maximum 4096 bits = 512 bytes */
    char *random_data = malloc(data_length >> 3);

    quiddikey_context_t context = {data_length, 0, 0, 0};

    int ret = QUIDDIKEY_GenerateRandom(QUIDDIKEY, (uint32_t *)random_data, &context);

#if DEBUG == 1
    printf("Generate Random : %x\n", random_data[0]);
#endif

    /* Release allocated space */
    free(random_data);

    return ret;
}

int test_get_key_operation(uint16_t key_length, quiddikey_key_scope_type_t scope)
{
    /* Allocate space for key maximum 4096 bits = 512 bytes */
    char *key = malloc(key_length >> 3);

    quiddikey_context_t context = {0, key_length, 0, 0};

    /* In which scope (FSM) we do key operation */
    if (scope == uQUIDDIKEY_Enrolled)
        context.key_scope_enrolled = (1 << uQUIDDIKEY_DOR);
    else if(scope == uQUIDDIKEY_Started)
        context.key_scope_started  = (1 << uQUIDDIKEY_DOR);
    else
        return -1;

    int ret = QUIDDIKEY_GetKey(QUIDDIKEY, (uint32_t *)key, &context);

#if DEBUG == 1
    printf("Get Key (FSM = %d) : %x\n", scope, key[0]);
#endif

    /* Release allocated space */
    free(key);

    return ret;
}

int test_wrap_generated_random_operation(uint16_t key_length, quiddikey_key_scope_type_t scope)
{
    /* Allocate space for key maximum 4096 bits = 512 bytes */
    char *key_code = malloc(GAP_QUIDDIKEY_KEY_CODE_LENGTH_BYTE(key_length));

    quiddikey_context_t context = {0, key_length, 0, 0};

    /* In which scope (FSM) we do key operation */
    if (scope == uQUIDDIKEY_Enrolled)
        context.key_scope_enrolled = (1 << uQUIDDIKEY_DOR);
    else if(scope == uQUIDDIKEY_Started)
        context.key_scope_started  = (1 << uQUIDDIKEY_DOR);
    else
        return -1;

    int ret = QUIDDIKEY_WrapGeneratedRandom(QUIDDIKEY, (uint32_t *)key_code, &context);

#if DEBUG == 1
    printf("Wrap Generated Random Key Code (FSM = %d) : %x\n", scope, key_code[0]);
#endif

    /* Release allocated space */
    free(key_code);

    return ret;
}

int test_wrap_unwrap_operation(uint16_t key_length, quiddikey_key_scope_type_t scope)
{
    int ret = 0;

    int key_bytes = (key_length >> 3);

    /* Allocate space for key maximum 4096 bits = 512 bytes */
    char *key      = malloc(key_bytes);
    /* Allocate space for key code, use key code length table */
    char *key_code = malloc(GAP_QUIDDIKEY_KEY_CODE_LENGTH_BYTE(key_length));

    /* Initialize key just for test */
    for (int i = 0; i < key_bytes; i++) {
        key[i] = i;
    }

    quiddikey_context_t context = {0, key_length, 0, 0};

    /* In which scope (FSM) we do key operation */
    if (scope == uQUIDDIKEY_Enrolled)
        context.key_scope_enrolled = (1 << uQUIDDIKEY_DOR);
    else if(scope == uQUIDDIKEY_Started)
        context.key_scope_started  = (1 << uQUIDDIKEY_DOR);
    else
        return -1;

    ret = QUIDDIKEY_Wrap(QUIDDIKEY, (uint32_t *)key, (uint32_t *)key_code, &context);

#if DEBUG == 1
    printf("Wrap Key (FSM = %d) : %x\n", scope, key_code[0]);
#endif


    /* Initialize key 0 for check */
    for (int i = 0; i < key_bytes; i++) {
        key[i] = 0;
    }
    ret = QUIDDIKEY_UnWrap(QUIDDIKEY, (uint32_t *)key_code, (uint32_t *)key, &context);

#if DEBUG == 1
    printf("UnWrap Key Code (FSM = %d) : %x\n", scope, key[0]);
#endif

    for (int i = 0; i < key_bytes; i++) {
        if(key[i] != (char) i)
            ret = -1;
    }

    /* Release allocated space */
    free(key_code);
    /* Release allocated space */
    free(key);

    return ret;
}

int test_zeroize_operation()
{
    int ret = QUIDDIKEY_Zeroize(QUIDDIKEY);

#if DEBUG == 1
    printf("Zeroize :\n");
#endif

    return ret;
}

int main()
{
    printf("FC mbed_os QUIDDIKEY driver test\n");

    /* Check read values */
    int error = 0;

    quiddikey_config_t config;

    QUIDDIKEY_Init(QUIDDIKEY, &config);

    /* Allocate space for AC (996bytes) */
    char *active_code = malloc(GAP_QUIDDIKEY_AC_BYTES);

    /* ENROLL */
    error = test_enroll_operation((uint32_t*) active_code);
    if (error) {
        printf("Test failed %d errors\n", error);
        return error;
    }

    /* Wrap <-> UnWrap */
    for  (int i = 64; i < 1024; i += 64) {
        error = test_wrap_unwrap_operation(i, uQUIDDIKEY_Enrolled);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }
    }
    for  (int i = 1024; i <= 4096; i += 1024) {
        error = test_wrap_unwrap_operation(i, uQUIDDIKEY_Enrolled);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }
    }

    /* Wrap Generated Random */
    for  (int i = 64; i < 1024; i += 64) {
        error = test_wrap_generated_random_operation(i, uQUIDDIKEY_Enrolled);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }
    }
    for  (int i = 1024; i <= 4096; i += 1024) {
        error = test_wrap_generated_random_operation(i, uQUIDDIKEY_Enrolled);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }
    }

    /* Get Key */
    for  (int i = 64; i < 1024; i += 64) {
        error = test_get_key_operation(i, uQUIDDIKEY_Enrolled);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }
    }
    for  (int i = 1024; i <= 4096; i += 1024) {
        error = test_get_key_operation(i, uQUIDDIKEY_Enrolled);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }
    }

    /* FSM twice */
    for (int i = 0; i < 2; i++) {

        /* STOP */
        error = test_stop_operation();
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }

        /* GENERATE RANDOM */
        error = test_generate_random_operation(64);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }

        /* START */
        error = test_start_operation((uint32_t*) active_code);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }
    }

    /* Release allocated space */
    free(active_code);

    /* GENERATE RANDOM */
    for  (int i = 64; i < 1024; i += 64) {
        error = test_generate_random_operation(i);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }
    }
    for  (int i = 1024; i <= 4096; i += 1024) {
        error = test_generate_random_operation(i);
        if (error) {
            printf("Test failed %d errors\n", error);
            return error;
        }
    }

    /* /\* Zeroized *\/ */
    /* error = test_zeroize_operation(); */
    /* if (error) { */
    /*     printf("Test failed %d errors\n", error); */
    /*     return error; */
    /* } */

    if (error) printf("Test failed %d errors\n", error);
    else printf("Test success\n");

    return error;
}
