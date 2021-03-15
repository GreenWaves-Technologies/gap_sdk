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


from graph.types.input_output import ConstantInputParameters
from graph.dim import Dim
from graph.types import (NNEdge, NoOPParameters, ReshapeParameters,
                         StridedSliceParameters)
from importer.common.constant_mixin import ConstantMixin
from importer.tflite2.common import LOG
from importer.tflite2.common.tflite_node import TFLiteNode
from importer.tflite2.tflite_schema_head.StridedSliceOptions import \
    StridedSliceOptions
from utils.node_id import NodeId

from ..backend_handler import BackendHandler
from ..handler import tflite_op


@tflite_op("STRIDED_SLICE")
class StridedSlice(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        node_opts = node.get_options(StridedSliceOptions)
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        x_shape = x[2].shape

        # begin end stride
        vec_begin = list(cls._verify_constant(inputs[1]))
        vec_end = list(cls._verify_constant(inputs[2]))
        vec_stride = list(cls._verify_constant(inputs[3]))
        for i in range(1, 4):
            node.input[i].used = True
        if any([vec is None for vec in [vec_begin, vec_end, vec_stride]]):
            raise NotImplementedError(
                "strided slice with variable begin end or stride is not supported")
        spec = zip(vec_begin, vec_end, vec_stride)
        begin_mask = node_opts.BeginMask()
        ellipsis_mask = node_opts.EllipsisMask()
        end_mask = node_opts.EndMask()
        new_axis_mask = node_opts.NewAxisMask()
        shrink_axis_mask = node_opts.ShrinkAxisMask()

        act_slice, out_shape, can_reshape = StridedSliceParameters.get_slice(
            x_shape, spec,
            begin_mask,
            end_mask, ellipsis_mask,
            new_axis_mask, shrink_axis_mask)

        if cls.is_constant(x):
            LOG.info("reducing %s to a constant", node.name)
            x_val = cls.get_constant(x)
            params = StridedSliceParameters(node.name, act_slice=act_slice, out_shape=out_shape)
            x_val = params.numpy_slice(x_val)
            params = ConstantInputParameters(node.name, value=x_val, constant_store=G.constant_store)
        else:
            if can_reshape:
                if list(x_shape) == list(out_shape):
                    LOG.info("converting strided slice %s to a noop", node.name)
                    params = NoOPParameters(node.name)
                else:
                    LOG.info("converting strided slice %s to a reshape", node.name)
                    in_shape = Dim.unnamed(x[2].known_shape, is_ordered=True)
                    out_shape = Dim.unnamed(out_shape, is_ordered=True)
                    params = ReshapeParameters(node.name, old_shape=in_shape, shape=out_shape)
            else:
                params = StridedSliceParameters(node.name, act_slice=act_slice, out_shape=out_shape)
            G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))

        if opts.get('load_quantization'):
            G.quantization[NodeId(params)] = cls.load_tf_quantization([node.input[0]], node.output)
        all_nodes[node.output[0]] = (params, 0, x[2].infer_mapping(out_shape, allow_bad_length=True))
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
