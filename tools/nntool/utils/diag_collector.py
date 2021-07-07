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

from collections import OrderedDict

import numpy as np
from matplotlib import pyplot as plt
from matplotlib.ticker import FormatStrFormatter

from utils.stats_funcs import qsnr

import traceback


class DiagCollector():
    stats = OrderedDict()
    cur_sets = []
    node_names = set()

    @classmethod
    def clear(cls, set_name=None):
        if set_name:
            if set_name in cls.stats:
                del cls.stats[set_name]
        else:
            cls.stats = OrderedDict()
            cls.node_names = set()

    @classmethod
    def active_set(cls, name):
        cls.cur_sets.append(cls.stats.setdefault(name, OrderedDict()))

    @classmethod
    def record(cls, name, val, scale=None, node=None):
        if not cls.cur_sets:
            return
        val = np.atleast_1d(val).copy()
        if scale is not None:
            val = val.astype(np.float32) * scale
        node_name = '' if node is None else node.name
        cls.node_names.add(node_name)
        for cur_set in cls.cur_sets:
            node_set = cur_set.setdefault(node_name, {})
            item = node_set.get(name, None)
            if item is None:
                node_set[name] = val
            elif item.shape == val.shape:
                node_set[name] = np.stack([node_set[name], val])
            else:
                node_set[name] = np.concatenate(
                    [node_set[name], np.expand_dims(val, 0)])

    @classmethod
    def record_ref(cls, name, ref, node=None):
        if cls.cur_sets is None:
            return
        assert node
        for cur_set in cls.cur_sets:
            node_set = cur_set.get(node.name)
            item_ref = node_set[ref]
            node_set[name] = item_ref

    @classmethod
    def deactivate(cls):
        cls.cur_sets = cls.cur_sets[:-1:]

    @classmethod
    def error(cls, name, axis=(0, ), sets=None, node_name=None):
        if node_name is None:
            node_name = next(iter(cls.node_names), '')
        if sets is None:
            sets = [
                val_set for val_set in cls.stats if name in cls.stats[val_set][node_name]]
        if axis is None:
            return {cur_set: np.abs(cls.stats[sets[0]][node_name][name] - cls.stats[cur_set][node_name][name])
                    for cur_set in sets[1::]}
        elif isinstance(axis, int):
            axis = tuple([axis])
        return {cur_set: np.max(
            np.abs(cls.stats[sets[0]][node_name][name] -
                   cls.stats[cur_set][node_name][name]),
            axis=cls.axis_tuple(cls.stats[sets[0]][node_name][name], axis))
            for cur_set in sets[1::]}

    @classmethod
    def qsnr(cls, name_set1, name_set2, axis=None, node_name=None):
        if axis is None:
            axis = 0
        if node_name is None:
            node_name = next(iter(cls.node_names), '')

        set1 = cls.stats[name_set1][node_name]
        set2 = cls.stats[name_set2][node_name]
        return {k: qsnr(v, set2[k], axis=axis) if k in set2 else None
                for k, v in set1.items()}

    @classmethod
    def compare(cls, name_set1, name_set2, node_name, val, idx):
        set1 = cls.stats[name_set1][node_name][val][idx]
        set2 = cls.stats[name_set2][node_name][val][idx]
        return set1, set2

    @classmethod
    def axis_tuple(cls, val, axis):
        if isinstance(axis, int):
            axis = (axis,)
        return tuple(i for i in range(len(val.shape)) if i not in axis)

    @classmethod
    def consolodate(cls, name_set, val_name, axis=None, node_name=None):
        val = cls.stats[name_set]
        if node_name is None:
            node_name = next(iter(val))
        val = val[node_name][val_name]
        if axis is not None:
            axis = cls.axis_tuple(val, axis)
        return {
            'min': np.min(val, axis=axis),
            'max': np.max(val, axis=axis)
        }

    @classmethod
    def store_ranges(cls, details, stats_set, *vars):
        for var in vars:
            details[f'range_{var}'] = cls.consolodate(stats_set, var)

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
