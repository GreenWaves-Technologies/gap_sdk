
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

#ifndef __ARCHI_RISCV_DBG_DM_STRUCTS__
#define __ARCHI_RISCV_DBG_DM_STRUCTS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS STRUCTS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

typedef union {
  struct {
    unsigned int data            :32; // Data value
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_data0_t;

typedef union {
  struct {
    unsigned int data            :32; // Data value
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_data1_t;

typedef union {
  struct {
    unsigned int dmactive        :1 ; // This  bit  serves  as  a  reset  signal  for  the  DebugModule itself.0:  The  modules  state,  including  authenticationmechanism, takes its reset values (thedmactivebitis the only bit which can be written to somethingother than its reset value).1:  The module functions normally.No other mechanism should exist that may resultin  resetting  the  Debug  Module  after  power  up,with the possible (but not recommended) excep-tion of a global reset signal that resets the entireplatform.A debugger may pulse this bit low to get the De-bug Module into a known state.Implementations may pay attention to this bit tofurther aid debugging, for example by preventingthe Debug Module from being power gated whiledebugging is active.
    unsigned int ndmreset        :1 ; // This bit controls the reset signal from the DM tothe  rest  of  the  system.   The  signal  should  resetevery  part  of  the  system,  including  every  hart,except for the DM and any logic required to accessthe DM. To perform a system reset the debuggerwrites 1, and then writes 0 to deassert the reset.
    unsigned int clrresethaltreq :1 ; // This optional field clears the halt-on-reset requestbit for all currently selected harts.Writes apply to the new value ofhartselandhasel.
    unsigned int setresethaltreq :1 ; // This  optional  field  writes  the  halt-on-reset  re-quest  bit  for  all  currently  selected  harts,  unlessclrresethaltreqis  simultaneously  set  to  1.   Whenset to 1, each selected hart will halt upon the nextdeassertion of its reset.  The halt-on-reset requestbit  is  not  automatically  cleared.   The  debuggermust write toclrresethaltreqto clear it.Writes apply to the new value ofhartselandhasel.Ifhasresethaltreqis  0,  this  field  is  not  imple-mented.
    unsigned int padding0:2 ;
    unsigned int hartselhi       :10; // The high 10 bits ofhartsel:  the DM-specific indexof the hart to select.  This hart is always part ofthe currently selected harts.
    unsigned int hartsello       :10; // The low 10 bits ofhartsel:  the DM-specific indexof the hart to select.  This hart is always part ofthe currently selected harts.
    unsigned int hasel           :1 ; // Selects the definition of currently selected harts.0:  There is a single currently selected hart, thatis selected byhartsel.1:  There may be multiple currently selected harts the hart selected byhartsel, plus those selectedby the hart array mask register.An implementation which does not implement thehart array mask register must tie this field to 0.A  debugger  which  wishes  to  use  the  hart  arraymask register feature should set this bit and readback to see if the functionality is supported.
    unsigned int padding1:1 ;
    unsigned int ackhavereset    :1 ; // 0:  No effect.1:  Clearshaveresetfor any selected harts.Writes apply to the new value ofhartselandhasel.
    unsigned int hartreset       :1 ; // This optional field writes the reset bit for all thecurrently selected harts.  To perform a reset thedebugger writes 1, and then writes 0 to deassertthe reset signal.While this bit is 1, the debugger must not changewhich harts are selected.If this feature is not implemented, the bit alwaysstays 0, so after writing 1 the debugger can readthe register back to see if the feature is supported.Writes apply to the new value ofhartselandhasel.
    unsigned int resumereq       :1 ; // Writing  1  causes  the  currently  selected  harts  toresume  once,  if  they  are  halted  when  the  writeoccurs.  It also clears the resume ack bit for thoseharts.resumereqis ignored ifhaltreqis set.Writes apply to the new value ofhartselandhasel.
    unsigned int haltreq         :1 ; // Writing 0 clears the halt request bit for all cur-rently selected harts.  This may cancel outstand-ing halt requests for those harts.Writing 1 sets the halt request bit for all currentlyselected harts.  Running harts will halt whenevertheir halt request bit is set.Writes apply to the new value ofhartselandhasel.
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_dmcontrol_t;

typedef union {
  struct {
    unsigned int version         :4 ; // 0:  There is no Debug Module present.1:  There is a Debug Module and it conforms toversion 0.11 of this specification.2:  There is a Debug Module and it conforms toversion 0.13 of this specification.15:  There is a Debug Module but it does not con-form to any available version of this spec.
    unsigned int confstrptrvalid :1 ; // 0:confstrptr0confstrptr3hold  informationwhich is not relevant to the configuration string.1:confstrptr0confstrptr3hold  the  addressof the configuration string.
    unsigned int hasresethaltreq :1 ; // 1  if  this  Debug  Module  supports  halt-on-resetfunctionality  controllable  by  thesetresethaltreqandclrresethaltreqbits.  0 otherwise.
    unsigned int authbusy        :1 ; // 0:  The authentication module is ready to processthe next read/write toauthdata.1:  The authentication module is busy.  Accessingauthdataresults in unspecified behavior.authbusyonly becomes set in immediate responseto an access toauthdata.
    unsigned int authenticated   :1 ; // 0:   Authentication  is  required  before  using  theDM.1:  The authentication check has passed.On components that dont implement authentica-tion, this bit must be preset as 1.
    unsigned int anyhalted       :1 ; // This field is 1 when any currently selected hart ishalted.
    unsigned int allhalted       :1 ; // This  field  is  1  when  all  currently  selected  hartsare halted.
    unsigned int anyrunning      :1 ; // This field is 1 when any currently selected hart isrunning.
    unsigned int allrunning      :1 ; // This  field  is  1  when  all  currently  selected  hartsare running.
    unsigned int anyunavail      :1 ; // This field is 1 when any currently selected hart isunavailable.
    unsigned int allunavail      :1 ; // This  field  is  1  when  all  currently  selected  hartsare unavailable.
    unsigned int anynonexistent  :1 ; // This  field  is  1  when  any  currently  selected  hartdoes not exist in this platform.
    unsigned int allnonexistent  :1 ; // This field is 1 when all currently selected harts donot exist in this platform.
    unsigned int anyresumeack    :1 ; // This  field  is  1  when  any  currently  selected  harthas acknowledged its last resume request.
    unsigned int allresumeack    :1 ; // This  field  is  1  when  all  currently  selected  hartshave acknowledged their last resume request.
    unsigned int anyhavereset    :1 ; // This field is 1 when at least one currently selectedhart  has  been  reset  and  reset  has  not  been  ac-knowledged for that hart.
    unsigned int allhavereset    :1 ; // This  field  is  1  when  all  currently  selected  hartshave been reset and reset has not been acknowl-edged for any of them.
    unsigned int padding0:2 ;
    unsigned int impebreak       :1 ; // If 1, then there is an implicitebreakinstructionat  the  non-existent  word  immediately  after  theProgram  Buffer.   This  saves  the  debugger  fromhaving to write theebreakitself, and allows theProgram Buffer to be one word smaller.This must be 1 whenprogbufsizeis 1.
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_dmstatus_t;

typedef union {
  struct {
    unsigned int dataaddr        :12; // Ifdataaccessis  0:  The number of  the  first  CSRdedicated to shadowing thedataregisters.Ifdataaccessis 1:  Signed address of RAM wherethedataregisters  are  shadowed,  to  be  used  toaccess relative tozero.
    unsigned int datasize        :4 ; // Ifdataaccessis 0:  Number of CSRs dedicated toshadowing thedataregisters.Ifdataaccessis 1:  Number of 32-bit words in thememory  map  dedicated  to  shadowing  thedataregisters.Since  there  are  at  most  12dataregisters,  thevalue in this register must be 12 or smaller.
    unsigned int dataaccess      :1 ; // 0:  Thedataregisters  are  shadowed  in  the  hartby CSRs.  Each CSR is DXLEN bits in size, andcorresponds to a single argument, per Table 3.1.1:  Thedataregisters are shadowed in the hartsmemory map.  Each register takes up 4 bytes inthe memory map.
    unsigned int padding0:3 ;
    unsigned int nscratch        :4 ; // Number  ofdscratchregisters  available  for  thedebugger to use during program buffer execution,starting fromdscratch0. The debugger can makeno assumptions about the contents of these regis-ters between commands.
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_hartinfo_t;

typedef union {
  struct {
    unsigned int datacount       :4 ; // Number  ofdataregisters  that  are  implementedas part of the abstract command interface.  Validsizes are 1  12.
    unsigned int padding0:4 ;
    unsigned int cmderr          :3 ; // Gets set if an abstract command fails.  The bits inthis field remain set until they are cleared by writ-ing 1 to them.  No abstract command is starteduntil the value is reset to 0.This field only contains a valid value ifbusyis 0.0 (none):  No error.1  (busy):   An  abstract  command  was  executingwhilecommand,abstractcs,  orabstractautowas written, or when one of thedataorprogbufregisters was read or written.  This status is onlywritten ifcmderrcontains 0.2 (not supported): The requested command is notsupported, regardless of whether the hart is run-ning or not.3  (exception):  An  exception  occurred  while  ex-ecuting  the  command  (e.g.  while  executing  theProgram Buffer).4 (halt/resume):  The abstract command couldntexecute  because  the  hart  wasnt  in  the  requiredstate (running/halted), or unavailable.5  (bus):  The  abstract  command  failed  due  to  abus error (e.g. alignment, access size, or timeout).7 (other):  The command failed for another rea-son.
    unsigned int padding1:1 ;
    unsigned int busy            :1 ; // 1:  An abstract command is currently being exe-cuted.This bit is set as soon ascommandis written, andis not cleared until that command has completed.
    unsigned int padding2:11;
    unsigned int progbufsize     :5 ; // Size of the Program Buffer, in 32-bit words.  Validsizes are 0 - 16.
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_abstractcs_t;

typedef union {
  struct {
    unsigned int control         :24; // This  field  is  interpreted  in  a  command-specificmanner, described for each abstract command.
    unsigned int cmdtype         :8 ; // The type determines the overall functionality ofthis abstract command.
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_command_t;

typedef union {
  struct {
    unsigned int sbaccess8       :1 ; // 1 when 8-bit system bus accesses are supported.
    unsigned int sbaccess16      :1 ; // 1 when 16-bit system bus accesses are supported.
    unsigned int sbaccess32      :1 ; // 1 when 32-bit system bus accesses are supported.
    unsigned int sbaccess64      :1 ; // 1 when 64-bit system bus accesses are supported.
    unsigned int sbaccess128     :1 ; // 1 when 128-bit system bus accesses are supported.
    unsigned int sbasize         :7 ; // Width  of  system  bus  addresses  in  bits.   (0  indi-cates there is no bus access support.)
    unsigned int sberror         :3 ; // When the Debug Modules system bus master en-counters an error, this field gets set.  The bits inthis field remain set until they are cleared by writ-ing  1  to  them.   While  this  field  is  non-zero,  nomore system bus accesses can be initiated by theDebug Module.An  implementation  may  report  Other  (7)  forany error condition.0:  There was no bus error.1:  There was a timeout.2:  A bad address was accessed.3:  There was an alignment error.4:  An access of unsupported size was requested.7:  Other.
    unsigned int sbreadondata    :1 ; // When 1, every read fromsbdata0automaticallytriggers a system bus read at the (possibly auto-incremented) address.
    unsigned int sbautoincrement :1 ; // When 1,sbaddressis incremented by the accesssize (in bytes) selected insbaccessafter every sys-tem bus access.
    unsigned int sbaccess        :3 ; // Select  the  access  size  to  use  for  system  bus  ac-cesses.0:  8-bit1:  16-bit2:  32-bit3:  64-bit4:  128-bitIfsbaccesshas  an  unsupported  value  when  theDM  starts  a  bus  access,  the  access  is  not  per-formed andsberroris set to 4.
    unsigned int sbreadonaddr    :1 ; // When 1, every write tosbaddress0automaticallytriggers a system bus read at the new address.
    unsigned int sbbusy          :1 ; // When 1, indicates the system bus master is busy.(Whether the system bus itself is busy is related,but not the same thing.)  This bit goes high im-mediately  when  a  read  or  write  is  requested  forany reason, and does not go low until the accessis fully completed.Writes tosbcswhilesbbusyis high result in un-defined behavior.  A debugger must not write tosbcsuntil it readssbbusyas 0.
    unsigned int sbbusyerror     :6 ; // Set  when  the  debugger  attempts  to  read  datawhile  a  read  is  in  progress,  or  when  the  debug-ger initiates a new access while one is already inprogress (whilesbbusyis set).  It remains set untilits explicitly cleared by the debugger.While this field is set, no more system bus accessescan be initiated by the Debug Module.
    unsigned int padding0:1 ;
    unsigned int sbversion       :3 ; // 0: The System Bus interface conforms to mainlinedrafts of this spec older than 1 January, 2018.1: The System Bus interface conforms to this ver-sion of the spec.Other values are reserved for future versions.
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_sbcs_t;

typedef union {
  struct {
    unsigned int data            :32; // Data value
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_progbuf0_t;

typedef union {
  struct {
    unsigned int data            :32; // Data value
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_progbuf1_t;

typedef union {
  struct {
    unsigned int data            :32; // Data value
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_progbuf2_t;

typedef union {
  struct {
    unsigned int data            :32; // Data value
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_progbuf3_t;

typedef union {
  struct {
    unsigned int data            :32; // Data value
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_progbuf4_t;

typedef union {
  struct {
    unsigned int data            :32; // Data value
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_progbuf5_t;

typedef union {
  struct {
    unsigned int data            :32; // Data value
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_progbuf6_t;

typedef union {
  struct {
    unsigned int data            :32; // Data value
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_progbuf7_t;

typedef union {
  struct {
    unsigned int address         :32; // Address
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_sbaddress0_t;

typedef union {
  struct {
    unsigned int hgselect        :1 ; // 0:  Operate on harts.1:  Operate on DM external triggers.If  there  are  no  DM  external  triggers,  this  fieldmust be tied to 0.
    unsigned int hgwrite         :1 ; // Whenhgselectis 0, writing 1 changes the groupof all selected harts to the value written togroup.When 1 is written andhgselectis 0, for every se-lected hart the DM will change its group to thevalue written togroup, if the hardware supportsthat group for that hart.When  1  is  written  andhgselectis  1,  the  DMwill  change  the  group  of  the  DM  external  trig-ger selected bydmexttriggerto the value writtentogroup, if the hardware supports that group forthat trigger.Writing 0 has no effect.
    unsigned int group           :5 ; // Whenhgselectis 0, contains the group of the hartspecified byhartsel.Whenhgselectis 1, contains the group of the DMexternal trigger selected bydmexttrigger.Writes only have an effect ifhgwriteis also written1.Group  numbers  are  contiguous  starting  at  0,with  the  highest  number  being  implementation-dependent, and possibly different between differ-ent group types.  Debuggers should read back thisfield after writing to confirm they are using a hartgroup that is supported.If groups arent implemented, then this entire fieldis 0.
    unsigned int dmexttrigger    :4 ; // This field contains the currently selected DM ex-ternal trigger.If a non-existent trigger value is written here, thehardware will change it to a valid one or 0 if noDM external triggers exist.
    unsigned int grouptype       :1 ; // 0:  The remaining fields in this register configurehalt groups.1:  The remaining fields in this register configureresume groups.
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_dmcs2_t;

typedef union {
  struct {
    unsigned int data            :32; // Data value
  };
  unsigned int raw;
} __attribute__((packed)) riscv_dbg_dm_sbdata0_t;

#endif

#endif
