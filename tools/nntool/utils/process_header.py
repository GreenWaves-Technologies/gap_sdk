# Copyright (C) 2022  GreenWaves Technologies, SAS

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

import os
import re
import sys
from collections.abc import Mapping

import numpy as np

from .diag_collector import thread_singleton


class ProcessHeader(Mapping):
    P_DEFINE = re.compile('^[\t ]*#[\t ]*define[\t ]+([a-zA-Z0-9_]+)[\t ]+')

    P_MACRO = re.compile(
        '^[\t ]*#[\t ]*define[\t ]+'
        '([a-zA-Z0-9_]+)\(([_a-zA-Z][_a-zA-Z0-9]*)\)[\t ]+')

    P_INCLUDE = re.compile('^[\t ]*#[\t ]*include[\t ]+<([a-zA-Z0-9_/\.]+)')

    P_COMMENT = re.compile(r'/\*([^*]+|\*+[^/])*(\*+/)?')
    P_CPP_COMMENT = re.compile('//.*')

    P_CHAR = re.compile(r"'(\\.[^\\]*|[^\\])'")

    P_HEX = re.compile(r"0x([0-9a-fA-F]+)L?")
    IGNORES = [P_COMMENT, P_CPP_COMMENT]

    def __init__(self, searchdirs=[]) -> None:
        self._env = {}
        self._filedict = {}
        self._searchdirs = searchdirs

    def __getitem__(self, __k):
        return self._env[__k] # @IgnoreException

    def __iter__(self):
        return iter([k for k in self._env if k[0] != '__builtins__'])

    def __len__(self) -> int:
        return len(self._env) - (1 if '__builtins__' in self else 0)

    @property
    def env(self):
        return self._env

    @staticmethod
    def pytify(body):
        # replace ignored patterns by spaces
        for pattern in ProcessHeader.IGNORES:
            body = pattern.sub(' ', body)
        # replace char literals by ord(...)
        body = ProcessHeader.P_CHAR.sub("ord('\\1')", body)
        # Compute negative hexadecimal constants
        start = 0
        UMAX = 2*(sys.maxsize+1)
        while 1:
            match = ProcessHeader.P_HEX.search(body, start)
            if not match:
                break
            span, element = match.span()
            val = int(body[slice(*match.span(1))], 16)
            if val > sys.maxsize:
                val -= UMAX
                body = body[:span] + "(" + str(val) + ")" + body[element:]
            start = span + 1
        return body

    def open_file(self, filename):
        try:
            fp = open(filename)  # @IgnoreException
            return fp
        except IOError:
            pass
        for searchdir in self._searchdirs:
            try:
                # @IgnoreException
                fp = open(os.path.join(searchdir, filename))
                return fp
            except IOError:
                pass
        raise IOError(f'{filename} not found')

    def process(self, fp):
        if isinstance(fp, str):
            fp = self.open_file(fp)
        lineno = 0
        while 1:
            line = fp.readline()
            if not line:
                break
            lineno = lineno + 1
            match = self.P_DEFINE.match(line)
            if match:
                # gobble up continuation lines
                while line[-2:] == '\\\n':
                    nextline = fp.readline()
                    if not nextline:
                        break
                    lineno = lineno + 1
                    line = line + nextline
                name = match.group(1)
                body = line[match.end():]
                body = self.pytify(body)
                stmt = '%s = %s\n' % (name, body.strip())
#pylint: disable=exec-used
                exec(stmt, self._env)
            match = self.P_MACRO.match(line)
            if match:
                macro, arg = match.group(1, 2)
                body = line[match.end():]
                body = self.pytify(body)
                stmt = 'def %s(%s): return %s\n' % (macro, arg, body)
#pylint: disable=exec-used
                exec(stmt, self._env)
            match = self.P_INCLUDE.match(line)
            if match:
                regs = match.regs
                part_1, part_2 = regs[1]
                filename = line[part_1:part_2]
                if filename not in self._filedict:
                    self._filedict[filename] = None
                    self.process(self.open_file(filename))

    def __str__(self) -> str:
        vals = ", ".join([f'{repr(k)}:{repr(v)}' for k, v in self])
        return f'{{{vals}}}'


class InfosBase(ProcessHeader):
    PREFIX = 'AT_INF_'
    LEN_SUFFIX = '_LEN'

    def __init__(self, filename, searchdirs) -> None:
        super().__init__(searchdirs=searchdirs)
        self.process(filename)

    def inf(self, __k):
        return self._env[f'{self.PREFIX}{__k.upper()}']

    def inf_len(self, __k):
        this_key = f'{self.PREFIX}{__k.upper()}'
        this_len_key = f'{this_key}{self.LEN_SUFFIX}'
        if this_len_key in self._env:
            return self._env[this_len_key]
        this_val = self[this_key]
        other_keys = [k for k in self if not(k.endswith(self.LEN_SUFFIX) or k == this_key) and
                      self[k] == this_val and f'{k}{self.LEN_SUFFIX}' in self]
        if len(other_keys) != 1:
            raise ValueError(f'unique length key for {__k} not found')
        return self[f'{other_keys[0]}{self.LEN_SUFFIX}']

    def gen_infos_array(self, len_key, **vals):
        keys = sorted([(key, self.inf(key), self.inf_len(key))
                       for key in vals], key=lambda x: x[1])
        bvals = np.full((self.inf(len_key),), 0, dtype=np.uint8)
        for key, info, info_len in keys:
            val = np.atleast_1d(vals[key])
            val = val.newbyteorder('>')
            val = np.frombuffer(val.tobytes(), dtype=np.uint8)
            val_len = len(val)
            if val_len > info_len:
                raise ValueError(
                    f'value for {key} is too long {val_len}>{info_len}')
            bvals[info:info+len(val):1] = val
        return bvals
