# boot mode is shifted by 1 to the left
set BOOT_MODE_JTAG 8
set HI_SPEED_MODE 1

set _WORKAREASIZE 0xFFFFFFFF
adapter_khz 1500

interface ftdi
transport select jtag

set _CHIPNAME gap8

#both trst and srst are present, they work normaly and srst does not gate jtag
reset_config trst_and_srst separate srst_nogate
#trst_and_srst 

# Gap8 advanced debug interface JTAG TAP ID
set _CPUTAPID  0x149511C3

jtag newtap $_CHIPNAME cpu -irlen 4 -expected-id $_CPUTAPID

set _TARGETNAME $_CHIPNAME.cpu
target create $_TARGETNAME gap8 -endian little -chain-position $_TARGETNAME


# If ADBG_USE_HISPEED is set (options bit 1), status bits will be skipped
# on burst reads and writes to improve download speeds.
# This option must match the RTL configured option.

# Select core 0 + configure all memory
core_select 0
$_TARGETNAME configure -work-area-phys 0x0 -work-area-size $_WORKAREASIZE -work-area-backup 0

# Select the TAP core we are using --> gap8 version of adv dbg itf
tap_select gap8_adv_debug_itf
# Select the debug unit core we are using. This debug unit as an option.

set ADV_DBG_UNIT_OPTS $BOOT_MODE_JTAG|$HI_SPEED_MODE
du_select adv_dbg_unit [expr $ADV_DBG_UNIT_OPTS]

init
