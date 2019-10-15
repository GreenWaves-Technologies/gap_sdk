#include "val_interfaces.h"
#include "pal_mbed_os_intf.h"

#ifdef ITS_TEST
void test_entry_s007(val_api_t *val_api, psa_api_t *psa_api);
#elif PS_TEST
void test_entry_p007(val_api_t *val_api, psa_api_t *psa_api);
#endif

int main(void)
{
#ifdef ITS_TEST
    test_start(test_entry_s007, COMPLIANCE_TEST_STORAGE);
#elif PS_TEST
    test_start(test_entry_p007, COMPLIANCE_TEST_STORAGE);
#endif
}
