import cv2
import numpy as np
import matplotlib.pyplot as plt
import pdb

width = 640
height = 480

# with open("./BUILD/GAP9_V2/GCC_RISCV_FREERTOS/Out.ppm", "rb") as rawimg:
#     # Read the packed 16bits
#     bayer_im0 = np.fromfile(rawimg, np.uint16, width * height)
#     bayer_im = np.reshape(bayer_im0, (height, width))

#     # Apply Demosacing (COLOR_BAYER_BG2BGR gives the best result out of the 4 combinations).
#     bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_BG2RGB)  # The result is BGR format with 16 bits per pixel and 12 bits range [0, 2^10-1].

#     # pdb.set_trace()

#     # Show image for testing (multiply by 64 because imshow requires full uint16 range [0, 2^16-1]).
#     cv2.imshow('bgr', bgr*64)
#     cv2.waitKey()
#     cv2.destroyAllWindows()

#     bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_BG2RGB)
#     cv2.imwrite("./BG2RGB.png", bgr*64)
#     bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_BG2BGR)
#     cv2.imwrite("./BG2BGR.png", bgr*64)

#     bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_GB2RGB)
#     cv2.imwrite("./GB2RGB.png", bgr*64)
#     bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_GB2BGR)
#     cv2.imwrite("./GB2BGR.png", bgr*64)

#     bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_GR2RGB)
#     cv2.imwrite("./GR2RGB.png", bgr*64)
#     bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_GR2BGR)
#     cv2.imwrite("./GR2BGR.png", bgr*64)

#     bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_RG2RGB)
#     cv2.imwrite("./RG2RGB.png", bgr*64)
#     bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_RG2BGR)
#     cv2.imwrite("./RG2BGR.png", bgr*64)

with open("./BUILD/GAP9_V2/GCC_RISCV_FREERTOS/Out.ppm", "rb") as rawimg:
    # Read the packed 8bits
    bayer_im0 = np.fromfile(rawimg, np.uint8, width * height)
    bayer_im = np.reshape(bayer_im0, (height, width))

    # Apply Demosacing (COLOR_BAYER_BG2BGR gives the best result out of the 4 combinations).
    bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_BG2RGB)  # The result is BGR format with 16 bits per pixel and 12 bits range [0, 2^10-1].

    # pdb.set_trace()

    cv2.imshow('bgr', bgr)
    cv2.waitKey()
    cv2.destroyAllWindows()

    # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_BG2RGB)
    # cv2.imwrite("./BG2RGB.png", bgr)
    # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_BG2BGR)
    # cv2.imwrite("./BG2BGR.png", bgr)

    # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_GB2RGB)
    # cv2.imwrite("./GB2RGB.png", bgr)
    # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_GB2BGR)
    # cv2.imwrite("./GB2BGR.png", bgr)

    # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_GR2RGB)
    # cv2.imwrite("./GR2RGB.png", bgr)
    # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_GR2BGR)
    # cv2.imwrite("./GR2BGR.png", bgr)

    # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_RG2RGB)
    # cv2.imwrite("./RG2RGB.png", bgr)
    # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_RG2BGR)
    # cv2.imwrite("./RG2BGR.png", bgr)
