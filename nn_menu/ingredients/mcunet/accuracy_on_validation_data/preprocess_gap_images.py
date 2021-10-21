import argparse
import os
import numpy as np
from tqdm import tqdm
from PIL import Image
from torchvision import transforms
from tqdm import tqdm


# Training settings
parser = argparse.ArgumentParser()

# dataset args.
parser.add_argument('--val-dir', default='dataset/imagenet/val',
                    help='path to validation data')
parser.add_argument('--out-dir', help='path to output preprocessed data')

args = parser.parse_args()


if __name__ == '__main__':

    resolution = 160
    transform=transforms.Compose([
    	  transforms.Resize(int(resolution * 256 / 224)),
    	  transforms.CenterCrop(resolution)
        ])
    os.makedirs(args.out_dir, exist_ok=True)
    images_list = []
    for path, subdirs, files in os.walk(args.val_dir):
        os.makedirs(os.path.join(args.out_dir, os.path.basename(path)), exist_ok=True)
        for name in files:
            images_list.append(os.path.join(os.path.basename(path), name))
    for file in tqdm(images_list):
        img = Image.open(os.path.join(args.val_dir, file))
        img = transform(img)
        img = img.convert('RGB')

        img.save(os.path.join(args.out_dir, file.replace(".JPEG", ".ppm")))