I2S microphones
---------------

This model can be used to simulate several microphones connected to the chip I2S interfaces.

This model supports the following parameters

=================== ==================================================== ================= ==================
Name                Description                                          Default value     Optional/Mandatory
=================== ==================================================== ================= ==================
interface           I2S interface where the device is connected.         i2s0              Mandatory
config.pdm          Activate PDM modulation                              true              Optional
config.ddr          Activate Dual Data Rate (a data is sent on raising   false             Optional
                    and falling edges)
config.stim_left    Path to the WAVE file use as stimuli for left        null              Optional
                    channel
config.stim_right   Path to the WAVE file use as stimuli for right       null              Optional
                    channel
=================== ==================================================== ================= ==================

Here is an example: ::

  [board.devices.i2s_microphone]
  include = devices/i2s_microphone.json
  interface = i2s0
  config.stim_left = myfile_left.wav
  config.stim_right = myfile_right.wav
  config.ddr = false
  config.pdm = true