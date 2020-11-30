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
from graph.dim import FcFilterDim
from graph.types import FcParameters
from graph.types.base import NNEdge
from importer.common.provisional_dim import ProvisionalDim
from importer.tflite2.common import check
from importer.tflite2.handlers.backend.filter_mixin import FilterMixin
from importer.tflite2.tflite_schema_head.FullyConnectedOptions import \
    FullyConnectedOptions
from utils.sparse_list import SparseList

from ..backend_handler import BackendHandler
from ..handler import partial_support, ps_description, tflite_op


@tflite_op("FULLY_CONNECTED")
@partial_support(True)
@ps_description("weights_format and asymmetric_quantize_inputs parameters are ignored")
class FullyConnected(FilterMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        node_opts = node.get_options(FullyConnectedOptions)
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]

        x = inputs[0]
        x_shape = x[2].shape
        x_known_shape = x[2].known_shape
        inp_sz = np.prod(np.array(x_known_shape))
        weights = inputs[1]
        weights_shape = weights[2].shape
        out_c = weights_shape[0]

        filt_dim = FcFilterDim(weights_shape[0], *x_known_shape)
        node.input[1].used = True
        check(filt_dim.sz == inp_sz, "filter doesn't match input size")

        if len(node.input) > 2:
            node.input[2].used = True

        keep_dims = node_opts.KeepNumDims()

        in_hint = [str(i) for i in range(len(x_known_shape) - 1)] + ['c']
        out_hint = in_hint.copy() if keep_dims else ['c']

        params = FcParameters(node.name, filt=filt_dim, has_bias=True,
                              in_dims_hint=SparseList([in_hint]),
                              out_dims_hint=SparseList([out_hint]),
                              constant_store=G.constant_store,
                              keep_dims=keep_dims)

        if opts.get('load_dequantized'):
            cls.load_dequantized_filter_parameters(params, node.input)
        else:
            cls.load_filter_parameters(G, params, node.input, node.output, opts)

        if x_shape[0] is None:
            out_shape = x_shape[:-1:] + [out_c] if keep_dims else [x_shape[0], out_c]
        else:
            out_shape = x_known_shape[:-1:] + [out_c] if keep_dims else [out_c]
        pout_dims = ProvisionalDim(out_shape)

        G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        aparams = cls.fuse_activation(node_opts, node.name, params, **kwargs)
        all_nodes[node.output[0]] = (aparams, 0, pout_dims)
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_2(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_5(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_7(cls, node, **kwargs):
        return cls._common(node, **kwargs)
