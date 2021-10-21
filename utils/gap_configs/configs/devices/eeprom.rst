EEPROM
------

This model supports the following parameters

=================== ==================================================== ================= ==================
Name                Description                                          Default value     Optional/Mandatory
=================== ==================================================== ================= ==================
interface           Interface where the device is connected.             i2c0              Optional
config.mem_size     EEPROM size                                          65536             Optional
=================== ==================================================== ================= ==================

Here is an example: ::

  [board.devices.eeprom]
  include = devices/eeprom.json
  interface = i2c0
  config.mem_size = 65536
