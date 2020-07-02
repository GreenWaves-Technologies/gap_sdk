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

from generation.at_types.tc_arg_info import InputArgInfo
from generation.generators.generator_decorators import generation_function
from graph.types import InputParameters


@generation_function("inputs", (InputParameters,))
def inputs_input_generator(gen, node, qrec, edge) -> bool:
    gen.globals.append(InputArgInfo(qrec.out_qs[edge.from_idx].ctype, edge.params.name,
                                    gen.opts['default_input_home_location'],
                                    gen.opts['default_input_exec_location'],
                                    allocate=node.at_options.allocate,
                                    extern_input_pointer=node.at_options.extern_input_pointer))
    return True
