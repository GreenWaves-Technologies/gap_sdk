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

import gsystree as st


class Hyperram(st.Component):
    """
    Hyperram

    This instantiates a s27ks0641 RAM (hyperram).

    Attributes
    ----------
    size : int
        Size of the RAM (default: 0x00800000).

    """

    def __init__(self, parent, name, size=0x00800000):
        super(Hyperram, self).__init__(parent, name)

        # Register all parameters as properties so that they can be overwritten from the command-line
        self.add_property('size', size)

        self.add_property('vp_component', "devices.hyperbus.hyperram_impl")
