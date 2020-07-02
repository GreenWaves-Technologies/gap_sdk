#
# Copyright (C) 2020  GreenWaves Technologies, SAS
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
# 
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

# 
# Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
#

import vp_core as vp
from elftools.elf.elffile import ELFFile
import os

class component(vp.component):

    implementation = 'utils.loader_impl'

    def load(self):


        binaries = self.get_json().get_child_str('load-binary_eval')
        if binaries is not None:
            binaries = [eval(binaries)]
        else:
            binaries = self.get_json().get('binaries').get_dict()


        elffile = None

        if binaries is not None:
            for binary in binaries:
                with open(binary, 'rb') as file:
                    elffile = ELFFile(file)
                    for segment in elffile.iter_segments():
                        if segment['p_type'] == 'PT_LOAD':
                            self.get_impl().module.loader_io_req(
                                self.get_impl().instance,
                                segment['p_paddr'],
                                segment['p_filesz'],
                                True,
                                segment.data()
                            )
                            if segment['p_filesz'] < segment['p_memsz']:
                                self.get_impl().module.loader_memset(
                                    self.get_impl().instance,
                                    segment['p_paddr'] + segment['p_filesz'],
                                    segment['p_memsz'] - segment['p_filesz'],
                                    0
                                )

            set_pc_addr = self.get_json().get_child_int('set_pc_addr')
            set_pc_offset = self.get_json().get_child_int('set_pc_offset')
            start_addr = self.get_json().get_child_int('start_addr')
            start_value = self.get_json().get_child_int('start_value')

            if set_pc_addr is not None and elffile is not None:
                entry = elffile.header['e_entry']
                if set_pc_offset is not None:
                    entry += set_pc_offset
                self.get_impl().module.loader_io_req(
                    self.get_impl().instance,
                    set_pc_addr,
                    4,
                    True,
                    (entry).to_bytes(4, byteorder='little')
                )

            if start_addr is not None:
                self.get_impl().module.loader_io_req(
                    self.get_impl().instance,
                    start_addr,
                    4,
                    True,
                    (start_value).to_bytes(4, byteorder='little')
                )
