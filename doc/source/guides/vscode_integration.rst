.. _guide_vscode_integration:

VSCode Integration
==================

.. warning::

    VSCode support is experimental. Use at your own risk.

SDK examples can be compiled, run and debug on boards from VSCode.

Prerequisites
-------------

To be able to debug your application, you will need to obtain a recent
GDB from the official RISC-V GNU toolchain.
You can find it here: https://github.com/riscv-collab/riscv-gnu-toolchain.
It is possible to either compile it following the README there or to download
a precompiled toolchain from the release page.
Once acquired, either by compiling or downloading, please install/unpack it
in a known location.

In this guide, it will be assumed that you used the XDG standard user directory
:file:`~/.local/bin` or another directory covered by your :code:`$PATH`
environment variable.

This way, you may access GDB using :code:`riscv64-unknown-elf-gdb` directly.
Also note that only GDB should be used from that toolchain, please continue to
use the toolchain provided by GreenWaves Technologies for code compilation as
otherwise you will encounter severe performance regressions.


Setup
-----
VSCode is using some files such as :file:`tasks.json` and :file:`launch.json`
to know how to build and run on the target.

Once the SDK is configured, you can generate a sample version of these files
from an example with:

.. code-block:: shell

    cd examples/pmsis/helloworld
    make vscode

Everything should be properly setup for building, run and debug the example on GAP EVK with the embedded ftdi chip.

For using your customer boards, other jtag probe or even you want to change to use another gdb toolchain, you may need to modify the following lines in the :file:`tasks.json` and the :file:`launch.json`.

For changing the gdb toolchain: file: `launch.json`

.. code-block:: none

    "miDebuggerPath": "<absolute path to your gdb>",

For changing the openocd, jtag probe: file: `tasks.json`

.. code-block:: none

    "command": "@OCD_install_path@ -c 'gdb_port 3333; telnet_port disabled; tcl_port disabled' -f '@OPENOCD_CABLE_CFG@' -f '<sdk_path>/utils/openocd_tools/tcl/gap9revb.tcl'"

@OCD_install_path@ : Specify your openocd path
@OPENOCD_CABLE_CFG@ : Specify your jtag probe config file path


Launch VSCode
-------------

From your example folder, execute ``code .``

You should see vscode with all the files from your example visible on the left
panel. You can now edit and save all the files.

Build the example
-----------------

Click on "Terminal"->"Run Task"->"Build from Makefile"->"Continue without
scanning the task output" to build your test.

Check any problem that occurs in the terminal window of vscode. If you don’t
see anything happening, you may have run vscode without the ``--log`` trace
option.


Launch OpenOCD
--------------

Click on "Terminal"->"Run Task"->"Openocd"->"Continue without scanning the task output"

You should see on the terminal window, OpenOCD being launched and connecting to
the target.

You can keep this terminal opened for several runs of your test, and you can
kill it with the trash icon if something goes really wrong or you want to close
it.

Once it's launched, you will see:

.. code-block:: none

    Open On-Chip Debugger 0.10.0+dev-00841-g1449af5bd (2021-07-02-17:05)
    Licensed under GNU GPL v2
    For bug reports, read
            http://openocd.org/doc/doxygen/bugs.html
    Info : auto-selecting first available session transport "jtag". To override use 'transport select <transport>'.
    TAP: gap9.riscv

    TAP: gap9.pulp

    Info : clock speed 1000 kHz
    jtag init
    ret1=00000000
    ret2=00000000
    ret1=80007A16
    ret=03
    INIT: confreg polling done
    Info : datacount=2 progbufsize=8
    Info : Examined RISC-V core; found 10 harts
    Info :  hart 0: currently disabled
    Info :  hart 1: currently disabled
    Info :  hart 2: currently disabled
    Info :  hart 3: currently disabled
    Info :  hart 4: currently disabled
    Info :  hart 5: currently disabled
    Info :  hart 6: currently disabled
    Info :  hart 7: currently disabled
    Info :  hart 8: currently disabled
    Info :  hart 9: XLEN=32, misa=0x40901124
    examine done
    Info : JTAG tap: gap9.riscv tap/device found: 0x20020bcb (mfg: 0x5e5 (<unknown>), part: 0x0020, ver: 0x2)
    Info : JTAG tap: gap9.pulp tap/device found: 0x20021bcb (mfg: 0x5e5 (<unknown>), part: 0x0021, ver: 0x2)
    Info : Listening on port 3333 for gdb connections
    Ready for Remote Connections
    Info : tcl server disabled
    Info : telnet server disabled


Launch the example
------------------

Click on "Run"->"Start Debugging" and then do normal vscode debug operations,
like running, stopping, adding breakpoints and so on.

All the printf from the application is visible on the openocd terminal.



