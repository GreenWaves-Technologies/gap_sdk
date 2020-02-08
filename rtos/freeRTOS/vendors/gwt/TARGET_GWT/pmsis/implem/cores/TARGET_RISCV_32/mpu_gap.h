/******************************************************************************
 * @file     mpu_gap.h
 * @brief    CMSIS MPU API for GAP MPU
 * @version  V1.0.0
 * @date     03. Feb. 2018
 ******************************************************************************/
/*
 * Copyright (c) 2017 GreenWaves Technologies SAS. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RISCV_MPU_GAP_H
#define RISCV_MPU_GAP_H

#define GAP_MPU_PAGE_SIZE_LOG2           6U
#define GAP_MPU_PAGE_SIZE                ((uint8_t)0x40U)

#define GAP_MPU_FC_TCDM_RULE             0U
#define GAP_MPU_L2_RULE                  1U
#define GAP_MPU_APB_RULE                 2U

#define GAP_MPU_NB_RULES_EACH_REGION     8U

#define GAP_MPU_FC_TCDM_AREA             0U
#define GAP_MPU_FC_PERIPH_AREA           1U
#define GAP_MPU_FC_CLUSTER_TCDM_AREA     2U
#define GAP_MPU_FC_CLUSTER_PERIPH_AREA   3U

#define GAP_MPU_L2_L2_AREA               0U

#define GAP_MPU_APB_FC_TCDM_AREA         0U
#define GAP_MPU_APB_FC_PERIPH_AREA       1U
#define GAP_MPU_APB_APB_AREA             2U



/**
* MPU Region RULE Register Value
*
* \param Area       Region to choose between FC TCDM, FC Periph or L2.
* \param Base       Base page number (Each page is 64 bytes).
* \param Size       Number of pages to open.
*/
#define GAP_MPU_RULE(Area, Base, Size)                                 \
    ((((Area                            ) << MPU_TYPE_RULE_AREA_Pos) & MPU_TYPE_RULE_AREA_Msk)     | \
     (((Base >> GAP_MPU_PAGE_SIZE_LOG2 ) << MPU_TYPE_RULE_BASE_Pos) & MPU_TYPE_RULE_BASE_Msk)     | \
     (((Size                            ) << MPU_TYPE_RULE_SIZE_Pos)   & MPU_TYPE_RULE_SIZE_Msk)   | \
     (MPU_TYPE_RULE_STATE_Msk))

/**
* Struct for a single MPU Region
*/
typedef struct _GAP_MPU_Region_t {
  uint32_t RULE; //!< The rule register value
} GAP_MPU_Region_t;

/** Enable the MPU.
* \param MPU_Enable Default access permissions for unconfigured regions.
*/
__STATIC_INLINE void GAP_MPU_Enable(uint32_t MPU_Enable)
{
  __DSB();
  __ISB();
  MPU->MPU_ENABLE = MPU_Enable;
}

/** Disable the MPU.
*/
__STATIC_INLINE void GAP_MPU_Disable(void)
{
  __DSB();
  __ISB();
  MPU->MPU_ENABLE = 0U;
}

/** Return total page number for a given region size,.
* \param Region_Size The size of a region
*/
__STATIC_INLINE uint32_t GAP_MPU_GetNumberPages(uint32_t Region_Size)
{
  return (Region_Size + GAP_MPU_PAGE_SIZE - 1) >> GAP_MPU_PAGE_SIZE_LOG2;
}

/** Clear and disable the given MPU region.
 * \param Region_Type Region Type to be cleared.
 * \param Region_Number Region number to be cleared.
 */
__STATIC_INLINE void GAP_MPU_ClrRegion(uint32_t Region_Type, uint32_t Region_Number)
{
    if(Region_Number < GAP_MPU_NB_RULES_EACH_REGION) {
        if(Region_Type == GAP_MPU_FC_TCDM_RULE)
            MPU->FC_TCDM_RULE[Region_Number] &= ~MPU_TYPE_RULE_STATE_Msk;
        else if (Region_Type == GAP_MPU_L2_RULE)
            MPU->L2_RULE[Region_Number]      &= ~MPU_TYPE_RULE_STATE_Msk;
        else if(Region_Type == GAP_MPU_APB_RULE)
            MPU->APB_RULE[Region_Number]     &= ~MPU_TYPE_RULE_STATE_Msk;
    }
}

/** Configure an MPU region.
 * \param Region_Type Region Type to be cleared.
* \param Region_Number  Rule number.
* \param Rule Value for rule register.
*/
__STATIC_INLINE void GAP_MPU_SetRegion(uint32_t Region_Type, uint32_t Region_Number, uint32_t Rule)
{
    if(Region_Number < GAP_MPU_NB_RULES_EACH_REGION) {
        if(Region_Type == GAP_MPU_FC_TCDM_RULE)
            MPU->FC_TCDM_RULE[Region_Number] = Rule;
        else if (Region_Type == GAP_MPU_L2_RULE)
            MPU->L2_RULE[Region_Number]      = Rule;
        else if(Region_Type == GAP_MPU_APB_RULE)
            MPU->APB_RULE[Region_Number]     = Rule;
    }
}

/** Memcopy with strictly ordered memory access, e.g. for register targets.
* \param dst Destination data is copied to.
* \param src Source data is copied from.
* \param len Amount of data words to be copied.
*/
__STATIC_INLINE void orderedCpy(volatile uint32_t* dst, const uint32_t* __RESTRICT src, uint32_t len)
{
  uint32_t i;
  for (i = 0U; i < len; ++i)
  {
    dst[i] = src[i];
  }
}

/** Load the given number of MPU regions from a table.
* \param table Pointer to the MPU configuration table.
* \param fc_cnt Amount of FC regions to be configured.
* \param l2_cnt Amount of L2 regions to be configured.
* \param apb_cnt Amount of APB regions to be configured.
*/
__STATIC_INLINE void GAP_MPU_Load(GAP_MPU_Region_t const* table, uint32_t fc_cnt, uint32_t l2_cnt, uint32_t apb_cnt)
{
    static const uint32_t rowWordSize = sizeof(GAP_MPU_Region_t)/4U;
    orderedCpy(MPU->FC_TCDM_RULE, &(table->RULE), fc_cnt*rowWordSize);
    orderedCpy(MPU->L2_RULE,      &((table + fc_cnt)->RULE), l2_cnt*rowWordSize);
    orderedCpy(MPU->APB_RULE,     &((table + fc_cnt + l2_cnt)->RULE), apb_cnt*rowWordSize);
}

#endif
