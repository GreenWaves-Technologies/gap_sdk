#!/usr/bin/env python3

# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import time
import os
import matplotlib.pyplot as plt
from PIL import Image as PImage
import os.path

img_n=0
path = "dump_out_imgs/"

fig = plt.figure()
ax = fig.add_subplot(111)
plt.ion()
plt.show()

while 1:
	image = "img_{:04d}.pgm".format(img_n)
	#in case dir is emptied restart the counter from 0
	if not os.listdir(path):
		img_n=0
	try:
		img = plt.imread(path + image)
		ax.imshow(img)
		fig.canvas.set_window_title(image)
		plt.draw()
		plt.pause(0.001)
		img_n+=1
	except Exception as e:
		time.sleep(1)
	time.sleep(1)