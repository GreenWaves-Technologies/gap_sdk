
# Copyright (C) 2020  GreenWaves TechnoLOGies, SAS

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

import os
from copy import deepcopy

from graph.constant_store import ConstantStore
from graph.dim import Dim
from graph.matches.matchers.duplicate_constants import MatchDuplicateConstants
from graph.matches.matchers.remove_quantize_operators import \
    RemoveQuantizeOperators
from graph.nngraph import NNGraph
from graph.types import (ConcatParameters, ConstantInputParameters, NNEdge,
                         SigmoidActivationParameters, SoftMaxParameters,
                         SplitParameters)
from importer.common.clean_dangling_nodes import clean_dangling_nodes
from importer.common.get_reasonable_name import get_reasonable_name
from importer.tflite2.common.tflite_graph import TFLiteGraph
from importer.tflite2.tflite_schema_head.Model import Model
from quantization.new_qrec import QRec
from quantization.quantization_set import QuantizationSet
from quantization.unified_quantizer import UnifiedQuantizer
from utils.add_sys_path import add_sys_path
from utils.node_id import NodeId

from ..common.provisional_dim import ProvisionalDim
from ..importer_base import ImporterBase
from .common import LOG, check
from .common.handler_helper import get_all_backend_handlers
from .fix_split_in_edges import fix_split_in_edges
from .remove_concats import remove_concats

# pylint: disable=E1101


class TFLiteImporter(ImporterBase):
    def __init__(self, *args, **kwargs) -> None:
        super(TFLiteImporter, self).__init__(*args, **kwargs)
        self._name_cache = {}
        self._provisional_outputs = None

    @property
    def provisional_outputs(self):
        return self._provisional_outputs

    @staticmethod
    def find_nodes_with_bad_quantization(G):
        if G.quantization is None:
            raise ValueError('can only be called on a quantized graph')
        qrecs = G.quantization
        res = set()
        for edge in G.edges():
            nodes = [edge.from_node, edge.to_node]
            eqrecs = tuple(qrecs.get(NodeId(node)) for node in nodes)
            # any node with a missing qrec is added to the list
            missing_qrecs = False
            for node, qrec in zip(nodes, eqrecs):
                if qrec is None:
                    res.add(node)
                    missing_qrecs = True
            if missing_qrecs:
                continue
            # check if the edge input and output have the same quantization
            from_qrec, to_qrec = eqrecs
            if from_qrec.out_qs[edge.from_idx] != to_qrec.in_qs[edge.to_idx]:
                res |= set(nodes)
        return res


    def create_graph(self, filename, opts):
        opts = self.get_opts(opts)
        self._name_cache = {}
        add_sys_path(os.path.dirname(__file__))
        buf = open(filename, "rb").read()
        model = Model.GetRootAsModel(buf, 0)
        LOG.info("Importing TFLITE model version %s", model.Version())
        check(model.Version() == 3, "Only support version 3 graphs at present")
        if model.SubgraphsLength() > 1:
            LOG.warning(
                "nntool only supports one subgraph. There may be errors loading this graph.")
        G = NNGraph(model=model,
                    filename=filename,
                    name=opts.get('name'),
                    constant_store=ConstantStore())
        if opts.get('load_quantization'):
            G.quantization = QuantizationSet()
            G.has_quantized_parameters = True
            G.quantization.schemes_present.add('SQ8')

        self._import_tflite_graph(G, model, opts)
        clean_dangling_nodes(G)
        fix_split_in_edges(G)
        MatchDuplicateConstants().match(G)
        # DrawGraphReporter().report(G)
        G.add_dimensions()
        remove_concats(G)
        if opts['remove_quantize_ops']:
            RemoveQuantizeOperators().match(G)
            G.add_dimensions()

        if opts.get('load_quantization'):
            # get rid of qrecs on nodes that were not used
            to_remove = []
            for nid in G.quantization:
                if nid.node_name not in G:
                    to_remove.append(nid)
            for nid in to_remove:
                del G.quantization[nid]
            nodes_with_bad_quantization = self.find_nodes_with_bad_quantization(G)
            quantizer = UnifiedQuantizer.from_quantized_graph(G)
            # check for quantization problems
            # 1) need to force softmax/Sigmoid input to POW2 quantization
            # 2) need to check that all concats and splits have same input and
            #    output quantization
            # 3) Need to check that all nodes have qrecs and that they are consistent
            nodes_with_bad_quantization |= set(G.nodes(node_classes=(
                ConcatParameters,
                SoftMaxParameters,
                SplitParameters,
                SigmoidActivationParameters)))
            G.quantization = quantizer.quantize(G, start_nodes=nodes_with_bad_quantization)
            G.add_dimensions()

        return G

    def _import_tflite_graph(self, G: NNGraph, model, opts: dict):
        name_cache = set()
        graph = TFLiteGraph.from_model(model, 0, anonymise=opts.get('anonymise'), name_cache=name_cache)
        handlers = self._get_handlers(graph.model_version)
        all_nodes = {}
        constants = self._get_all_constants(
            G, graph.tensors, load_quantization=opts.get('load_quantization'),
            name_cache=name_cache, anonymise=opts.get('anonymise'))
        all_nodes.update(constants)
        inputs = self._get_input_nodes(
            G, graph.input, load_quantization=opts.get('load_quantization'))
        all_nodes.update(inputs)
        self._provisional_outputs = self._get_output_nodes(
            G, graph.output, load_quantization=opts.get('load_quantization'))
        self._import_nodes(G, graph, handlers, all_nodes,
                           self._provisional_outputs, opts)
        return G

    @staticmethod
    def _get_dim_from_shape(tf_shape):
        return Dim.unnamed([d.dim_value for d in tf_shape.dim
                            if (d.dim_value > 0 and d.dim_param == "")])

    def _get_handlers(self, graph_version):
        return get_all_backend_handlers(graph_version)

    @staticmethod
    def _load_quantization(qrecs, node_recs):
        for tensor in node_recs:
            qtype = tensor.qtype
            if qtype:
                qtype = qtype.make_symmetric_signed()
                setattr(qtype, 'is_input', True)
                qrecs[NodeId(node_recs[tensor][0])] = QRec.scaled(
                    in_qs=[qtype], out_qs=[qtype])

    def _get_all_constants(self, G, tensors, load_quantization=False, anonymise=False, name_cache=None):
        node_recs = {
            tensor: (
                ConstantInputParameters(
                    get_reasonable_name(
                        tensor.name, name_cache=name_cache, anonymise=anonymise),
                    dims=Dim.unnamed(tensor.shape),
                    value=tensor.value if load_quantization else tensor.dqvalue,
                    qtype=tensor.qtype if load_quantization else None,
                    constant_store=G.constant_store),
                0,
                ProvisionalDim.from_tflite_shape(tensor.shape)
            )
            for tensor in tensors if tensor.is_constant
        }
        if load_quantization:
            self._load_quantization(G.quantization, node_recs)

        return node_recs

    def _get_input_nodes(self, G, inputs, load_quantization=False):
        prov_dims = {
            idx: ProvisionalDim.from_tflite_shape(inp.shape, check_for_batch=0)
            for idx, inp in enumerate(inputs)
        }
        hints = {
            idx: (['h', 'w', 'c'] if (len(dim.shape) == 4 and
                                      (dim.shape[-1] == 1 or dim.shape[-1] == 3))
                  else None)
            for idx, dim in prov_dims.items()
        }
        node_recs = {
            inp: (
                G.add_input(Dim.unnamed(prov_dims[idx].known_shape).apply_naming_hints(hints[idx]),
                            in_dim_hint=[hints[idx]] if hints[idx] else None,
                            out_dim_hint=[hints[idx]] if hints[idx] else None),
                0,
                prov_dims[idx]
            )
            for idx, inp in enumerate(inputs)
        }
        if load_quantization:
            self._load_quantization(G.quantization, node_recs)
        return node_recs

    def _get_output_nodes(self, G, outputs, load_quantization=False):
        node_recs = {
            outp: (
                G.add_output(),
                0,
                ProvisionalDim.from_tflite_shape(outp.shape)
            )
            for outp in outputs
        }
        if load_quantization:
            self._load_quantization(G.quantization, node_recs)
        return node_recs

    def _import_nodes(self, G, graph, handlers, all_nodes, outputs, opts):
        for node in graph.nodes:
            handler = handlers.get(node.op_name, None)
            if not handler:
                raise ValueError("no handler found for %s" % node.op_type)
            if node.is_custom and handler:
                handler = handler.get(node.custom_op_name, None)
                if not handler:
                    raise ValueError("no handler found for custom operation %s" %
                                     node.custom_op_name)

            params = handler.handle(
                node, all_nodes=all_nodes, G=G, opts=opts, importer=self)
            if params is None:
                continue
            for idx, out_tensor in enumerate(node.output):
                output = outputs.get(out_tensor)
                if not output:
                    continue
                G.add_edge(NNEdge(from_node=params,
                                  to_node=output[0], from_idx=idx, to_idx=output[1]))
                if opts.get('load_quantization'):
                    qtype = deepcopy(
                        G.quantization[NodeId(params)].out_qs[idx])
                    G.quantization[NodeId(output[0])] = QRec.scaled(
                        in_qs=[qtype],
                        out_qs=[qtype]
                    )
