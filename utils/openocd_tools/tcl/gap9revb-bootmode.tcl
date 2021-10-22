adapter_khz     500

adapter driver remote_bitbang
remote_bitbang_port 9999
remote_bitbang_host localhost

reset_config srst_only srst_nogate

set _CHIPNAME gap9

jtag newtap $_CHIPNAME riscv -irlen 5 -expected-id 0x20020bcb
jtag newtap $_CHIPNAME pulp -irlen 4 -expected-id 0x20021bcb

foreach t [jtag names] {
	puts [format "TAP: %s\n" $t]
}


set _TAP_RISCV $_CHIPNAME.riscv
set _TAP_PULP $_CHIPNAME.pulp
set _CL0 $_CHIPNAME.cl0
set _CL1 $_CHIPNAME.cl1
set _CL2 $_CHIPNAME.cl2
set _CL3 $_CHIPNAME.cl3
set _CL4 $_CHIPNAME.cl4
set _CL5 $_CHIPNAME.cl5
set _CL6 $_CHIPNAME.cl6
set _CL7 $_CHIPNAME.cl7
set _CL8 $_CHIPNAME.cl8
set _FC  $_CHIPNAME.fc

target create $_FC riscv -chain-position $_TAP_RISCV -coreid 0x9 
#target create $_CL0 riscv -chain-position $_TARGETNAME -coreid 0x0 -defer-examine
#target create $_CL1 riscv -chain-position $_TARGETNAME -coreid 0x1 -defer-examine
#target create $_CL2 riscv -chain-position $_TARGETNAME -coreid 0x2 -defer-examine
#target create $_CL3 riscv -chain-position $_TARGETNAME -coreid 0x3 -defer-examine
#target create $_CL4 riscv -chain-position $_TARGETNAME -coreid 0x4 -defer-examine
#target create $_CL5 riscv -chain-position $_TARGETNAME -coreid 0x5 -defer-examine
target create $_CL6 riscv -chain-position $_TAP_RISCV -coreid 0x6 -defer-examine
target create $_CL7 riscv -chain-position $_TAP_RISCV -coreid 0x7 -defer-examine
#target create $_CL8 riscv -chain-position $_TARGETNAME -coreid 0x8 -defer-examine
target smp $_CL6 $_CL7
#target create $_CL8 riscv -chain-position $_TARGETNAME -coreid 0x8 -gdb-port 6666 -defer-examine
#target create $_CL8 riscv -chain-position $_TARGETNAME -coreid 0x8 -gdb-port 6666


$_CL6 configure -rtos hwthread
$_CL7 configure -rtos hwthread

proc cl6_attach_proc { } {
    $::_CL6 arp_examine
    $::_CL7 arp_examine
    # since smp, this will halt all concerned code
    $::_CL6 arp_halt
    #$::_CL7 arp_halt
    $::_CL6 arm semihosting enable
    $::_CL7 arm semihosting enable
}
$_CL6 configure -event gdb-attach cl6_attach_proc

gdb_report_data_abort enable
gdb_report_register_access_error enable

riscv set_reset_timeout_sec 1440
riscv set_command_timeout_sec 1440

# prefer to use sba for system bus access
riscv set_prefer_sba on


proc poll_confreg { value } {
    irscan $::_TAP_PULP 0x6
    # size then value
    set ret [eval drscan $::_TAP_PULP 0x8 $value]
    puts "ret=$ret"
    while { !$ret } {
        irscan $::_TAP_PULP 0x6
        # size then value
        set ret [eval drscan $::_TAP_PULP 0x8 $value]
        puts "ret=$ret"
    }
}

proc jtag_init {} {
    puts "jtag init"
    targets $::_FC
    jtag_reset 0 1
    sleep 1
    jtag_reset 0 0
    sleep 1
    # wait for jtag ready
    poll_confreg 0x1
    echo "confreg polling done"
    jtag arp_init-reset
}

proc init_reset {mode} {
    puts "hello"
    targets $::_FC
    jtag_reset 0 1
    sleep 1
    jtag_reset 0 0
    sleep 1
    # wait for jtag ready
    poll_confreg 0x1
    echo "confreg polling done"
    jtag arp_init-reset
}

proc load_and_start_binary { elf_file pc_entry } {
    targets $::_FC
    # first ensure we are rest and halt so that pc is accessible
    #$::_FC arp_reset assert 1
    reset halt
    load_image ${elf_file} 0x0 elf
    reg pc ${pc_entry}
    resume
}


# dump jtag chain
#scan_chain

targets $_FC
init


#targets
#ftdi_set_signal nSRST 1
halt

$::_FC arm semihosting enable

echo "Ready for Remote Connections"
