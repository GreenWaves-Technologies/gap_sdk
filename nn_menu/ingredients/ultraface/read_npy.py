import argparse
import numpy as np


def create_parser():
    # create the top-level parser
    parser = argparse.ArgumentParser(prog='Read .npy')

    parser.add_argument('-npy', '--npy', required=True,
                        help="path to .npy file")
    return parser


def main():
    parser = create_parser()
    args = parser.parse_args()
    np.set_printoptions(threshold=np.inf)
    npy_file = args.npy
    data = np.load(npy_file, allow_pickle=True)
    print(data[0][0])
    print(data[0][0].shape)
    print(data[1])
    print(data[1][0].shape)
    boxes = data[187][0]
    print(np.transpose(boxes, (1, 0)))
    print(boxes.shape)
    conf = data[168][0]
    print(conf)
    print(conf.shape)

if __name__ == '__main__':
    main()
