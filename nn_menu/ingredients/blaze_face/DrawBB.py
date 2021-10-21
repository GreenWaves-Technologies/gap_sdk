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

im = np.array(Image.open('images/francesco_cropped_r.ppm'), dtype=np.uint8)
#im = np.array(Image.open('images/croppedpgmfile1.ppm'), dtype=np.uint8)

# Create figure and axes
fig,ax = plt.subplots(1)

# Display the image
ax.imshow(im)

rect = patches.Rectangle((12,30),46,46,linewidth=1,edgecolor='r',facecolor='none')
ax.add_patch(rect)
kp = patches.Circle((26,43),radius=1,color='green')
ax.add_patch(kp)
kp = patches.Circle((46,44),radius=1,color='green')
ax.add_patch(kp)
kp = patches.Circle((38,57),radius=1,color='green')
ax.add_patch(kp)
kp = patches.Circle((38,66),radius=1,color='green')
ax.add_patch(kp)
kp = patches.Circle((14,46),radius=1,color='green')
ax.add_patch(kp)
kp = patches.Circle((55,46),radius=1,color='green')
ax.add_patch(kp)



################################

plt.show()