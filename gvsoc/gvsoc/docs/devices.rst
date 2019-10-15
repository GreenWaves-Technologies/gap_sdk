Devices
-------

The virtual platform is by default simulating only a stand-alone chip with a few default devices which are required to boot a simple example. Device models such as camera, flash or microphones can be connected in order to run full applications.

The devices to be simulated must be specified using the standard runner feature for customizing peripherals, see :ref:`device_config`.


.. GDB
.. ---

.. To use GDB with the virtual platform, the Pulp debug bridge must be used to connect to the virtual platform
.. and to open an RSP server where GDB can be connected to communicate with the virtual platform as if the real
.. chip would be used.

.. For that, the application must first be configured with the following option: ::

..   $ make conf gdb=1

.. Then the virtual platform must be launched with the usual command: ::

..   $ make run

.. This should start the virtual platform but quickly stop waiting for an incoming connection with the following 
.. message: ::

..   Proxy listening on port 37539

.. The Pulp debug bridge must then be launched and connected from another terminal with the same SDK configured
.. in the same way, with the following command: ::

..   $ plpbridge --config=<config file path> gdb wait --rsp-port=2345

.. The config file must be the one generated for the application being debugged. Its path
.. can be seen on the pulp-run command which is generated when *make run* is executed.

.. The RSP port can be any port, it just needs to correspond to the one used when opening GDB.

.. Then from another terminal, GDB must be launched and connected: ::

..   $ riscv32-unknown-elf-gdb
..   $ (gdb) target remote :2345

.. The port specified here must be the same as the one specified in the plpbridge command.

.. There are currently a few GDB issues. First it is using by default the 64bit mode if no binary is specified.
.. So a binary must always be specified until this is fixed (with option file). Then breakpoints with compressed instructions generate a GDB error. To avoid it, the following gdb property must be set: ::

..   $ (gdb) set riscv use_compressed_breakpoint off

.. There are also a few limitations, watchpoints are not supported.
