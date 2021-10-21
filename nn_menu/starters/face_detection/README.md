# Face Detection

This repostiry implements on GAP a custom CNN that does pedestrian detection. 

# Getting Started

### Prerequisites

To compile and run the code you need to install the [GAP SDK](https://github.com/GreenWaves-Technologies/gap_sdk). Follow the readme at this repository install it:

https://github.com/GreenWaves-Technologies/gap_sdk

### Simple run of the application

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

This demo is also meant to be ran with camera collected frames in realtime. To run on Gapuino board with images grubbed from a real camera and show t, you need a gapuino combo kit + a [TFT adafruit LCD screen](https://learn.adafruit.com/adafruit-2-8-tft-touch-shield-v2) . 

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

