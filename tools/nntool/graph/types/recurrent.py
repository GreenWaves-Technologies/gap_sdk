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

from .base import Parameters
from .input_output import ConstantInputParameters


class RecurrentOutputParameters(Parameters):
    op_name = "recurrent_output"

    def __init__(self, *args, input_node: ConstantInputParameters = None, **kwargs):
        super(RecurrentOutputParameters, self).__init__(*args, **kwargs)
        self._input_node = input_node

    def get_output_size(self, in_dims):
        return [self._input_node.dims]

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0

    def __str__(self):
        return "RO {} {} {}".format(
            self._input_node.dims,
            self._input_node.name,
            self.at_options
        )

    def clone(self, name, groupn=None):
        raise NotImplementedError()
