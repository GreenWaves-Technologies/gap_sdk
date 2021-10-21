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


import numpy as np
from PIL import Image, ImageDraw, ImageFont


#INTPUT_FILE = "test_images/test_000_0000084_resized_rgb.ppm"
INTPUT_FILE = "../images/china_1.ppm"
#INTPUT_FILE = "output0.ppm"
OUTPUT_FILE = "result.png"


img_in = Image.open(INTPUT_FILE).convert('RGB')
print(img_in.size)
#img_in = img_in.resize((input_details[0]['shape'][2], input_details[0]['shape'][1]))
draw = ImageDraw.Draw(img_in)


# Copy Paste code here:
#draw.rectangle((ymin, xmin, ymax, xmax), outline=(255, 255, 0))
draw.rectangle((80, 137, 80+225, 137+82), outline=(255, 255, 0))


################################

img_in.show()
img_in.save(OUTPUT_FILE)
