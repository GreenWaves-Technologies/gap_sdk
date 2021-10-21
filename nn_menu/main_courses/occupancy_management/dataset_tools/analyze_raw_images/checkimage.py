
import argparse
import numpy as np
import os
import cv2
from argparse import ArgumentParser
import matplotlib.pyplot as plt

def validate_file(f):
    if not os.path.exists(f):
        raise argparse.ArgumentTypeError("{0} does not exist".format(f))
    return f

def validate_path(f):
    if not os.path.isdir(f):
        raise argparse.ArgumentTypeError("{0} does not exist".format(f))
    return f

'image normalization'
def norm(img, Min, Max):
  img = Max * (img - np.min(img))/\
    (np.max(img)-np.min(img))+Min
  return img

def load_raw_bin_img(path_to_file, img_width, img_height, data_type):
  img = np.fromfile(path_to_file, dtype=data_type, sep="")
  img = img.reshape([img_width, img_height])
  return img

def simple_image_correction(path_to_raw_bin_file,path_to_offset_bin):

  base=os.path.basename(path_to_raw_bin_file)
  test_filename = os.path.splitext(base)[0]
  
  'image info'
  img_width, img_height = 80 , 80
  data_type = 'uint16'
  
  'load raw binary image'
  raw_img = load_raw_bin_img(path_to_raw_bin_file, img_width, img_height, 'uint16')
  
  'load raw binary offset '
  offset_img = load_raw_bin_img(path_to_offset_bin, img_width, img_height, 'uint16')
  
  calibrated_img = raw_img  - (offset_img - np.average(offset_img))
  
  return calibrated_img, raw_img, offset_img


if __name__ == "__main__":
  
  parser = ArgumentParser(description="FPN-cancellation from raw binary images using sensor offset image.")
  parser.add_argument("-f", "--image_bin", dest="image_bin", required=True, type=validate_file, help="path to input raw image (.bin).")
  parser.add_argument("-o", "--offset_bin", dest="offset_bin", required=True, type=validate_file, help="path to the offset image (.bin) being used for FPN removement.")
  args = parser.parse_args()
  
  #fig = make_subplots(rows=2, cols=5, start_cell="top-left",
  #        subplot_titles=("Raw Image Hist", "Raw Offset Hist","Corrected Image Hist", "Raw Image","Simple Correction Image"))
  #fig['layout'].update(height = 1200, width = 1800, title = 'Image Analysis: '+args.image_bin)


  fig, ax = plt.subplots(1,4)
  fig.set_size_inches(18.5, 10.5)
  img_num=1

  #raw_img = load_raw_bin_img(args.image_bin, 80, 80, 'uint16')
  #offset_img = load_raw_bin_img(args.offset_bin, 80, 80, 'uint16')

  calibrated_img, raw_img, offset_img = simple_image_correction(args.image_bin,args.offset_bin)

  hist_r,bins_r = np.histogram(raw_img.ravel(),bins=range(0, 16384, 250))
  bins_r = 0.5 * (bins_r[:-1] + bins_r[1:])
  
  hist_o,bins_o = np.histogram(offset_img.ravel(),bins=range(0, 16384, 250))
  bins_o = 0.5 * (bins_o[:-1] + bins_o[1:])
  hist_c,bins_c = np.histogram(calibrated_img.ravel(),bins=range(-8191, 8192, 100))
  bins_c = 0.5 * (bins_c[:-1] + bins_c[1:])

  cvclipped_badpx_img = cv2.cvtColor(norm(calibrated_img,0,255).astype(np.uint8), cv2.COLOR_GRAY2BGR)
  #fig.add_trace(go.Image(z=cvclipped_badpx_img),row=img_num, col=5)
  
  raw_image_norm = cv2.cvtColor(norm(raw_img,0,255).astype(np.uint8), cv2.COLOR_GRAY2BGR)
  #fig.add_trace(go.Image(z=raw_image_norm),row=img_num, col=4)
  #fig.write_html("results.pdf")
  #fig.show()

  ax[0].set_title("Raw Image Histogram")
  mu_r = raw_img.mean()
  median_r = np.median(raw_img)
  sigma_r = raw_img.std()
  min_r = raw_img.min()
  max_r = raw_img.max()
  textstr = '\n'.join((
    r'$\mu=%.2f$' % (mu_r, ),
    r'$\mathrm{median}=%.2f$' % (median_r, ),
    r'$\sigma=%.2f$' % (sigma_r, ),
    r'$\min val=%.2f$' % (min_r, ),
    r'$\max val=%.2f$' % (max_r, )))


  ax[0].hist(raw_img.ravel(), bins=range(0, 16384, 100))
  # these are matplotlib.patch.Patch properties
  props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
  # place a text box in upper left in axes coords
  ax[0].text(0.50, 0.95, textstr, transform=ax[0].transAxes, fontsize=10,
        verticalalignment='top',ma='right', bbox=props)

  ax[1].set_title("Offset Image Histogram")
  mu_r = offset_img.mean()
  median_r = np.median(offset_img)
  sigma_r = offset_img.std()
  min_r = offset_img.min()
  max_r = offset_img.max()
  textstr = '\n'.join((
    r'$\mu=%.2f$' % (mu_r, ),
    r'$\mathrm{median}=%.2f$' % (median_r, ),
    r'$\sigma=%.2f$' % (sigma_r, ),
    r'$\min val=%.2f$' % (min_r, ),
    r'$\max val=%.2f$' % (max_r, )))

  ax[1].hist(offset_img.ravel(), bins=range(0, 16384, 100))

  props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
  # place a text box in upper left in axes coords
  ax[1].text(0.50, 0.95, textstr, transform=ax[1].transAxes, fontsize=10,
        verticalalignment='top',ma='right', bbox=props)

  ax[2].set_title("Calibrated Image Histogram")
  mu_r = calibrated_img.mean()
  median_r = np.median(calibrated_img)
  sigma_r = calibrated_img.std()
  min_r = calibrated_img.min()
  max_r = calibrated_img.max()
  textstr = '\n'.join((
    r'$\mu=%.2f$' % (mu_r, ),
    r'$\mathrm{median}=%.2f$' % (median_r, ),
    r'$\sigma=%.2f$' % (sigma_r, ),
    r'$\min val=%.2f$' % (min_r, ),
    r'$\max val=%.2f$' % (max_r, )))

  ax[2].hist(calibrated_img.ravel(), bins=range(-8192, 8191, 100))

  props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
  # place a text box in upper left in axes coords
  ax[2].text(0.50, 0.95, textstr, transform=ax[2].transAxes, fontsize=10,
        verticalalignment='top',ma='right', bbox=props)

  ax[3].imshow(cvclipped_badpx_img)
  

  plt.show()

  