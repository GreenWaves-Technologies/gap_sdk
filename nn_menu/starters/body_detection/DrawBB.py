import matplotlib.pyplot as plt
import matplotlib.patches as patches
from PIL import Image
import numpy as np

im = np.array(Image.open('test_samples/img_OUT0.pgm'), dtype=np.uint8)

# Create figure and axes
fig,ax = plt.subplots(1)

# Display the image
ax.imshow(im)

# Copy Paste code here:

rect = patches.Rectangle((74,27),28,79,linewidth=1,edgecolor='r',facecolor='none')
ax.add_patch(rect)


################################

# Add the patch to the Axes
ax.add_patch(rect)

plt.show()
