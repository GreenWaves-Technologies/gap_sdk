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

import logging
from typing import Mapping, Optional, Sequence, Union, Tuple

import numpy as np
from nntool.graph.types.fusions import FusionNodeBase
from nntool.quantization.new_qrec import QRec
from nntool.utils.graph import Graph, Node
from nntool.execution.graph_executer import GraphExecuter
from nntool.graph.types import (ActivationFusionNodeBase,
                         ConstantInputNode, FilterFusionNodeBase,
                         FusionInputNode, FusionOutputNode, OutputNode,
                         InputNode, MatMulOpFusionNode,
                         PaddedAddFusionNode, NNNodeBase)
from nntool.execution.kernels.kernel_executer import KernelExecuter
from nntool.execution.quantization_mode import QuantizationMode

LOG = logging.getLogger(__name__)


class FastGraphExecuter():
    def __init__(self,
                 G: Graph,
                 qrecs: Optional[Mapping[Union[str, Tuple], QRec]] = None):
        self._G = G
        self._qrecs = qrecs

    @staticmethod
    def collect_outputs(saved_outputs, G: Graph, node: Node):
        # collect outputs from previous nodes
        # InputNode is already set above
        if isinstance(node, (InputNode, FusionInputNode, ConstantInputNode)):
            inputs = None
        else:
            inputs = [None]*len(node.in_dims)
            for i, edge in enumerate(G.indexed_in_edges(node.name)):
                inputs[i] = saved_outputs[edge]
                del saved_outputs[edge]
        return inputs

    @staticmethod
    def save_output(saved_outputs, G: Graph, node: Node, outputs):
        if isinstance(node, (OutputNode, FusionOutputNode)):
            saved_outputs[node] = outputs
        else:
            for edge in G.out_edges(node):
                saved_outputs[edge] = outputs[edge.from_idx]


    def execute_iterator(self,
                         in_tensors: Sequence[np.ndarray],
                         qmode: QuantizationMode = QuantizationMode.none(),
                         parent_node=None,
                         saved_outputs={},
                         G=None):
        if G is None:
            G = self._G

        inputs = sorted([node for node in G.inputs() if isinstance(node, (InputNode, FusionInputNode))], key=lambda node: node.name)
        inputs.extend(node for node in G.inputs() if isinstance(node, ConstantInputNode))
        for node in G.topological_sort(inputs):
            step_idx = node.step_idx

            # collect outputs from previous nodes
            # InputNode is already set above
            output_tensors = self.collect_outputs(saved_outputs, G, node)

            if parent_node:
                nid = (parent_node.name, node.name)
            else:
                nid = node.name

            if isinstance(node, (FusionInputNode, FusionOutputNode)):
                qrec = None
            else:
                if self._qrecs and qmode.get_quantized(node, step_idx):
                    if nid not in self._qrecs:
                        LOG.warning(
                            "no quantization parameters on %s", node.name)
                        qrec = None
                    else:
                        qrec = self._qrecs[nid]
                else:
                    qrec = None

            if isinstance(node, (FilterFusionNodeBase, ActivationFusionNodeBase, PaddedAddFusionNode, MatMulOpFusionNode)):

                self.execute_iterator(
                        output_tensors,
                        qmode=qmode,
                        parent_node=node,
                        G=node.subgraph,
                        saved_outputs=saved_outputs
                )
                f_outputs = node.subgraph.outputs()
                num_outputs = max(f_output.idx for f_output in f_outputs) + 1
                output_tensors = [None]*num_outputs
                for f_output in f_outputs:
                    output_tensors[f_output.idx] = saved_outputs[f_output][0]
                    del saved_outputs[f_output][0]

            elif isinstance(node, (InputNode, FusionInputNode)):
                output_tensors = KernelExecuter.execute(
                    node, in_tensors, qrec, details=None)
            else:
                output_tensors = KernelExecuter.execute(
                    node, output_tensors, qrec, details=None)

            self.save_output(saved_outputs, G, node, output_tensors)
        return saved_outputs

    def execute(self,
                in_tensors: Sequence[np.ndarray],
                qmode: QuantizationMode = None):

        if qmode is None:
            qmode = QuantizationMode.none()

        outputs_dict = self.execute_iterator(in_tensors, qmode)
        outputs = []
        for out_node in self._G.outputs():
            outputs.append(outputs_dict[out_node])

        return outputs
