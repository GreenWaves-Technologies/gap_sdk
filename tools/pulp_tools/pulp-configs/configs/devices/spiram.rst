SPI ram model
---------------

This model supports the following parameters

=================== ==================================================== ================= ==================
Name                Description                                          Default value     Optional/Mandatory
=================== ==================================================== ================= ==================
interface           Interface where the device is connected.             spim0             Optional
cs                  Chip select where the device is connected.           0                 Optional
config.mem_size     Ram size                                             0x00800000        Optional
=================== ==================================================== ================= ==================

Here is an example: ::

  [board.devices.spiram]
  include = devices/spiram.json
  interface = spim0
  cs = 0
  config.mem_size = 0x00800000