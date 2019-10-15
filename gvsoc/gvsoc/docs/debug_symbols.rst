Debug symbols
-------------

Some features like instruction traces can use debug symbols to display more information. These features are by default enabled and can be disabled with the option *\-\-no-debug-syms*.

To have such features working, the binaries must be compile in debug mode so that debug symbols are present in the binaries and the virtual platform can generate debug symbols information.

The toolchain must be accessible for this option to work, either by making sure it is in accessible through environment variable PATH or by defining this environement variable: ::

  export PULP_RISCV_GCC_TOOLCHAIN=<path containing bin/riscv32-unknown-elf-readelf>

Once this works, the instruction trace should look like the following: ::

  9398037447: 466538: [/sys/board/chip/soc/fc/insn                         ] _get_next_timeout_expiry:167     M 1c001d7c sw                  ra, 28(sp)         ra:1c002154  sp:1b000db0  PA:1b000dcc

There is a column which displays the debug information. There are 2 information separated by *:*, the first one is the function which this instruction belongs to, and the second is the line number of the instruction in the source code.
