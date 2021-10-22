SPI verification testbench
--------------------------

This model supports the following parameters

=================== ==================================================== ================= ==================
Name                Description                                          Default value     Optional/Mandatory
=================== ==================================================== ================= ==================
interface           Interface where the device is connected.             spim0             Optional
cs                  Chip select where the device is connected.           0                 Optional
config.mem_size     Memory size                                          1048576           Optional
config.tx_file.path If specified, this dumps incoming SPI stream to                        Optional
                    specified file.
config.tx_file.qpi  If it is true, this dumps to the file in quad SPI                      Optional
=================== ==================================================== ================= ==================

Here is an example: ::

  [board.devices.spim_verif]
  include = devices/spim_verif.json
  interface = spim0
  cs = 0
  config.mem_size = 1048576