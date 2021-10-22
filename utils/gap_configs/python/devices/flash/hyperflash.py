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

from devices.flash.flash import Flash

class Hyperflash(Flash):
    """
    Hyperflash

    This instantiates a s26ks512s flash (hyperflash).

    Attributes
    ----------
    size : int
        Size of the flash (default: 0x04000000).

    """

    def __init__(self, parent, name, size=0x04000000):
        super(Hyperflash, self).__init__(parent, name)

        # Register all parameters as properties so that they can be overwritten from the command-line
        self.add_property('content/partitions/readfs/files', [])
        self.add_property('content/partitions/readfs/type', 'readfs')
        self.add_property('content/partitions/readfs/enabled', False)

        self.add_property('content/partitions/hostfs/files', [])
        self.add_property('content/partitions/hostfs/type', 'hostfs')

        self.add_property('content/partitions/lfs/root_dir', None)
        self.add_property('content/partitions/lfs/type', 'lfs')
        self.add_property('content/partitions/lfs/enabled', False)

        self.add_property('vp_component', "devices.hyperbus.hyperflash_impl")

        self.add_property('writeback', True)
        self.add_property('size', size)

        # TODO this is needed by GAPY but is not aligned with the size given to model
        # That should be resolved once the flash images are built by the system tree instead of gapy
        self.add_property('datasheet/type', 'hyper')
        self.add_property('datasheet/size', '64MB')
        self.add_property('datasheet/block-size', '256KB')
