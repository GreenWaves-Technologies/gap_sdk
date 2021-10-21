import os
import random
import argparse
import shutil
from tqdm import tqdm
from PIL import Image

parser = argparse.ArgumentParser(description='Get random subtest from dataset')
parser.add_argument('-id', '--in_dir', type=str, help="Path to images directory")
parser.add_argument('-od', '--out_dir', type=str, help="Path to output directory")
parser.add_argument('-n', '--num_images', type=int, default=100, help="Number of images in quant set")


if __name__ == '__main__':
    args = parser.parse_args()
    in_dir = args.in_dir
    out_dir = args.out_dir
    num_images = args.num_images

    if not os.path.exists(in_dir):
        raise ValueError(f"Wrong path to images: {args.in_dir}")
    os.makedirs(out_dir, exist_ok=True)

    images_list = []
    for path, _, names in tqdm(os.walk(in_dir)):
        for file in names:
            images_list.append(os.path.join(os.path.basename(path), file))
    random.seed(42)
    randomly_chosen = random.sample(images_list, num_images)
    for file in randomly_chosen:
        os.makedirs(os.path.dirname(os.path.join(out_dir, file)), exist_ok=True)
        shutil.copy2(os.path.join(in_dir, file), os.path.join(out_dir, file))

