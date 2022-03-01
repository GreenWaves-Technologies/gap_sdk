// This example shows how to program efuses.  //
// This is a permanent operation. Once and efuse is programmed, the same value is
// read, even after power-up. This can be used to remember some settings.
// Be careful that there are some efuses reserved for the ROM to specify the
// boot mode, check the specifications to know which efuses can be used.

#include <pmsis.h>
#ifdef __GAP9__
#include "gap9_fuser_map.h"
#include <hal/efuse/efuse_v1.h>
#else
#include "fuser_map.h"
#endif

#define FUSER_REG_NUM       (128)

#ifdef DUMP_REG
#ifdef __GAP9__
unsigned int fuser_read[FUSER_REG_NUM];
#else
unsigned char fuser_read[FUSER_REG_NUM];
#endif
#endif

pi_fuser_reg_t *fuser_map_check;

static int entry()
{
    int err = 0;
    printf("Fuser Start\n");

    // Before writing the efuse, we must activate the program operation
    // Once activated, we can wrote as many efuses as we want
    plp_efuse_startProgram();

    for (int i=0; i<(sizeof(fuser_map)/sizeof(pi_fuser_reg_t)); i++)
    {
#ifdef __GAP9__
        efuse_program(fuser_map[i].id, fuser_map[i].val);
#else
        plp_efuse_writeByte(fuser_map[i].id, fuser_map[i].val);
#endif
    }

    // Close the current operation once done
    plp_efuse_sleep();

    fuser_map_check = (pi_fuser_reg_t *) pmsis_l2_malloc (sizeof(fuser_map));

    // Before reading the efuse, we must activate the read operation
    // Once activated, we can wrote as many efuses as we want
    plp_efuse_startRead();

#ifdef DUMP_REG
    for (int i=0; i<(FUSER_REG_NUM); i++)
    {
        fuser_read[i] = plp_efuse_readWord(i);
    }
#endif

    for (int i=0; i<(sizeof(fuser_map)/sizeof(pi_fuser_reg_t)); i++)
    {
        fuser_map_check[i].id = fuser_map[i].id;
        fuser_map_check[i].val = plp_efuse_readWord(fuser_map_check[i].id);
    }

    // Close the current operation once done
    plp_efuse_sleep();

    for (int i=0; i<(sizeof(fuser_map)/sizeof(pi_fuser_reg_t)); i++)
    {
        if (fuser_map_check[i].val != fuser_map[i].val)
        {
            printf("Read efuse %d: 0x%x, should be 0x%x\n", fuser_map_check[i].id, fuser_map_check[i].val, fuser_map[i].val);
            err ++;
        }
    }

#ifdef DUMP_REG
    for (int i=0; i<(FUSER_REG_NUM); i++)
    {
#ifndef DUMP_ALL_REG
        if(fuser_read[i])
#endif
            printf("Read efuse %d: 0x%x\n", i, fuser_read[i]);
    }
#endif
    pmsis_l2_malloc_free (fuser_map_check, sizeof(fuser_map));
    
    if(err)
        printf("Fuse failed with %d errors\n", err);
    else
        printf("Fuse success \n");

    return err;
}


static void pmsis_wrapper(void)
{
    int retval = entry();
    pmsis_exit(retval);
}


int main(void)
{
    return pmsis_kickoff((void *)pmsis_wrapper);
}

