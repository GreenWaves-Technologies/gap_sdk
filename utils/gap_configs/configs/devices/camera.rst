Camera
---------

This model supports the following parameters

=================== ==================================================== ================= ================= ==================
Name                Description                                          Possible values   Default value     Optional/Mandatory
=================== ==================================================== ================= ================= ==================
interface           Interface where the device is connected.             Any CPI interface cpi0              Optional
ctrl_interface      Control interface where the device is connected.     Any I2C interface i2c0              Optional
config.model        Camera model                                         himax             himax             Optional
config.color-mode   Camera color model                                   gray, raw         gray              Optional
=================== ==================================================== ================= ================= ==================

Here is an example: ::

  [board.devices.camera]
  include = devices/camera.json
  interface = cpi0
  ctrl_interface = i2c0
  config.model = himax
  config.color-mode = gray
