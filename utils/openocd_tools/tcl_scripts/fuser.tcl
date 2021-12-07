# LICENCE: GPLv2 (see COPYING)
# Copyright Greenwaves Technologies 2019
# Author: Antoine Faravelon (antoine.faravelon@greenwaves-technologies.com)

#typedef struct
#{
#	int fuser_ver_maj ;         +0
#	int fuser_ver_min ;         +4
#	int bridge_to_gap ;         +8
#	int gap_to_bridge ;         +12
#	int command ;               +16
#	int bit_pos ;               +20
#	int bit_len ;               +24
#	int status ;                +28
#	int log_level ;             +32
#	char buffer[EFUSE_SIZE] ;   +36
#} t_synchro ;

# gap_fuse_terminate: signal gap side fuser to exit
proc gap_fuse_terminate {device_struct_ptr_addr} {
	mem2array device_struct_ptr 32 $device_struct_ptr_addr 1
	set bridge_to_gap   [expr $device_struct_ptr(0)  +8 ]
	puts "signal gap to exit "
	mww [expr $bridge_to_gap] 0x2
}

# gap_fuse_open: wait for gap_to_bridge rdy signal
proc gap_fuse_open {device_struct_ptr_addr} {
	mem2array device_struct_ptr 32 $device_struct_ptr_addr 1
	set fuser_ver_maj   [expr $device_struct_ptr(0)  +0 ]
	set fuser_ver_min   [expr $device_struct_ptr(0)  +4 ]
	set gap_to_bridge   [expr $device_struct_ptr(0)  +12]
	puts "Wait for gap init"
	mem2array gap_to_bridge_r 32 $gap_to_bridge 1
	while { [expr $gap_to_bridge_r(0) != 1] } {
		sleep 1
		mem2array gap_to_bridge_r 32 $gap_to_bridge 1
	}
}

# gap fuser once: fuse bits according to buffer at bit_pos + (bit_len -1)
# Does not close the flasher on gap side
proc gap_fuse_once {device_struct_ptr_addr i_cmd i_bit_pos i_bit_len i_log_lvl i_buffer_len} {
	upvar fuse_array i_buffer
	# set pointers to right addresses
	mem2array device_struct_ptr 32 $device_struct_ptr_addr 1
	set fuser_ver_maj   [expr $device_struct_ptr(0)  +0 ]
	set fuser_ver_min   [expr $device_struct_ptr(0)  +4 ]
	set bridge_to_gap   [expr $device_struct_ptr(0)  +8 ]
	set gap_to_bridge   [expr $device_struct_ptr(0)  +12]
	set command         [expr $device_struct_ptr(0)  +16]
	set bit_pos         [expr $device_struct_ptr(0)  +20]
	set bit_len         [expr $device_struct_ptr(0)  +24]
	set status          [expr $device_struct_ptr(0)  +28]
	set log_lvl         [expr $device_struct_ptr(0)  +32]
	set buffer          [expr $device_struct_ptr(0)  +36]
	# rest synchronisation vars
	# GAP RDY  <--- 0
	# HOST RDY <--- 1 / signal to begin app
	# write fuse array if write command --> write in words, be careful
	mww [expr $bit_pos] [expr $i_bit_pos]
	mww [expr $bit_len] [expr $i_bit_len]
	mww [expr $log_lvl] [expr $i_log_lvl]
	mww [expr $command] [expr $i_cmd    ]

	if {$i_cmd == 0x1} {
		for { set i 0}  {$i < $i_buffer_len} {incr i} {
		#puts "writting $i_buffer($i) at address [expr $buffer + $i*4]"
			mww [expr $buffer + $i*4] [expr $i_buffer($i)]
		} 
	}

	puts "signal gap that setup is done"
	mww [expr $bridge_to_gap] 0x1

	puts "Wait for fuser to finish"
	mem2array gap_to_bridge_r 32 $gap_to_bridge 1
	while { [expr $gap_to_bridge_r(0) != 2] } {
		sleep 1
		mem2array gap_to_bridge_r 32 $gap_to_bridge 1
	}
	# read fuse array if read command --> read in words, be careful with size
	if {$i_cmd == 0x0} {
		mem2array i_buffer 32 [expr $buffer] [expr $i_buffer_len]
	}
	puts "fuse_once is done"
}

proc fuse_boot_from_flash {} {
	array set fuse_array {
		0 0x10
		1 0
		2 0
		3 0
		4 0
		5 0
		6 0
		7 0
		8 0
		9 0
		10 0
		11 0
		12 0
		13 0
		14 0
		15 0
		16 0
		17 0
		18 0
		19 0
		20 0
		21 0
		22 0
		23 0
		24 0
		25 0
		26 0
		27 0
		28 0
		29 0
		30 0
		31 0x0
	}
	gap_fuse_once 0x1c000190 0x1 0 8 0xf 32
}

proc fuse_flash_hyperflash {} {
	array set fuse_array {
		0 0x1
		1 0
		2 0
		3 0
		4 0
		5 0
		6 0
		7 0
		8 0
		9 0
		10 0
		11 0
		12 0
		13 0
		14 0
		15 0
		16 0
		17 0
		18 0
		19 0
		20 0
		21 0
		22 0
		23 0
		24 0
		25 0
		26 0
		27 0
		28 0
		29 0
		30 0
		31 0x0
	}
	gap_fuse_once 0x1c000190 0x1 296 8 0xf 32
}

proc fuse_hyperflash_boot {gap_tools_path} {
	gap8_jtag_load_binary_and_start ${gap_tools_path}/gap_bins/gap_fuser-gapoc_a.elf elf
	puts "${gap_tools_path}/gap_bins/gap_fuser-gapoc_a.elf"
	gap_fuse_open 0x1c000190
	fuse_boot_from_flash
	fuse_flash_hyperflash
	# now close the flasher
	gap_fuse_terminate 0x1c000190
}

proc fuse_spiflash_boot {gap_tools_path} {
	gap8_jtag_load_binary_and_start ${gap_tools_path}/gap_bins/gap_fuser.elf elf
	gap_fuse_open 0x1c000190
	fuse_boot_from_flash
	# now close the flasher
	gap_fuse_terminate 0x1c000190
}
