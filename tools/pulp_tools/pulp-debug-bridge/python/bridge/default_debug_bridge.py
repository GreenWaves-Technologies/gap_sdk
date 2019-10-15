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

from __future__ import print_function
import ctypes
import os
import os.path
import json_tools as js
from elftools.elf.elffile import ELFFile
import time
from bridge.misc.portable import to_bytes
import struct
import sys
import platform

class CableException(Exception):
    pass

class CableCreationException(CableException):
    def __init__(self, error_string):
        self.error_string = error_string

    def __str__(self):
        return self.error_string

class DebugBridgeException(Exception):
    pass

class XferInvalidObjectException(DebugBridgeException):
    pass

class XferInvalidAnnexException(DebugBridgeException):
    pass

class XferErrorException(DebugBridgeException):

    def __init__(self, error_code):
        self.error_code = error_code

    def __str__(self):
        return "E {0:02X}".format(self.error_code)

class Ctype_cable(object):

    def __init__(self, top, module, config, system_config):

        self.module = module
        self.gdb_handle = None

        # Register entry points with appropriate arguments
        self.module.cable_cb_t = ctypes.CFUNCTYPE(None, ctypes.c_int)

        self.module.cable_new.argtypes = [ctypes.c_char_p, ctypes.c_char_p, self.module.cable_cb_t]
        self.module.cable_new.restype = ctypes.c_bool

        self.module.cable_write.argtypes = \
            [ctypes.c_int, ctypes.c_int, ctypes.c_char_p]
        self.module.cable_write.restype = ctypes.c_bool

        self.module.cable_read.argtypes = \
            [ctypes.c_int, ctypes.c_int, ctypes.c_char_p]
        self.module.cable_write.restype = ctypes.c_bool

        self.module.chip_reset.argtypes = \
            [ctypes.c_bool]
        self.module.chip_reset.restype = ctypes.c_bool

        self.module.jtag_reset.argtypes = \
            [ctypes.c_bool]
        self.module.cable_write.restype = ctypes.c_bool

        self.module.jtag_soft_reset.argtypes = \
            [ctypes.c_void_p]
        self.module.cable_write.restype = ctypes.c_bool

        self.module.cable_jtag_set_reg.argtypes = \
            [ctypes.c_int, ctypes.c_int, ctypes.c_uint]
        self.module.cable_jtag_set_reg.restype = ctypes.c_bool

        self.module.cable_jtag_get_reg.argtypes = \
            [ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
        self.module.cable_jtag_get_reg.restype = ctypes.c_bool

        config_string = None

        if config is not None:
            config_string = config.dump_to_string().encode('utf-8')

        def cmd_cb_hook(state):
            top.log(0, "Cable state change {}".format(state==0 and "connected" or "disconnected"))
            if state == 1 and hasattr(self, "disconnected_cb"):
                self.disconnected_cb()

        self.cb_hook = self.module.cable_cb_t(cmd_cb_hook)
        if not self.module.cable_new(config_string, system_config.dump_to_string().encode('utf-8'), self.cb_hook):
            raise CableCreationException('Failed to initialize cable with error: ' +
                self.module.bridge_get_error().decode('utf-8'))


    def call_if_disconnected(self, fn):
        self.disconnected_cb = fn

    def write(self, addr, size, buffer):
        data = (ctypes.c_char * size).from_buffer(bytearray(buffer))
        return self.module.cable_write(addr, size, data)

    def read(self, addr, size):
        data = (ctypes.c_char * size)()
        if not self.module.cable_read(addr, size, data):
            return None

        result = []
        for elem in data:
            result.append(elem)

        return result

    def chip_reset(self, value):
        return self.module.chip_reset(value)

    def set_VQPS(self, value):
        return self.module.cable_set_VQPS(value)

    def jtag_reset(self, value):
        return self.module.jtag_reset(value)

    def jtag_soft_reset(self):
        return self.module.jtag_soft_reset()

    def jtag_set_reg(self, reg, width, value):
        return self.module.cable_jtag_set_reg(reg, width, value)

    def jtag_get_reg(self, reg, width, value):
        out_value = ctypes.c_int()
        if not self.module.cable_jtag_get_reg(reg, width, ctypes.byref(out_value), value):
            return None

        return out_value.value

class debug_bridge(object):

    def __init__(self, config, binaries=[], verbose=0):
        self.verbose = verbose
        self.config = config
        self.cable = None
        self.cable_name = config.get_child_str('**/debug_bridge/cable/type')
        if self.cable_name is None:
            self.cable_name = 'ftdi'
        self.binaries = binaries
        self.callbacks = []
        self.has_loopers = False
        self.cable_config = config.get('**/debug_bridge/cable')
        self.is_started = False
        # Load the library which provides generic services through
        # python / C++ bindings
        # The library is located in the same directory as this file
        if platform.system() == "Windows":
            libname = "libpulpdebugbridge.dll"
            fullpath = os.path.join(os.path.dirname(__file__), libname)
            self.module = ctypes.windll.LoadLibrary(fullpath)
        else:
            libname = "libpulpdebugbridge.so"
            fullpath = os.path.join(os.path.dirname(__file__), libname)
            self.module = ctypes.CDLL(fullpath)

        self.module.bridge_get_error.argtypes = []
        self.module.bridge_get_error.restype = ctypes.c_char_p

        self.module.bridge_init.argtypes = [ctypes.c_char_p, ctypes.c_int]

        self.module.bridge_deinit.argtypes = []

        self.module.execute_cb_typ = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p)

        self.module.bridge_add_execute.argtypes = [self.module.execute_cb_typ]

        self.module.bridge_add_repeat_start.argtypes = [ctypes.c_longlong, ctypes.c_int]

        self.module.bridge_add_repeat_end.argtypes = []

        self.module.bridge_add_delay.argtypes = [ctypes.c_longlong]

        self.module.bridge_add_wait_exit.argtypes = []

        self.module.bridge_start.argtypes = []
        self.module.bridge_start.restype = ctypes.c_int

        self.module.bridge_reqloop_init.argtypes = [ctypes.c_uint, ctypes.c_int]

        self.module.bridge_reqserver_init.argtypes = [ctypes.c_int, ctypes.c_int]

        #self.module.bridge_reqserver_reset.argtypes = []

        self.module.bridge_target_stopped.argtypes = [ctypes.c_int]

        self.module.bridge_reqloop_start.argtypes = [ctypes.c_bool]

        self.module.bridge_reqloop_stop.argtypes = [ctypes.c_bool]

        self.module.bridge_set_log_level.argtypes = [ctypes.c_int]

        self.cmd_func_typ = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_char_p,
          ctypes.POINTER(ctypes.c_char), ctypes.c_int)

        self.module.gdb_server_open.argtypes = [ctypes.c_int,
          self.cmd_func_typ, ctypes.c_char_p]
        self.module.gdb_server_open.restype = ctypes.c_bool

        self.timer_cb_typ = ctypes.CFUNCTYPE(ctypes.c_longlong)
        self.module.bridge_loop_timeout.argtypes = [self.timer_cb_typ, ctypes.c_longlong]

        self.module.gdb_send_str.argtypes = [ctypes.c_char_p]
        self.module.gdb_send_str.restype = ctypes.c_bool

        self.module.gdb_server_close.argtypes = []

        self.module.gdb_server_abort.argtypes = []

        self.module.gdb_server_refresh_target.argtypes = []

        self.module.get_max_log_level.restype = ctypes.c_int

        self.module.bridge_init(config.dump_to_string().encode('utf-8'), verbose)

        self.capabilities("")
        # self.capabilities("qXfer:features:read+")

#self.module.jtag_shift.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_char_p)]

    def __mount_cable(self):
        if self.cable_name is None:
            raise Exception("Trying to mount cable while no cable was specified")

        if self.cable_name.split('@')[0] in ['ftdi', 'jtag-proxy']:
            self.__mount_ctype_cable()
            pass
        else:
            raise Exception('Unknown cable: ' + self.cable_name)

    def __mount_ctype_cable(self):

        self.cable = Ctype_cable(top=self, module = self.module, config = self.cable_config, system_config = self.config)
        self.cable.call_if_disconnected(self.abort)

    def log(self, level, *args):
        if self.verbose >= level:
            print (*args)
            sys.stdout.flush()

    def get_cable(self):
        if self.cable is None:
            self.__mount_cable()

        return self.cable

    def fuse_validate(self, args, do_write):
        raise Exception('Fuses are not supported on this target')

    def fuse(self, bit_offset, bit_len, buffer = None, fmt = "binary"):
        raise Exception('Fuses are not supported on this target')

    def load_jtag(self):
        return self.load_default()

    def load_jtag_hyper(self):
        raise Exception('JTAG boot is not supported on this target')

    def load_elf(self, binary):
        with open(binary, 'rb') as file_stream:
            elffile = ELFFile(file_stream)

            for segment in elffile.iter_segments():

                if segment['p_type'] == 'PT_LOAD':

                    data = segment.data()
                    addr = segment['p_paddr']
                    size = len(data)

                    self.log(1, 'Loading section (base: 0x%x, size: 0x%x)' % (addr, size))

                    if not self.write(addr, size, data):
                        return False

                    if segment['p_filesz'] < segment['p_memsz']:
                        addr = segment['p_paddr'] + segment['p_filesz']
                        size = segment['p_memsz'] - segment['p_filesz']

                        self.log(1, 'Init section to 0 (base: 0x%x, size: 0x%x)' % (addr, size))
                        if not self.write(addr, size, [0] * size):
                            return False


            set_pc_addr_config = self.config.get('**/debug_bridge/set_pc_addr')

            if set_pc_addr_config is not None:
                set_pc_offset_config = self.config.get('**/debug_bridge/set_pc_offset')
                entry = elffile.header['e_entry']

                if set_pc_offset_config is not None:
                    entry += set_pc_offset_config.get_int()

                self.log(1, 'Setting PC (base: 0x%x, value: 0x%x)' % (set_pc_addr_config.get_int(), entry))

                return self.write_32(set_pc_addr_config.get_int(), entry)

        return True

    def load(self, mode=None):
        if mode is None:
            mode = 'jtag'

        if mode == 'jtag':
            return self.load_jtag()
        elif mode == 'jtag_hyper':
            return self.load_jtag_hyper()
        else:
            return self.load_default()

    def load_default(self):
        for binary in self.binaries:
            if not self.load_elf(binary=binary):
                return False

        return True

    def start(self):
        start_addr_config = self.config.get('**/debug_bridge/start_addr')
        if start_addr_config is not None:
            self.is_started = True

            self.log(1, 'Starting (base: 0x%x, value: 0x%x)' % (start_addr_config.get_int(), self.config.get('**/debug_bridge/start_value').get_int()))

            return self.write_32(start_addr_config.get_int(), self.config.get('**/debug_bridge/start_value').get_int())
        return False

    def stop(self):

        stop_addr_config = self.config.get('**/debug_bridge/stop_addr')
        if stop_addr_config is not None:
            self.is_started = False
            return self.write_32(stop_addr_config.get_int(), self.config.get('**/debug_bridge/stop_value').get_int()) != 0
        return True

    def decode_addr(self, taddr):
        try:
            addr = int(taddr, 0)
        except:
            addr = self._get_binary_symbol_addr(taddr)
        return addr

    def read(self, addr, size):
        return self.get_cable().read(addr, size)

    def write(self, addr, size, buffer):
        return self.get_cable().write(addr, size, buffer)

    def write_int(self, addr, value, size):
        return self.write(addr, size, to_bytes(value, size, byteorder='little'))

    def write_32(self, addr, value):
        return self.write_int(addr, value, 4)

    def write_16(self, addr, value):
        return self.write_int(addr, value, 2)

    def write_8(self, addr, value):
        return self.write_int(addr, value, 1)

    def read_int(self, addr, size):
        byte_array = None
        for byte in self.read(addr, size):
            if byte_array == None:
                byte_array = byte
            else:
                byte_array += byte
        return struct.unpack("<i",byte_array)[0]
            # int.from_bytes(byte_array, byteorder='little')

    def read_32(self, addr):
        return self.read_int(addr, 4)

    def read_16(self, addr):
        return self.read_int(addr, 2)

    def read_8(self, addr):
        return self.read_int(addr, 1)

    def _get_binary_symbol_addr(self, name):
        for binary in self.binaries:
            with open(binary, 'rb') as file:
                elf = ELFFile(file)
                for section in elf.iter_sections():
                    if section.header['sh_type'] == 'SHT_SYMTAB':
                        for symbol in section.iter_symbols():
                            if symbol.name == name:
                                # t_section=symbol.entry['st_shndx']
                                t_vaddr=symbol.entry['st_value']
                                return t_vaddr
        return True

    def reset(self):
        return self.get_cable().jtag_reset(True) and self.get_cable().jtag_reset(False) and self.get_cable().chip_reset(True) and self.get_cable().chip_reset(False)

    def init_reqloop(self):
        # First get address of the structure used to communicate between
        # the bridge and the runtime
        addr = self._get_binary_symbol_addr('__rt_debug_struct_ptr')
        if addr == 0:
            addr = self._get_binary_symbol_addr('debugStruct_ptr')
        self.log(1, "debug address 0x{:08x} contents 0x{:08x}".format(addr, self.read_32(addr)))
        self.module.bridge_reqloop_init(addr, 1)
        self.has_loopers = True

    def ioloop(self):
        self.init_reqloop()
        self.module.bridge_reqloop_start(True)
        return True

    def reqloop(self):
        self.init_reqloop()
        port = self.config.get_int('**/debug_bridge/reqserver/port')
        max_req = self.config.get_int('**/debug_bridge/reqserver/max')
        self.module.bridge_reqserver_init(port, max_req)
        self.module.bridge_reqloop_start(True)
        return True

    def flash(self):
        raise Exception('Flash is not supported on this target')

    def encode_bytes(self, sbuf, buf, buf_len, start=0):
        enc = sbuf.encode('ascii')
        if len(enc) + 1 > buf_len - start:
            return -len(enc)
        for i in range(len(enc)):
            buf[i+start] = enc[i]
        buf[len(enc)+start] = b'\000'
        return len(enc) + start

    def qrcmd_debug_level(self, cmd, buf, buf_len):
        level = int(cmd.split()[1])
        self.log(1, "Log level set to {}".format(level))
        self.module.bridge_set_log_level(level)
        return self.encode_bytes("OK", buf, buf_len)

    def doreset(self, run):
        if self.has_loopers:
            self.module.bridge_reqloop_stop(False)
        self.stop()
        self.load()
        self.module.gdb_server_refresh_target()
        if self.has_loopers:
            self.module.bridge_reqloop_start(False)
        if run:
            self.start()
        return True

    def qrcmd_reset(self, cmd, buf, buf_len):
        cmd = cmd.split(' ')
        if len(cmd) == 1:
            cmd = "run"
        elif len(cmd) == 2:
            cmd = cmd[1]
        else:
            return self.encode_bytes("", buf, buf_len)
        if "run".startswith(cmd):
            cmd = "run"
        elif "halt".startswith(cmd):
            cmd = "halt"
        else:
            return self.encode_bytes("", buf, buf_len)

        if self.doreset(cmd == "run"):
            return self.encode_bytes("OK", buf, buf_len)
        else:
            return self.encode_bytes("E00", buf, buf_len)

    def qrcmd_shutdown(self, cmd, buf, buf_len):
        self.abort()
        return self.encode_bytes("OK", buf, buf_len)

    def hex_string(self, s):
        res = []
        for b in s:
            res.append('{0:02x}'.format(ord(b)))
        return ''.join(res)

    def qrcmd_help(self, cmd, buf, buf_len, commands):
        init = 'Command List\n'
        for c in commands:
            self.encode_bytes('O'+self.hex_string(init+commands[c]+'\n'), buf, buf_len)
            self.module.gdb_send_str(buf)
            init = ''
        return self.encode_bytes("OK", buf, buf_len)

    def qrcmd_cb(self, cmd, buf, buf_len):
        commands = {
            u"reset":       "reset [run|halt]     - resets the target",
            u"debug_level": "debug_level (0-{})    - sets bridge debug level".format(self.module.get_max_log_level()),
            u"shutdown":    "shutdown             - shuts down the bridge",
            u"help":        "help                 - gets list of supported commands"
        }
        try:
            cmd = bytearray.fromhex(cmd).decode('ascii')
            self.log(2, "Receive rCmd: "+cmd)
            cmd_name = cmd.split(' ')[0].lower()
            cmd_selected = None
            for c in commands:
                if c.startswith(cmd_name):
                    if cmd_selected is None:
                        cmd_selected = c
                    else:
                        return self.encode_bytes("", buf, buf_len)
            if cmd_selected is None:
                return self.encode_bytes("", buf, buf_len)
            elif cmd_selected == "help":
                return self.qrcmd_help(cmd, buf, buf_len, commands)
            else:
                handler = getattr(self, 'qrcmd_'+cmd_selected)
                res = handler(cmd, buf, buf_len)
                return res
        except:
            return self.encode_bytes("E00", buf, buf_len)

    def qxfer_read_cb(self, obj, annex, offset, length, buf, buf_len):
        try:
            if obj == "exec-file":
                if len(self.binaries) > 0:
                    obj_val = os.path.abspath(self.binaries[0])
                else:
                    obj_val = ""
            else:
                obj_val = self.qxfer_read(obj, annex)

            if offset >= len(obj_val):
                obj_val = "l"
            elif offset + length >= len(obj_val):
                obj_val = "l" + obj_val[offset:]
            else:
                obj_val = "m" + obj_val[offset:offset+length-1]

        except XferInvalidObjectException:
            obj_val = ""
        except XferInvalidAnnexException:
            obj_val = "E00"
        except XferErrorException as err:
            obj_val = str(err)
        except Exception:
            obj_val = ""

        return self.encode_bytes(obj_val, buf, buf_len)

    def capabilities(self, extra):
        capstr = "qXfer:exec-file:read+"
        if len(extra) > 0:
            capstr = capstr + ";" + extra
        self.capabilities_str = capstr.encode('ascii')

    def cmd_cb(self, cmd, buf, buf_len):
        # try:
        cmd = cmd.decode('ascii')
        if cmd.startswith("qXfer"):
            cmd = cmd.split(":")
            if len(cmd) != 5:
                raise Exception()
            offlen = cmd[4].split(',')
            if cmd[2] == "read":
                return self.qxfer_read_cb(cmd[1], cmd[3], int(offlen[0], base=16), int(offlen[1], base=16), buf, buf_len)
        elif cmd.startswith("qRcmd"):
            if len(cmd) < len("qRcmd") + 2:
                return self.encode_bytes("E01", buf, buf_len)
            cmd = cmd.split(',')
            if len(cmd) != 2:
                return self.encode_bytes("E01", buf, buf_len)

            return self.qrcmd_cb(cmd[1], buf, buf_len)
        # disabled for the moment since this causes issues with breakpoints for an unknown reason
        elif cmd.startswith("__gdb_tgt_run"):
            self.doreset(False)
        elif cmd.startswith("__gdb_tgt_res"):
            ret = 0
            if self.has_loopers:
                self.module.bridge_target_stopped(False)
                ret = 1
            return ret
        elif cmd.startswith("__gdb_tgt_hlt"):
            ret = 0
            if self.has_loopers:
                self.module.bridge_target_stopped(True)
                ret = 1
            return ret
        elif cmd.startswith("__is_started"):
            return self.is_started and 1 or 0
        elif cmd.startswith("__start_target"):
            return self.start()
        elif cmd.startswith("__stop_target"):
            return self.stop()
        if buf_len > 0:
            return self.encode_bytes("", buf, buf_len)
        else:
            return 0
        # except:
        #     print("Unexpected error:", sys.exc_info()[0])
        #     return self.encode_bytes("E00", buf, buf_len)

    def gdb(self, port):
        def cmd_cb_hook(cmd, buf, buf_len):
            return self.cmd_cb(cmd, buf, buf_len)

        self.cmd_func_ptr = self.cmd_func_typ(cmd_cb_hook)
        self.module.gdb_server_open(
            port,
            self.cmd_func_ptr,
            self.capabilities_str)
        return 0

    def abort(self):
        self.has_loopers = False
        self.module.bridge_reqloop_stop(True)
        self.module.gdb_server_abort()

    def close(self):
        self.has_loopers = False
        self.module.bridge_reqloop_stop(True)
        self.module.gdb_server_close()

    def run_loop(self):
        self.module.bridge_start()
        self.callbacks = None
        self.log(1, "Loop exited")

    def add_execute(self, cb):
        cbt = self.module.execute_cb_typ(cb)
        self.callbacks.append(cbt)
        self.module.bridge_add_execute(cbt)

    def add_wait_exit(self):
        self.module.bridge_add_wait_exit()
        self.log(1, "Program exited")

    def add_repeat_start(self, delay, count):
        self.module.bridge_add_repeat_start(delay, count)

    def add_repeat_end(self):
        self.module.bridge_add_repeat_end()

    def add_delay(self, delay):
        self.module.bridge_add_delay(delay)

    def deinit(self):
        self.module.bridge_deinit()

