.. _guide_debugging:

Debugging an application with GDB
=================================

.. warning::

    All instructions in this document are only valid with GAP9.
    **GAP8 has no support for debugging**.

.. note::

    - Cluster debugging is not yet available but will be in a future release.

    - GDB support is **only available when using a board**.

    - Debugging will not work if your application performs actions that might
      disable the JTAG connection (deep sleep as an example). To debug such applications
      please remove all such actions.



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

Using GDB to debug your application
-----------------------------------

Firstly, instead of running the application on the platform with the usual command,
you can open a GDB server with the following command:

.. code-block:: bash

    make run gdbserver=1

Note the :code:`gdbserver=1` which is the item of interest here.
This will launch the execution but will stop before the application is started,
waiting for GDB to connect and issue commands.

On the terminal, you should see:

.. code-block:: none

    Ready for Remote Connections

This means a gdbserver, connected to your board is now active on port
:code:`localhost:3333`.


If needed, another GDB port can be specified with the following option:

.. code-block:: bash

    make run gdbserver=1 gdbport=3333

Now, in another terminal, execute the following command
(assuming your application is named `test`):

.. code-block:: bash

    riscv64-unknown-elf-gdb BUILD/GAP9_V2/GCC_RISCV_FREERTOS/test

This will open a GDB prompt, you can then enter the following GDB commands:

.. code-block:: bash

    (gdb) target remote :3333
    (gdb) load

And you should see a message about code sections loading. This means that
your application binary is now loaded on board.
You may now use regular GDB features such as breakpoints. Once you have setup
your breakpoints, you can run with :code:`continue`.

If you need to reset the board use :code:`monitor reset` and then :code:`load` again.

