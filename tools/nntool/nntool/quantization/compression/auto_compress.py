# Copyright (C) 2021  GreenWaves Technologies, SAS

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


import math
import typing
from abc import ABC, abstractmethod
from collections.abc import Iterable, Sized
from typing import Any, Sequence, Tuple

import numpy as np
from nntool.execution.graph_executer import GraphExecuter
from nntool.execution.quantization_mode import QuantizationMode
from nntool.interpreter.shell_utils import glob_input_files
from nntool.utils.data_importer import import_data
from nntool.utils.maximizer import Maximizer
from nntool.utils.validation_utils import ValidateBase

from .compress import CompressionError

if typing.TYPE_CHECKING:
    from nntool.graph.nngraph import NNGraph


def value_compression(graph, node):
    nid = node.name
    qtype = graph.quantization[nid].out_qs[0] if graph.quantization and nid in graph.quantization else None
    qbits = qtype.bits if qtype else 8
    full_size = int(math.ceil((node.value.size * qbits)/8))
    return full_size - node.compressed_value.size

class DataLoaderBase(ABC, Sized, Iterable):

    @abstractmethod
    def set_filter(self, values):
        """given a set of returned labels filter this from the iteration"""
        pass

class FileDataLoader(DataLoaderBase):
    def __init__(self, input_files, num_inputs, input_args=None):
        self._input_files = list(glob_input_files(input_files, num_inputs))
        self._idx = 0
        if input_args is None:
            input_args = {}
        self._input_args = input_args
        self._filter = None

    def __iter__(self):
        self._idx = 0
        return self

    def __next__(self):
        while True:
            if self._idx >= len(self._input_files):
                raise StopIteration()
            idx = self._idx
            self._idx += 1
            label = "_".join(self._input_files[idx])
            if self._filter is None or label in self._filter:
                break
        data = [import_data(input_file, **self._input_args) for input_file in self._input_files[idx]]
        return label, data

    def __len__(self):
        return len(self._input_files)

    def set_filter(self, values):
        self._filter = values

def print_progress(msg, newline):
    print(msg, end='\n' if newline else '', flush=True)


class AutoCompress():
    def __init__(self,
                 graph: "NNGraph",
                 labels_and_inputs: Sequence[Tuple[Any, Sequence[np.ndarray]]],
                 validation: ValidateBase,
                 start_qsnr=30,
                 min_step=0.5,
                 base_inputs=None) -> None:
        self._graph = graph
        self._labels_and_inputs = labels_and_inputs
        self._validation = validation
        self._start_qsnr = start_qsnr
        self._min_step = min_step
        self._base_inputs = base_inputs


    @property
    def base_inputs(self):
        return self._base_inputs

    def tune_compression(self, nodes, progress=None, **kwargs):
        compression = 0
        for node in nodes:
            nid = node.name
            if self._graph.quantization and nid in self._graph.quantization:
                qtype = self._graph.quantization[nid].out_qs[0]
            else:
                qtype = None

            try:
                kwargs['qtype'] = qtype
                node.compress_value(**kwargs)
                comp_val = node.compressed_value
                qbits = qtype.bits if qtype else 8
                compression += math.ceil(node.value.size *
                                         qbits / 8) - comp_val.size
                node.use_compressed = True
                if progress:
                    progress(node, True)

            except CompressionError:
                comp_val = None
                node.clear_compression()
                if progress:
                    progress(node, False)

        return compression

    def tune_qsnr(self, nodes, qsnr, progress=None):
        return self.tune_compression(nodes, progress=progress, min_qsnr=qsnr)

    def tune_bits(self, nodes, bits, threshold=None, sparse=False, progress=None):
        return self.tune_compression(nodes, progress=progress, bits=bits, threshold=threshold, force_sparse=sparse)

    def validate(self, qmode, break_on_error=False, labels_and_inputs=None, progress=None):
        if labels_and_inputs is None:
            labels_and_inputs = self._labels_and_inputs

        good_inputs = []
        bad_inputs = []
        good_margin = 0.0
        bad_margin = 0.0
        for label, data in labels_and_inputs:

            executer = GraphExecuter(
                self._graph, qrecs=self._graph.quantization)
            outputs = executer.execute(data, qmode=qmode, silent=True)
            res = self._validation.validate(data,
                                            outputs,
                                            input_name=label)

            if res.validated:
                good_margin += res.margin
                good_inputs.append(label)
            else:
                bad_margin += res.margin
                bad_inputs.append(label)
                if break_on_error:
                    break
            if progress:
                progress(res.validated)
        if good_inputs:
            good_margin /= len(good_inputs)
        if bad_inputs:
            bad_margin /= len(bad_inputs)
        return good_inputs, good_margin, bad_inputs, bad_margin

    def layer_compression(self, nodes):
        return {node: value_compression(self._graph, node)
                for node in nodes if node.compressed_value and node.use_compressed}

    def finetune(self, nodes, progress=None, quantize=False):
        sizes = [(node, node.compressed_value)
                 for node in nodes if node.compressed_value and node.use_compressed]
        nodes = [size[0] for size in sizes]
        base_inputs = self.get_base_inputs(nodes, progress, quantize)
        if not base_inputs:
            raise ValueError('none of the input samples validates correct')
        for node in nodes:
            if node.compressed_value:
                node.use_compressed = True

        def opt_func(bits, threshold, sparse, node, state):
            if progress:
                progress('compressing: ', False)
            compression = self.tune_bits(
                [node],
                bits,
                threshold=threshold,
                sparse=sparse,
                progress=(lambda _, comp: progress('+' if comp else '-', False)) if progress else None)
            if not compression or ('best_compression' in state and state['best_compression'] > compression):
                if bits == 8 and sparse:
                    raise CompressionError("could not compress graph")
                return None
            state['best_compression'] = compression
            if progress:
                progress('', True)
                progress('validating: ', False)
            self._labels_and_inputs.set_filter(state['cur_inputs'])
            good_inputs, good_margin, bad_inputs, bad_margin = self.validate(
                state['qmode'],
                break_on_error=state['final'],
                progress=(lambda pred: progress('+' if pred else '-', False)) if progress else None)
            if progress:
                progress('', True)
                progress(
                    f'good {len(good_inputs)} ({good_margin:.2f}) bad {len(bad_inputs)} ({bad_margin:.2f})', True)
            if bad_inputs:
                state['cur_inputs'] = bad_inputs
                del state['best_compression']
                return None
            return compression

        maximizer = Maximizer(opt_func, 2, 8, int_step=True)
        while sizes:
            sizes.sort(key=lambda x: x[1].size)

            tune_idx = -1
            node = None
            while node is None and abs(tune_idx) <= len(sizes):
                node, comp_val = sizes[tune_idx]
                cur_bits = comp_val.bits
                if cur_bits > 2:
                    cur_step = max(cur_bits//2, 1)
                    cur_bits = max(cur_bits - cur_step, 2)
                else:
                    tune_idx -= 1
                    node = None

            if node is None:
                break

            if progress:
                progress(f'finetuning {node.name}', True)
            qmode = QuantizationMode.none()
            dir_start = 'down'
            opt_state = {
                'cur_inputs': base_inputs.copy(),
                'final': False,
                'qmode': qmode
            }
            while True:
                res = maximizer.run(
                    cur_bits, None, False, node, opt_state,
                    progress=lambda cur, step, direct: progress(
                        f'bits {cur} step {step} direction {direct}', True),
                    start_step=cur_step,
                    dir_start=dir_start)
                del sizes[tune_idx]
                if res is None:
                    break
                if quantize and opt_state['qmode'] == QuantizationMode.none():
                    if progress:
                        progress('analysing quantized', True)
                    opt_state['qmode'] = QuantizationMode.all_dequantize()
                elif opt_state['cur_inputs'] != base_inputs:
                    if progress:
                        progress('check with all inputs', True)
                else:
                    break
                opt_state['final'] = True
                opt_state['cur_inputs'] = base_inputs.copy()
                cur_bits = res[1]
                cur_step = 1
                dir_start = 'up'

            if res is None:
                if progress:
                    progress(f'{node.name} cannot be further optimised', True)
                self.tune_bits(
                    [node],
                    comp_val.bits,
                    progress=lambda _, comp: progress('+' if comp else '-', False))
            else:
                if progress:
                    progress(
                        f'{node.name} tune bits to {res[1]} compressed by {res[0]} bytes', True)
                self.tune_bits(
                    [node],
                    res[1],
                    progress=lambda _, comp: progress('+' if comp else '-', False))
                if progress:
                    progress('', True)

    def tune_all(self, nodes, progress=None, quantize=False):
        base_inputs = self.get_base_inputs(nodes, progress, quantize)
        if not base_inputs:
            raise ValueError('none of the input samples validates correct')

        def opt_func(qsnr, state):
            if progress:
                progress('compressing: ', False)
            compression = self.tune_qsnr(
                nodes,
                qsnr,
                progress=(lambda _, comp: progress('+' if comp else '-', False)) if progress else None)
            if not compression or ('best_compression' in state and state['best_compression'] > compression):
                if qsnr == 0:
                    raise CompressionError("could not compress graph")
                return None
            state['best_compression'] = compression
            if progress:
                progress('', True)
                progress('validating: ', False)
            self._labels_and_inputs.set_filter(state['cur_inputs'])
            good_inputs, good_margin, bad_inputs, bad_margin = self.validate(
                state['qmode'],
                progress=(lambda pred: progress('+' if pred else '-', False)) if progress else None)
            if progress:
                progress('', True)
                progress(
                    f'good {len(good_inputs)} ({good_margin:.2f}) bad {len(bad_inputs)} ({bad_margin:.2f})', True)
            if bad_inputs:
                if not state['final']:
                    state['cur_inputs'] = bad_inputs
                del state['best_compression']
                return None
            return compression

        qmode = QuantizationMode.none()
        dir_start = 'down'

        opt_state = {
            'cur_inputs': base_inputs.copy(),
            'final': False,
            'qmode': qmode
        }
        start_qsnr = 30
        start_step = 15
        maximizer = Maximizer(opt_func, 0, 120)
        while True:
            res = maximizer.run(
                start_qsnr, opt_state,
                progress=(lambda cur, step, direct: progress(
                    f'QSNR {cur} step {step} direction {direct}', True)) if progress else None,
                start_step=start_step,
                dir_start=dir_start)
            if quantize and opt_state['qmode'] == QuantizationMode.none():
                if progress:
                    progress('analysing quantized', True)
                opt_state['qmode'] = QuantizationMode.all_dequantize()
            elif opt_state['cur_inputs'] != base_inputs:
                if progress:
                    progress('check with all inputs', True)
                opt_state['final'] = True
            else:
                break
            opt_state['cur_inputs'] = base_inputs.copy()
            start_qsnr = res[1]
            start_step = 0.5
            dir_start = 'up'

        if progress:
            progress(
                f'tune QSNR to best {res[1]} compressed by {res[0]} bytes', True)
        self.tune_qsnr(
            nodes,
            res[1],
            progress=(lambda _, comp: progress('+' if comp else '-', False)) if progress else None)
        if progress:
            progress('', True)

        return res[1]

    def get_base_inputs(self, nodes, progress, quantize):
        if self._base_inputs is None:
            for node in nodes:
                node.use_compressed = False
            if progress:
                progress(
                    f"validation without compression {'quantized: ' if quantize else ': '}", False)
            self._labels_and_inputs.set_filter(None)
            base_inputs, good_margin, bad_inputs, bad_margin = self.validate(
                QuantizationMode.all_dequantize() if quantize else QuantizationMode.none(),
                labels_and_inputs=self._labels_and_inputs,
                progress=lambda pred: progress('+' if pred else '-', False)) if progress else None
            if progress:
                progress('', True)
                progress(
                    f'good {len(base_inputs)} ({good_margin:.2f}) bad {len(bad_inputs)} ({bad_margin:.2f})', True)
            self._base_inputs = base_inputs
        else:
            base_inputs = self._base_inputs
        return base_inputs
