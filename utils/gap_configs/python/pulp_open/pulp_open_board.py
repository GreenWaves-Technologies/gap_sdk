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
from pulp_open.pulp_open import Pulp_open
from devices.flash.hyperflash import Hyperflash
from devices.flash.spiflash import Spiflash
from devices.flash.atxp032 import Atxp032
from devices.ram.hyperram import Hyperram
from tools.runner.runner import Runner_target
from devices.testbench.testbench import Testbench


class Pulp_open_board(Runner_target):

    def __init__(self, parent, name, ram_type: str='s27ks0641', ram_itf: int=0, ram_cs: int=0,
            flash_type: str='spiflash', flash_itf: int=0, flash_cs: int=0):

        super(Pulp_open_board, self).__init__(parent, name, boot_mode="flash", boot_device="flash")

        # Register all parameters as properties so that they can be overwritten from the command-line
        self.add_property('ram_type', ram_type)
        self.add_property('ram_itf', ram_itf)
        self.add_property('ram_cs', ram_cs)
        self.add_property('flash_type', flash_type)
        self.add_property('flash_itf', flash_itf)
        self.add_property('flash_cs', flash_cs)

        self.add_properties({
            "runner": {
                "modules": {
                    "gvsoc": "gv.chips.pulp"
                }
            }
        })


        # Pulp
        pulp = Pulp_open(self, 'chip')

        # Flash
        if flash_type == 's26ks512s':
            flash = Hyperflash(self, 'flash')

        elif flash_type == 'spiflash':
            flash = Spiflash(self, 'flash')

        elif flash_type == 'atxp032':
            flash = Atxp032(self, 'flash')

        else:
            raise RuntimeError('Unknown flash type: ' + flash_type)

        if flash_type == 'spiflash':
            self.bind(pulp, 'spim%d_cs%d' % (flash_itf, flash_cs), flash, 'cs')
            self.bind(pulp, 'spim%d_cs%d_data' % (flash_itf, flash_cs), flash, 'input')
        else:
            self.bind(pulp, 'hyper%d_cs%d' % (flash_itf, flash_cs), flash, 'cs')
            self.bind(pulp, 'hyper%d_cs%d_data' % (flash_itf, flash_cs), flash, 'input')

        # Ram
        if ram_type == 's27ks0641':
            ram = Hyperram(self, 'ram')

        else:
            raise RuntimeError('Unknown ram type: ' + ram_type)

        self.bind(pulp, 'hyper%d_cs%d' % (ram_itf, ram_cs), ram, 'cs')
        self.bind(pulp, 'hyper%d_cs%d_data' % (ram_itf, ram_cs), ram, 'input')
