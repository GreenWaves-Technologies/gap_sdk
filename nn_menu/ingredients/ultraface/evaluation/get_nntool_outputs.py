import glob
import os
import argparse
import numpy as np
from PIL import Image
from tqdm import tqdm

from execution.graph_executer import GraphExecuter
from execution.quantization_mode import QuantizationMode


def main():
    parser = argparse.ArgumentParser(prog='Dump nntool outputs')

    parser.add_argument('-dp', '--data_path', required=True, help="path to test dataset")
    parser.add_argument('-op', '--output_path', required=True, help="path to output folder")
    args = parser.parse_args()

    QUANTIZE = True
    if QUANTIZE:
        qmode = QuantizationMode.all_dequantize()
    else:
        qmode = None

    dataset_files = glob.glob(os.path.join(args.data_path, "*.ppm"))
    executer = GraphExecuter(graphs[0]["G"], qrecs=graphs[0]["G"].quantization)
    os.makedirs(args.output_path, exist_ok=True)

    for filename in tqdm(dataset_files):
        input_x = np.array(Image.open(filename))

        outputs = executer.execute([input_x], qmode=qmode, silent=True)
        out_boxes = np.transpose(outputs[187][0], (1, 0))
        out_scores = outputs[168][0]
        filename = os.path.basename(filename)
        np.savetxt(os.path.join(args.output_path, filename.replace(".ppm", ".csv")), np.column_stack((out_boxes, out_scores)), delimiter=";")

if __name__ == '__main__':
    main()