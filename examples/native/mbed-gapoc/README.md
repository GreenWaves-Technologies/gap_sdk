# GAPoc_BSP

A repository to use in conjunction with GAPPoc boards.

(NB - throughout the code and doc you may find references to GAPOC, GAPPoc or GAPoc. These acronyms all refer to the same thing: the "GAP8 Proof of concept" platform, outlined below.)

This repo contains Board Support Package (BSP) files with examples of code to run on the GAPPoc Platform.


>> The GAPPoc (GAP8 Proof of concept) platform integrates on a single board a GAP8 chip along with fairly invariant components (memory, crystal osc., etc.) plus a number of peripherals (sensors and radio mainly).
Different versions of GAPPoc address different classes of applications:
- GAPPoc A targets image analysis applications and includes a monochrome VGA sensor and BLE radio
- GAPPoc B targets thermal IR analysis applications and include a connector dedicated to a specific IR Sensor satellite board and the same BLE radio
- other versions will follow.
GAPPoc boards are architectured in a modular way, so there will be common blocks between one version of GAPPoc and another one.


>> This repo is structured to reflect the different flavors of GAPPoc :

- sub-directory GAPOC_BSP: contains source and header files useful to any version of GAPPoc,

- sub-directory GAPOC_A contains projects running on GAPOC_A. 
   For example, in directory GAPOC_A/test_GAPOC_BlinkLED, you will find source files to run a basic LED blinking test along with a Makefile to build and
 run the project
   ("make clean all" in this directory to clean and build, "make run" to execute.
   
- Similarly, sub-directory GAPOC_B contains projects running on GAPOC_B, etc.

- etc.


>> GAPPoc boards rely on the standard GAP8 SDK. Instructions for download and setup can be found here : https://greenwaves-technologies.com/sdk-manuals/
To use GAPPoc boards with an external Olimex probe, you need to do the following :
- add the following udev rules in file /etc/udev/rules.d/90-ftdi_gapuino.rules , to that the Olimex probe is recognized by your PC :
    ATTRS{idVendor}=="15ba", ATTRS{idProduct}=="002b", MODE="0666", GROUP="dialout"
  and trigger them by appropriate means (session restart if needed)
- make sure the following flag is set in your Makefile, to indicate you'll be using an external Olimex probe (based on an FTDI chip):  
   PLPBRIDGE_FLAGS += -ftdi
(also, remember to do once a "source sourceme.sh" in your gap_sdk root directory in the terminal window you'll be using, as described in the SDK documentation).


>> Licensing :
GAPPoc-related software code is provided under the terms of the Apache license, see LICENSE file.

GAPPoc hardware information is provided under the terms of the SolderPAd 2.0 harwdare license, see license file in hardware related directories.
