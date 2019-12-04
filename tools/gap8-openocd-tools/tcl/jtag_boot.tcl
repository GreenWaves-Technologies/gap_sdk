proc gap8_jtag_load_binary_and_start {file_name type} {
	puts "Loading binary through JTAG"
	# make sure the cpu is halted before loading
	halt
	# maybe asserting srst would be more safe here (uDMA)
	load_image $file_name 0x0 $type
	# Set PC at reset vector addr
	mww 0x1b302000 0x1c000080
	# unhalt the cpu
	resume
}

# Select the TAP core we are using --> gap8 version of adv dbg itf
tap_select gap8_adv_debug_itf
# Select the debug unit core we are using and boot mode

set ADV_DBG_UNIT_OPTS $BOOT_MODE_JTAG|$HI_SPEED_MODE
du_select adv_dbg_unit [expr $ADV_DBG_UNIT_OPTS]

init

# halt the core to avoid needless bootcode spin
halt
