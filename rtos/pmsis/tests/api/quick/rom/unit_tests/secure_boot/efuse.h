#include <stdint.h>
#include <pmsis.h>

#include "archi/efuse/efuse.h"

#define  EFUSE_SIZE 128

/** EFUSE_REGS - Registers Layout Typedef */
//typedef struct {
//  volatile uint32_t regs[128];            /**< EFUSE_Registers, offset: 0x000 */
//} efuse_regs_t;


//#define efuse_ctrl ((efuse_t*)ARCHI_EFUSE_ADDR)
//#define efuse_regs ((efuse_regs_t*)(ARCHI_EFUSE_ADDR+0x200))


/*!
 * @name Efuse read and program control.
 * @{
 */

/*
 *sets the values for the 3 diffrent timing intervals that are used by the IP
 * bits[ 9: 0] sets the short interval  default   5 cycles
 * bits[19:10] sets the medium interval default  50 cycles
 * bits[29:20] sets the long interval   default 400 cycles
 */
static inline void efuse_config_timings(uint32_t timervalues)
{
    efuse_ctrl->cfg = timervalues;
}

static inline void efuse_start_read()
{
    efuse_ctrl->cmd = EFUSE_CMD_READ(1);
}

static inline void efuse_start_program()
{
    efuse_ctrl->cmd = EFUSE_CMD_WRITE(1);
}

static inline void efuse_sleep()
{
    efuse_ctrl->cmd = EFUSE_CMD_IDLE(1);
}

/* @} */

/*!
 * @name Efuse register set and get.
 * @{
 */
static inline void efuse_program(uint32_t index, uint32_t value)
{
    for (int i = 0; i < 32; i++)
    {
        if(value & 0x1)
            efuse_regs->regs[index] = i;
        value >>= 1;
    }
}

static inline uint32_t efuse_read(uint32_t index)
{
    return efuse_regs->regs[index];
}

static inline void efuse_prot_page(uint32_t page_id)
{
    efuse_ctrl->page_protect = 1 << page_id;
}

static inline void efuse_unprot_page(uint32_t page_id)
{
    efuse_ctrl->page_protect = ~(1 << page_id);
}

static inline void efuse_prot_page_lock(void)
{
    efuse_ctrl->page_protect = (1 << 31);
}
/* @} */
