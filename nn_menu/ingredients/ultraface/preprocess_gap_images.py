import argparse
import os
import numpy as np
from tqdm import tqdm
import cv2


# Training settings
parser = argparse.ArgumentParser()

# dataset args.
parser.add_argument('--val-dir', default='data/WIDER/val',
                    help='path to validation data')
parser.add_argument('--out-dir', help='path to output preprocessed data')

args = parser.parse_args()


if __name__ == '__main__':
    os.makedirs(args.out_dir, exist_ok=True)
    for path, subdirs, names in tqdm(os.walk(args.val_dir)):
        for file in names:
            image = cv2.imread(os.path.join(path, file))
            image = cv2.resize(image, (320, 240))

            cv2.imwrite(os.path.join(args.out_dir, file.replace(".jpg", ".ppm")), image)