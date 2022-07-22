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

import numpy as np
from nntool.graph.types import ConstantInputNode, NNEdge
from nntool.graph.types.misc import NoOPNode, PadNode
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op, partial_support, ps_description


@onnx_op("Pad")
@partial_support(True)
@ps_description("Only constant pad amounts are supported."
                "Only 2D padding is supported. Padding must be successfully fused into a "
                "convolution or pooling operation to generate successfully. There is currently "
                "no AutoTiler Pad generator.")
class Pad(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, mode='constant', pads=None, constant_value=0, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape

        ndim = len(x_shape)
        npad = len(pads)//2
        if npad != ndim:
            if all(not pad for pad in pads):
                logger.warning(f'Pad {valid_name} has {npad} pad values and {ndim} input rank. '
                               'Since pad is zero this is ignored but it probably indicates a bug in the ONNX graph.')
                params = NoOPNode(valid_name, desc="eliminated pad of 0")
                pshape = x_shape
                G.add_edge(
                    NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
                all_nodes[node.output[0]] = (params, 0, ProvisionalDim(pshape), x[3])
                return params
            else:
                raise ValueError(
                    f'Eroor in ONNX graph - pad {valid_name} has {npad} pad values and {ndim} input rank.')
        apads = np.array([[pads[idx], pads[idx+ndim]] for idx in range(ndim)])
        # apads = np.array(pads).reshape((-1, 2))
        if cls.is_constant(x):
            logger.info("reducing %s to a constant", valid_name)
            val = cls.get_constant(x)
            if mode == 'constant':
                val = np.pad(val, apads, mode=mode,
                             constant_values=constant_value)
            else:
                val = np.pad(val, apads, mode=mode)
            params = ConstantInputNode(valid_name, value=val)
            pshape = [
                dim + sum(apads[idx]) if dim is not None else None for idx, dim in enumerate(x_shape)]
            all_nodes[node.output[0]] = (
                params, 0, ProvisionalDim(pshape), x[3])
            return params

        if mode != 'constant':
            raise ValueError('%s - pad mode %s is not supported' %
                             (valid_name, mode))

        if any(sum(pad) > 0 and x_shape[idx] is None for idx, pad in enumerate(apads)):
            raise ValueError(
                f'unknown/batch axis is being padded in {valid_name}. Manipulation of '
                'unknown/batch axis is not supported')
        trimmed_pads = tuple(
            [pad for idx, pad in enumerate(apads) if x_shape[idx] is not None])

        if all(sum(trimmed_pad) == 0 for trimmed_pad in trimmed_pads):
            params = NoOPNode(valid_name, desc="eliminated pad of 0")
            pshape = x_shape
        else:
            pshape = [
                dim + sum(apads[idx]) if dim is not None else None for idx, dim in enumerate(x_shape)]
            # pshape = [None if dim is None else dim + sum(apads[idx]) for idx, dim in enumerate(x_shape)]
            padvals = [(constant_value, constant_value)] * len(trimmed_pads)
            params = PadNode(
                valid_name, padding=trimmed_pads, pad_vals=padvals)
        G.add_edge(
            NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(pshape), x[3])
        return params

    @classmethod
    def _args_version(cls, node, padkey='pads', **kwargs):
        mode = node.attrs.get('mode', 'constant')
        pads = node.attrs.get(padkey)
        value = node.attrs.get('value')
        return cls._common(node, mode=mode, pads=pads, constant_value=value, **kwargs)

    @classmethod
    def _inputs_version(cls, node, **kwargs):
        mode = node.attrs.get('mode', 'constant')
        if len(node.input) > 1 and node.input[1]:
            y = kwargs['all_nodes'][node.input[1]]
            pads = tuple(cls.get_constant(y))
            if len(node.input) > 2 and node.input[2]:
                z = kwargs['all_nodes'][node.input[2]]
                value = cls.get_constant(z)
            else:
                value = 0
        else:
            raise ValueError('no pads input for node %s' %
                             kwargs['valid_name'])
        return cls._common(node, mode=mode, pads=pads, constant_value=value, **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._args_version(node, padkey='paddings', **kwargs)

    @classmethod
    def version_2(cls, node, **kwargs):
        return cls._args_version(node, padkey='pads', **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._inputs_version(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._inputs_version(node, **kwargs)
