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

from graph.matches.matches import get_matches
from graph.types.constant_input import ConstantInputParameters
from quantization.verify_quantization import verify_quantization
from quantization.quantizer.new_quantizer import NewQuantizer


def fusions(nngraph, *match_names, no_postprocess: bool = False):
    state = ConstantInputParameters.save_compression_state(nngraph)
    try:
        match_group = get_matches(*match_names)
        while match_group:
            match_group.match(nngraph)
            nngraph.add_dimensions()
            if no_postprocess:
                break
            if match_group.run_qtune:
                quantizer = NewQuantizer(nngraph)
                quantizer.quantize()
            if match_group.run_adjust:
                nngraph.adjust_order()
            if match_group.run_again:
                match_group = get_matches(*match_group.run_again)
            else:
                match_group = None

        if nngraph.quantization and verify_quantization(nngraph):
            quantizer = NewQuantizer(nngraph)
            quantizer.quantize()
            problems = verify_quantization(nngraph)
            if problems:
                problems = "\n".join(problems)
                raise ValueError(f'quantization issue after fusions\n{problems}\n')
    finally:
        ConstantInputParameters.restore_compression_state(nngraph, state)
