Input file: docs/IP_REFERENCES/RISCV_DBG_DM_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +------------------------------------------+------+-----+------------------------------------+
    |                   Name                   |Offset|Width|            Description             |
    +==========================================+======+=====+====================================+
    |:ref:`data0<riscv_dbg_dm_data0>`          |     4|   32|Abstract Data 0                     |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`data1<riscv_dbg_dm_data1>`          |     5|   32|Abstract Data 1                     |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`dmcontrol<riscv_dbg_dm_dmcontrol>`  |    16|   32|Debug Module Control                |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`dmstatus<riscv_dbg_dm_dmstatus>`    |    17|   32|Debug Module Status                 |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`hartinfo<riscv_dbg_dm_hartinfo>`    |    18|   32|Hart Info                           |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`abstractcs<riscv_dbg_dm_abstractcs>`|    22|   32|Abstract Control and Status         |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`command<riscv_dbg_dm_command>`      |    23|   32|Abstract Command                    |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`sbcs<riscv_dbg_dm_sbcs>`            |    56|   32|System Bus Access Control and Status|
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`progbuf0<riscv_dbg_dm_progbuf0>`    |    32|   32|Program Buffer 0                    |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`progbuf1<riscv_dbg_dm_progbuf1>`    |    33|   32|Program Buffer 1                    |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`progbuf2<riscv_dbg_dm_progbuf2>`    |    34|   32|Program Buffer 2                    |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`progbuf3<riscv_dbg_dm_progbuf3>`    |    35|   32|Program Buffer 3                    |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`progbuf4<riscv_dbg_dm_progbuf4>`    |    36|   32|Program Buffer 4                    |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`progbuf5<riscv_dbg_dm_progbuf5>`    |    37|   32|Program Buffer 5                    |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`progbuf6<riscv_dbg_dm_progbuf6>`    |    38|   32|Program Buffer 6                    |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`progbuf7<riscv_dbg_dm_progbuf7>`    |    39|   32|Program Buffer 7                    |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`sbaddress0<riscv_dbg_dm_sbaddress0>`|    57|   32|System Bus Address 31:0             |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`dmcs2<riscv_dbg_dm_dmcs2>`          |    50|   32|Debug Module Control and Status 2   |
    +------------------------------------------+------+-----+------------------------------------+
    |:ref:`sbdata0<riscv_dbg_dm_sbdata0>`      |    60|   32|System Bus data 31:0                |
    +------------------------------------------+------+-----+------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Abstract Data 0
                #define RISCV_DBG_DM_DATA0_OFFSET                0x4
        
                // Abstract Data 1
                #define RISCV_DBG_DM_DATA1_OFFSET                0x5
        
                // Debug Module Control
                #define RISCV_DBG_DM_DMCONTROL_OFFSET            0x10
        
                // Debug Module Status
                #define RISCV_DBG_DM_DMSTATUS_OFFSET             0x11
        
                // Hart Info
                #define RISCV_DBG_DM_HARTINFO_OFFSET             0x12
        
                // Abstract Control and Status
                #define RISCV_DBG_DM_ABSTRACTCS_OFFSET           0x16
        
                // Abstract Command
                #define RISCV_DBG_DM_COMMAND_OFFSET              0x17
        
                // System Bus Access Control and Status
                #define RISCV_DBG_DM_SBCS_OFFSET                 0x38
        
                // Program Buffer 0
                #define RISCV_DBG_DM_PROGBUF0_OFFSET             0x20
        
                // Program Buffer 1
                #define RISCV_DBG_DM_PROGBUF1_OFFSET             0x21
        
                // Program Buffer 2
                #define RISCV_DBG_DM_PROGBUF2_OFFSET             0x22
        
                // Program Buffer 3
                #define RISCV_DBG_DM_PROGBUF3_OFFSET             0x23
        
                // Program Buffer 4
                #define RISCV_DBG_DM_PROGBUF4_OFFSET             0x24
        
                // Program Buffer 5
                #define RISCV_DBG_DM_PROGBUF5_OFFSET             0x25
        
                // Program Buffer 6
                #define RISCV_DBG_DM_PROGBUF6_OFFSET             0x26
        
                // Program Buffer 7
                #define RISCV_DBG_DM_PROGBUF7_OFFSET             0x27
        
                // System Bus Address 31:0
                #define RISCV_DBG_DM_SBADDRESS0_OFFSET           0x39
        
                // Debug Module Control and Status 2
                #define RISCV_DBG_DM_DMCS2_OFFSET                0x32
        
                // System Bus data 31:0
                #define RISCV_DBG_DM_SBDATA0_OFFSET              0x3c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_data0_get(uint32_t base);
        static inline void riscv_dbg_dm_data0_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_data1_get(uint32_t base);
        static inline void riscv_dbg_dm_data1_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_dmcontrol_get(uint32_t base);
        static inline void riscv_dbg_dm_dmcontrol_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_dmstatus_get(uint32_t base);
        static inline void riscv_dbg_dm_dmstatus_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_hartinfo_get(uint32_t base);
        static inline void riscv_dbg_dm_hartinfo_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_abstractcs_get(uint32_t base);
        static inline void riscv_dbg_dm_abstractcs_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_command_get(uint32_t base);
        static inline void riscv_dbg_dm_command_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_sbcs_get(uint32_t base);
        static inline void riscv_dbg_dm_sbcs_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_progbuf0_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf0_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_progbuf1_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf1_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_progbuf2_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf2_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_progbuf3_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf3_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_progbuf4_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf4_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_progbuf5_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf5_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_progbuf6_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf6_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_progbuf7_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf7_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_sbaddress0_get(uint32_t base);
        static inline void riscv_dbg_dm_sbaddress0_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_dmcs2_get(uint32_t base);
        static inline void riscv_dbg_dm_dmcs2_set(uint32_t base, uint32_t value);

        static inline uint32_t riscv_dbg_dm_sbdata0_get(uint32_t base);
        static inline void riscv_dbg_dm_sbdata0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** RISCV_DBG_DM_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t data0;  // Abstract Data 0
            volatile uint32_t data1;  // Abstract Data 1
            volatile uint32_t reserved_0[1];  // Reserved/Not used.
            volatile uint32_t dmcontrol;  // Debug Module Control
            volatile uint32_t dmstatus;  // Debug Module Status
            volatile uint32_t hartinfo;  // Hart Info
            volatile uint32_t abstractcs;  // Abstract Control and Status
            volatile uint32_t command;  // Abstract Command
            volatile uint32_t reserved_1[7];  // Reserved/Not used.
            volatile uint32_t sbcs;  // System Bus Access Control and Status
            volatile uint32_t progbuf0;  // Program Buffer 0
            volatile uint32_t progbuf1;  // Program Buffer 1
            volatile uint32_t progbuf2;  // Program Buffer 2
            volatile uint32_t progbuf3;  // Program Buffer 3
            volatile uint32_t progbuf4;  // Program Buffer 4
            volatile uint32_t progbuf5;  // Program Buffer 5
            volatile uint32_t progbuf6;  // Program Buffer 6
            volatile uint32_t progbuf7;  // Program Buffer 7
            volatile uint32_t reserved_2[3];  // Reserved/Not used.
            volatile uint32_t sbaddress0;  // System Bus Address 31:0
            volatile uint32_t dmcs2;  // Debug Module Control and Status 2
            volatile uint32_t reserved_3[1];  // Reserved/Not used.
            volatile uint32_t sbdata0;  // System Bus data 31:0
        } __attribute__((packed)) riscv_dbg_dm_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_riscv_dbg_dm : public vp::regmap
        {
        public:
            vp_riscv_dbg_dm_data0 data0;
            vp_riscv_dbg_dm_data1 data1;
            vp_riscv_dbg_dm_dmcontrol dmcontrol;
            vp_riscv_dbg_dm_dmstatus dmstatus;
            vp_riscv_dbg_dm_hartinfo hartinfo;
            vp_riscv_dbg_dm_abstractcs abstractcs;
            vp_riscv_dbg_dm_command command;
            vp_riscv_dbg_dm_sbcs sbcs;
            vp_riscv_dbg_dm_progbuf0 progbuf0;
            vp_riscv_dbg_dm_progbuf1 progbuf1;
            vp_riscv_dbg_dm_progbuf2 progbuf2;
            vp_riscv_dbg_dm_progbuf3 progbuf3;
            vp_riscv_dbg_dm_progbuf4 progbuf4;
            vp_riscv_dbg_dm_progbuf5 progbuf5;
            vp_riscv_dbg_dm_progbuf6 progbuf6;
            vp_riscv_dbg_dm_progbuf7 progbuf7;
            vp_riscv_dbg_dm_sbaddress0 sbaddress0;
            vp_riscv_dbg_dm_dmcs2 dmcs2;
            vp_riscv_dbg_dm_sbdata0 sbdata0;
        };

|

.. _riscv_dbg_dm_data0:

data0
"""""

Abstract Data 0

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    |31:0 |R/W|data|Data value |
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Abstract Data 0
                #define RISCV_DBG_DM_DATA0_OFFSET                0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_data0_get(uint32_t base);
        static inline void riscv_dbg_dm_data0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data value (access: R/W)
        #define RISCV_DBG_DM_DATA0_DATA_BIT                                  0
        #define RISCV_DBG_DM_DATA0_DATA_WIDTH                                32
        #define RISCV_DBG_DM_DATA0_DATA_MASK                                 0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_DATA0_DATA_GET(value)                 (GAP_BEXTRACTU((value),32,0))
        #define RISCV_DBG_DM_DATA0_DATA_GETS(value)                (GAP_BEXTRACT((value),32,0))
        #define RISCV_DBG_DM_DATA0_DATA_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
        #define RISCV_DBG_DM_DATA0_DATA(val)                       ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :32; // Data value
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_data0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_data0 : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
        };

|

.. _riscv_dbg_dm_data1:

data1
"""""

Abstract Data 1

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    |31:0 |R/W|data|Data value |
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Abstract Data 1
                #define RISCV_DBG_DM_DATA1_OFFSET                0x5

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_data1_get(uint32_t base);
        static inline void riscv_dbg_dm_data1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data value (access: R/W)
        #define RISCV_DBG_DM_DATA1_DATA_BIT                                  0
        #define RISCV_DBG_DM_DATA1_DATA_WIDTH                                32
        #define RISCV_DBG_DM_DATA1_DATA_MASK                                 0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_DATA1_DATA_GET(value)                 (GAP_BEXTRACTU((value),32,0))
        #define RISCV_DBG_DM_DATA1_DATA_GETS(value)                (GAP_BEXTRACT((value),32,0))
        #define RISCV_DBG_DM_DATA1_DATA_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
        #define RISCV_DBG_DM_DATA1_DATA(val)                       ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :32; // Data value
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_data1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_data1 : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
        };

|

.. _riscv_dbg_dm_dmcontrol:

dmcontrol
"""""""""

Debug Module Control

.. table:: 

    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name      |                                                                                                                                                                                                                                                                                                                                                                      Description                                                                                                                                                                                                                                                                                                                                                                      |
    +=====+===+===============+=======================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |    0|R/W|dmactive       |This  bit  serves  as  a  reset  signal  for  the  DebugModule itself.0:  The  module’s  state,  including  authenticationmechanism, takes its reset values (thedmactivebitis the only bit which can be written to somethingother than its reset value).1:  The module functions normally.No other mechanism should exist that may resultin  resetting  the  Debug  Module  after  power  up,with the possible (but not recommended) excep-tion of a global reset signal that resets the entireplatform.A debugger may pulse this bit low to get the De-bug Module into a known state.Implementations may pay attention to this bit tofurther aid debugging, for example by preventingthe Debug Module from being power gated whiledebugging is active.|
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|ndmreset       |This bit controls the reset signal from the DM tothe  rest  of  the  system.   The  signal  should  resetevery  part  of  the  system,  including  every  hart,except for the DM and any logic required to accessthe DM. To perform a system reset the debuggerwrites 1, and then writes 0 to deassert the reset.                                                                                                                                                                                                                                                                                                                                                                                                                                      |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    2|R/W|clrresethaltreq|This optional field clears the halt-on-reset requestbit for all currently selected harts.Writes apply to the new value ofhartselandhasel.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    3|R/W|setresethaltreq|This  optional  field  writes  the  halt-on-reset  re-quest  bit  for  all  currently  selected  harts,  unlessclrresethaltreqis  simultaneously  set  to  1.   Whenset to 1, each selected hart will halt upon the nextdeassertion of its reset.  The halt-on-reset requestbit  is  not  automatically  cleared.   The  debuggermust write toclrresethaltreqto clear it.Writes apply to the new value ofhartselandhasel.Ifhasresethaltreqis  0,  this  field  is  not  imple-mented.                                                                                                                                                                                                                                                                  |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15:6 |R/W|hartselhi      |The high 10 bits ofhartsel:  the DM-specific indexof the hart to select.  This hart is always part ofthe currently selected harts.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |25:16|R/W|hartsello      |The low 10 bits ofhartsel:  the DM-specific indexof the hart to select.  This hart is always part ofthe currently selected harts.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   26|R/W|hasel          |Selects the definition of currently selected harts.0:  There is a single currently selected hart, thatis selected byhartsel.1:  There may be multiple currently selected harts– the hart selected byhartsel, plus those selectedby the hart array mask register.An implementation which does not implement thehart array mask register must tie this field to 0.A  debugger  which  wishes  to  use  the  hart  arraymask register feature should set this bit and readback to see if the functionality is supported.                                                                                                                                                                                                                                  |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   28|R/W|ackhavereset   |0:  No effect.1:  Clearshaveresetfor any selected harts.Writes apply to the new value ofhartselandhasel.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   29|R/W|hartreset      |This optional field writes the reset bit for all thecurrently selected harts.  To perform a reset thedebugger writes 1, and then writes 0 to deassertthe reset signal.While this bit is 1, the debugger must not changewhich harts are selected.If this feature is not implemented, the bit alwaysstays 0, so after writing 1 the debugger can readthe register back to see if the feature is supported.Writes apply to the new value ofhartselandhasel.                                                                                                                                                                                                                                                                                               |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   30|R/W|resumereq      |Writing  1  causes  the  currently  selected  harts  toresume  once,  if  they  are  halted  when  the  writeoccurs.  It also clears the resume ack bit for thoseharts.resumereqis ignored ifhaltreqis set.Writes apply to the new value ofhartselandhasel.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   31|R/W|haltreq        |Writing 0 clears the halt request bit for all cur-rently selected harts.  This may cancel outstand-ing halt requests for those harts.Writing 1 sets the halt request bit for all currentlyselected harts.  Running harts will halt whenevertheir halt request bit is set.Writes apply to the new value ofhartselandhasel.                                                                                                                                                                                                                                                                                                                                                                                                                              |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Debug Module Control
                #define RISCV_DBG_DM_DMCONTROL_OFFSET            0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_dmcontrol_get(uint32_t base);
        static inline void riscv_dbg_dm_dmcontrol_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_dmcontrol : public vp::reg_32
        {
        public:
            inline void dmactive_set(uint32_t value);
            inline uint32_t dmactive_get();
            inline void ndmreset_set(uint32_t value);
            inline uint32_t ndmreset_get();
            inline void clrresethaltreq_set(uint32_t value);
            inline uint32_t clrresethaltreq_get();
            inline void setresethaltreq_set(uint32_t value);
            inline uint32_t setresethaltreq_get();
            inline void hartselhi_set(uint32_t value);
            inline uint32_t hartselhi_get();
            inline void hartsello_set(uint32_t value);
            inline uint32_t hartsello_get();
            inline void hasel_set(uint32_t value);
            inline uint32_t hasel_get();
            inline void ackhavereset_set(uint32_t value);
            inline uint32_t ackhavereset_get();
            inline void hartreset_set(uint32_t value);
            inline uint32_t hartreset_get();
            inline void resumereq_set(uint32_t value);
            inline uint32_t resumereq_get();
            inline void haltreq_set(uint32_t value);
            inline uint32_t haltreq_get();
        };

|

.. _riscv_dbg_dm_dmstatus:

dmstatus
""""""""

Debug Module Status

.. table:: 

    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name      |                                                                                                                                            Description                                                                                                                                            |
    +=====+===+===============+===================================================================================================================================================================================================================================================================================================+
    |3:0  |R  |version        |0:  There is no Debug Module present.1:  There is a Debug Module and it conforms toversion 0.11 of this specification.2:  There is a Debug Module and it conforms toversion 0.13 of this specification.15:  There is a Debug Module but it does not con-form to any available version of this spec.|
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |4    |R  |confstrptrvalid|0:confstrptr0–confstrptr3hold  informationwhich is not relevant to the configuration string.1:confstrptr0–confstrptr3hold  the  addressof the configuration string.                                                                                                                                |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |5    |R  |hasresethaltreq|1  if  this  Debug  Module  supports  halt-on-resetfunctionality  controllable  by  thesetresethaltreqandclrresethaltreqbits.  0 otherwise.                                                                                                                                                        |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |6    |R  |authbusy       |0:  The authentication module is ready to processthe next read/write toauthdata.1:  The authentication module is busy.  Accessingauthdataresults in unspecified behavior.authbusyonly becomes set in immediate responseto an access toauthdata.                                                    |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |7    |R  |authenticated  |0:   Authentication  is  required  before  using  theDM.1:  The authentication check has passed.On components that don’t implement authentica-tion, this bit must be preset as 1.                                                                                                                  |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |8    |R  |anyhalted      |This field is 1 when any currently selected hart ishalted.                                                                                                                                                                                                                                         |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9    |R  |allhalted      |This  field  is  1  when  all  currently  selected  hartsare halted.                                                                                                                                                                                                                               |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10   |R  |anyrunning     |This field is 1 when any currently selected hart isrunning.                                                                                                                                                                                                                                        |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |11   |R  |allrunning     |This  field  is  1  when  all  currently  selected  hartsare running.                                                                                                                                                                                                                              |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |12   |R  |anyunavail     |This field is 1 when any currently selected hart isunavailable.                                                                                                                                                                                                                                    |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |13   |R  |allunavail     |This  field  is  1  when  all  currently  selected  hartsare unavailable.                                                                                                                                                                                                                          |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |14   |R  |anynonexistent |This  field  is  1  when  any  currently  selected  hartdoes not exist in this platform.                                                                                                                                                                                                           |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15   |R  |allnonexistent |This field is 1 when all currently selected harts donot exist in this platform.                                                                                                                                                                                                                    |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |16   |R  |anyresumeack   |This  field  is  1  when  any  currently  selected  harthas acknowledged its last resume request.                                                                                                                                                                                                  |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |17   |R  |allresumeack   |This  field  is  1  when  all  currently  selected  hartshave acknowledged their last resume request.                                                                                                                                                                                              |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18   |R  |anyhavereset   |This field is 1 when at least one currently selectedhart  has  been  reset  and  reset  has  not  been  ac-knowledged for that hart.                                                                                                                                                               |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |19   |R  |allhavereset   |This  field  is  1  when  all  currently  selected  hartshave been reset and reset has not been acknowl-edged for any of them.                                                                                                                                                                     |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |22   |R  |impebreak      |If 1, then there is an implicitebreakinstructionat  the  non-existent  word  immediately  after  theProgram  Buffer.   This  saves  the  debugger  fromhaving to write theebreakitself, and allows theProgram Buffer to be one word smaller.This must be 1 whenprogbufsizeis 1.                    |
    +-----+---+---------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Debug Module Status
                #define RISCV_DBG_DM_DMSTATUS_OFFSET             0x11

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_dmstatus_get(uint32_t base);
        static inline void riscv_dbg_dm_dmstatus_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_dmstatus : public vp::reg_32
        {
        public:
            inline void version_set(uint32_t value);
            inline uint32_t version_get();
            inline void confstrptrvalid_set(uint32_t value);
            inline uint32_t confstrptrvalid_get();
            inline void hasresethaltreq_set(uint32_t value);
            inline uint32_t hasresethaltreq_get();
            inline void authbusy_set(uint32_t value);
            inline uint32_t authbusy_get();
            inline void authenticated_set(uint32_t value);
            inline uint32_t authenticated_get();
            inline void anyhalted_set(uint32_t value);
            inline uint32_t anyhalted_get();
            inline void allhalted_set(uint32_t value);
            inline uint32_t allhalted_get();
            inline void anyrunning_set(uint32_t value);
            inline uint32_t anyrunning_get();
            inline void allrunning_set(uint32_t value);
            inline uint32_t allrunning_get();
            inline void anyunavail_set(uint32_t value);
            inline uint32_t anyunavail_get();
            inline void allunavail_set(uint32_t value);
            inline uint32_t allunavail_get();
            inline void anynonexistent_set(uint32_t value);
            inline uint32_t anynonexistent_get();
            inline void allnonexistent_set(uint32_t value);
            inline uint32_t allnonexistent_get();
            inline void anyresumeack_set(uint32_t value);
            inline uint32_t anyresumeack_get();
            inline void allresumeack_set(uint32_t value);
            inline uint32_t allresumeack_get();
            inline void anyhavereset_set(uint32_t value);
            inline uint32_t anyhavereset_get();
            inline void allhavereset_set(uint32_t value);
            inline uint32_t allhavereset_get();
            inline void impebreak_set(uint32_t value);
            inline uint32_t impebreak_get();
        };

|

.. _riscv_dbg_dm_hartinfo:

hartinfo
""""""""

Hart Info

.. table:: 

    +-----+---+----------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                                                                                                                  Description                                                                                                                                   |
    +=====+===+==========+================================================================================================================================================================================================================================================================================+
    |11:0 |R  |dataaddr  |Ifdataaccessis  0:  The number of  the  first  CSRdedicated to shadowing thedataregisters.Ifdataaccessis 1:  Signed address of RAM wherethedataregisters  are  shadowed,  to  be  used  toaccess relative tozero.                                                               |
    +-----+---+----------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15:12|R  |datasize  |Ifdataaccessis 0:  Number of CSRs dedicated toshadowing thedataregisters.Ifdataaccessis 1:  Number of 32-bit words in thememory  map  dedicated  to  shadowing  thedataregisters.Since  there  are  at  most  12dataregisters,  thevalue in this register must be 12 or smaller.|
    +-----+---+----------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |16   |R  |dataaccess|0:  Thedataregisters  are  shadowed  in  the  hartby CSRs.  Each CSR is DXLEN bits in size, andcorresponds to a single argument, per Table 3.1.1:  Thedataregisters are shadowed in the hart’smemory map.  Each register takes up 4 bytes inthe memory map.                     |
    +-----+---+----------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |23:20|R  |nscratch  |Number  ofdscratchregisters  available  for  thedebugger to use during program buffer execution,starting fromdscratch0. The debugger can makeno assumptions about the contents of these regis-ters between commands.                                                            |
    +-----+---+----------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Hart Info
                #define RISCV_DBG_DM_HARTINFO_OFFSET             0x12

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_hartinfo_get(uint32_t base);
        static inline void riscv_dbg_dm_hartinfo_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_hartinfo : public vp::reg_32
        {
        public:
            inline void dataaddr_set(uint32_t value);
            inline uint32_t dataaddr_get();
            inline void datasize_set(uint32_t value);
            inline uint32_t datasize_get();
            inline void dataaccess_set(uint32_t value);
            inline uint32_t dataaccess_get();
            inline void nscratch_set(uint32_t value);
            inline uint32_t nscratch_get();
        };

|

.. _riscv_dbg_dm_abstractcs:

abstractcs
""""""""""

Abstract Control and Status

.. table:: 

    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       |
    +=====+===+===========+=========================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |3:0  |R  |datacount  |Number  ofdataregisters  that  are  implementedas part of the abstract command interface.  Validsizes are 1 – 12.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |
    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10:8 |R/W|cmderr     |Gets set if an abstract command fails.  The bits inthis field remain set until they are cleared by writ-ing 1 to them.  No abstract command is starteduntil the value is reset to 0.This field only contains a valid value ifbusyis 0.0 (none):  No error.1  (busy):   An  abstract  command  was  executingwhilecommand,abstractcs,  orabstractautowas written, or when one of thedataorprogbufregisters was read or written.  This status is onlywritten ifcmderrcontains 0.2 (not supported): The requested command is notsupported, regardless of whether the hart is run-ning or not.3  (exception):  An  exception  occurred  while  ex-ecuting  the  command  (e.g.  while  executing  theProgram Buffer).4 (halt/resume):  The abstract command couldn’texecute  because  the  hart  wasn’t  in  the  requiredstate (running/halted), or unavailable.5  (bus):  The  abstract  command  failed  due  to  abus error (e.g. alignment, access size, or timeout).7 (other):  The command failed for another rea-son.|
    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |12   |R  |busy       |1:  An abstract command is currently being exe-cuted.This bit is set as soon ascommandis written, andis not cleared until that command has completed.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |
    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |28:24|R  |progbufsize|Size of the Program Buffer, in 32-bit words.  Validsizes are 0 - 16.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Abstract Control and Status
                #define RISCV_DBG_DM_ABSTRACTCS_OFFSET           0x16

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_abstractcs_get(uint32_t base);
        static inline void riscv_dbg_dm_abstractcs_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_abstractcs : public vp::reg_32
        {
        public:
            inline void datacount_set(uint32_t value);
            inline uint32_t datacount_get();
            inline void cmderr_set(uint32_t value);
            inline uint32_t cmderr_get();
            inline void busy_set(uint32_t value);
            inline uint32_t busy_get();
            inline void progbufsize_set(uint32_t value);
            inline uint32_t progbufsize_get();
        };

|

.. _riscv_dbg_dm_command:

command
"""""""

Abstract Command

.. table:: 

    +-----+---+-------+-------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                           Description                                           |
    +=====+===+=======+=================================================================================================+
    |23:0 |R/W|control|This  field  is  interpreted  in  a  command-specificmanner, described for each abstract command.|
    +-----+---+-------+-------------------------------------------------------------------------------------------------+
    |31:24|R/W|cmdtype|The type determines the overall functionality ofthis abstract command.                           |
    +-----+---+-------+-------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Abstract Command
                #define RISCV_DBG_DM_COMMAND_OFFSET              0x17

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_command_get(uint32_t base);
        static inline void riscv_dbg_dm_command_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // This  field  is  interpreted  in  a  command-specificmanner, described for each abstract command. (access: R/W)
        #define RISCV_DBG_DM_COMMAND_CONTROL_BIT                             0
        #define RISCV_DBG_DM_COMMAND_CONTROL_WIDTH                           24
        #define RISCV_DBG_DM_COMMAND_CONTROL_MASK                            0xffffff
        
        // The type determines the overall functionality ofthis abstract command. (access: R/W)
        #define RISCV_DBG_DM_COMMAND_CMDTYPE_BIT                             24
        #define RISCV_DBG_DM_COMMAND_CMDTYPE_WIDTH                           8
        #define RISCV_DBG_DM_COMMAND_CMDTYPE_MASK                            0xff000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_COMMAND_CONTROL_GET(value)            (GAP_BEXTRACTU((value),24,0))
        #define RISCV_DBG_DM_COMMAND_CONTROL_GETS(value)           (GAP_BEXTRACT((value),24,0))
        #define RISCV_DBG_DM_COMMAND_CONTROL_SET(value,field)      (GAP_BINSERT((value),(field),24,0))
        #define RISCV_DBG_DM_COMMAND_CONTROL(val)                  ((val) << 0)
        
        #define RISCV_DBG_DM_COMMAND_CMDTYPE_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define RISCV_DBG_DM_COMMAND_CMDTYPE_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define RISCV_DBG_DM_COMMAND_CMDTYPE_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define RISCV_DBG_DM_COMMAND_CMDTYPE(val)                  ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int control         :24; // This  field  is  interpreted  in  a  command-specificmanner, described for each abstract command.
            unsigned int cmdtype         :8 ; // The type determines the overall functionality ofthis abstract command.
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_command_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_command : public vp::reg_32
        {
        public:
            inline void control_set(uint32_t value);
            inline uint32_t control_get();
            inline void cmdtype_set(uint32_t value);
            inline uint32_t cmdtype_get();
        };

|

.. _riscv_dbg_dm_sbcs:

sbcs
""""

System Bus Access Control and Status

.. table:: 

    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name      |                                                                                                                                                                                                                                                          Description                                                                                                                                                                                                                                                          |
    +=====+===+===============+===============================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |    0|R  |sbaccess8      |1 when 8-bit system bus accesses are supported.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R  |sbaccess16     |1 when 16-bit system bus accesses are supported.                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    2|R  |sbaccess32     |1 when 32-bit system bus accesses are supported.                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    3|R  |sbaccess64     |1 when 64-bit system bus accesses are supported.                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R  |sbaccess128    |1 when 128-bit system bus accesses are supported.                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |11:5 |R  |sbasize        |Width  of  system  bus  addresses  in  bits.   (0  indi-cates there is no bus access support.)                                                                                                                                                                                                                                                                                                                                                                                                                                 |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |14:12|R/W|sberror        |When the Debug Module’s system bus master en-counters an error, this field gets set.  The bits inthis field remain set until they are cleared by writ-ing  1  to  them.   While  this  field  is  non-zero,  nomore system bus accesses can be initiated by theDebug Module.An  implementation  may  report  “Other”  (7)  forany error condition.0:  There was no bus error.1:  There was a timeout.2:  A bad address was accessed.3:  There was an alignment error.4:  An access of unsupported size was requested.7:  Other.|
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   15|R/W|sbreadondata   |When 1, every read fromsbdata0automaticallytriggers a system bus read at the (possibly auto-incremented) address.                                                                                                                                                                                                                                                                                                                                                                                                              |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   16|R/W|sbautoincrement|When 1,sbaddressis incremented by the accesssize (in bytes) selected insbaccessafter every sys-tem bus access.                                                                                                                                                                                                                                                                                                                                                                                                                 |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |19:17|R/W|sbaccess       |Select  the  access  size  to  use  for  system  bus  ac-cesses.0:  8-bit1:  16-bit2:  32-bit3:  64-bit4:  128-bitIfsbaccesshas  an  unsupported  value  when  theDM  starts  a  bus  access,  the  access  is  not  per-formed andsberroris set to 4.                                                                                                                                                                                                                                                                         |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   20|R/W|sbreadonaddr   |When 1, every write tosbaddress0automaticallytriggers a system bus read at the new address.                                                                                                                                                                                                                                                                                                                                                                                                                                    |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   21|R  |sbbusy         |When 1, indicates the system bus master is busy.(Whether the system bus itself is busy is related,but not the same thing.)  This bit goes high im-mediately  when  a  read  or  write  is  requested  forany reason, and does not go low until the accessis fully completed.Writes tosbcswhilesbbusyis high result in un-defined behavior.  A debugger must not write tosbcsuntil it readssbbusyas 0.                                                                                                                          |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |27:22|R/W|sbbusyerror    |Set  when  the  debugger  attempts  to  read  datawhile  a  read  is  in  progress,  or  when  the  debug-ger initiates a new access while one is already inprogress (whilesbbusyis set).  It remains set untilit’s explicitly cleared by the debugger.While this field is set, no more system bus accessescan be initiated by the Debug Module.                                                                                                                                                                               |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:29|R  |sbversion      |0: The System Bus interface conforms to mainlinedrafts of this spec older than 1 January, 2018.1: The System Bus interface conforms to this ver-sion of the spec.Other values are reserved for future versions.                                                                                                                                                                                                                                                                                                                |
    +-----+---+---------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // System Bus Access Control and Status
                #define RISCV_DBG_DM_SBCS_OFFSET                 0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_sbcs_get(uint32_t base);
        static inline void riscv_dbg_dm_sbcs_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_sbcs : public vp::reg_32
        {
        public:
            inline void sbaccess8_set(uint32_t value);
            inline uint32_t sbaccess8_get();
            inline void sbaccess16_set(uint32_t value);
            inline uint32_t sbaccess16_get();
            inline void sbaccess32_set(uint32_t value);
            inline uint32_t sbaccess32_get();
            inline void sbaccess64_set(uint32_t value);
            inline uint32_t sbaccess64_get();
            inline void sbaccess128_set(uint32_t value);
            inline uint32_t sbaccess128_get();
            inline void sbasize_set(uint32_t value);
            inline uint32_t sbasize_get();
            inline void sberror_set(uint32_t value);
            inline uint32_t sberror_get();
            inline void sbreadondata_set(uint32_t value);
            inline uint32_t sbreadondata_get();
            inline void sbautoincrement_set(uint32_t value);
            inline uint32_t sbautoincrement_get();
            inline void sbaccess_set(uint32_t value);
            inline uint32_t sbaccess_get();
            inline void sbreadonaddr_set(uint32_t value);
            inline uint32_t sbreadonaddr_get();
            inline void sbbusy_set(uint32_t value);
            inline uint32_t sbbusy_get();
            inline void sbbusyerror_set(uint32_t value);
            inline uint32_t sbbusyerror_get();
            inline void sbversion_set(uint32_t value);
            inline uint32_t sbversion_get();
        };

|

.. _riscv_dbg_dm_progbuf0:

progbuf0
""""""""

Program Buffer 0

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    |31:0 |R/W|data|Data value |
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Program Buffer 0
                #define RISCV_DBG_DM_PROGBUF0_OFFSET             0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_progbuf0_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data value (access: R/W)
        #define RISCV_DBG_DM_PROGBUF0_DATA_BIT                               0
        #define RISCV_DBG_DM_PROGBUF0_DATA_WIDTH                             32
        #define RISCV_DBG_DM_PROGBUF0_DATA_MASK                              0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_PROGBUF0_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define RISCV_DBG_DM_PROGBUF0_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define RISCV_DBG_DM_PROGBUF0_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define RISCV_DBG_DM_PROGBUF0_DATA(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :32; // Data value
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_progbuf0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_progbuf0 : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
        };

|

.. _riscv_dbg_dm_progbuf1:

progbuf1
""""""""

Program Buffer 1

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    |31:0 |R/W|data|Data value |
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Program Buffer 1
                #define RISCV_DBG_DM_PROGBUF1_OFFSET             0x21

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_progbuf1_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data value (access: R/W)
        #define RISCV_DBG_DM_PROGBUF1_DATA_BIT                               0
        #define RISCV_DBG_DM_PROGBUF1_DATA_WIDTH                             32
        #define RISCV_DBG_DM_PROGBUF1_DATA_MASK                              0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_PROGBUF1_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define RISCV_DBG_DM_PROGBUF1_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define RISCV_DBG_DM_PROGBUF1_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define RISCV_DBG_DM_PROGBUF1_DATA(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :32; // Data value
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_progbuf1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_progbuf1 : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
        };

|

.. _riscv_dbg_dm_progbuf2:

progbuf2
""""""""

Program Buffer 2

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    |31:0 |R/W|data|Data value |
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Program Buffer 2
                #define RISCV_DBG_DM_PROGBUF2_OFFSET             0x22

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_progbuf2_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data value (access: R/W)
        #define RISCV_DBG_DM_PROGBUF2_DATA_BIT                               0
        #define RISCV_DBG_DM_PROGBUF2_DATA_WIDTH                             32
        #define RISCV_DBG_DM_PROGBUF2_DATA_MASK                              0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_PROGBUF2_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define RISCV_DBG_DM_PROGBUF2_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define RISCV_DBG_DM_PROGBUF2_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define RISCV_DBG_DM_PROGBUF2_DATA(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :32; // Data value
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_progbuf2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_progbuf2 : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
        };

|

.. _riscv_dbg_dm_progbuf3:

progbuf3
""""""""

Program Buffer 3

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    |31:0 |R/W|data|Data value |
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Program Buffer 3
                #define RISCV_DBG_DM_PROGBUF3_OFFSET             0x23

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_progbuf3_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data value (access: R/W)
        #define RISCV_DBG_DM_PROGBUF3_DATA_BIT                               0
        #define RISCV_DBG_DM_PROGBUF3_DATA_WIDTH                             32
        #define RISCV_DBG_DM_PROGBUF3_DATA_MASK                              0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_PROGBUF3_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define RISCV_DBG_DM_PROGBUF3_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define RISCV_DBG_DM_PROGBUF3_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define RISCV_DBG_DM_PROGBUF3_DATA(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :32; // Data value
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_progbuf3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_progbuf3 : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
        };

|

.. _riscv_dbg_dm_progbuf4:

progbuf4
""""""""

Program Buffer 4

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    |31:0 |R/W|data|Data value |
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Program Buffer 4
                #define RISCV_DBG_DM_PROGBUF4_OFFSET             0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_progbuf4_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data value (access: R/W)
        #define RISCV_DBG_DM_PROGBUF4_DATA_BIT                               0
        #define RISCV_DBG_DM_PROGBUF4_DATA_WIDTH                             32
        #define RISCV_DBG_DM_PROGBUF4_DATA_MASK                              0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_PROGBUF4_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define RISCV_DBG_DM_PROGBUF4_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define RISCV_DBG_DM_PROGBUF4_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define RISCV_DBG_DM_PROGBUF4_DATA(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :32; // Data value
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_progbuf4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_progbuf4 : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
        };

|

.. _riscv_dbg_dm_progbuf5:

progbuf5
""""""""

Program Buffer 5

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    |31:0 |R/W|data|Data value |
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Program Buffer 5
                #define RISCV_DBG_DM_PROGBUF5_OFFSET             0x25

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_progbuf5_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data value (access: R/W)
        #define RISCV_DBG_DM_PROGBUF5_DATA_BIT                               0
        #define RISCV_DBG_DM_PROGBUF5_DATA_WIDTH                             32
        #define RISCV_DBG_DM_PROGBUF5_DATA_MASK                              0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_PROGBUF5_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define RISCV_DBG_DM_PROGBUF5_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define RISCV_DBG_DM_PROGBUF5_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define RISCV_DBG_DM_PROGBUF5_DATA(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :32; // Data value
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_progbuf5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_progbuf5 : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
        };

|

.. _riscv_dbg_dm_progbuf6:

progbuf6
""""""""

Program Buffer 6

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    |31:0 |R/W|data|Data value |
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Program Buffer 6
                #define RISCV_DBG_DM_PROGBUF6_OFFSET             0x26

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_progbuf6_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data value (access: R/W)
        #define RISCV_DBG_DM_PROGBUF6_DATA_BIT                               0
        #define RISCV_DBG_DM_PROGBUF6_DATA_WIDTH                             32
        #define RISCV_DBG_DM_PROGBUF6_DATA_MASK                              0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_PROGBUF6_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define RISCV_DBG_DM_PROGBUF6_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define RISCV_DBG_DM_PROGBUF6_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define RISCV_DBG_DM_PROGBUF6_DATA(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :32; // Data value
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_progbuf6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_progbuf6 : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
        };

|

.. _riscv_dbg_dm_progbuf7:

progbuf7
""""""""

Program Buffer 7

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    |31:0 |R/W|data|Data value |
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Program Buffer 7
                #define RISCV_DBG_DM_PROGBUF7_OFFSET             0x27

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_progbuf7_get(uint32_t base);
        static inline void riscv_dbg_dm_progbuf7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data value (access: R/W)
        #define RISCV_DBG_DM_PROGBUF7_DATA_BIT                               0
        #define RISCV_DBG_DM_PROGBUF7_DATA_WIDTH                             32
        #define RISCV_DBG_DM_PROGBUF7_DATA_MASK                              0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_PROGBUF7_DATA_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define RISCV_DBG_DM_PROGBUF7_DATA_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define RISCV_DBG_DM_PROGBUF7_DATA_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define RISCV_DBG_DM_PROGBUF7_DATA(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :32; // Data value
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_progbuf7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_progbuf7 : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
        };

|

.. _riscv_dbg_dm_sbaddress0:

sbaddress0
""""""""""

System Bus Address 31:0

.. table:: 

    +-----+---+-------+-----------+
    |Bit #|R/W| Name  |Description|
    +=====+===+=======+===========+
    |31:0 |R/W|address|Address    |
    +-----+---+-------+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // System Bus Address 31:0
                #define RISCV_DBG_DM_SBADDRESS0_OFFSET           0x39

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_sbaddress0_get(uint32_t base);
        static inline void riscv_dbg_dm_sbaddress0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Address (access: R/W)
        #define RISCV_DBG_DM_SBADDRESS0_ADDRESS_BIT                          0
        #define RISCV_DBG_DM_SBADDRESS0_ADDRESS_WIDTH                        32
        #define RISCV_DBG_DM_SBADDRESS0_ADDRESS_MASK                         0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_SBADDRESS0_ADDRESS_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define RISCV_DBG_DM_SBADDRESS0_ADDRESS_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define RISCV_DBG_DM_SBADDRESS0_ADDRESS_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define RISCV_DBG_DM_SBADDRESS0_ADDRESS(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int address         :32; // Address
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_sbaddress0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_sbaddress0 : public vp::reg_32
        {
        public:
            inline void address_set(uint32_t value);
            inline uint32_t address_get();
        };

|

.. _riscv_dbg_dm_dmcs2:

dmcs2
"""""

Debug Module Control and Status 2

.. table:: 

    +-----+---+------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name    |                                                                                                                                                                                                                                                                       Description                                                                                                                                                                                                                                                                        |
    +=====+===+============+==========================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |    0|R/W|hgselect    |0:  Operate on harts.1:  Operate on DM external triggers.If  there  are  no  DM  external  triggers,  this  fieldmust be tied to 0.                                                                                                                                                                                                                                                                                                                                                                                                                       |
    +-----+---+------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|hgwrite     |Whenhgselectis 0, writing 1 changes the groupof all selected harts to the value written togroup.When 1 is written andhgselectis 0, for every se-lected hart the DM will change its group to thevalue written togroup, if the hardware supportsthat group for that hart.When  1  is  written  andhgselectis  1,  the  DMwill  change  the  group  of  the  DM  external  trig-ger selected bydmexttriggerto the value writtentogroup, if the hardware supports that group forthat trigger.Writing 0 has no effect.                                         |
    +-----+---+------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |6:2  |R/W|group       |Whenhgselectis 0, contains the group of the hartspecified byhartsel.Whenhgselectis 1, contains the group of the DMexternal trigger selected bydmexttrigger.Writes only have an effect ifhgwriteis also written1.Group  numbers  are  contiguous  starting  at  0,with  the  highest  number  being  implementation-dependent, and possibly different between differ-ent group types.  Debuggers should read back thisfield after writing to confirm they are using a hartgroup that is supported.If groups aren’t implemented, then this entire fieldis 0.|
    +-----+---+------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10:7 |R/W|dmexttrigger|This field contains the currently selected DM ex-ternal trigger.If a non-existent trigger value is written here, thehardware will change it to a valid one or 0 if noDM external triggers exist.                                                                                                                                                                                                                                                                                                                                                          |
    +-----+---+------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   11|R/W|grouptype   |0:  The remaining fields in this register configurehalt groups.1:  The remaining fields in this register configureresume groups.                                                                                                                                                                                                                                                                                                                                                                                                                          |
    +-----+---+------------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Debug Module Control and Status 2
                #define RISCV_DBG_DM_DMCS2_OFFSET                0x32

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_dmcs2_get(uint32_t base);
        static inline void riscv_dbg_dm_dmcs2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
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

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_dmcs2 : public vp::reg_32
        {
        public:
            inline void hgselect_set(uint32_t value);
            inline uint32_t hgselect_get();
            inline void hgwrite_set(uint32_t value);
            inline uint32_t hgwrite_get();
            inline void group_set(uint32_t value);
            inline uint32_t group_get();
            inline void dmexttrigger_set(uint32_t value);
            inline uint32_t dmexttrigger_get();
            inline void grouptype_set(uint32_t value);
            inline uint32_t grouptype_get();
        };

|

.. _riscv_dbg_dm_sbdata0:

sbdata0
"""""""

System Bus data 31:0

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    |31:0 |R/W|data|Data value |
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // System Bus data 31:0
                #define RISCV_DBG_DM_SBDATA0_OFFSET              0x3c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t riscv_dbg_dm_sbdata0_get(uint32_t base);
        static inline void riscv_dbg_dm_sbdata0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data value (access: R/W)
        #define RISCV_DBG_DM_SBDATA0_DATA_BIT                                0
        #define RISCV_DBG_DM_SBDATA0_DATA_WIDTH                              32
        #define RISCV_DBG_DM_SBDATA0_DATA_MASK                               0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RISCV_DBG_DM_SBDATA0_DATA_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define RISCV_DBG_DM_SBDATA0_DATA_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define RISCV_DBG_DM_SBDATA0_DATA_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define RISCV_DBG_DM_SBDATA0_DATA(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :32; // Data value
          };
          unsigned int raw;
        } __attribute__((packed)) riscv_dbg_dm_sbdata0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_riscv_dbg_dm_sbdata0 : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
        };

|
