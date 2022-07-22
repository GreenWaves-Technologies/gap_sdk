# Copyright (C) 2022  GreenWaves Technologies, SAS

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

from nntool.expressions.symbolic.symbol import Infos
from nntool.graph.types import (ConvFusionNode, ExpressionFusionNode,
                         LinearFusionNode)
from nntool.utils.exception import NNToolInternelError
from nntool.utils.graph import GraphView

from ..matcher import Matcher, description, match_name, run_after, needs_valid_dimension

LOG = logging.getLogger(__name__)

@match_name("common_fused_expressions")
@description("finds fused expressions that can be reduced to a single kernel with info parameters")
@run_after('fuse_gap_convs', 'fuse_gap_linear')
@needs_valid_dimension(True)
class CommonFusedExpressios(Matcher):

    def _match(self, G: GraphView, **kwargs) -> bool:
        if not G.quantization:
            return False

        all_funcs = []
        infoed_funcs = {}
        # find fused expressions
        for fusion in G.nodes(node_classes=(ConvFusionNode, LinearFusionNode)):
            expression = fusion.subgraph.nodes(
                node_classes=ExpressionFusionNode)
            if not expression:
                continue
            if len(expression) > 1:
                raise NNToolInternelError(f'{fusion.name} contains more than one expression')
            expression = expression[0]
            nid = (fusion.name, expression.name)
            qrec = G.quantization.get(nid)
            if not qrec:
                continue
            ass = qrec.cache['qfunc_col']
            if ass.infos:
                infoed_funcs.setdefault(ass.infos[0], set()).add(ass)
            all_funcs.append((nid, ass))
        infoed_funcs = {frozenset(func_set): infos for infos, func_set in infoed_funcs.items()} 
        funcs = all_funcs.copy()
        matches = []
        # check for equivalent groups
        while funcs:
            nid, func = funcs.pop(0)
            this_match = [(nid, func)]
            this_vars = []
            this_idx = []
            for other_idx, (other_nid, other_func) in enumerate(funcs):
                if other_func.equivalent(func):
                    constants = func.extract_constants(other_func)
                    if not this_vars:
                        this_vars.append(constants[0])
                    this_vars.append(constants[1])
                    this_match.append((other_nid, other_func))
                    this_idx.append(other_idx)

            funcs = [func for idx, func in enumerate(funcs) if idx not in this_idx]

            if this_vars:
                # check if this set of functions is already infoed
                if infoed_funcs:
                    func_set = frozenset(func for _, func in this_match)
                    if func_set in infoed_funcs:
                        continue
                this_vars = list(zip(*[all_vals for idx, all_vals in enumerate(zip(*this_vars))
                                    if len(set(val.value.item() for val in all_vals)) > 1]))
                if not this_vars: # functions are identical with same vars
                    this_vars = [[]] * len(this_match)
                matches.append(list(zip(this_match, this_vars)))
        match_constants = [
            (match, list(zip(*[consts for _, consts in match]))) for match in matches]
        # update constants in equivalent groups with InfoRefs
        for idx, (match, constants) in enumerate(match_constants):
            infos, refs = Infos.get_infos_from_constants(constants)
            infos.name = f"custom_{idx}"
            for expr_idx, ((expr_nid, expr), const_vars) in enumerate(match):
                for ref_idx, ref in enumerate(refs):
                    expr.substitute(const_vars[ref_idx],
                                    ref[expr_idx], update=True)
                # same infos class indicates same kernel
                expr.infos = (infos, expr_idx)
                G.quantization[expr_nid].cache['qfunc_col'] = expr



        return bool(match_constants)
