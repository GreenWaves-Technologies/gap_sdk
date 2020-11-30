
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

#ifndef __ARCHI_RISCV_DBG_DM_REGFIELD__
#define __ARCHI_RISCV_DBG_DM_REGFIELD__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS FIELDS
//

// Data value (access: R/W)
#define RISCV_DBG_DM_DATA0_DATA_BIT                                  0
#define RISCV_DBG_DM_DATA0_DATA_WIDTH                                32
#define RISCV_DBG_DM_DATA0_DATA_MASK                                 0xffffffff

// Data value (access: R/W)
#define RISCV_DBG_DM_DATA1_DATA_BIT                                  0
#define RISCV_DBG_DM_DATA1_DATA_WIDTH                                32
#define RISCV_DBG_DM_DATA1_DATA_MASK                                 0xffffffff

// This  bit  serves  as  a  reset  signal  for  the  DebugModule itself.0:  The  modules  state,  including  authenticationmechanism, takes its reset values (thedmactivebitis the only bit which can be written to somethingother than its reset value).1:  The module functions normally.No other mechanism should exist that may resultin  resetting  the  Debug  Module  after  power  up,with the possible (but not recommended) excep-tion of a global reset signal that resets the entireplatform.A debugger may pulse this bit low to get the De-bug Module into a known state.Implementations may pay attention to this bit tofurther aid debugging, for example by preventingthe Debug Module from being power gated whiledebugging is active. (access: R/W)
#define RISCV_DBG_DM_DMCONTROL_DMACTIVE_BIT                          0
#define RISCV_DBG_DM_DMCONTROL_DMACTIVE_WIDTH                        1
#define RISCV_DBG_DM_DMCONTROL_DMACTIVE_MASK                         0x1

// This bit controls the reset signal from the DM tothe  rest  of  the  system.   The  signal  should  resetevery  part  of  the  system,  including  every  hart,except for the DM and any logic required to accessthe DM. To perform a system reset the debuggerwrites 1, and then writes 0 to deassert the reset. (access: R/W)
#define RISCV_DBG_DM_DMCONTROL_NDMRESET_BIT                          1
#define RISCV_DBG_DM_DMCONTROL_NDMRESET_WIDTH                        1
#define RISCV_DBG_DM_DMCONTROL_NDMRESET_MASK                         0x2

// This optional field clears the halt-on-reset requestbit for all currently selected harts.Writes apply to the new value ofhartselandhasel. (access: R/W)
#define RISCV_DBG_DM_DMCONTROL_CLRRESETHALTREQ_BIT                   2
#define RISCV_DBG_DM_DMCONTROL_CLRRESETHALTREQ_WIDTH                 1
#define RISCV_DBG_DM_DMCONTROL_CLRRESETHALTREQ_MASK                  0x4

// This  optional  field  writes  the  halt-on-reset  re-quest  bit  for  all  currently  selected  harts,  unlessclrresethaltreqis  simultaneously  set  to  1.   Whenset to 1, each selected hart will halt upon the nextdeassertion of its reset.  The halt-on-reset requestbit  is  not  automatically  cleared.   The  debuggermust write toclrresethaltreqto clear it.Writes apply to the new value ofhartselandhasel.Ifhasresethaltreqis  0,  this  field  is  not  imple-mented. (access: R/W)
#define RISCV_DBG_DM_DMCONTROL_SETRESETHALTREQ_BIT                   3
#define RISCV_DBG_DM_DMCONTROL_SETRESETHALTREQ_WIDTH                 1
#define RISCV_DBG_DM_DMCONTROL_SETRESETHALTREQ_MASK                  0x8

// The high 10 bits ofhartsel:  the DM-specific indexof the hart to select.  This hart is always part ofthe currently selected harts. (access: R/W)
#define RISCV_DBG_DM_DMCONTROL_HARTSELHI_BIT                         6
#define RISCV_DBG_DM_DMCONTROL_HARTSELHI_WIDTH                       10
#define RISCV_DBG_DM_DMCONTROL_HARTSELHI_MASK                        0xffc0

// The low 10 bits ofhartsel:  the DM-specific indexof the hart to select.  This hart is always part ofthe currently selected harts. (access: R/W)
#define RISCV_DBG_DM_DMCONTROL_HARTSELLO_BIT                         16
#define RISCV_DBG_DM_DMCONTROL_HARTSELLO_WIDTH                       10
#define RISCV_DBG_DM_DMCONTROL_HARTSELLO_MASK                        0x3ff0000

// Selects the definition of currently selected harts.0:  There is a single currently selected hart, thatis selected byhartsel.1:  There may be multiple currently selected harts the hart selected byhartsel, plus those selectedby the hart array mask register.An implementation which does not implement thehart array mask register must tie this field to 0.A  debugger  which  wishes  to  use  the  hart  arraymask register feature should set this bit and readback to see if the functionality is supported. (access: R/W)
#define RISCV_DBG_DM_DMCONTROL_HASEL_BIT                             26
#define RISCV_DBG_DM_DMCONTROL_HASEL_WIDTH                           1
#define RISCV_DBG_DM_DMCONTROL_HASEL_MASK                            0x4000000

// 0:  No effect.1:  Clearshaveresetfor any selected harts.Writes apply to the new value ofhartselandhasel. (access: R/W)
#define RISCV_DBG_DM_DMCONTROL_ACKHAVERESET_BIT                      28
#define RISCV_DBG_DM_DMCONTROL_ACKHAVERESET_WIDTH                    1
#define RISCV_DBG_DM_DMCONTROL_ACKHAVERESET_MASK                     0x10000000

// This optional field writes the reset bit for all thecurrently selected harts.  To perform a reset thedebugger writes 1, and then writes 0 to deassertthe reset signal.While this bit is 1, the debugger must not changewhich harts are selected.If this feature is not implemented, the bit alwaysstays 0, so after writing 1 the debugger can readthe register back to see if the feature is supported.Writes apply to the new value ofhartselandhasel. (access: R/W)
#define RISCV_DBG_DM_DMCONTROL_HARTRESET_BIT                         29
#define RISCV_DBG_DM_DMCONTROL_HARTRESET_WIDTH                       1
#define RISCV_DBG_DM_DMCONTROL_HARTRESET_MASK                        0x20000000

// Writing  1  causes  the  currently  selected  harts  toresume  once,  if  they  are  halted  when  the  writeoccurs.  It also clears the resume ack bit for thoseharts.resumereqis ignored ifhaltreqis set.Writes apply to the new value ofhartselandhasel. (access: R/W)
#define RISCV_DBG_DM_DMCONTROL_RESUMEREQ_BIT                         30
#define RISCV_DBG_DM_DMCONTROL_RESUMEREQ_WIDTH                       1
#define RISCV_DBG_DM_DMCONTROL_RESUMEREQ_MASK                        0x40000000

// Writing 0 clears the halt request bit for all cur-rently selected harts.  This may cancel outstand-ing halt requests for those harts.Writing 1 sets the halt request bit for all currentlyselected harts.  Running harts will halt whenevertheir halt request bit is set.Writes apply to the new value ofhartselandhasel. (access: R/W)
#define RISCV_DBG_DM_DMCONTROL_HALTREQ_BIT                           31
#define RISCV_DBG_DM_DMCONTROL_HALTREQ_WIDTH                         1
#define RISCV_DBG_DM_DMCONTROL_HALTREQ_MASK                          0x80000000

// 0:  There is no Debug Module present.1:  There is a Debug Module and it conforms toversion 0.11 of this specification.2:  There is a Debug Module and it conforms toversion 0.13 of this specification.15:  There is a Debug Module but it does not con-form to any available version of this spec. (access: R)
#define RISCV_DBG_DM_DMSTATUS_VERSION_BIT                            0
#define RISCV_DBG_DM_DMSTATUS_VERSION_WIDTH                          4
#define RISCV_DBG_DM_DMSTATUS_VERSION_MASK                           0xf

// 0:confstrptr0confstrptr3hold  informationwhich is not relevant to the configuration string.1:confstrptr0confstrptr3hold  the  addressof the configuration string. (access: R)
#define RISCV_DBG_DM_DMSTATUS_CONFSTRPTRVALID_BIT                    4
#define RISCV_DBG_DM_DMSTATUS_CONFSTRPTRVALID_WIDTH                  1
#define RISCV_DBG_DM_DMSTATUS_CONFSTRPTRVALID_MASK                   0x10

// 1  if  this  Debug  Module  supports  halt-on-resetfunctionality  controllable  by  thesetresethaltreqandclrresethaltreqbits.  0 otherwise. (access: R)
#define RISCV_DBG_DM_DMSTATUS_HASRESETHALTREQ_BIT                    5
#define RISCV_DBG_DM_DMSTATUS_HASRESETHALTREQ_WIDTH                  1
#define RISCV_DBG_DM_DMSTATUS_HASRESETHALTREQ_MASK                   0x20

// 0:  The authentication module is ready to processthe next read/write toauthdata.1:  The authentication module is busy.  Accessingauthdataresults in unspecified behavior.authbusyonly becomes set in immediate responseto an access toauthdata. (access: R)
#define RISCV_DBG_DM_DMSTATUS_AUTHBUSY_BIT                           6
#define RISCV_DBG_DM_DMSTATUS_AUTHBUSY_WIDTH                         1
#define RISCV_DBG_DM_DMSTATUS_AUTHBUSY_MASK                          0x40

// 0:   Authentication  is  required  before  using  theDM.1:  The authentication check has passed.On components that dont implement authentica-tion, this bit must be preset as 1. (access: R)
#define RISCV_DBG_DM_DMSTATUS_AUTHENTICATED_BIT                      7
#define RISCV_DBG_DM_DMSTATUS_AUTHENTICATED_WIDTH                    1
#define RISCV_DBG_DM_DMSTATUS_AUTHENTICATED_MASK                     0x80

// This field is 1 when any currently selected hart ishalted. (access: R)
#define RISCV_DBG_DM_DMSTATUS_ANYHALTED_BIT                          8
#define RISCV_DBG_DM_DMSTATUS_ANYHALTED_WIDTH                        1
#define RISCV_DBG_DM_DMSTATUS_ANYHALTED_MASK                         0x100

// This  field  is  1  when  all  currently  selected  hartsare halted. (access: R)
#define RISCV_DBG_DM_DMSTATUS_ALLHALTED_BIT                          9
#define RISCV_DBG_DM_DMSTATUS_ALLHALTED_WIDTH                        1
#define RISCV_DBG_DM_DMSTATUS_ALLHALTED_MASK                         0x200

// This field is 1 when any currently selected hart isrunning. (access: R)
#define RISCV_DBG_DM_DMSTATUS_ANYRUNNING_BIT                         10
#define RISCV_DBG_DM_DMSTATUS_ANYRUNNING_WIDTH                       1
#define RISCV_DBG_DM_DMSTATUS_ANYRUNNING_MASK                        0x400

// This  field  is  1  when  all  currently  selected  hartsare running. (access: R)
#define RISCV_DBG_DM_DMSTATUS_ALLRUNNING_BIT                         11
#define RISCV_DBG_DM_DMSTATUS_ALLRUNNING_WIDTH                       1
#define RISCV_DBG_DM_DMSTATUS_ALLRUNNING_MASK                        0x800

// This field is 1 when any currently selected hart isunavailable. (access: R)
#define RISCV_DBG_DM_DMSTATUS_ANYUNAVAIL_BIT                         12
#define RISCV_DBG_DM_DMSTATUS_ANYUNAVAIL_WIDTH                       1
#define RISCV_DBG_DM_DMSTATUS_ANYUNAVAIL_MASK                        0x1000

// This  field  is  1  when  all  currently  selected  hartsare unavailable. (access: R)
#define RISCV_DBG_DM_DMSTATUS_ALLUNAVAIL_BIT                         13
#define RISCV_DBG_DM_DMSTATUS_ALLUNAVAIL_WIDTH                       1
#define RISCV_DBG_DM_DMSTATUS_ALLUNAVAIL_MASK                        0x2000

// This  field  is  1  when  any  currently  selected  hartdoes not exist in this platform. (access: R)
#define RISCV_DBG_DM_DMSTATUS_ANYNONEXISTENT_BIT                     14
#define RISCV_DBG_DM_DMSTATUS_ANYNONEXISTENT_WIDTH                   1
#define RISCV_DBG_DM_DMSTATUS_ANYNONEXISTENT_MASK                    0x4000

// This field is 1 when all currently selected harts donot exist in this platform. (access: R)
#define RISCV_DBG_DM_DMSTATUS_ALLNONEXISTENT_BIT                     15
#define RISCV_DBG_DM_DMSTATUS_ALLNONEXISTENT_WIDTH                   1
#define RISCV_DBG_DM_DMSTATUS_ALLNONEXISTENT_MASK                    0x8000

// This  field  is  1  when  any  currently  selected  harthas acknowledged its last resume request. (access: R)
#define RISCV_DBG_DM_DMSTATUS_ANYRESUMEACK_BIT                       16
#define RISCV_DBG_DM_DMSTATUS_ANYRESUMEACK_WIDTH                     1
#define RISCV_DBG_DM_DMSTATUS_ANYRESUMEACK_MASK                      0x10000

// This  field  is  1  when  all  currently  selected  hartshave acknowledged their last resume request. (access: R)
#define RISCV_DBG_DM_DMSTATUS_ALLRESUMEACK_BIT                       17
#define RISCV_DBG_DM_DMSTATUS_ALLRESUMEACK_WIDTH                     1
#define RISCV_DBG_DM_DMSTATUS_ALLRESUMEACK_MASK                      0x20000

// This field is 1 when at least one currently selectedhart  has  been  reset  and  reset  has  not  been  ac-knowledged for that hart. (access: R)
#define RISCV_DBG_DM_DMSTATUS_ANYHAVERESET_BIT                       18
#define RISCV_DBG_DM_DMSTATUS_ANYHAVERESET_WIDTH                     1
#define RISCV_DBG_DM_DMSTATUS_ANYHAVERESET_MASK                      0x40000

// This  field  is  1  when  all  currently  selected  hartshave been reset and reset has not been acknowl-edged for any of them. (access: R)
#define RISCV_DBG_DM_DMSTATUS_ALLHAVERESET_BIT                       19
#define RISCV_DBG_DM_DMSTATUS_ALLHAVERESET_WIDTH                     1
#define RISCV_DBG_DM_DMSTATUS_ALLHAVERESET_MASK                      0x80000

// If 1, then there is an implicitebreakinstructionat  the  non-existent  word  immediately  after  theProgram  Buffer.   This  saves  the  debugger  fromhaving to write theebreakitself, and allows theProgram Buffer to be one word smaller.This must be 1 whenprogbufsizeis 1. (access: R)
#define RISCV_DBG_DM_DMSTATUS_IMPEBREAK_BIT                          22
#define RISCV_DBG_DM_DMSTATUS_IMPEBREAK_WIDTH                        1
#define RISCV_DBG_DM_DMSTATUS_IMPEBREAK_MASK                         0x400000

// Ifdataaccessis  0:  The number of  the  first  CSRdedicated to shadowing thedataregisters.Ifdataaccessis 1:  Signed address of RAM wherethedataregisters  are  shadowed,  to  be  used  toaccess relative tozero. (access: R)
#define RISCV_DBG_DM_HARTINFO_DATAADDR_BIT                           0
#define RISCV_DBG_DM_HARTINFO_DATAADDR_WIDTH                         12
#define RISCV_DBG_DM_HARTINFO_DATAADDR_MASK                          0xfff

// Ifdataaccessis 0:  Number of CSRs dedicated toshadowing thedataregisters.Ifdataaccessis 1:  Number of 32-bit words in thememory  map  dedicated  to  shadowing  thedataregisters.Since  there  are  at  most  12dataregisters,  thevalue in this register must be 12 or smaller. (access: R)
#define RISCV_DBG_DM_HARTINFO_DATASIZE_BIT                           12
#define RISCV_DBG_DM_HARTINFO_DATASIZE_WIDTH                         4
#define RISCV_DBG_DM_HARTINFO_DATASIZE_MASK                          0xf000

// 0:  Thedataregisters  are  shadowed  in  the  hartby CSRs.  Each CSR is DXLEN bits in size, andcorresponds to a single argument, per Table 3.1.1:  Thedataregisters are shadowed in the hartsmemory map.  Each register takes up 4 bytes inthe memory map. (access: R)
#define RISCV_DBG_DM_HARTINFO_DATAACCESS_BIT                         16
#define RISCV_DBG_DM_HARTINFO_DATAACCESS_WIDTH                       1
#define RISCV_DBG_DM_HARTINFO_DATAACCESS_MASK                        0x10000

// Number  ofdscratchregisters  available  for  thedebugger to use during program buffer execution,starting fromdscratch0. The debugger can makeno assumptions about the contents of these regis-ters between commands. (access: R)
#define RISCV_DBG_DM_HARTINFO_NSCRATCH_BIT                           20
#define RISCV_DBG_DM_HARTINFO_NSCRATCH_WIDTH                         4
#define RISCV_DBG_DM_HARTINFO_NSCRATCH_MASK                          0xf00000

// Number  ofdataregisters  that  are  implementedas part of the abstract command interface.  Validsizes are 1  12. (access: R)
#define RISCV_DBG_DM_ABSTRACTCS_DATACOUNT_BIT                        0
#define RISCV_DBG_DM_ABSTRACTCS_DATACOUNT_WIDTH                      4
#define RISCV_DBG_DM_ABSTRACTCS_DATACOUNT_MASK                       0xf

// Gets set if an abstract command fails.  The bits inthis field remain set until they are cleared by writ-ing 1 to them.  No abstract command is starteduntil the value is reset to 0.This field only contains a valid value ifbusyis 0.0 (none):  No error.1  (busy):   An  abstract  command  was  executingwhilecommand,abstractcs,  orabstractautowas written, or when one of thedataorprogbufregisters was read or written.  This status is onlywritten ifcmderrcontains 0.2 (not supported): The requested command is notsupported, regardless of whether the hart is run-ning or not.3  (exception):  An  exception  occurred  while  ex-ecuting  the  command  (e.g.  while  executing  theProgram Buffer).4 (halt/resume):  The abstract command couldntexecute  because  the  hart  wasnt  in  the  requiredstate (running/halted), or unavailable.5  (bus):  The  abstract  command  failed  due  to  abus error (e.g. alignment, access size, or timeout).7 (other):  The command failed for another rea-son. (access: R/W)
#define RISCV_DBG_DM_ABSTRACTCS_CMDERR_BIT                           8
#define RISCV_DBG_DM_ABSTRACTCS_CMDERR_WIDTH                         3
#define RISCV_DBG_DM_ABSTRACTCS_CMDERR_MASK                          0x700

// 1:  An abstract command is currently being exe-cuted.This bit is set as soon ascommandis written, andis not cleared until that command has completed. (access: R)
#define RISCV_DBG_DM_ABSTRACTCS_BUSY_BIT                             12
#define RISCV_DBG_DM_ABSTRACTCS_BUSY_WIDTH                           1
#define RISCV_DBG_DM_ABSTRACTCS_BUSY_MASK                            0x1000

// Size of the Program Buffer, in 32-bit words.  Validsizes are 0 - 16. (access: R)
#define RISCV_DBG_DM_ABSTRACTCS_PROGBUFSIZE_BIT                      24
#define RISCV_DBG_DM_ABSTRACTCS_PROGBUFSIZE_WIDTH                    5
#define RISCV_DBG_DM_ABSTRACTCS_PROGBUFSIZE_MASK                     0x1f000000

// This  field  is  interpreted  in  a  command-specificmanner, described for each abstract command. (access: R/W)
#define RISCV_DBG_DM_COMMAND_CONTROL_BIT                             0
#define RISCV_DBG_DM_COMMAND_CONTROL_WIDTH                           24
#define RISCV_DBG_DM_COMMAND_CONTROL_MASK                            0xffffff

// The type determines the overall functionality ofthis abstract command. (access: R/W)
#define RISCV_DBG_DM_COMMAND_CMDTYPE_BIT                             24
#define RISCV_DBG_DM_COMMAND_CMDTYPE_WIDTH                           8
#define RISCV_DBG_DM_COMMAND_CMDTYPE_MASK                            0xff000000

// 1 when 8-bit system bus accesses are supported. (access: R)
#define RISCV_DBG_DM_SBCS_SBACCESS8_BIT                              0
#define RISCV_DBG_DM_SBCS_SBACCESS8_WIDTH                            1
#define RISCV_DBG_DM_SBCS_SBACCESS8_MASK                             0x1

// 1 when 16-bit system bus accesses are supported. (access: R)
#define RISCV_DBG_DM_SBCS_SBACCESS16_BIT                             1
#define RISCV_DBG_DM_SBCS_SBACCESS16_WIDTH                           1
#define RISCV_DBG_DM_SBCS_SBACCESS16_MASK                            0x2

// 1 when 32-bit system bus accesses are supported. (access: R)
#define RISCV_DBG_DM_SBCS_SBACCESS32_BIT                             2
#define RISCV_DBG_DM_SBCS_SBACCESS32_WIDTH                           1
#define RISCV_DBG_DM_SBCS_SBACCESS32_MASK                            0x4

// 1 when 64-bit system bus accesses are supported. (access: R)
#define RISCV_DBG_DM_SBCS_SBACCESS64_BIT                             3
#define RISCV_DBG_DM_SBCS_SBACCESS64_WIDTH                           1
#define RISCV_DBG_DM_SBCS_SBACCESS64_MASK                            0x8

// 1 when 128-bit system bus accesses are supported. (access: R)
#define RISCV_DBG_DM_SBCS_SBACCESS128_BIT                            4
#define RISCV_DBG_DM_SBCS_SBACCESS128_WIDTH                          1
#define RISCV_DBG_DM_SBCS_SBACCESS128_MASK                           0x10

// Width  of  system  bus  addresses  in  bits.   (0  indi-cates there is no bus access support.) (access: R)
#define RISCV_DBG_DM_SBCS_SBASIZE_BIT                                5
#define RISCV_DBG_DM_SBCS_SBASIZE_WIDTH                              7
#define RISCV_DBG_DM_SBCS_SBASIZE_MASK                               0xfe0

// When the Debug Modules system bus master en-counters an error, this field gets set.  The bits inthis field remain set until they are cleared by writ-ing  1  to  them.   While  this  field  is  non-zero,  nomore system bus accesses can be initiated by theDebug Module.An  implementation  may  report  Other  (7)  forany error condition.0:  There was no bus error.1:  There was a timeout.2:  A bad address was accessed.3:  There was an alignment error.4:  An access of unsupported size was requested.7:  Other. (access: R/W)
#define RISCV_DBG_DM_SBCS_SBERROR_BIT                                12
#define RISCV_DBG_DM_SBCS_SBERROR_WIDTH                              3
#define RISCV_DBG_DM_SBCS_SBERROR_MASK                               0x7000

// When 1, every read fromsbdata0automaticallytriggers a system bus read at the (possibly auto-incremented) address. (access: R/W)
#define RISCV_DBG_DM_SBCS_SBREADONDATA_BIT                           15
#define RISCV_DBG_DM_SBCS_SBREADONDATA_WIDTH                         1
#define RISCV_DBG_DM_SBCS_SBREADONDATA_MASK                          0x8000

// When 1,sbaddressis incremented by the accesssize (in bytes) selected insbaccessafter every sys-tem bus access. (access: R/W)
#define RISCV_DBG_DM_SBCS_SBAUTOINCREMENT_BIT                        16
#define RISCV_DBG_DM_SBCS_SBAUTOINCREMENT_WIDTH                      1
#define RISCV_DBG_DM_SBCS_SBAUTOINCREMENT_MASK                       0x10000

// Select  the  access  size  to  use  for  system  bus  ac-cesses.0:  8-bit1:  16-bit2:  32-bit3:  64-bit4:  128-bitIfsbaccesshas  an  unsupported  value  when  theDM  starts  a  bus  access,  the  access  is  not  per-formed andsberroris set to 4. (access: R/W)
#define RISCV_DBG_DM_SBCS_SBACCESS_BIT                               17
#define RISCV_DBG_DM_SBCS_SBACCESS_WIDTH                             3
#define RISCV_DBG_DM_SBCS_SBACCESS_MASK                              0xe0000

// When 1, every write tosbaddress0automaticallytriggers a system bus read at the new address. (access: R/W)
#define RISCV_DBG_DM_SBCS_SBREADONADDR_BIT                           20
#define RISCV_DBG_DM_SBCS_SBREADONADDR_WIDTH                         1
#define RISCV_DBG_DM_SBCS_SBREADONADDR_MASK                          0x100000

// When 1, indicates the system bus master is busy.(Whether the system bus itself is busy is related,but not the same thing.)  This bit goes high im-mediately  when  a  read  or  write  is  requested  forany reason, and does not go low until the accessis fully completed.Writes tosbcswhilesbbusyis high result in un-defined behavior.  A debugger must not write tosbcsuntil it readssbbusyas 0. (access: R)
#define RISCV_DBG_DM_SBCS_SBBUSY_BIT                                 21
#define RISCV_DBG_DM_SBCS_SBBUSY_WIDTH                               1
#define RISCV_DBG_DM_SBCS_SBBUSY_MASK                                0x200000

// Set  when  the  debugger  attempts  to  read  datawhile  a  read  is  in  progress,  or  when  the  debug-ger initiates a new access while one is already inprogress (whilesbbusyis set).  It remains set untilits explicitly cleared by the debugger.While this field is set, no more system bus accessescan be initiated by the Debug Module. (access: R/W)
#define RISCV_DBG_DM_SBCS_SBBUSYERROR_BIT                            22
#define RISCV_DBG_DM_SBCS_SBBUSYERROR_WIDTH                          6
#define RISCV_DBG_DM_SBCS_SBBUSYERROR_MASK                           0xfc00000

// 0: The System Bus interface conforms to mainlinedrafts of this spec older than 1 January, 2018.1: The System Bus interface conforms to this ver-sion of the spec.Other values are reserved for future versions. (access: R)
#define RISCV_DBG_DM_SBCS_SBVERSION_BIT                              29
#define RISCV_DBG_DM_SBCS_SBVERSION_WIDTH                            3
#define RISCV_DBG_DM_SBCS_SBVERSION_MASK                             0xe0000000

// Data value (access: R/W)
#define RISCV_DBG_DM_PROGBUF0_DATA_BIT                               0
#define RISCV_DBG_DM_PROGBUF0_DATA_WIDTH                             32
#define RISCV_DBG_DM_PROGBUF0_DATA_MASK                              0xffffffff

// Data value (access: R/W)
#define RISCV_DBG_DM_PROGBUF1_DATA_BIT                               0
#define RISCV_DBG_DM_PROGBUF1_DATA_WIDTH                             32
#define RISCV_DBG_DM_PROGBUF1_DATA_MASK                              0xffffffff

// Data value (access: R/W)
#define RISCV_DBG_DM_PROGBUF2_DATA_BIT                               0
#define RISCV_DBG_DM_PROGBUF2_DATA_WIDTH                             32
#define RISCV_DBG_DM_PROGBUF2_DATA_MASK                              0xffffffff

// Data value (access: R/W)
#define RISCV_DBG_DM_PROGBUF3_DATA_BIT                               0
#define RISCV_DBG_DM_PROGBUF3_DATA_WIDTH                             32
#define RISCV_DBG_DM_PROGBUF3_DATA_MASK                              0xffffffff

// Data value (access: R/W)
#define RISCV_DBG_DM_PROGBUF4_DATA_BIT                               0
#define RISCV_DBG_DM_PROGBUF4_DATA_WIDTH                             32
#define RISCV_DBG_DM_PROGBUF4_DATA_MASK                              0xffffffff

// Data value (access: R/W)
#define RISCV_DBG_DM_PROGBUF5_DATA_BIT                               0
#define RISCV_DBG_DM_PROGBUF5_DATA_WIDTH                             32
#define RISCV_DBG_DM_PROGBUF5_DATA_MASK                              0xffffffff

// Data value (access: R/W)
#define RISCV_DBG_DM_PROGBUF6_DATA_BIT                               0
#define RISCV_DBG_DM_PROGBUF6_DATA_WIDTH                             32
#define RISCV_DBG_DM_PROGBUF6_DATA_MASK                              0xffffffff

// Data value (access: R/W)
#define RISCV_DBG_DM_PROGBUF7_DATA_BIT                               0
#define RISCV_DBG_DM_PROGBUF7_DATA_WIDTH                             32
#define RISCV_DBG_DM_PROGBUF7_DATA_MASK                              0xffffffff

// Address (access: R/W)
#define RISCV_DBG_DM_SBADDRESS0_ADDRESS_BIT                          0
#define RISCV_DBG_DM_SBADDRESS0_ADDRESS_WIDTH                        32
#define RISCV_DBG_DM_SBADDRESS0_ADDRESS_MASK                         0xffffffff

// 0:  Operate on harts.1:  Operate on DM external triggers.If  there  are  no  DM  external  triggers,  this  fieldmust be tied to 0. (access: R/W)
#define RISCV_DBG_DM_DMCS2_HGSELECT_BIT                              0
#define RISCV_DBG_DM_DMCS2_HGSELECT_WIDTH                            1
#define RISCV_DBG_DM_DMCS2_HGSELECT_MASK                             0x1

// Whenhgselectis 0, writing 1 changes the groupof all selected harts to the value written togroup.When 1 is written andhgselectis 0, for every se-lected hart the DM will change its group to thevalue written togroup, if the hardware supportsthat group for that hart.When  1  is  written  andhgselectis  1,  the  DMwill  change  the  group  of  the  DM  external  trig-ger selected bydmexttriggerto the value writtentogroup, if the hardware supports that group forthat trigger.Writing 0 has no effect. (access: R/W)
#define RISCV_DBG_DM_DMCS2_HGWRITE_BIT                               1
#define RISCV_DBG_DM_DMCS2_HGWRITE_WIDTH                             1
#define RISCV_DBG_DM_DMCS2_HGWRITE_MASK                              0x2

// Whenhgselectis 0, contains the group of the hartspecified byhartsel.Whenhgselectis 1, contains the group of the DMexternal trigger selected bydmexttrigger.Writes only have an effect ifhgwriteis also written1.Group  numbers  are  contiguous  starting  at  0,with  the  highest  number  being  implementation-dependent, and possibly different between differ-ent group types.  Debuggers should read back thisfield after writing to confirm they are using a hartgroup that is supported.If groups arent implemented, then this entire fieldis 0. (access: R/W)
#define RISCV_DBG_DM_DMCS2_GROUP_BIT                                 2
#define RISCV_DBG_DM_DMCS2_GROUP_WIDTH                               5
#define RISCV_DBG_DM_DMCS2_GROUP_MASK                                0x7c

// This field contains the currently selected DM ex-ternal trigger.If a non-existent trigger value is written here, thehardware will change it to a valid one or 0 if noDM external triggers exist. (access: R/W)
#define RISCV_DBG_DM_DMCS2_DMEXTTRIGGER_BIT                          7
#define RISCV_DBG_DM_DMCS2_DMEXTTRIGGER_WIDTH                        4
#define RISCV_DBG_DM_DMCS2_DMEXTTRIGGER_MASK                         0x780

// 0:  The remaining fields in this register configurehalt groups.1:  The remaining fields in this register configureresume groups. (access: R/W)
#define RISCV_DBG_DM_DMCS2_GROUPTYPE_BIT                             11
#define RISCV_DBG_DM_DMCS2_GROUPTYPE_WIDTH                           1
#define RISCV_DBG_DM_DMCS2_GROUPTYPE_MASK                            0x800

// Data value (access: R/W)
#define RISCV_DBG_DM_SBDATA0_DATA_BIT                                0
#define RISCV_DBG_DM_SBDATA0_DATA_WIDTH                              32
#define RISCV_DBG_DM_SBDATA0_DATA_MASK                               0xffffffff

#endif
