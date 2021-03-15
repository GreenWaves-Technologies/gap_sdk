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

from graph.matches.matcher import Matcher
from graph.types import (HSigmoidActivationParameters,
                         HSwishActivationParameters,
                         SigmoidActivationParameters)
from quantization.qtype import QType
from utils.graph import GraphView
from utils.node_id import NodeId

from .equalize_sym_mult_concats import propagate_qtype_up


class PropagateUpSigSwishInputQ(Matcher):
    NAME = "propagate_up_swish"
    DESCRIPTION = """hard sigmoid and hard swish require 6 to be representable"""

    def match(self, G: GraphView, set_identity: bool = True):
        if not G.quantization:
            return
        sigs_swishes = [node for node in G.nodes() if isinstance(
            node, (SigmoidActivationParameters, HSigmoidActivationParameters, HSwishActivationParameters))]
        qrecs = [G.quantization[NodeId(node)] for node in sigs_swishes]
        for sig_swish, qrec in zip(sigs_swishes, qrecs):
            in_edge = [edge for edge in G.in_edges(sig_swish.name) if edge.to_idx == 0][0]
            in_q = qrec.in_qs[0]
            min_val, max_val = in_q.min_val, in_q.max_val
            if isinstance(sig_swish, (HSigmoidActivationParameters, SigmoidActivationParameters)):
                # Hard sigmoid implements a RELU, be sure 6 can be representable
                min_val, max_val = 0, 6
            elif isinstance(sig_swish, HSwishActivationParameters):
                min_val, max_val = 0, in_q.max_val * 6

            new_in_q = QType.from_min_max_sq(min_val=min_val,
                                             max_val=max_val,
                                             dtype=in_q.dtype)
            propagate_qtype_up(G, new_in_q, in_edge)

        if set_identity:
            self.set_identity(G)

        return False
