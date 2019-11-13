/**
 * Defines/types and small low level driver for udma spim main functionnalities
 */

#ifndef __UDMA_SPIM_H__
#define __UDMA_SPIM_H__

#include "targets/pmsis_targets.h"

#define SPIM(id) ((spi_t *) UDMA_SPIM(id))
#include "pmsis_hal/udma/udma_spim/udma_spim_gap8.h"

#endif
