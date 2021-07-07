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
from abc import abstractproperty

from graph.types import (ActivationFusion, ActivationParameters,
                         GlobalPoolingParameters, HSigmoidActivationParameters,
                         HSwishActivationParameters, LeakyActivationParameters,
                         MatMulOpFusionParameters, MatMulOpParameters,
                         MatrixAddParameters, MatrixMulParameters, NNEdge,
                         PoolingParameters, ReluActivationParameters,
                         SigmoidActivationParameters)
from graph.types.base import Transposable
from quantization.new_qrec import QRec
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, description, groups, match_name, run_after

LOG = logging.getLogger("nntool." + __name__)


VALID_GEN_ACTIVATIONS_POW2 = (
    ReluActivationParameters,
)

VALID_GEN_ACTIVATIONS_SQ8 = (
    ReluActivationParameters,
    LeakyActivationParameters,
    HSigmoidActivationParameters,
    HSwishActivationParameters,
    SigmoidActivationParameters
)

VALID_MATMUL_ACTIVATIONS_POW2 = (
    ReluActivationParameters,
    LeakyActivationParameters,
    HSigmoidActivationParameters,
    HSwishActivationParameters,
    SigmoidActivationParameters
)

VALID_GEN_ACTIVATIONS_FLOAT = ()

VALID_FUSIONS = {
    PoolingParameters: {
        'scaled': (VALID_GEN_ACTIVATIONS_SQ8, ActivationFusion),
        'symmetric': (VALID_GEN_ACTIVATIONS_POW2, ActivationFusion),
        'float': (VALID_GEN_ACTIVATIONS_FLOAT, ActivationFusion),
    },
    GlobalPoolingParameters: {
        'scaled': (VALID_GEN_ACTIVATIONS_SQ8, ActivationFusion),
        'symmetric': (VALID_GEN_ACTIVATIONS_POW2, ActivationFusion),
        'float': (VALID_GEN_ACTIVATIONS_FLOAT, ActivationFusion),
    },
    MatrixAddParameters: {
        'scaled': (VALID_GEN_ACTIVATIONS_SQ8, ActivationFusion),
        'symmetric': (VALID_GEN_ACTIVATIONS_POW2, ActivationFusion),
        'float': (VALID_GEN_ACTIVATIONS_FLOAT, ActivationFusion),
    },
    MatrixMulParameters: {
        'scaled': (VALID_GEN_ACTIVATIONS_SQ8, ActivationFusion),
        'symmetric': (VALID_GEN_ACTIVATIONS_POW2, ActivationFusion),
        'float': (VALID_GEN_ACTIVATIONS_FLOAT, ActivationFusion),
    },
    MatMulOpParameters: {
        'scaled': (VALID_GEN_ACTIVATIONS_SQ8, MatMulOpFusionParameters),
        'symmetric': (VALID_GEN_ACTIVATIONS_POW2, MatMulOpFusionParameters),
        'float': (VALID_GEN_ACTIVATIONS_FLOAT, MatMulOpFusionParameters),
    }
}


class FusionMatch():
    def __init__(self, default_ktype) -> None:
        self.node = None
        self.active = None
        self.default_ktype = default_ktype
        self.match_rec = None
        self.fusions_class = None
        self.order = []

    def add_node(self, G, params):
        if isinstance(params, Transposable) and params.has_transpose:
            return None
        if self.node is None:
            if params.__class__ in VALID_FUSIONS:
                self.node = params
                nid = NodeId(params)
                if G.quantization and nid in G.quantization:
                    qrec = G.quantization[nid]
                    current_ktype = qrec.ktype
                else:
                    current_ktype = self.default_ktype
                if current_ktype not in VALID_FUSIONS[params.__class__]:
                    return None
                match_rec = VALID_FUSIONS[params.__class__][current_ktype]
                if len(match_rec) == 3:
                    if not match_rec[2](params):
                        return None
                self.match_rec = match_rec
                self.order.append(params)
                return self
            return None
        if params.__class__ not in self.match_rec[0]:
            return None
        self.order.append(params)
        self.active = params
        self.fusions_class = self.match_rec[1]
        return self

    @property
    def fusion_type(self):
        return '_'.join([params.CLS_OP_NAME if not isinstance(params, ActivationParameters)
                         else 'active' for params in self.order])


@run_after('fuse_gap_pool', 'fuse_external_bias_matbul')
class MatchOpActivation(Matcher):

    @abstractproperty
    def _default_ktype(self):
        raise NotImplementedError()

    def get_node_list(self, G, params, result):
        if not result.add_node(G, params):
            return result
        out_edges = G.out_edges(params.name)
        if len(out_edges) > 1:
            return result
        return self.get_node_list(G, out_edges[0].to_node, result)

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False

        for node in G.nodes(node_classes=tuple(VALID_FUSIONS.keys())):
            node_list = self.get_node_list(
                G, node, FusionMatch(self._default_ktype))
            if node_list is None or len(node_list.order) < 2:
                continue
            LOG.info("fusing nodes %s", ",".join(
                (node.name for node in node_list.order)))
            has_modified_graph = True
            subgraph = GraphView()
            last_node = None
            for snode in node_list.order:
                if last_node is not None:
                    subgraph.add_edge(
                        NNEdge(from_node=last_node, to_node=snode))
                last_node = snode
            # assumption here is that the first node could have multiple inputs but definitely has only
            # one output
            input_mapping = [[(node_list.node, idx)]
                             for idx in range(G.num_in_edges(node_list.node.name))]
            output_mapping = [(last_node, 0)]
            pnode = node_list.fusions_class(
                node_list.node.name + '_fusion',
                fusion_type=node_list.fusion_type,
                subgraph=subgraph,
                input_mapping=input_mapping,
                output_mapping=output_mapping)
            if G.quantization:
                # if there are quantization stats then clear them. They need to be created again
                G.quantization.stats = None
                qrecs = G.quantization.get_all(pnode.contained_nodes())
                if qrecs:
                    prec = QRec.copy_ktype(
                        qrecs[0], in_qs=qrecs[0].in_qs, out_qs=qrecs[-1].out_qs)
                    for fnode in pnode.contained_nodes():
                        G.quantization.move_to_fusion(fnode, pnode)
                    G.quantization[NodeId(pnode)] = prec
            in_edges = G.in_edges(node_list.node.name)
            out_edges = G.out_edges(last_node.name)
            for snode in node_list.order:
                G.remove(snode)
            for edge in in_edges:
                G.add_edge(NNEdge(edge.from_node, pnode,
                                  from_idx=edge.from_idx, to_idx=edge.to_idx))
            for edge in out_edges:
                G.add_edge(NNEdge(pnode, edge.to_node,
                                  from_idx=edge.from_idx, to_idx=edge.to_idx))

        if set_identity:
            self.set_identity(G)

        return has_modified_graph


@groups('scaled')
@match_name("fuse_op_activation_scale8")
@description('Fuse non-filter nodes and activations to match GAP AutoTiler SQ8 kernels')
class MatchOpActivationScaleKernels(MatchOpActivation):

    @property
    def _default_ktype(self):
        return 'scaled'


@groups('symmetric')
@match_name("fuse_op_activation_pow2")
@description('Fuse non-filter nodes and activations to match GAP AutoTiler POW2 kernels')
class MatchOpActivationPow2Kernels(MatchOpActivation):

    @property
    def _default_ktype(self):
        return 'symmetric'
