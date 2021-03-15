from collections import OrderedDict

import numpy as np
from matplotlib import pyplot as plt
from matplotlib.ticker import FormatStrFormatter


class DiagCollector():
    stats = OrderedDict()
    cur_set = None
    node_names = set()

    @classmethod
    def clear(cls):
        cls.stats = OrderedDict()
        cls.node_names = set()

    @classmethod
    def active_set(cls, name):
        cls.cur_set = cls.stats.setdefault(name, OrderedDict())

    @classmethod
    def record(cls, name, val, scale=None, node=None):
        if cls.cur_set is None:
            return
        val = np.atleast_1d(val).copy()
        if scale is not None:
            val = val.astype(np.float32) * scale
        node_name = '' if node is None else node.name
        cls.node_names.add(node_name)
        node_set = cls.cur_set.setdefault(node_name, {})
        item = node_set.get(name, None)
        if item is None:
            node_set[name] = val
        elif item.shape == val.shape:
            node_set[name] = np.stack([node_set[name], val])
        else:
            node_set[name] = np.concatenate([node_set[name], np.expand_dims(val, 0)])

    @classmethod
    def deactivate(cls):
        cls.cur_set = None

    @classmethod
    def error(cls, name, axis=(0, ), sets=None, node_name=None):
        if node_name is None:
            node_name = next(iter(cls.node_names), '')
        if sets is None:
            sets = [val_set for val_set in cls.stats if name in cls.stats[val_set][node_name]]
        if axis is None:
            return {cur_set: np.abs(cls.stats[sets[0]][node_name][name] - cls.stats[cur_set][node_name][name])
                    for cur_set in sets[1::]}
        elif isinstance(axis, int):
            axis = tuple([axis])
        return {cur_set: np.max(
            np.abs(cls.stats[sets[0]][node_name][name] - cls.stats[cur_set][node_name][name]),
            axis=tuple(i for i in range(len(cls.stats[sets[0]][node_name][name].shape)) if i not in axis)
        )
            for cur_set in sets[1::]}

    @classmethod
    def plot_error(cls, names, axis=(0, ), sets=None, node_name=None):
        if isinstance(names, str):
            names = [names]

        if node_name is None:
            node_name = next(iter(cls.node_names), '')

        for name in names:
            errs = DiagCollector.error(name, axis=axis, node_name=node_name)
            for stat_set, val in errs.items():
                plt.plot(val, label=f'{name}_{stat_set}')

        plt.title(f'stats for {node_name}')
        plt.gca().yaxis.set_major_formatter(FormatStrFormatter('%.2f'))
        plt.legend()
        plt.show()
