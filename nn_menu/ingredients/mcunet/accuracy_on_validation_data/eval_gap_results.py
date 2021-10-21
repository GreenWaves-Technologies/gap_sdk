import sys

import torch
import argparse

import os
import numpy as np
import pandas as pd 
from tqdm import tqdm
from torchvision import datasets, transforms
from PIL import Image


# Training settings
parser = argparse.ArgumentParser()

# dataset args.
parser.add_argument('--dataset', default='imagenet', type=str)
parser.add_argument('--val-dir', default='dataset/imagenet/val',
                    help='path to validation data')
parser.add_argument('--gap-dir', default='dataset/imagenet/gap_outputs',
                    help='path to gap csv data')
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

def eval_gap(data):
    pred, _, target = data
    output = torch.from_numpy(pred).view(1, -1)
    acc1, acc5 = accuracy(output, target.view(1), topk=(1, 5))

    return acc1.item(), acc5.item()


if __name__ == '__main__':

    print(' * reading gap results ...')
    preds = []
    for file in tqdm(sorted(os.listdir(args.gap_dir))):
        gap_data = pd.read_csv(os.path.join(args.gap_dir, file),
                    delimiter=';', 
                    header=None, 
                    index_col=False)
        gap_data = gap_data.dropna(axis='columns')
        gap_data = gap_data.values
        #np.set_printoptions(threshold=2000)
        #print(gap_data)
        preds.append(gap_data)

    # we first cache the whole test set into memory for faster data loading
    print(' * start caching the test set...', end='')
    val_loader = get_val_dataset(160)  # range [0, 1]
    val_loader_cache = [v for v in val_loader]
    images = torch.cat([torch.from_numpy(np.array(v[0])) for v in val_loader_cache], dim=0)
    targets = torch.cat([v[1] for v in val_loader_cache], dim=0)
    val_loader_cache = [[pred, x, y] for pred, x, y in zip(preds, images, targets)]
    print('done.')
    print(' * dataset size:', len(val_loader_cache))

    # use multi-processing for faster evaluation
    n_thread = 32 
    from multiprocessing import Pool

    p = Pool(n_thread)
    correctness1 = []
    correctness5 = []

    pbar = tqdm(p.imap_unordered(eval_gap, val_loader_cache), total=len(val_loader_cache),
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
