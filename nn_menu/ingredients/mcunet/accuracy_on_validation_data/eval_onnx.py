import sys

sys.path.append(".")

import torch
import argparse
import torch.backends.cudnn as cudnn

import os
import numpy as np
import onnxruntime as rt
from torchvision import datasets, transforms

np.set_printoptions(threshold=sys.maxsize)

# Training settings
parser = argparse.ArgumentParser()
parser.add_argument('--onnx_path', default=None, help='path to onnx model for evaluation')

# dataset args.
parser.add_argument('--dataset', default='imagenet', type=str)
parser.add_argument('--val-dir', default='dataset/imagenet/val',
                    help='path to validation data')
parser.add_argument('--batch-size', type=int, default=256,
                    help='input batch size for training')
parser.add_argument('-j', '--workers', default=16, type=int, metavar='N',
                    help='number of data loading workers')

args = parser.parse_args()



def get_val_dataset(resolution):
    # NOTE: we do not use normalization for tf-lite evaluation; the input is normalized to 0-1
    kwargs = {'num_workers': args.workers, 'pin_memory': False}
    if args.dataset == 'imagenet':
        val_dataset = \
            datasets.ImageFolder(args.val_dir,
                                 transform=transforms.Compose([
                                     transforms.Resize(int(resolution * 256 / 224)),
                                     transforms.CenterCrop(resolution),
                                     transforms.ToTensor(),
                                     transforms.Normalize(mean=[0.5, 0.5, 0.5], std=[0.5, 0.5, 0.5])
                                 ]))

    val_loader = torch.utils.data.DataLoader(
        val_dataset, batch_size=args.batch_size,
        shuffle=False, **kwargs)
    return val_loader

def accuracy(output, target, topk=(1,)):
	""" Computes the precision@k for the specified values of k """
	maxk = max(topk)
	batch_size = target.size(0)

	_, pred = output.topk(maxk, 1, True, True)
	pred = pred.t()
	correct = pred.eq(target.reshape(1, -1).expand_as(pred))

	res = []
	for k in topk:
		correct_k = correct[:k].reshape(-1).float().sum(0, keepdim=True)
		res.append(correct_k.mul_(100.0 / batch_size).mean())
	return res

def eval_image(data):
    image, target = data
    if len(image.shape) == 3:
        image = image.unsqueeze(0)
    image_np = image.cpu().numpy()
    sess = rt.InferenceSession(args.onnx_path)
    input_name = sess.get_inputs()[0].name
    label_name = sess.get_outputs()[0].name
    output_data = sess.run([label_name], {input_name: image_np.astype(np.float32)})[0]

    output = torch.from_numpy(output_data).view(1, -1)
    acc1, acc5 = accuracy(output, target.view(1), topk=(1, 5))

    return acc1.item(), acc5.item()


if __name__ == '__main__':
    resolution = 160

    # we first cache the whole test set into memory for faster data loading
    print(' * start caching the test set...', end='')
    val_loader = get_val_dataset(resolution)  # range [0, 1]
    val_loader_cache = [v for v in val_loader]
    images = torch.cat([v[0] for v in val_loader_cache], dim=0)
    targets = torch.cat([v[1] for v in val_loader_cache], dim=0)

    val_loader_cache = [[x, y] for x, y in zip(images, targets)]
    print('done.')
    print(' * dataset size:', len(val_loader_cache))

    # use multi-processing for faster evaluation
    n_thread = 32
    from multiprocessing import Pool
    from tqdm import tqdm

    p = Pool(n_thread)
    correctness1 = []
    correctness5 = []

    pbar = tqdm(p.imap_unordered(eval_image, val_loader_cache), total=len(val_loader_cache),
                desc='Evaluating...')
    for idx, (acc1, acc5) in enumerate(pbar):
        correctness1.append(acc1)
        correctness5.append(acc5)
        pbar.set_postfix({
            'top1': sum(correctness1) / len(correctness1),
            'top5': sum(correctness5) / len(correctness5),
        })
    print('* top1: {:.2f}%, top5: {:.2f}%'.format(
        sum(correctness1) / len(correctness1),
        sum(correctness5) / len(correctness5)
    ))
