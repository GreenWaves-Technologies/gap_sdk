# LICENCE: GPLv2 (see COPYING)
# Copyright Greenwaves Technologies 2019
# Author: Antoine Faravelon (antoine.faravelon@greenwaves-technologies.com)

# comm struct
#  ____________________
# |    Content  | Size |
# |------0------|------|
# | HOST RDY    | (4)  |
# |-----+4------|------|
# | GAP RDY     | (4)  |
# |-----+8------|------|
# | Buff ptr    | (4)  |
# |-----+12-----|------|
# | Buff Size   | (4)  |
# |-----+16-----|------| --- 
# | FLASH RUN   | (4)  | # for flasher only
# |-----+20-----|------|
# | FLASH_ADDR  | (4)  | 
# |-----+24-----|------|
# | FLASH_SIZE  | (4)  |
# |-----+28-----|------|
# | FLASH_TYPE  | (4)  |
# |_____________|______|

# Flash types:
# HYPERFLASH = 0
# SPI FLASH  = 1

# gap flasher ctrl: load a bin ImageName of size ImageSize to flash at addr 0x0+flash_offset
proc gap_dumper_ctrl {ImageName ImageSize flash_offset sector_size flash_type device_struct_ptr_addr} {
	# set pointers to right addresses
    set count [expr 0x0]
    mem2array device_struct_ptr 32 $device_struct_ptr_addr 1
    while { [expr (($device_struct_ptr(0) == 0xdeadbeef)\
            || ($device_struct_ptr(0) == 0x0)) && ($count < 0x80)] } {
        mem2array device_struct_ptr 32 $device_struct_ptr_addr 1
        sleep 100
        set count [expr $count + 0x1]
    }
    if { [expr $count == 0x80] } {
        puts "Dumper script could not connect to board, check your cables"
        exit
    }
    puts "device struct address is $device_struct_ptr(0)"
	set host_rdy 	    [expr $device_struct_ptr(0) + 0 ]
	set gap_rdy 	    [expr $device_struct_ptr(0) + 4 ]
	set buff_ptr_addr 	[expr $device_struct_ptr(0) + 8 ]
	set buff_size_addr 	[expr $device_struct_ptr(0) + 12 ]
	set flash_run 	    [expr $device_struct_ptr(0) + 16 ]
	set flash_addr 	    [expr $device_struct_ptr(0) + 20 ]
	set flash_size 	    [expr $device_struct_ptr(0) + 24 ]
    set flash_type_addr [expr $device_struct_ptr(0) + 28 ]
	set size            [expr $ImageSize]
	# rest synchronisation vars
	# GAP RDY  <--- 0
	mww [expr $gap_rdy] 0x0
	mww [expr $flash_type_addr] [expr $flash_type]
	# tell the chip we are going to flash
	mww [expr $flash_run] 0x1
	# HOST RDY <--- 1 / signal to begin app
	mww [expr $host_rdy] 0x1
	mem2array buff_ptr 32 $buff_ptr_addr 1
	set curr_offset [expr 0]
	set iter [expr 0]
	puts "going to wait on addr GAP_RDY"
	while { $size > 0 } {
	if { $size > $sector_size } {
            set curr_size [expr $sector_size]
            set size [expr $size - $sector_size]
        } else {
            set curr_size [expr $size]
            set size [expr 0]
        }
        # spin on gap rdy: wait for current flash write to finish
		mem2array wait1 32 $gap_rdy 1
		while { [expr $wait1(0) != 1] } {
			mem2array wait1 32 $gap_rdy 1
			sleep 1
		}
		puts "wait on gap_rdy done witg buff ptr $buff_ptr"
		mww [expr $host_rdy] 0x0
        if { $size == 0 } {
            mww [expr $flash_run] 0x0
        }

        mww [expr $flash_addr] $curr_offset
        mww [expr $flash_size] $curr_size
        # Shift addr to the left, and set the normal base addr as min to throw
        # away bin we already read
        puts "loading image with addr  $buff_ptr(0) and size $curr_size"
        dump_image ${ImageName}-${iter} $buff_ptr(0) $curr_size
        puts "load image done"
        set curr_offset [expr $curr_offset + $curr_size]
		#signal app we wrote our buff
        # ACK the gap rdy now that we wrote sector (flasher may work)
		mww [expr $gap_rdy] 0x0
        set iter [expr $iter + 0x1]
        #signal we are rdy when flasher is
        mww [expr $host_rdy] 0x1
	}
        # just ensure flasher app does not continue
	mem2array wait1 32 $flash_run 1
	while { [expr $wait1(0) != 1] } {
		mem2array wait1 32 $flash_run 1
		sleep 1
	}
	puts "flasher is done, exiting"
        mww [expr $gap_rdy]   0x0
        mww [expr $host_rdy]  0x0
}

proc gap_dump_raw {image_name image_size gap_tools_path} {
	# flash the flasher
	gap8_jtag_load_binary_and_start ${gap_tools_path}/gap_bins/gap_dumper@gapuino8.elf elf
	sleep 100
	# flash the flash image with the flasher
	gap_dumper_ctrl $image_name $image_size 0 0x40000 0 0x1c000090
	sleep 2
}
