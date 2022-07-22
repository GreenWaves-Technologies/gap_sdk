# Copyright (C) 2020  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import threading
from collections import OrderedDict

import numpy as np
#from matplotlib import pyplot as plt
#from matplotlib.ticker import FormatStrFormatter

from nntool.utils.stats_funcs import qsnr


def thread_singleton(cls_) -> type:

    class ThreadSingleton(cls_):  # type: ignore
        __instances = {}
        __initialized = set()

        def __new__(cls, *args, **kwargs):
            instances = ThreadSingleton.__instances.get(cls)
            if instances is None:
                ThreadSingleton.__instances[cls] = instances = {
                    threading.get_ident(): super().__new__(cls, *args, **kwargs)}
            instance = instances.get(threading.get_ident())
            if instance is None:
                instances[threading.get_ident()] = instance = super().__new__(
                    cls, *args, **kwargs)
            return instance

        def __init__(self, *args, **kwargs):
            if object.__hash__(self) not in ThreadSingleton.__initialized:
                ThreadSingleton.__initialized.add(object.__hash__(self))
                super().__init__(*args, **kwargs)

    return ThreadSingleton


@thread_singleton
class DiagCollectors():
    def __init__(self) -> None:
        self.stats = OrderedDict()
        self.cur_sets = []
        self.node_names = set()
        self.dequantize = True

    def clear(self, set_name=None):
        if set_name:
            if set_name in self.stats:
                del self.stats[set_name]
        else:
            self.stats = OrderedDict()
            self.node_names = set()

    def active_set(self, name):
        self.cur_sets.append(self.stats.setdefault(name, OrderedDict()))

    def record(self, name, val, scale=None, zero_point=0, node=None):
        if not self.cur_sets:
            return
        val = np.atleast_1d(val).copy()
        if self.dequantize:
            if zero_point:
                val = val.astype(np.float32) - zero_point
            if scale is not None:
                val = val.astype(np.float32) * scale
        node_name = '' if node is None else node.name
        self.node_names.add(node_name)
        for cur_set in self.cur_sets:
            node_set = cur_set.setdefault(node_name, {})
            item = node_set.get(name, None)
            if item is None:
                node_set[name] = val
            elif item.shape == val.shape:
                node_set[name] = np.stack([node_set[name], val])
            else:
                node_set[name] = np.concatenate(
                    [node_set[name], np.expand_dims(val, 0)])

    def record_ref(self, name, ref, node=None):
        if self.cur_sets is None:
            return
        assert node
        for cur_set in self.cur_sets:
            node_set = cur_set.get(node.name)
            item_ref = node_set[ref]
            node_set[name] = item_ref

    def deactivate(self):
        self.cur_sets = self.cur_sets[:-1:]

    def error(self, name, axis=(0, ), sets=None, node_name=None):
        if node_name is None:
            node_name = next(iter(self.node_names), '')
        if sets is None:
            sets = [
                val_set for val_set in self.stats if name in self.stats[val_set][node_name]]
        if axis is None:
            return {cur_set: np.abs(self.stats[sets[0]][node_name][name] - self.stats[cur_set][node_name][name])
                    for cur_set in sets[1::]}
        elif isinstance(axis, int):
            axis = tuple([axis])
        return {cur_set: np.max(
            np.abs(self.stats[sets[0]][node_name][name] -
                   self.stats[cur_set][node_name][name]),
            axis=self.axis_tuple(self.stats[sets[0]][node_name][name], axis))
            for cur_set in sets[1::]}

    def qsnr(self, name_set1, name_set2, axis=None, node_name=None):
        if axis is None:
            axis = 0
        if node_name is None:
            node_name = next(iter(self.node_names), '')

        set1 = self.stats[name_set1][node_name]
        set2 = self.stats[name_set2][node_name]
        return {k: qsnr(v, set2[k], axis=axis) if k in set2 else None
                for k, v in set1.items()}

    def compare(self, name_set1, name_set2, node_name, val, idx):
        set1 = self.stats[name_set1][node_name][val][idx]
        set2 = self.stats[name_set2][node_name][val][idx]
        return set1, set2

    def axis_tuple(self, val, axis):
        if isinstance(axis, int):
            axis = (axis,)
        return tuple(i for i in range(len(val.shape)) if i not in axis)

    def consolodate(self, name_set, val_name, axis=None, node_name=None):
        val = self.stats[name_set]
        if node_name is None:
            node_name = next(iter(val))
        val = val[node_name][val_name]
        if axis is not None:
            axis = self.axis_tuple(val, axis)
        return {
            'min': np.min(val, axis=axis),
            'max': np.max(val, axis=axis)
        }

    def store_ranges(self, details, stats_set, *vars):
        for var in vars:
            details[f'range_{var}'] = self.consolodate(stats_set, var)

    # def plot_error(self, names, axis=(0, ), sets=None, node_name=None):
    #     if isinstance(names, str):
    #         names = [names]

    #     if node_name is None:
    #         node_name = next(iter(self.node_names), '')

    #     for name in names:
    #         errs = DiagCollector.error(name, axis=axis, node_name=node_name)
    #         for stat_set, val in errs.items():
    #             plt.plot(val, label=f'{name}_{stat_set}')

    #     plt.title(f'stats for {node_name}')
    #     plt.gca().yaxis.set_major_formatter(FormatStrFormatter('%.2f'))
    #     plt.legend()
    #     plt.show()

DiagCollector = DiagCollectors()
