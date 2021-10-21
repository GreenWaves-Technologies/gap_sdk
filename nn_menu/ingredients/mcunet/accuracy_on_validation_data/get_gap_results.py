import os
import argparse
from PIL import Image
from tqdm import tqdm

parser = argparse.ArgumentParser(description='Get random subtest from dataset')
parser.add_argument('-id', '--in_dir', type=str, help="Path to images directory")
parser.add_argument('-od', '--out_dir', type=str, help="Path to output directory")


if __name__ == '__main__':
    args = parser.parse_args()
    in_dir = args.in_dir
    out_dir = args.out_dir

    if not os.path.exists(in_dir):
        raise ValueError(f"Wrong path to images: {args.in_dir}")
    os.makedirs(out_dir, exist_ok=True)

    images_list = []
    for path, subdirs, files in os.walk(in_dir):
        for name in files:
            images_list.append(os.path.join(os.path.basename(path), name))
    for file in tqdm(images_list):
        csv_name = file.replace("/", "_")
        csv_name = csv_name.replace(".ppm", ".csv")
        image_path = os.path.join(in_dir, file)
        out_csv_path = os.path.join(out_dir, csv_name)
        run_string = "./exe {0} {1}".format(image_path, out_csv_path)
        print(run_string)
        os.system(run_string)


