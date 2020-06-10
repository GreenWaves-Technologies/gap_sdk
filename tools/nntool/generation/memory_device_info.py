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

from typing import Sequence, Optional

from utils.json_serializable import JsonSerializable

from .code_block import CodeBlock, quote

AT_L3_RAM_DEVICES = ('AT_MEM_L3_HRAM', 'AT_MEM_L3_QSPIRAM', 'AT_MEM_L3_OSPIRAM')
AT_L3_FLASH_DEVICES = ('AT_MEM_L3_HFLASH', 'AT_MEM_L3_QSPIFLASH', 'AT_MEM_L3_OSPIFLASH', 'AT_MEM_L3_MRAMFLASH')

# SetMemoryDeviceInfos(4,
#         AT_MEM_L1, L1Memory, "Dronet_L1_Memory", 0, 0,
#         AT_MEM_L2, L2Memory, "Dronet_L2_Memory", 0, 0,
#         AT_MEM_L3_HRAM, L3Memory, "Dronet_L3_Memory", 0, 1,
#         AT_MEM_L3_HFLASH, 20*1024*1024, "Dronet_L3_Flash", "Dronet_L3_Flash_Const.dat", 1
# );

class MemoryDeviceInfo():
    def __init__(self, memory_area: str, size_var: str, name: str,
                 ext_managed: bool = False, filename: Optional[str] = None):
        super().__setattr__('info', {
            'memory_area': memory_area,
            'size_var': size_var,
            'name': name,
            'ext_managed': ext_managed,
            'filename': filename
        })

    @classmethod
    def fromdict(cls, info):
        obj = cls(None, None, None)
        super(cls, obj).__setattr__('info', info)
        return obj

    def todict(self):
        return self.info

    def __getattr__(self, k):
        if k in self.info:
            return self.info[k]
        raise AttributeError()

    def __setattr__(self, k, val):
        if k in self.info:
            self.info[k] = val
        else:
            raise AttributeError()

class MemoryDeviceInfos(JsonSerializable):
    def __init__(self, infos: Sequence[MemoryDeviceInfo]):
        self.infos = infos

    @classmethod
    def default(cls):
        return cls([
            MemoryDeviceInfo('AT_MEM_L1', 'L1Memory', '{graphname}_L1_Memory'),
            MemoryDeviceInfo('AT_MEM_L2', 'L2Memory', '{graphname}_L2_Memory'),
            MemoryDeviceInfo('AT_MEM_L3_HRAM', 'L3Memory',
                             '{graphname}_L3_Memory', ext_managed=0),
            MemoryDeviceInfo('AT_MEM_L3_HFLASH',
                             'L3Flash',
                             '{graphname}_L3_Flash',
                             filename='{graphname}_L3_Flash_Const.dat',
                             ext_managed=0),
        ])

    @classmethod
    def _dencapsulate(cls, val):
        return cls([MemoryDeviceInfo.fromdict(info) for info in val])

    def _encapsulate(self):
        return [info.todict() for info in self.infos]

    def set_l2_ram_ext_managed(self, ext_managed):
        for info in self.infos:
            if info.memory_area == 'AT_MEM_L2':
                info.ext_managed = ext_managed and 1 or 0

    def set_l3_ram_ext_managed(self, ext_managed, l3_ram_device):
        for info in self.infos:
            if info.memory_area in AT_L3_RAM_DEVICES:
                info.memory_area = l3_ram_device
                info.ext_managed = ext_managed and 1 or 0

    def set_l3_flash_ext_managed(self, ext_managed, l3_flash_device):
        for info in self.infos:
            if info.memory_area in AT_L3_FLASH_DEVICES:
                info.memory_area = l3_flash_device
                info.ext_managed = ext_managed and 1 or 0

    def gen(self, G, code_block: CodeBlock):
        code_block.write('SetMemoryDeviceInfos({},', len(self.infos))
        code_block.indent()
        first = True
        graphname = G.name
        for info in self.infos:
            if first:
                first = False
            else:
                code_block.append_last(',')

            code_block.write('{}, {}, "{}", {}, {}',
                             info.memory_area,
                             info.size_var,
                             info.name.format(graphname=graphname),
                             0 if info.filename is None else\
                                 quote(info.filename.format(graphname=graphname)),
                             1 if info.ext_managed else 0)
        code_block.deindent()
        code_block.write(');')
