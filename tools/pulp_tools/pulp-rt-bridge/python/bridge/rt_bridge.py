#
# Copyright (C) 2018 ETH Zurich and University of Bologna
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)

import ctypes
import os
import os.path
import json_tools as js
from elftools.elf.elffile import ELFFile
import time

class rt_bridge(object):

    def __init__(self, config, binaries=[], verbose=False):
        self.config = config
        self.binaries = binaries
        self.verbose = verbose

        lib_path=os.path.join('librtbridge.so')
        self.module = ctypes.CDLL(lib_path)

        self.module.rtbridge_reqloop_open.argtypes = [ctypes.c_uint]
        self.module.rtbridge_reqloop_open.restype = ctypes.c_void_p
        
        self.module.rtbridge_reqloop_close.argtypes = [ctypes.c_void_p, ctypes.c_int]


    def _get_binary_symbol_addr(self, name):
        for binary in self.binaries:
            with open(binary, 'rb') as file:
                elf = ELFFile(file)
                for section in elf.iter_sections():
                    if section.header['sh_type'] == 'SHT_SYMTAB':
                        for symbol in section.iter_symbols():
                            if symbol.name == name:
                                t_section=symbol.entry['st_shndx']
                                t_vaddr=symbol.entry['st_value']
                                return t_vaddr
        return 0


    def reqloop(self):

        # First get address of the structure used to communicate between
        # the bridge and the runtime
        addr = self._get_binary_symbol_addr('__rt_debug_struct_ptr')
        if addr == 0:
            addr = self._get_binary_symbol_addr('debugStruct_ptr')

        self.reqloop_handle = self.module.rtbridge_reqloop_open(addr)
        self.module.rtbridge_reqloop_close(self.reqloop_handle, False)

        return 0
