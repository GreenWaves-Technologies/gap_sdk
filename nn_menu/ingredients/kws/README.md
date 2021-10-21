# Keyword Spotting

This repository contains an implementation of the google KWS Convolution Neural Network.


## Getting Started

### Prerequisites

To compile and run the code you need to install the [GAP SDK](https://github.com/GreenWaves-Technologies/gap_sdk). Follow the readme at this repository install it:

https://github.com/GreenWaves-Technologies/gap_sdk

### Build and Run

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
