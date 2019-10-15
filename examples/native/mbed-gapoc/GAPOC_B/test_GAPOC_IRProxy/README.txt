This example code obtains thermal IR images from the IR Proxy and perfroms some minimal post-processing to get an exploitable picture.
The result can be displayed live on an LCD (AdaFruit 2.8 LCD shield, see test_GAP_LCD/INFO.txt for connections).
It's also possible to store a processed frame as .ppm on PC file system (through slow JTAG bridge) for display on PC.

The test starts with calibration phase during which the user should obturate the lens FOV with a shutter (piece of plastic, thumb...). The shuttering can be stopped after a few seconds (adjutable in code). The shutter phase is indicated either by a black square on the LCD screen when the LCD isu sed, or by turning on green LED3 when the LCD is not used.

