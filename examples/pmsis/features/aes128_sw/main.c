#include "pmsis.h"
#include "AesLib.h"

#define TEST_BUFF_SIZE      (40600) 
#define TEST_KEY_HI         (0x1122334455667788)
#define TEST_KEY_LO         (0x9900AABBCCDDEEFF)
#define TEST_IV             (0x1122334455667788)

#if defined (__PULP_OS__)
RT_FC_DATA aes_data_t aes_data;
#else
GAP_FC_DATA aes_data_t aes_data;
#endif

static void load_key(unsigned char * key, unsigned char * iv)
{
    for(int i=0; i<8; i++)
    {
        key[i] = (TEST_KEY_LO>>(i*8)) & 0xFF;
        key[(i+8)] = (TEST_KEY_HI>>(i*8)) & 0xFF;
        iv[i] = (TEST_IV>>(i*8)) & 0xFF;
    }
    for (int i=0; i<16; i++)
    {
        printf("%x", key[i]);
    }
    printf("\n");
    for (int i=0; i<8; i++)
    {
        printf("%x", iv[i]);
    }
    printf("\n");
}

static void buff_init (unsigned char * buff, unsigned int size)
{
    for (unsigned int i=0; i<size; i++)
        buff[i] = i;
}

void aes128()

{
    pi_freq_set(PI_FREQ_DOMAIN_FC, 250000000);
	unsigned char IV[2][MAXBC];
	unsigned char Key[4][MAXBC];

    load_key((unsigned char *)Key, (unsigned char *)IV);

	int bufferSize = TEST_BUFF_SIZE;
	unsigned char *buffer = pmsis_l2_malloc(bufferSize);
	unsigned char *buffer_enc = pmsis_l2_malloc(bufferSize);
	unsigned char *buffer_dec = pmsis_l2_malloc(bufferSize);
    unsigned int  cycles[4];
    buff_init(buffer, bufferSize);
    pi_perf_conf(1 << PI_PERF_CYCLES | 1 << PI_PERF_ACTIVE_CYCLES);
    pi_perf_reset();
    pi_perf_start();

    cycles[0] = pi_perf_read(PI_PERF_CYCLES);
    
	AesBuildLUT(&aes_data);

    pi_perf_stop();
    cycles[1] = pi_perf_read(PI_PERF_CYCLES);
    pi_perf_start();

	AesCtrCipher(&aes_data, buffer, buffer_enc, Key, IV, bufferSize, 1);

    pi_perf_stop();
    cycles[2] = pi_perf_read(PI_PERF_CYCLES);
    pi_perf_start();

	AesCtrCipher(&aes_data, buffer_enc, buffer_dec, Key, IV, bufferSize, 0);

    pi_perf_stop();
    cycles[3] = pi_perf_read(PI_PERF_CYCLES);

    for(int i=0 ; i<bufferSize; i++)
    {   
        if (buffer[i] != buffer_dec[i] )
            printf("ERROR of enc/dec on %d\n", i);
    }

    printf("check done \n");

    printf("Build LUT takes %d cycles\n", (cycles[1]-cycles[0]));
    printf("Encryption takes %d cycles\n", (cycles[2]-cycles[1]));
    printf("Decryption takes %d cycles\n", (cycles[3]-cycles[2]));

    pmsis_l2_malloc_free(buffer, bufferSize);
    pmsis_l2_malloc_free(buffer_enc, bufferSize);
    pmsis_l2_malloc_free(buffer_dec, bufferSize);

	pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS TEST AES ***\n\n");
    return pmsis_kickoff((void *) aes128);
}

