.. _guide_flashing:

Flash applications and data
---------------------------

Flash an application
====================

Flashing an application allows it to run without external intervention.

In order to flash an application you need to follow this procedure:

- Clean the project using :code:`make clean`
- Compile and flash using :code:`make all CONFIG_BOOT_DEVICE=<boot_device> io=uart`

    - :code:`CONFIG_BOOT_DEVICE=<boot_device>` selects the flashing location.
      Option values are described in the :ref:`guide_app_makefile_options` guide.
    - :code:`io=uart` tells printf outputs to use UART instead of JTAG to avoid getting stuck doing semihost calls.

- select the correct boot mode, either via boot pads or via efuses.
- reboot

Flash data
==========

Flash data using filesystems
""""""""""""""""""""""""""""

Using ReadFS
''''''''''''

ReadFS is a small read-only filesystem.
To flash some files/directories simply add in the application Makefile:

- a :code:`READFS_FILES` variable pointing to the files you want to include
- a :code:`READFS_DIRS` variable pointing to the directories you want to include recursively
- a :code:`READFS_DIRS_NO_REC` variable pointing to the directories you want to include non-recursively

The image will be generated and flashed automatically when executing the :code:`make all` command.
