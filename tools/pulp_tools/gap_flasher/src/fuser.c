#include <stdio.h>
#include "rt/rt_api.h"
#include "pulp.h"

#define  EFUSE_SIZE 128

#define gap_to_bridge_LOADED  1
#define gap_to_bridge_FINISHED  1
#define gap_to_bridge_BUSY  0

#define bridge_to_gap_EXIT 2
#define bridge_to_gap_START 1
#define bridge_to_gap_WAIT 0

#define CMD_READ 0
#define CMD_WRITE 1

#define STATUS_OK 0
#define STATUS_ERROR 1

typedef struct
{
	int fuser_ver_maj ;
	int fuser_ver_min ;
	int bridge_to_gap ;
	int gap_to_bridge ;
	int command ;
	int bit_pos ;
	int bit_len ;
	int status ;
	int log_level ;
	char buffer[EFUSE_SIZE] ;
} t_synchro ;

volatile RT_L2_DATA t_synchro fuseHeader={1,0,0,0,0,0,0,0,1,{0}} ;

#define LOG(__LVL, __FMT...) if (fuseHeader.log_level >= (__LVL)) printf(__FMT)
#define bit_set(a, b) ( a =  ((unsigned char)(a)) | 1<<(7-b))
#define bit_get(a, b) ( (a)>>(7-b) & 0x1)

// #define __SIMULATE_WRITE

void efuse_burn_bit(unsigned int addr, unsigned char pos)
{
#ifdef __SIMULATE_WRITE
	printf("[Fuser] SIMULATE: Set reg %d bit %d\n", addr, pos);
#else
	// Write must be enabled before calling this function
    *(volatile int*) (ARCHI_EFUSE_ADDR+0x200+addr*4) = pos ;
#endif
}

void efuse_start_program()
{
#ifndef __SIMULATE_WRITE
	plp_efuse_configTimings(250<<20 | 50<<10 | 5) ;
	plp_efuse_startProgram() ; 
#endif
}

void efuse_end_program()
{
#ifndef __SIMULATE_WRITE
	plp_efuse_sleep();
#endif
}

void calculate_bit_offsets(int *start_byte, int *bit_shift, int *last_bit, int *end_byte, int *num_bytes, int *last_mask)
{
	*start_byte = fuseHeader.bit_pos/8;
	*bit_shift = fuseHeader.bit_pos%8;
	*last_bit = fuseHeader.bit_pos + fuseHeader.bit_len - 1;
	*end_byte = *last_bit/8;
	*num_bytes = fuseHeader.bit_len/8;
	int end_bits = fuseHeader.bit_len%8;
	if (end_bits == 0)
		*last_mask = 0xff;
	else{
		*last_mask = (0xff << (8-end_bits))&0xff;
		(*num_bytes)++;
	}
	LOG(4, "[Fuser]: start byte %d bit shift %d last_bit %d end_byte %d num_bytes %d last_mask %x end_bits %d\n",
		*start_byte, *bit_shift, *last_bit, *end_byte, *num_bytes, (unsigned int)*last_mask, end_bits);
}

void read_fuses(volatile char * buffer, int start_byte, int bit_shift, int last_bit, int end_byte, int num_bytes, int last_mask)
{
	LOG (1, "[Fuser]: Start fuse read\n");

	for (int i=0; i <= num_bytes; i++) buffer[i] = 0;
	plp_efuse_configTimings(250<<20 | 50<<10 | 5) ;
	plp_efuse_startRead() ;
	int buf_byte = 0;
	for (int i=start_byte; i <= end_byte; i++)
	{
		char b = plp_efuse_readByte(i);
		LOG(4, "[Fuser]: Read fuse byte %d value 0x%02x\n", i, b);
		buffer[buf_byte] |= (b<<bit_shift);
		if (buf_byte != 0)
		{
			buffer[buf_byte-1] |= (b>>(7-bit_shift));
		}
		buf_byte++;
	}
	plp_efuse_sleep();
	buffer[num_bytes-1] &= last_mask;
}

int validate_write(volatile char * desired, volatile char * current, int num_bytes)
{
	for (int i=0; i<num_bytes; i++) {
		if (~desired[i] & current[i]) {
		    LOG (0, "[Fuser]: Error Fuse already blown\n");
			return 0;
		}
	}
	return 1;
}

#define IS_SET_BIGE(__char, __bit) (((__char)>>(7-(__bit)))&0x1)

void write_fuses(volatile char * desired, volatile char * current, int start_byte, int bit_shift, int bit_len)
{
	LOG (1, "[Fuser]: Start fuse write\n");

	efuse_start_program();

	int buf_byte = 0, buf_bit = 0, fuse_byte = start_byte, fuse_bige_bit = bit_shift;

	for (int i = 0; i < bit_len; i++) {
		if (IS_SET_BIGE(desired[buf_byte], buf_bit) && !IS_SET_BIGE(current[buf_byte], buf_bit)) {
			efuse_burn_bit(fuse_byte, 7-fuse_bige_bit);
		}
		if (++buf_bit > 7) {
			buf_byte++; buf_bit = 0;
		}
		if (++fuse_bige_bit > 7) {
			fuse_byte++; fuse_bige_bit = 0;
		}
	}

	efuse_end_program();
}

int verify_same(volatile char * b1, volatile char * b2, int num_bytes, int last_mask)
{
	for (int i=0; i<num_bytes - 1; i++) {
		if (b1[i] != b2[i]) {
			return 0;
		}
	}
	if (num_bytes > 0 && (b1[num_bytes - 1]&last_mask) != (b2[num_bytes-1]&last_mask)) {
		return 0;
	}
	return 1;
}

int main()
{
    LOG(1, "[Fuser]: Fuse Version %d.%d start\n", fuseHeader.fuser_ver_maj, fuseHeader.fuser_ver_min);

	char read_buf[EFUSE_SIZE];

	// Notify python that efuser has loaded
	fuseHeader.gap_to_bridge=gap_to_bridge_LOADED ;

	while (1) {

	    LOG (1, "[Fuser]: Wait for fuse operation\n");
		// wait the autorisation from python
		while (fuseHeader.bridge_to_gap==bridge_to_gap_WAIT) { }
        if (fuseHeader.bridge_to_gap==bridge_to_gap_EXIT) break;
		fuseHeader.bridge_to_gap = bridge_to_gap_WAIT;

	    LOG (1, "[Fuser]: Start fuse operation\n");
	
		if (fuseHeader.bit_pos+fuseHeader.bit_len>EFUSE_SIZE*8)
		{
		    LOG (0, "[Fuser]: Fuse data length error\n");
			fuseHeader.status = STATUS_ERROR;
			fuseHeader.gap_to_bridge=gap_to_bridge_FINISHED ;
			continue;
		}

		int start_byte, bit_shift, last_bit, end_byte, num_bytes, last_mask;
		calculate_bit_offsets(&start_byte, &bit_shift, &last_bit, &end_byte, &num_bytes, &last_mask);

		switch (fuseHeader.command) {
			case CMD_READ:
				read_fuses(&fuseHeader.buffer[0], start_byte, bit_shift, last_bit, end_byte, num_bytes, last_mask);
				fuseHeader.status = STATUS_OK;
				break;
			case CMD_WRITE:
				read_fuses(&read_buf[0], start_byte, bit_shift, last_bit, end_byte, num_bytes, last_mask);
				if (!validate_write(&fuseHeader.buffer[0], &read_buf[0], num_bytes))
				{
					fuseHeader.status = STATUS_ERROR;
					break;
				}
				write_fuses(&fuseHeader.buffer[0], &read_buf[0], start_byte, bit_shift, fuseHeader.bit_len);
				LOG (1, "[Fuser]: Verify fuse write\n");
				read_fuses(&read_buf[0], start_byte, bit_shift, last_bit, end_byte, num_bytes, last_mask);
				if (!verify_same(&read_buf[0], &fuseHeader.buffer[0], num_bytes, last_mask))
				{
#ifdef __SIMULATE_WRITE
					LOG(0, "[Fuser]: Fuse contents verification failed - but we are simulating\n");
#else
					LOG(0, "[Fuser]: Fuse contents verification failed\n");
					fuseHeader.status = STATUS_ERROR;
					break;
#endif
				}
				fuseHeader.status = STATUS_OK;
				break;
			default:
				LOG(0, "[Fuser]: Invalid command\n");
				fuseHeader.status = STATUS_ERROR;
				break;
		}

        LOG (1, "[Fuser]: Indicate fuse operation complete\n");
		// Notify python that efuser has finished the command
		fuseHeader.gap_to_bridge=gap_to_bridge_FINISHED ;
	}
	return 0;
}
