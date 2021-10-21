SPI flash model
---------------

This model supports the following parameters

=================== ==================================================== ================= ==================
Name                Description                                          Default value     Optional/Mandatory
=================== ==================================================== ================= ==================
interface           Interface where the device is connected.             spim0             Optional
cs                  Chip select where the device is connected.           0                 Optional
config.size         Flash size                                           0x00800000        Optional
=================== ==================================================== ================= ==================

Here is an example: ::

  [board.devices.spiflash]
  include = devices/spiflash.json
  interface = spim0
  cs = 0
  config.size = 0x00800000