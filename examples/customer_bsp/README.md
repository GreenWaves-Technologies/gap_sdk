# Intro

## Architecture
.    
├── bsp                          The BSP source code: describe how the board is connected    
│   ├── gapoc_c.c    
│   └── gapoc_c.h    
├── bsp_src.mk                   Makefile for this BSP, include all the BSP source here    
├── examples                     Examples which is using this BSP    
│   ├── blink_led    
│   ├── camera_hm0360    
│   └── flash    
├── gapoc_c.sh                   Config file for using this customer BSP    
└── README.md                    This file     


#To use this BSP
1. cd customer_bsp (this folder), source gapoc_c.sh
2. cd gap_sdk and source config/gap8_v3.sh
3. compile sdk: make clean sdk (optional, if you need pulpOS and it hasn't been installed for GAP8_V3 target)

** Be careful: don't use any BSP config files in config, but ONLY use chip config files. Like gap8_v3.sh

#To make your BSP
1. modify the 2 files in the ./bsp folder, and adapt them to your customer boards.
2. modify the src.mk in this folder and add/remove the drivers according to your needs.
3. modify the shell script in this folder.
4. Add your examples and applications. Please use the examples (source code and Makefile) here as reference.
