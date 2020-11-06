/*
 * Copyright (C) 2019 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __POS_IMPLEM_LOCK_H__
#define __POS_IMPLEM_LOCK_H__


static inline unsigned int pos_tas_addr(unsigned int addr)
{
    return addr | (1<<ARCHI_L1_TAS_BIT);
}


static inline signed char pos_tas_lock_8(unsigned int addr)
{
    __asm__ __volatile__ ("" : : : "memory");
    signed char result = *(volatile signed char *)pos_tas_addr(addr);
    __asm__ __volatile__ ("" : : : "memory");
    return result;
}


static inline void pos_tas_unlock_8(unsigned int addr, signed char value)
{
    __asm__ __volatile__ ("" : : : "memory");
    *(volatile signed char *)addr = value;
    __asm__ __volatile__ ("" : : : "memory");
}


static inline signed short pos_tas_lock_16(unsigned int addr)
{
    __asm__ __volatile__ ("" : : : "memory");
    signed short result = *(volatile signed short *)pos_tas_addr(addr);
    __asm__ __volatile__ ("" : : : "memory");
    return result;
}


static inline void pos_tas_unlock_16(unsigned int addr, signed short value)
{
    __asm__ __volatile__ ("" : : : "memory");
    *(volatile signed short *)addr = value;
    __asm__ __volatile__ ("" : : : "memory");
}


static inline int pos_tas_lock_32(unsigned int addr)
{
    __asm__ __volatile__ ("" : : : "memory");
    signed int result = *(volatile signed int *)pos_tas_addr(addr);
    __asm__ __volatile__ ("" : : : "memory");
    return result;
}


static inline void pos_tas_unlock_32(unsigned int addr, signed int value)
{
    __asm__ __volatile__ ("" : : : "memory");
    *(volatile signed int *)addr = value;
    __asm__ __volatile__ ("" : : : "memory");
}


static inline void pos_cl_mutex_init(pos_cl_mutex_t *mutex)
{
    mutex->value = 0;
}


static inline void pos_cl_mutex_lock(pos_cl_mutex_t *mutex)
{
    while(pos_tas_lock_32((uint32_t)&mutex->value) == -1)
    {
        eu_evt_maskWaitAndClr(1<<POS_EVENT_CLUSTER_SYNC);
    }
}


static inline void pos_cl_mutex_unlock(pos_cl_mutex_t *mutex)
{
    pos_tas_unlock_32((uint32_t)&mutex->value, 0);
    eu_evt_trig(eu_evt_trig_addr(POS_EVENT_CLUSTER_SYNC), 0);
}


#endif