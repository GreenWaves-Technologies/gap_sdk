adapter_khz     5000
transport select jtag
# Channel 1 is taken by Xilinx JTAG
#reset_config srst_pulls_trst
reset_config trst_and_srst
#adapter_nsrst_assert_width 1000
#adapter_nsrst_delay 1000
#ftdi_tdo_sample_edge falling
set _CHIPNAME riscv
jtag newtap $_CHIPNAME cpu -irlen 5 -expected-id 0x00000001
jtag newtap $_CHIPNAME unknown0 -irlen 4 -expected-id 0x10102001
foreach t [jtag names] {
    puts [format "TAP: %s\n" $t]
}
set _TARGETNAME $_CHIPNAME.cpu

target create $_TARGETNAME riscv -chain-position $_TARGETNAME -coreid 0x3e0
gdb_report_data_abort enable
gdb_report_register_access_error enable

riscv set_reset_timeout_sec 120
riscv set_command_timeout_sec 120
# prefer to use sba for system bus access
riscv set_prefer_sba on
proc jtag_init {} {
    puts "----------- jtag init"
    # ensure chip reset done: this might not always be what we want
    jtag_reset 0 0
    sleep 1
    jtag_reset 0 1
    sleep 10
    jtag_reset 0 0
    sleep 1
    # ensure jtag reset is done
    pathmove RESET
    pathmove IDLE
    # "going to examine"
    #riscv.cpu arp_examine
    # "examination done"
    puts "----------- jtag init done"
}
proc init_reset {mode} {
    puts "----------- init reset"
    # ensure chip reset done: this might not always be what we want
    # ensure jtag reset is done
    jtag_reset 0 0
    sleep 1
    jtag_reset 0 1
    sleep 10
    jtag_reset 0 0
    sleep 20
    pathmove RESET
    pathmove IDLE
    # "going to examine"
    #riscv.cpu arp_examine
    # "examination done"
    #if { $mode == 0x1} {
    #    riscv.cpu arp_halt
    #}
}
proc load_and_start_binary { elf_file pc_entry } {
    puts "----------- load and start bin"
    # first ensure we are rest and halt so that pc is accessible
    riscv.cpu mww 0x1A100008 0x0fff1907
    riscv.cpu mww 0x1A100018 0x0fff1907
    riscv.cpu mww 0x1A100028 0x0fff1907
    riscv.cpu mww 0x1A100004 0xd0885f5e
    riscv.cpu mww 0x1A100014 0xd0885f5e
    riscv.cpu mww 0x1A100024 0xd0885f5e
    #reset halt
    load_image ${elf_file} 0x0 elf
    reg pc ${pc_entry}
    resume
}
# dump jtag chain
#scan_chain
#telnet_port 6666
init
reset halt
riscv.cpu arm semihosting enable
echo "Ready for Remote Connections"
