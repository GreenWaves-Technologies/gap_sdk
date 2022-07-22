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

import typing

from nntool.graph.matches.matches import get_matches
from nntool.graph.types.constant_input import ConstantInputNode
from nntool.quantization.verify_quantization import verify_quantization
from nntool.quantization.quantizer.new_quantizer import NewQuantizer

if typing.TYPE_CHECKING:
    from nntool.graph.nngraph import NNGraph


def fusions(
        nngraph: 'NNGraph',
        *match_names,
        no_postprocess: bool = False,
        exclude_match_names: typing.Sequence[str] = None):
    state = ConstantInputNode.save_compression_state(nngraph)
    try:
        match_group = get_matches(*match_names)
        while match_group:
            if exclude_match_names:
                match_group.exclude_matches(*exclude_match_names)
            nngraph.add_dimensions(quiet=True)
            match_group.match(nngraph)
            nngraph.add_dimensions(quiet=True)
            if no_postprocess:
                break
            if match_group.run_qtune:
                quantizer = NewQuantizer.from_quantized_graph(nngraph)
                quantizer.quantize()
            if match_group.run_adjust:
                nngraph.adjust_order()
            if match_group.run_again:
                match_group = get_matches(*match_group.run_again)
            else:
                match_group = None

        if nngraph.quantization and verify_quantization(nngraph):
            quantizer = NewQuantizer.from_quantized_graph(nngraph)
            quantizer.quantize()
            problems = verify_quantization(nngraph)
            if problems:
                problems = "\n".join(problems)
                raise ValueError(
                    f'quantization issue after fusions\n{problems}\n')
    finally:
        ConstantInputNode.restore_compression_state(nngraph, state)
