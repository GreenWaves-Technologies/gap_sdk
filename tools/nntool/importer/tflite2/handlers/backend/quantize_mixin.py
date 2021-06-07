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

from copy import deepcopy
from graph.types.others import NoOPParameters

from graph.dim import Dim
from graph.types import ConstantInputParameters, NNEdge, QuantizeParameters
from importer.common.constant_mixin import ConstantMixin
from importer.tflite2.common import LOG
from quantization.new_qrec import QRec
from utils.node_id import NodeId

class QuantizeMixin(ConstantMixin):
    @classmethod
    def common_quantize(cls, in_qtype, out_qtype, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        opts = kwargs['opts']
        G = kwargs['G']
        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        in_qtype = in_qtype.make_symmetric_signed()
        out_qtype = out_qtype.make_symmetric_signed()
        if cls.is_constant(x):
            LOG.info("reducing %s to a constant", node.name)
            if out_qtype:
                val = x[0].value_as(out_qtype)
            else:
                val = cls.get_constant(x)
            params = ConstantInputParameters(node.name,
                                             value=val,
                                             dims=Dim.unnamed(val.shape),
                                             qtype=out_qtype,
                                             constant_store=G.constant_store)
            if opts.get('load_quantization'):
                G.quantization[NodeId(params)] = QRec.scaled(
                    in_qs=[out_qtype], out_qs=[out_qtype])
        else:
            if in_qtype == out_qtype:
                LOG.info('removing (de)quantize node %s with no effect', node.name)
                params = NoOPParameters(node.name, desc="quantize with no effect")
            elif in_qtype.dtype == out_qtype.dtype:
                LOG.info('removing (de)quantize node %s with scale change', node.name)
                params = NoOPParameters(node.name, desc="quantize with scale change")
                out_qtype = in_qtype
            else:
                params = QuantizeParameters(
                    node.name, from_qtype=in_qtype, to_qtype=out_qtype)
            G.add_edge(
                NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))

            if opts.get('load_quantization'):
                G.quantization[NodeId(params)] = QRec.scaled(
                    in_qs=[in_qtype], out_qs=[out_qtype])
        all_nodes[node.output[0]] = (params, 0, deepcopy(x[2]))
        return params
