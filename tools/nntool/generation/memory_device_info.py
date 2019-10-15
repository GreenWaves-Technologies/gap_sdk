# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from typing import Sequence, Optional
from .code_block import CodeBlock, quote

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
        raise AttributeError()

class MemoryDeviceInfos():
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
    def fromdict(cls, infos):
        return cls([MemoryDeviceInfo.fromdict(info) for info in infos])

    def todict(self):
        return [info.todict() for info in self.infos]

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
