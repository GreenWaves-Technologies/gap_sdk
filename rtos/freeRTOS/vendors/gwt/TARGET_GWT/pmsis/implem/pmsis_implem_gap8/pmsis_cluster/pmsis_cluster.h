#ifndef __PI_DRIVER_CLUSTER_INCLUDES_H__
#define __PI_DRIVER_CLUSTER_INCLUDES_H__

/*
 * SHOULD NOT BE HERE !
 * Function is implemented in cl_to_fc_delegate.h
 */
void cl_wait_task(uint8_t *done);
void cl_notify_task_done(uint8_t *done, uint8_t cluster_id);

/* pmsis_api includes. */
#include "pmsis/cluster/cl_malloc.h"
#include "pmsis/cluster/cl_pmsis_api.h"
/* Cluster sync. */
#include "pmsis/cluster/cluster_sync/cl_synchronisation.h"
#include "pmsis/cluster/cluster_sync/fc_to_cl_delegate.h"
#include "pmsis/cluster/cluster_sync/cl_to_fc_delegate.h"
/* Cluster team. */
#include "pmsis/cluster/cluster_team/cl_team.h"
/* DMA. */
#include "pmsis/cluster/dma/cl_dma.h"

/* PMSIS cluster drivers includes. */
#include "pmsis_cluster/drivers/delegate/hyperbus/hyperbus_cl_internal.h"
#include "pmsis_cluster/cl_malloc_internal.h"
/* DMA. */
#include "pmsis_cluster/drivers/dma/cl_dma.h"

#endif  /* __PI_DRIVER_CLUSTER_INCLUDES_H__ */
