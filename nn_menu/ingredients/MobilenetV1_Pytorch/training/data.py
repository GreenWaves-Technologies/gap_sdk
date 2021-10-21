import os
import torchvision.datasets as datasets
import torchvision.transforms as transforms

_IMAGENET_MAIN_PATH = '/home/rusci/Dataset/'
_GRAINCAM_MAIN_PATH = '/home/rusci/Work/quantized_neural_networks/dataset_grain/'
_DATASETS_MAIN_PATH = '/home/rusci/Work/quantized_neural_networks/datasets/'

_dataset_path = {
    'cifar10': os.path.join(_DATASETS_MAIN_PATH, 'CIFAR10'),
    'cifar100': os.path.join(_DATASETS_MAIN_PATH, 'CIFAR100'),
    'stl10': os.path.join(_DATASETS_MAIN_PATH, 'STL10'),
    'mnist': os.path.join(_DATASETS_MAIN_PATH, 'MNIST'),
    'imagenet': {
        'train': os.path.join(_IMAGENET_MAIN_PATH, 'train'),
        'val': os.path.join(_IMAGENET_MAIN_PATH, 'val')
    },
    'graincam': {
        'train': os.path.join(_GRAINCAM_MAIN_PATH, 'train'),
        'val': os.path.join(_GRAINCAM_MAIN_PATH, 'eval')
    }
}


def get_dataset(name, split='train', transform=None,
                target_transform=None, download=True):
    train = (split == 'train')
    if name == 'mnist':
        return datasets.MNIST(  root=_dataset_path['mnist'],
                                train=train,
                                transform=transform,
                                target_transform=target_transform,
                                download=download)
    elif name == 'cifar10':
        return datasets.CIFAR10(root=_dataset_path['cifar10'],
                                train=train,
                                transform=transform,
                                target_transform=target_transform,
                                download=download)
    elif name == 'cifar100':
        return datasets.CIFAR100(root=_dataset_path['cifar100'],
                                 train=train,
                                 transform=transform,
                                 target_transform=target_transform,
                                 download=download)
    elif name == 'imagenet':
        path = _dataset_path[name][split]
        return datasets.ImageFolder(root=path,
                                    transform=transform,
                                    target_transform=target_transform)

    elif name == 'graincam':
        path = _dataset_path[name][split]
        return datasets.ImageFolder(root=path,
                                    transform=transform,
                                    target_transform=target_transform)
def get_num_classes(name):
    if name == 'mnist':
        return   10
    elif name == 'cifar10':
        return   10
    elif name == 'cifar100':
        return  100
    elif name == 'imagenet':
        return 1000  
    elif name == 'graincam': 
        return 4