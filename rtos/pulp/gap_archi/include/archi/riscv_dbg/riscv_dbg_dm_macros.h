
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __ARCHI_RISCV_DBG_DM_MACROS__
#define __ARCHI_RISCV_DBG_DM_MACROS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include "archi/gap_utils.h"

#endif




//
// REGISTERS FIELDS MACROS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#define RISCV_DBG_DM_DATA0_DATA_GET(value)                 (GAP_BEXTRACTU((value),32,0))
#define RISCV_DBG_DM_DATA0_DATA_GETS(value)                (GAP_BEXTRACT((value),32,0))
#define RISCV_DBG_DM_DATA0_DATA_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
#define RISCV_DBG_DM_DATA0_DATA(val)                       ((val) << 0)

#define RISCV_DBG_DM_DATA1_DATA_GET(value)                 (GAP_BEXTRACTU((value),32,0))
#define RISCV_DBG_DM_DATA1_DATA_GETS(value)                (GAP_BEXTRACT((value),32,0))
#define RISCV_DBG_DM_DATA1_DATA_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
#define RISCV_DBG_DM_DATA1_DATA(val)                       ((val) << 0)

#define RISCV_DBG_DM_DMCONTROL_DMACTIVE_GET(value)         (GAP_BEXTRACTU((value),1,0))
#define RISCV_DBG_DM_DMCONTROL_DMACTIVE_GETS(value)        (GAP_BEXTRACT((value),1,0))
#define RISCV_DBG_DM_DMCONTROL_DMACTIVE_SET(value,field)   (GAP_BINSERT((value),(field),1,0))
#define RISCV_DBG_DM_DMCONTROL_DMACTIVE(val)               ((val) << 0)

#define RISCV_DBG_DM_DMCONTROL_NDMRESET_GET(value)         (GAP_BEXTRACTU((value),1,1))
#define RISCV_DBG_DM_DMCONTROL_NDMRESET_GETS(value)        (GAP_BEXTRACT((value),1,1))
#define RISCV_DBG_DM_DMCONTROL_NDMRESET_SET(value,field)   (GAP_BINSERT((value),(field),1,1))
#define RISCV_DBG_DM_DMCONTROL_NDMRESET(val)               ((val) << 1)

#define RISCV_DBG_DM_DMCONTROL_CLRRESETHALTREQ_GET(value)  (GAP_BEXTRACTU((value),1,2))
#define RISCV_DBG_DM_DMCONTROL_CLRRESETHALTREQ_GETS(value) (GAP_BEXTRACT((value),1,2))
#define RISCV_DBG_DM_DMCONTROL_CLRRESETHALTREQ_SET(value,field) (GAP_BINSERT((value),(field),1,2))
#define RISCV_DBG_DM_DMCONTROL_CLRRESETHALTREQ(val)        ((val) << 2)

#define RISCV_DBG_DM_DMCONTROL_SETRESETHALTREQ_GET(value)  (GAP_BEXTRACTU((value),1,3))
#define RISCV_DBG_DM_DMCONTROL_SETRESETHALTREQ_GETS(value) (GAP_BEXTRACT((value),1,3))
#define RISCV_DBG_DM_DMCONTROL_SETRESETHALTREQ_SET(value,field) (GAP_BINSERT((value),(field),1,3))
#define RISCV_DBG_DM_DMCONTROL_SETRESETHALTREQ(val)        ((val) << 3)

#define RISCV_DBG_DM_DMCONTROL_HARTSELHI_GET(value)        (GAP_BEXTRACTU((value),10,6))
#define RISCV_DBG_DM_DMCONTROL_HARTSELHI_GETS(value)       (GAP_BEXTRACT((value),10,6))
#define RISCV_DBG_DM_DMCONTROL_HARTSELHI_SET(value,field)  (GAP_BINSERT((value),(field),10,6))
#define RISCV_DBG_DM_DMCONTROL_HARTSELHI(val)              ((val) << 6)

#define RISCV_DBG_DM_DMCONTROL_HARTSELLO_GET(value)        (GAP_BEXTRACTU((value),10,16))
#define RISCV_DBG_DM_DMCONTROL_HARTSELLO_GETS(value)       (GAP_BEXTRACT((value),10,16))
#define RISCV_DBG_DM_DMCONTROL_HARTSELLO_SET(value,field)  (GAP_BINSERT((value),(field),10,16))
#define RISCV_DBG_DM_DMCONTROL_HARTSELLO(val)              ((val) << 16)

#define RISCV_DBG_DM_DMCONTROL_HASEL_GET(value)            (GAP_BEXTRACTU((value),1,26))
#define RISCV_DBG_DM_DMCONTROL_HASEL_GETS(value)           (GAP_BEXTRACT((value),1,26))
#define RISCV_DBG_DM_DMCONTROL_HASEL_SET(value,field)      (GAP_BINSERT((value),(field),1,26))
#define RISCV_DBG_DM_DMCONTROL_HASEL(val)                  ((val) << 26)

#define RISCV_DBG_DM_DMCONTROL_ACKHAVERESET_GET(value)     (GAP_BEXTRACTU((value),1,28))
#define RISCV_DBG_DM_DMCONTROL_ACKHAVERESET_GETS(value)    (GAP_BEXTRACT((value),1,28))
#define RISCV_DBG_DM_DMCONTROL_ACKHAVERESET_SET(value,field) (GAP_BINSERT((value),(field),1,28))
#define RISCV_DBG_DM_DMCONTROL_ACKHAVERESET(val)           ((val) << 28)

#define RISCV_DBG_DM_DMCONTROL_HARTRESET_GET(value)        (GAP_BEXTRACTU((value),1,29))
#define RISCV_DBG_DM_DMCONTROL_HARTRESET_GETS(value)       (GAP_BEXTRACT((value),1,29))
#define RISCV_DBG_DM_DMCONTROL_HARTRESET_SET(value,field)  (GAP_BINSERT((value),(field),1,29))
#define RISCV_DBG_DM_DMCONTROL_HARTRESET(val)              ((val) << 29)

#define RISCV_DBG_DM_DMCONTROL_RESUMEREQ_GET(value)        (GAP_BEXTRACTU((value),1,30))
#define RISCV_DBG_DM_DMCONTROL_RESUMEREQ_GETS(value)       (GAP_BEXTRACT((value),1,30))
#define RISCV_DBG_DM_DMCONTROL_RESUMEREQ_SET(value,field)  (GAP_BINSERT((value),(field),1,30))
#define RISCV_DBG_DM_DMCONTROL_RESUMEREQ(val)              ((val) << 30)

#define RISCV_DBG_DM_DMCONTROL_HALTREQ_GET(value)          (GAP_BEXTRACTU((value),1,31))
#define RISCV_DBG_DM_DMCONTROL_HALTREQ_GETS(value)         (GAP_BEXTRACT((value),1,31))
#define RISCV_DBG_DM_DMCONTROL_HALTREQ_SET(value,field)    (GAP_BINSERT((value),(field),1,31))
#define RISCV_DBG_DM_DMCONTROL_HALTREQ(val)                ((val) << 31)

#define RISCV_DBG_DM_DMSTATUS_VERSION_GET(value)           (GAP_BEXTRACTU((value),4,0))
#define RISCV_DBG_DM_DMSTATUS_VERSION_GETS(value)          (GAP_BEXTRACT((value),4,0))
#define RISCV_DBG_DM_DMSTATUS_VERSION_SET(value,field)     (GAP_BINSERT((value),(field),4,0))
#define RISCV_DBG_DM_DMSTATUS_VERSION(val)                 ((val) << 0)

#define RISCV_DBG_DM_DMSTATUS_CONFSTRPTRVALID_GET(value)   (GAP_BEXTRACTU((value),1,4))
#define RISCV_DBG_DM_DMSTATUS_CONFSTRPTRVALID_GETS(value)  (GAP_BEXTRACT((value),1,4))
#define RISCV_DBG_DM_DMSTATUS_CONFSTRPTRVALID_SET(value,field) (GAP_BINSERT((value),(field),1,4))
#define RISCV_DBG_DM_DMSTATUS_CONFSTRPTRVALID(val)         ((val) << 4)

#define RISCV_DBG_DM_DMSTATUS_HASRESETHALTREQ_GET(value)   (GAP_BEXTRACTU((value),1,5))
#define RISCV_DBG_DM_DMSTATUS_HASRESETHALTREQ_GETS(value)  (GAP_BEXTRACT((value),1,5))
#define RISCV_DBG_DM_DMSTATUS_HASRESETHALTREQ_SET(value,field) (GAP_BINSERT((value),(field),1,5))
#define RISCV_DBG_DM_DMSTATUS_HASRESETHALTREQ(val)         ((val) << 5)

#define RISCV_DBG_DM_DMSTATUS_AUTHBUSY_GET(value)          (GAP_BEXTRACTU((value),1,6))
#define RISCV_DBG_DM_DMSTATUS_AUTHBUSY_GETS(value)         (GAP_BEXTRACT((value),1,6))
#define RISCV_DBG_DM_DMSTATUS_AUTHBUSY_SET(value,field)    (GAP_BINSERT((value),(field),1,6))
#define RISCV_DBG_DM_DMSTATUS_AUTHBUSY(val)                ((val) << 6)

#define RISCV_DBG_DM_DMSTATUS_AUTHENTICATED_GET(value)     (GAP_BEXTRACTU((value),1,7))
#define RISCV_DBG_DM_DMSTATUS_AUTHENTICATED_GETS(value)    (GAP_BEXTRACT((value),1,7))
#define RISCV_DBG_DM_DMSTATUS_AUTHENTICATED_SET(value,field) (GAP_BINSERT((value),(field),1,7))
#define RISCV_DBG_DM_DMSTATUS_AUTHENTICATED(val)           ((val) << 7)

#define RISCV_DBG_DM_DMSTATUS_ANYHALTED_GET(value)         (GAP_BEXTRACTU((value),1,8))
#define RISCV_DBG_DM_DMSTATUS_ANYHALTED_GETS(value)        (GAP_BEXTRACT((value),1,8))
#define RISCV_DBG_DM_DMSTATUS_ANYHALTED_SET(value,field)   (GAP_BINSERT((value),(field),1,8))
#define RISCV_DBG_DM_DMSTATUS_ANYHALTED(val)               ((val) << 8)

#define RISCV_DBG_DM_DMSTATUS_ALLHALTED_GET(value)         (GAP_BEXTRACTU((value),1,9))
#define RISCV_DBG_DM_DMSTATUS_ALLHALTED_GETS(value)        (GAP_BEXTRACT((value),1,9))
#define RISCV_DBG_DM_DMSTATUS_ALLHALTED_SET(value,field)   (GAP_BINSERT((value),(field),1,9))
#define RISCV_DBG_DM_DMSTATUS_ALLHALTED(val)               ((val) << 9)

#define RISCV_DBG_DM_DMSTATUS_ANYRUNNING_GET(value)        (GAP_BEXTRACTU((value),1,10))
#define RISCV_DBG_DM_DMSTATUS_ANYRUNNING_GETS(value)       (GAP_BEXTRACT((value),1,10))
#define RISCV_DBG_DM_DMSTATUS_ANYRUNNING_SET(value,field)  (GAP_BINSERT((value),(field),1,10))
#define RISCV_DBG_DM_DMSTATUS_ANYRUNNING(val)              ((val) << 10)

#define RISCV_DBG_DM_DMSTATUS_ALLRUNNING_GET(value)        (GAP_BEXTRACTU((value),1,11))
#define RISCV_DBG_DM_DMSTATUS_ALLRUNNING_GETS(value)       (GAP_BEXTRACT((value),1,11))
#define RISCV_DBG_DM_DMSTATUS_ALLRUNNING_SET(value,field)  (GAP_BINSERT((value),(field),1,11))
#define RISCV_DBG_DM_DMSTATUS_ALLRUNNING(val)              ((val) << 11)

#define RISCV_DBG_DM_DMSTATUS_ANYUNAVAIL_GET(value)        (GAP_BEXTRACTU((value),1,12))
#define RISCV_DBG_DM_DMSTATUS_ANYUNAVAIL_GETS(value)       (GAP_BEXTRACT((value),1,12))
#define RISCV_DBG_DM_DMSTATUS_ANYUNAVAIL_SET(value,field)  (GAP_BINSERT((value),(field),1,12))
#define RISCV_DBG_DM_DMSTATUS_ANYUNAVAIL(val)              ((val) << 12)

#define RISCV_DBG_DM_DMSTATUS_ALLUNAVAIL_GET(value)        (GAP_BEXTRACTU((value),1,13))
#define RISCV_DBG_DM_DMSTATUS_ALLUNAVAIL_GETS(value)       (GAP_BEXTRACT((value),1,13))
#define RISCV_DBG_DM_DMSTATUS_ALLUNAVAIL_SET(value,field)  (GAP_BINSERT((value),(field),1,13))
#define RISCV_DBG_DM_DMSTATUS_ALLUNAVAIL(val)              ((val) << 13)

#define RISCV_DBG_DM_DMSTATUS_ANYNONEXISTENT_GET(value)    (GAP_BEXTRACTU((value),1,14))
#define RISCV_DBG_DM_DMSTATUS_ANYNONEXISTENT_GETS(value)   (GAP_BEXTRACT((value),1,14))
#define RISCV_DBG_DM_DMSTATUS_ANYNONEXISTENT_SET(value,field) (GAP_BINSERT((value),(field),1,14))
#define RISCV_DBG_DM_DMSTATUS_ANYNONEXISTENT(val)          ((val) << 14)

#define RISCV_DBG_DM_DMSTATUS_ALLNONEXISTENT_GET(value)    (GAP_BEXTRACTU((value),1,15))
#define RISCV_DBG_DM_DMSTATUS_ALLNONEXISTENT_GETS(value)   (GAP_BEXTRACT((value),1,15))
#define RISCV_DBG_DM_DMSTATUS_ALLNONEXISTENT_SET(value,field) (GAP_BINSERT((value),(field),1,15))
#define RISCV_DBG_DM_DMSTATUS_ALLNONEXISTENT(val)          ((val) << 15)

#define RISCV_DBG_DM_DMSTATUS_ANYRESUMEACK_GET(value)      (GAP_BEXTRACTU((value),1,16))
#define RISCV_DBG_DM_DMSTATUS_ANYRESUMEACK_GETS(value)     (GAP_BEXTRACT((value),1,16))
#define RISCV_DBG_DM_DMSTATUS_ANYRESUMEACK_SET(value,field) (GAP_BINSERT((value),(field),1,16))
#define RISCV_DBG_DM_DMSTATUS_ANYRESUMEACK(val)            ((val) << 16)

#define RISCV_DBG_DM_DMSTATUS_ALLRESUMEACK_GET(value)      (GAP_BEXTRACTU((value),1,17))
#define RISCV_DBG_DM_DMSTATUS_ALLRESUMEACK_GETS(value)     (GAP_BEXTRACT((value),1,17))
#define RISCV_DBG_DM_DMSTATUS_ALLRESUMEACK_SET(value,field) (GAP_BINSERT((value),(field),1,17))
#define RISCV_DBG_DM_DMSTATUS_ALLRESUMEACK(val)            ((val) << 17)

#define RISCV_DBG_DM_DMSTATUS_ANYHAVERESET_GET(value)      (GAP_BEXTRACTU((value),1,18))
#define RISCV_DBG_DM_DMSTATUS_ANYHAVERESET_GETS(value)     (GAP_BEXTRACT((value),1,18))
#define RISCV_DBG_DM_DMSTATUS_ANYHAVERESET_SET(value,field) (GAP_BINSERT((value),(field),1,18))
#define RISCV_DBG_DM_DMSTATUS_ANYHAVERESET(val)            ((val) << 18)

#define RISCV_DBG_DM_DMSTATUS_ALLHAVERESET_GET(value)      (GAP_BEXTRACTU((value),1,19))
#define RISCV_DBG_DM_DMSTATUS_ALLHAVERESET_GETS(value)     (GAP_BEXTRACT((value),1,19))
#define RISCV_DBG_DM_DMSTATUS_ALLHAVERESET_SET(value,field) (GAP_BINSERT((value),(field),1,19))
#define RISCV_DBG_DM_DMSTATUS_ALLHAVERESET(val)            ((val) << 19)

#define RISCV_DBG_DM_DMSTATUS_IMPEBREAK_GET(value)         (GAP_BEXTRACTU((value),1,22))
#define RISCV_DBG_DM_DMSTATUS_IMPEBREAK_GETS(value)        (GAP_BEXTRACT((value),1,22))
#define RISCV_DBG_DM_DMSTATUS_IMPEBREAK_SET(value,field)   (GAP_BINSERT((value),(field),1,22))
#define RISCV_DBG_DM_DMSTATUS_IMPEBREAK(val)               ((val) << 22)

#define RISCV_DBG_DM_HARTINFO_DATAADDR_GET(value)          (GAP_BEXTRACTU((value),12,0))
#define RISCV_DBG_DM_HARTINFO_DATAADDR_GETS(value)         (GAP_BEXTRACT((value),12,0))
#define RISCV_DBG_DM_HARTINFO_DATAADDR_SET(value,field)    (GAP_BINSERT((value),(field),12,0))
#define RISCV_DBG_DM_HARTINFO_DATAADDR(val)                ((val) << 0)

#define RISCV_DBG_DM_HARTINFO_DATASIZE_GET(value)          (GAP_BEXTRACTU((value),4,12))
#define RISCV_DBG_DM_HARTINFO_DATASIZE_GETS(value)         (GAP_BEXTRACT((value),4,12))
#define RISCV_DBG_DM_HARTINFO_DATASIZE_SET(value,field)    (GAP_BINSERT((value),(field),4,12))
#define RISCV_DBG_DM_HARTINFO_DATASIZE(val)                ((val) << 12)

#define RISCV_DBG_DM_HARTINFO_DATAACCESS_GET(value)        (GAP_BEXTRACTU((value),1,16))
#define RISCV_DBG_DM_HARTINFO_DATAACCESS_GETS(value)       (GAP_BEXTRACT((value),1,16))
#define RISCV_DBG_DM_HARTINFO_DATAACCESS_SET(value,field)  (GAP_BINSERT((value),(field),1,16))
#define RISCV_DBG_DM_HARTINFO_DATAACCESS(val)              ((val) << 16)

#define RISCV_DBG_DM_HARTINFO_NSCRATCH_GET(value)          (GAP_BEXTRACTU((value),4,20))
#define RISCV_DBG_DM_HARTINFO_NSCRATCH_GETS(value)         (GAP_BEXTRACT((value),4,20))
#define RISCV_DBG_DM_HARTINFO_NSCRATCH_SET(value,field)    (GAP_BINSERT((value),(field),4,20))
#define RISCV_DBG_DM_HARTINFO_NSCRATCH(val)                ((val) << 20)

#define RISCV_DBG_DM_ABSTRACTCS_DATACOUNT_GET(value)       (GAP_BEXTRACTU((value),4,0))
#define RISCV_DBG_DM_ABSTRACTCS_DATACOUNT_GETS(value)      (GAP_BEXTRACT((value),4,0))
#define RISCV_DBG_DM_ABSTRACTCS_DATACOUNT_SET(value,field) (GAP_BINSERT((value),(field),4,0))
#define RISCV_DBG_DM_ABSTRACTCS_DATACOUNT(val)             ((val) << 0)

#define RISCV_DBG_DM_ABSTRACTCS_CMDERR_GET(value)          (GAP_BEXTRACTU((value),3,8))
#define RISCV_DBG_DM_ABSTRACTCS_CMDERR_GETS(value)         (GAP_BEXTRACT((value),3,8))
#define RISCV_DBG_DM_ABSTRACTCS_CMDERR_SET(value,field)    (GAP_BINSERT((value),(field),3,8))
#define RISCV_DBG_DM_ABSTRACTCS_CMDERR(val)                ((val) << 8)

#define RISCV_DBG_DM_ABSTRACTCS_BUSY_GET(value)            (GAP_BEXTRACTU((value),1,12))
#define RISCV_DBG_DM_ABSTRACTCS_BUSY_GETS(value)           (GAP_BEXTRACT((value),1,12))
#define RISCV_DBG_DM_ABSTRACTCS_BUSY_SET(value,field)      (GAP_BINSERT((value),(field),1,12))
#define RISCV_DBG_DM_ABSTRACTCS_BUSY(val)                  ((val) << 12)

#define RISCV_DBG_DM_ABSTRACTCS_PROGBUFSIZE_GET(value)     (GAP_BEXTRACTU((value),5,24))
#define RISCV_DBG_DM_ABSTRACTCS_PROGBUFSIZE_GETS(value)    (GAP_BEXTRACT((value),5,24))
#define RISCV_DBG_DM_ABSTRACTCS_PROGBUFSIZE_SET(value,field) (GAP_BINSERT((value),(field),5,24))
#define RISCV_DBG_DM_ABSTRACTCS_PROGBUFSIZE(val)           ((val) << 24)

#define RISCV_DBG_DM_COMMAND_CONTROL_GET(value)            (GAP_BEXTRACTU((value),24,0))
#define RISCV_DBG_DM_COMMAND_CONTROL_GETS(value)           (GAP_BEXTRACT((value),24,0))
#define RISCV_DBG_DM_COMMAND_CONTROL_SET(value,field)      (GAP_BINSERT((value),(field),24,0))
#define RISCV_DBG_DM_COMMAND_CONTROL(val)                  ((val) << 0)

#define RISCV_DBG_DM_COMMAND_CMDTYPE_GET(value)            (GAP_BEXTRACTU((value),8,24))
#define RISCV_DBG_DM_COMMAND_CMDTYPE_GETS(value)           (GAP_BEXTRACT((value),8,24))
#define RISCV_DBG_DM_COMMAND_CMDTYPE_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
#define RISCV_DBG_DM_COMMAND_CMDTYPE(val)                  ((val) << 24)

#define RISCV_DBG_DM_SBCS_SBACCESS8_GET(value)             (GAP_BEXTRACTU((value),1,0))
#define RISCV_DBG_DM_SBCS_SBACCESS8_GETS(value)            (GAP_BEXTRACT((value),1,0))
#define RISCV_DBG_DM_SBCS_SBACCESS8_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
#define RISCV_DBG_DM_SBCS_SBACCESS8(val)                   ((val) << 0)

#define RISCV_DBG_DM_SBCS_SBACCESS16_GET(value)            (GAP_BEXTRACTU((value),1,1))
#define RISCV_DBG_DM_SBCS_SBACCESS16_GETS(value)           (GAP_BEXTRACT((value),1,1))
#define RISCV_DBG_DM_SBCS_SBACCESS16_SET(value,field)      (GAP_BINSERT((value),(field),1,1))
#define RISCV_DBG_DM_SBCS_SBACCESS16(val)                  ((val) << 1)

#define RISCV_DBG_DM_SBCS_SBACCESS32_GET(value)            (GAP_BEXTRACTU((value),1,2))
#define RISCV_DBG_DM_SBCS_SBACCESS32_GETS(value)           (GAP_BEXTRACT((value),1,2))
#define RISCV_DBG_DM_SBCS_SBACCESS32_SET(value,field)      (GAP_BINSERT((value),(field),1,2))
#define RISCV_DBG_DM_SBCS_SBACCESS32(val)                  ((val) << 2)

#define RISCV_DBG_DM_SBCS_SBACCESS64_GET(value)            (GAP_BEXTRACTU((value),1,3))
#define RISCV_DBG_DM_SBCS_SBACCESS64_GETS(value)           (GAP_BEXTRACT((value),1,3))
#define RISCV_DBG_DM_SBCS_SBACCESS64_SET(value,field)      (GAP_BINSERT((value),(field),1,3))
#define RISCV_DBG_DM_SBCS_SBACCESS64(val)                  ((val) << 3)

#define RISCV_DBG_DM_SBCS_SBACCESS128_GET(value)           (GAP_BEXTRACTU((value),1,4))
#define RISCV_DBG_DM_SBCS_SBACCESS128_GETS(value)          (GAP_BEXTRACT((value),1,4))
#define RISCV_DBG_DM_SBCS_SBACCESS128_SET(value,field)     (GAP_BINSERT((value),(field),1,4))
#define RISCV_DBG_DM_SBCS_SBACCESS128(val)                 ((val) << 4)

#define RISCV_DBG_DM_SBCS_SBASIZE_GET(value)               (GAP_BEXTRACTU((value),7,5))
#define RISCV_DBG_DM_SBCS_SBASIZE_GETS(value)              (GAP_BEXTRACT((value),7,5))
#define RISCV_DBG_DM_SBCS_SBASIZE_SET(value,field)         (GAP_BINSERT((value),(field),7,5))
#define RISCV_DBG_DM_SBCS_SBASIZE(val)                     ((val) << 5)

#define RISCV_DBG_DM_SBCS_SBERROR_GET(value)               (GAP_BEXTRACTU((value),3,12))
#define RISCV_DBG_DM_SBCS_SBERROR_GETS(value)              (GAP_BEXTRACT((value),3,12))
#define RISCV_DBG_DM_SBCS_SBERROR_SET(value,field)         (GAP_BINSERT((value),(field),3,12))
#define RISCV_DBG_DM_SBCS_SBERROR(val)                     ((val) << 12)

#define RISCV_DBG_DM_SBCS_SBREADONDATA_GET(value)          (GAP_BEXTRACTU((value),1,15))
#define RISCV_DBG_DM_SBCS_SBREADONDATA_GETS(value)         (GAP_BEXTRACT((value),1,15))
#define RISCV_DBG_DM_SBCS_SBREADONDATA_SET(value,field)    (GAP_BINSERT((value),(field),1,15))
#define RISCV_DBG_DM_SBCS_SBREADONDATA(val)                ((val) << 15)

#define RISCV_DBG_DM_SBCS_SBAUTOINCREMENT_GET(value)       (GAP_BEXTRACTU((value),1,16))
#define RISCV_DBG_DM_SBCS_SBAUTOINCREMENT_GETS(value)      (GAP_BEXTRACT((value),1,16))
#define RISCV_DBG_DM_SBCS_SBAUTOINCREMENT_SET(value,field) (GAP_BINSERT((value),(field),1,16))
#define RISCV_DBG_DM_SBCS_SBAUTOINCREMENT(val)             ((val) << 16)

#define RISCV_DBG_DM_SBCS_SBACCESS_GET(value)              (GAP_BEXTRACTU((value),3,17))
#define RISCV_DBG_DM_SBCS_SBACCESS_GETS(value)             (GAP_BEXTRACT((value),3,17))
#define RISCV_DBG_DM_SBCS_SBACCESS_SET(value,field)        (GAP_BINSERT((value),(field),3,17))
#define RISCV_DBG_DM_SBCS_SBACCESS(val)                    ((val) << 17)

#define RISCV_DBG_DM_SBCS_SBREADONADDR_GET(value)          (GAP_BEXTRACTU((value),1,20))
#define RISCV_DBG_DM_SBCS_SBREADONADDR_GETS(value)         (GAP_BEXTRACT((value),1,20))
#define RISCV_DBG_DM_SBCS_SBREADONADDR_SET(value,field)    (GAP_BINSERT((value),(field),1,20))
#define RISCV_DBG_DM_SBCS_SBREADONADDR(val)                ((val) << 20)

#define RISCV_DBG_DM_SBCS_SBBUSY_GET(value)                (GAP_BEXTRACTU((value),1,21))
#define RISCV_DBG_DM_SBCS_SBBUSY_GETS(value)               (GAP_BEXTRACT((value),1,21))
#define RISCV_DBG_DM_SBCS_SBBUSY_SET(value,field)          (GAP_BINSERT((value),(field),1,21))
#define RISCV_DBG_DM_SBCS_SBBUSY(val)                      ((val) << 21)

#define RISCV_DBG_DM_SBCS_SBBUSYERROR_GET(value)           (GAP_BEXTRACTU((value),6,22))
#define RISCV_DBG_DM_SBCS_SBBUSYERROR_GETS(value)          (GAP_BEXTRACT((value),6,22))
#define RISCV_DBG_DM_SBCS_SBBUSYERROR_SET(value,field)     (GAP_BINSERT((value),(field),6,22))
#define RISCV_DBG_DM_SBCS_SBBUSYERROR(val)                 ((val) << 22)

#define RISCV_DBG_DM_SBCS_SBVERSION_GET(value)             (GAP_BEXTRACTU((value),3,29))
#define RISCV_DBG_DM_SBCS_SBVERSION_GETS(value)            (GAP_BEXTRACT((value),3,29))
#define RISCV_DBG_DM_SBCS_SBVERSION_SET(value,field)       (GAP_BINSERT((value),(field),3,29))
#define RISCV_DBG_DM_SBCS_SBVERSION(val)                   ((val) << 29)

#define RISCV_DBG_DM_PROGBUF0_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define RISCV_DBG_DM_PROGBUF0_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define RISCV_DBG_DM_PROGBUF0_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define RISCV_DBG_DM_PROGBUF0_DATA(val)                    ((val) << 0)

#define RISCV_DBG_DM_PROGBUF1_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define RISCV_DBG_DM_PROGBUF1_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define RISCV_DBG_DM_PROGBUF1_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define RISCV_DBG_DM_PROGBUF1_DATA(val)                    ((val) << 0)

#define RISCV_DBG_DM_PROGBUF2_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define RISCV_DBG_DM_PROGBUF2_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define RISCV_DBG_DM_PROGBUF2_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define RISCV_DBG_DM_PROGBUF2_DATA(val)                    ((val) << 0)

#define RISCV_DBG_DM_PROGBUF3_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define RISCV_DBG_DM_PROGBUF3_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define RISCV_DBG_DM_PROGBUF3_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define RISCV_DBG_DM_PROGBUF3_DATA(val)                    ((val) << 0)

#define RISCV_DBG_DM_PROGBUF4_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define RISCV_DBG_DM_PROGBUF4_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define RISCV_DBG_DM_PROGBUF4_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define RISCV_DBG_DM_PROGBUF4_DATA(val)                    ((val) << 0)

#define RISCV_DBG_DM_PROGBUF5_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define RISCV_DBG_DM_PROGBUF5_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define RISCV_DBG_DM_PROGBUF5_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define RISCV_DBG_DM_PROGBUF5_DATA(val)                    ((val) << 0)

#define RISCV_DBG_DM_PROGBUF6_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define RISCV_DBG_DM_PROGBUF6_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define RISCV_DBG_DM_PROGBUF6_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define RISCV_DBG_DM_PROGBUF6_DATA(val)                    ((val) << 0)

#define RISCV_DBG_DM_PROGBUF7_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define RISCV_DBG_DM_PROGBUF7_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define RISCV_DBG_DM_PROGBUF7_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define RISCV_DBG_DM_PROGBUF7_DATA(val)                    ((val) << 0)

#define RISCV_DBG_DM_SBADDRESS0_ADDRESS_GET(value)         (GAP_BEXTRACTU((value),32,0))
#define RISCV_DBG_DM_SBADDRESS0_ADDRESS_GETS(value)        (GAP_BEXTRACT((value),32,0))
#define RISCV_DBG_DM_SBADDRESS0_ADDRESS_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
#define RISCV_DBG_DM_SBADDRESS0_ADDRESS(val)               ((val) << 0)

#define RISCV_DBG_DM_DMCS2_HGSELECT_GET(value)             (GAP_BEXTRACTU((value),1,0))
#define RISCV_DBG_DM_DMCS2_HGSELECT_GETS(value)            (GAP_BEXTRACT((value),1,0))
#define RISCV_DBG_DM_DMCS2_HGSELECT_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
#define RISCV_DBG_DM_DMCS2_HGSELECT(val)                   ((val) << 0)

#define RISCV_DBG_DM_DMCS2_HGWRITE_GET(value)              (GAP_BEXTRACTU((value),1,1))
#define RISCV_DBG_DM_DMCS2_HGWRITE_GETS(value)             (GAP_BEXTRACT((value),1,1))
#define RISCV_DBG_DM_DMCS2_HGWRITE_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
#define RISCV_DBG_DM_DMCS2_HGWRITE(val)                    ((val) << 1)

#define RISCV_DBG_DM_DMCS2_GROUP_GET(value)                (GAP_BEXTRACTU((value),5,2))
#define RISCV_DBG_DM_DMCS2_GROUP_GETS(value)               (GAP_BEXTRACT((value),5,2))
#define RISCV_DBG_DM_DMCS2_GROUP_SET(value,field)          (GAP_BINSERT((value),(field),5,2))
#define RISCV_DBG_DM_DMCS2_GROUP(val)                      ((val) << 2)

#define RISCV_DBG_DM_DMCS2_DMEXTTRIGGER_GET(value)         (GAP_BEXTRACTU((value),4,7))
#define RISCV_DBG_DM_DMCS2_DMEXTTRIGGER_GETS(value)        (GAP_BEXTRACT((value),4,7))
#define RISCV_DBG_DM_DMCS2_DMEXTTRIGGER_SET(value,field)   (GAP_BINSERT((value),(field),4,7))
#define RISCV_DBG_DM_DMCS2_DMEXTTRIGGER(val)               ((val) << 7)

#define RISCV_DBG_DM_DMCS2_GROUPTYPE_GET(value)            (GAP_BEXTRACTU((value),1,11))
#define RISCV_DBG_DM_DMCS2_GROUPTYPE_GETS(value)           (GAP_BEXTRACT((value),1,11))
#define RISCV_DBG_DM_DMCS2_GROUPTYPE_SET(value,field)      (GAP_BINSERT((value),(field),1,11))
#define RISCV_DBG_DM_DMCS2_GROUPTYPE(val)                  ((val) << 11)

#define RISCV_DBG_DM_SBDATA0_DATA_GET(value)               (GAP_BEXTRACTU((value),32,0))
#define RISCV_DBG_DM_SBDATA0_DATA_GETS(value)              (GAP_BEXTRACT((value),32,0))
#define RISCV_DBG_DM_SBDATA0_DATA_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
#define RISCV_DBG_DM_SBDATA0_DATA(val)                     ((val) << 0)

#endif

#endif
