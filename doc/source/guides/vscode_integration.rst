.. _guide_vscode_integration:

VSCode Integration
==================

SDK examples can be compiled and run on boards from VSCode.

Setup
-----

The GDB version included in the GAP toolchain is too old to properly work with
the debug, you need to download and install the riscv toolchain and make sure
riscv64-unknown-elf-gdb is in the ``PATH``.

VSCode is using some files such as :file:`tasks.json` and :file:`launch.json`
to know how to build and run on the target.

Once the SDK is configured, you can generate a sample version of these files
from an example with:

.. code-block:: shell

    cd examples/pmsis/helloworld
    make vscode

Everything should be properly setup for building the example but you need to
modify a few files for the execution. For the build, you may just need to
modify the file ``.vscode/script/build.sh`` to put the proper platform on this
command:

.. code-block:: shell

    make all platform=fpga


Then modify :file:`.vscode/tasks.json` and modify the openocd command to put
the absolute path to the files and add any needed option:

.. code-block:: none

    "command": "<OCD install path>/openocd.exe -f <OCD install path>/scripts/tcl/interface/ftdi/olimex-arm-usb-ocd-h.cfg -f <SDK install path>/tools/gap8-openocd-tools/tcl/gap9revb.tcl"

And finally modify :file:`.vscode/launch.json` and modify the gdb command to
match your IP address and openocd gdb port:

.. code-block:: none

    "text": "target extended-remote <IP address>:3333",

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


Launch the example
------------------

Click on "Run"->"Start Debugging" and then do normal vscode debug operations,
like running, stopping, adding breakpoints and so on.

All the printf from the application is visible on the openocd terminal.



