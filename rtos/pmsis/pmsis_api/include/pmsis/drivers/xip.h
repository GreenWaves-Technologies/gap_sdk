#pragma once

#include "pmsis/pmsis_types.h"

#define XIP_DEVICE_HYPER0 XIP_DEVICE0
#define XIP_DEVICE_HYPER1 XIP_DEVICE1
#define XIP_DEVICE_MRAM0 XIP_DEVICE2

#define PI_INLINE_XIP0 static inline

#ifndef PI_INLINE_XIP0
#define PI_INLINE_XIP0
#endif

typedef struct pi_xip_conf_s {
    uint8_t ro; // interpreted as bitfield for tlb
    uint8_t per_id; // ignored for tlb mode
    uint8_t tlb_en;
    uint16_t page_mask; // for auto mode
    xip_page_size_e page_size;
    func_t data_exception_handler;
    func_t instr_exception_handler;
    func_t cl_irq_handler;
} pi_xip_conf_t;

typedef struct pi_xip_data_err_s {
    uint32_t error_addr;
    uint32_t violation;
} pi_xip_data_err_t;

// mount a virtual area mapped to ext_addr and init corresponding device
// ro can not be less than that of device configuration
PI_INLINE_XIP0 int pi_xip_mount(pi_device_t *mount, void *virt_addr, uint32_t ext_addr,
        uint32_t mount_size, uint8_t cacheable);

// unmount an area starting at addr virt_addr and of size mount_size
// underlying device pages will be set inactive
// should only be used from l2 code
PI_INLINE_XIP0 void pi_xip_unmount(pi_device_t *mount);

// returns integer indicating how many free pages are left if can't find enough
// pages
PI_INLINE_XIP0 int pi_xip_free_page_mask_get(uint32_t *page_mask,
        uint8_t nb_pages);

// allocate page, ready to be used by tlb or mount points
// does not set active bit
PI_INLINE_XIP0 int  pi_xip_dcache_page_alloc(uint32_t page_id,
        xip_page_size_e page_size);

// Free page, realease l2 memory
PI_INLINE_XIP0 int  pi_xip_dcache_page_free(uint32_t page_id,
        xip_page_size_e page_size);

PI_INLINE_XIP0 void pi_xip_cluster_is_on_set(uint8_t cluster_is_on);

PI_INLINE_XIP0 int pi_xip_tlb_init(pi_device_t *device);

PI_INLINE_XIP0 void pi_xip_tlb_lru_update(void *virt_addr,
        uint32_t ext_addr, uint32_t per_id, uint8_t lock, uint8_t cacheable);

PI_INLINE_XIP0 void pi_xip_tlb_page_set(uint8_t page_id, void *virt_addr,
        uint32_t ext_addr, uint32_t per_id, uint8_t lock, uint8_t cacheable);
