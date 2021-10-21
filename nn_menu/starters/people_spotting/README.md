# People Spotting

This is an implementation of the visual wake words challenge winner at https://github.com/mit-han-lab/VWW.

The tflite file is a converted version of the trained float model and quantization is carried out in 8 bit activations and weights using sample data from the visual wake words dataset converted to ppm format.

The build command is:

```
make clean all
```

To run on GVSOC

```
make run platform=gvsoc
```

And on a connected GAPUINO or other GAP development board:

```
make run
```

This demo is also meant to be ran with camera collected frames in realtime. To run on Gapuino board with images grubbed from a real camera and show t, you need a special camera adaptor for the galaxycore color camera, and a modification on the board to correcty power supply the camera, the output is shown to a [TFT adafruit LCD screen](https://learn.adafruit.com/adafruit-2-8-tft-touch-shield-v2) . Write an email to support@greenwaves-technologies.com to have more info on how to get the hardware.

Once you have the hardware you can uncomment the following flags in the makefile:

```
## Mute printf in source code
SILENT=1
## Enable image grub from camera and disaply output to lcd
FROM_CAMERA=1

```


and then type a:
```
make clean all run
```
