### Copyright (C) 2020 GreenWaves Technologies
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

import matplotlib.pyplot as plt
import matplotlib.patches as patches
from PIL import Image
import numpy as np

im = np.array(Image.open('samples/im4.pgm'), dtype=np.uint8)

# Create figure and axes
fig,ax = plt.subplots(1)

# Display the image
ax.imshow(im)

# Copy Paste code here:
rect = patches.Rectangle((1,30),20,26,linewidth=1,edgecolor='r',facecolor='none')
ax.add_patch(rect)
rect = patches.Rectangle((36,56),18,22,linewidth=1,edgecolor='r',facecolor='none')
ax.add_patch(rect)
rect = patches.Rectangle((50,5),21,21,linewidth=1,edgecolor='r',facecolor='none')
ax.add_patch(rect)



################################

# Add the patch to the Axes
ax.add_patch(rect)

plt.show()