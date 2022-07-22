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

import importlib
import numpy as np
from nntool.execution.graph_executer import GraphExecuter
from nntool.graph.dim import Dim
from nntool.graph.types import ConstantInputNode, InputNode
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op, partial_support, ps_description


@onnx_op("Loop")
@partial_support(True)
@ps_description("Loops are not supported.")
class Loop(BackendHandler, ConstantMixin):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']

        inputs = [all_nodes[inp] for inp in node.input]

        if not all(cls.is_constant(inp) for inp in inputs):
            raise NotImplementedError(
                "nntool does not support import of graphs with evaluated loops")

        importer = kwargs['importer']
        NNGraph = importlib.import_module('nntool.graph.nngraph').NNGraph
        subgraph = NNGraph()
        all_nodes_clone = all_nodes.copy()
        importer.import_subgraph(
            subgraph, node.attrs['body'], {}, all_nodes=all_nodes_clone)
        if not all(isinstance(inp, (InputNode, ConstantInputNode)) for inp in subgraph.inputs()):
            raise NotImplementedError(
                "nntool does not support import of graphs with evaluated loops")
        subgraph.add_dimensions()
        for idx, inp in enumerate(subgraph.inputs()):
            inp.index = idx

        logger.info(f"reducing loop {valid_name} to a constant")
        count = inputs[0][0].value
        keep_going = inputs[1][0].value
        loop_carried = [inp[0].value for inp in inputs[2:]]
        outputs = [np.array([])] * len(node.output)
        while keep_going and count > 0:
            executer = GraphExecuter(subgraph)
            output_tensors = executer.execute(
                [count, keep_going] + loop_carried, silent=True)
            outp_vals = [output_tensors[node.step_idx][0]
                         for node in subgraph.outputs() if not isinstance(node, InputNode)]
            keep_going = outp_vals[0]
            for idx, val in enumerate(outp_vals[1:]):
                if idx < len(loop_carried):
                    loop_carried[idx] = outputs[idx] = val
                elif outputs[idx] is None:
                    outputs[idx] = val
                else:
                    outputs[idx] = np.concatenate((outputs[idx], val))
            count -= 1
        for idx, outp in enumerate(node.output):
            params = ConstantInputNode(G.unique_name(
                f'{valid_name}_out{idx}'), value=outputs[idx], dims=Dim.unnamed(outputs[idx].shape))
            all_nodes[outp] = (params, 0, ProvisionalDim(
                outputs[idx].shape), None)

        return None

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
