System traces
-------------

Description
...........

The virtual platform allows dumping architecture events to help developers debugging their applications by better showing what is happening in the system.

For example, it can show instructions being executed, DMA transfers, events generated, memory accesses and so on.

This feature can be enabled and configured through the option *\-\-trace*. This option takes an argument which specifies a regular expression of the path in the architecture where the traces must be enabled, and optionally a file where the traces should be dumped. All components whose path matches the specified one will dump traces. Several paths can be specified by using the option several times. Here is an example that activates instruction traces for core 0 and core 1: ::

  pulp-run --platform=gvsoc --config=gap_rev1 --binary=test prepare run --trace=pe0/insn --trace=pe1/insn"

The trace file should look like the following: ::

  194870000: 19487: [/sys/board/chip/soc/cluster/pe1/insn] M 1c001a96 c.li                a2, 0, 0           a2=00000000 
  194870000: 19487: [/sys/board/chip/soc/cluster/pe0/insn] M 1c001a2c beq                 a1, s4, 76         a1:00000020  s4:00000025

There is usually one line per event, although an event can sometimes takes several lines to display more information.

The number on the left gives the timestamp of the event, in picoseconds, and the one right after the number of cycles. Both are given because different blocks like clusters can have different frequencies. The timestamp is absolute and will increase linearly while the cycle count is local to the frequency domain.

The second part, which is a string, gives the path in the architecture where the event occurred. This is useful to differentiate blocks of the same kind that generate the same event. This path can also be used with the *\-\-trace* option to reduce the number of events.

The third part, which is also a string, is the information dumped by the event, and is totally specific to this event. In our example, the core simulator is just printing information about the instruction that has been executed.

Trace path
..........

One difficulty is usually to find out which paths should be activated to get the needed information. One method is to dump all the events with *\-\-trace=.**, then find out which one are interesting and then put them on the command line. Here are the paths for the main components (note that this can differ from one chip to another):

========================================= ===============================
Path                                      Description
========================================= ===============================
/sys/board/chip/cluster/pe0               Processing element, useful to see the IOs made by the core, and the instruction it executes. You can add */iss* to just get instruction events
/sys/board/chip/cluster/event_unit        Hardware synchronizer events, useful for debugging inter-core synchronization mechanisms
/sys/board/chip/cluster/pcache            Shared program cache accesses
/sys/board/chip/cluster/l1_ico            Shared L1 interconnect
/sys/board/chip/cluster/l1/bankX          L1 memory banks (the X should be replaced by the bank number)
/sys/board/chip/soc/l2                    L2 memory accesses
/sys/board/chip/cluster/dma               DMA events
========================================= ===============================

At first, the most interesting traces are the core instruction traces. As they show not only the instructions executed but also the registers accessed, their content and the memory accesses, they are very useful for debugging bugs like memory corruptions.

Instruction traces
..................

Here is an example of instruction trace: ::

  4890000: 489: [/sys/board/chip/soc/cluster/pe0/insn] M 1c001252 p.sw  0, 4(a5!)  a5=10000010  a5:1000000c  PA:1000000c

The event information dumped for executed instructions is using the following format: ::

  <address> <instruction> <operands> <operands info>

<address> is the address of the instruction.

<instruction> is the instruction label.

<operands> is the part of the decoded operands.

<operands info> is giving details about the operands values and how they are used.

The latter information is using the following convention:

  - When a register is accessed, its name is displayed followed by *=* if it is written or *:* if it is read. In case it is read and written, the register appears twice. It is followed by its value, which is the new one in case it is written.

  - When a memory access is done, *PA:* is displayed, followed by the address of the access.

  - The order of the statements is following the order on the decoded instruction

The memory accesses which are displayed are particularly interesting for tracking memory corruptions as they can be used to look for accesses to specific locations.

How to dump to a file
.....................

By default, all traces are dumped to the standard output and it is possible to specify the file where the traces should be dumped. The file must be given for every *\-\-trace* option. The same file can be used, to get all traces into the same file, or different files can be used.

Here is an example to get all possible traces into one file: ::

  make run PLT_OPT=--trace=.*:log.txt

And another example to get instruction traces to one file and L2 memory accesses to another file: ::

  make run PLT_OPT=--trace=insn:insn.txt --trace=l2:l2.txt
