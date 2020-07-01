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

from generation.at_types.tc_arg_info import OutputArgInfo
from generation.generators.generator_decorators import generation_function
from graph.types import OutputParameters


@generation_function("outputs", (OutputParameters,))
def outputs_output_generator(gen, node, qrec, edge) -> bool:
    eparams = edge.params
    gen.globals.append(OutputArgInfo(qrec.in_qs[edge.to_idx].ctype, eparams.name,
                                     gen.opts['default_output_home_location'],
                                     gen.opts['default_output_exec_location'],
                                     allocate=node.at_options.allocate,
                                     extern_output_pointer=node.at_options.extern_output_pointer))
    return True
