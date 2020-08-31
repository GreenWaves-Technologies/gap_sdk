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

# CKernel_Arg_T *TCArgInfo(
# 	char *ArgType,			/**< C argument type as a string */
# 	char *ArgName,			/**< C argument name as a string */
# 	ArgScope_T Scope,		/**< Argument scope, ARG_SCOPE_ARG: passed as an argument, ARG_SCOPE_GLOBAL: global symbol */
# 	ArgDirection_T Dir,		/**< Argument's direction (in, out or in out) */
# 	AT_MemLocation_T HomeLoc,	/**< Permanent location in the mem hierarch variants of L3 or L2 */
# 	AT_MemLocation_T ExecLoc,	/**< Location of the argument when it is used, can be != HomeLoc */
# 	ConstInit_T *ConstInit		/**< Descriptor for initialized constant */
# 	);

from typing import Optional

from generation.at_types.constant_info import ConstantInfo


class TCArgInfo():
    ARG_TYPES = {
        "bool": "char",
        "uint8": "unsigned char * __restrict__",
        "int8": "signed char * __restrict__",
        "uint16": "unsigned short * __restrict__",
        "int16": "signed short * __restrict__",
        "uint32": "unsigned int * __restrict__",
        "int32": "signed int * __restrict__",
        "unsigned char": "unsigned char * __restrict__",
        "signed char": "signed char * __restrict__",
        "unsigned short": "unsigned short int * __restrict__",
        "signed short": "short int * __restrict__",
        "short int": "short int * __restrict__",
        "unsigned int": "unsigned int * __restrict__",
        "signed int": "signed int * __restrict__",
    }
    ARG_SCOPES = [
        "ARG_SCOPE_UNDEF", "ARG_SCOPE_ARG", "ARG_SCOPE_ARG_ALLOC", "ARG_SCOPE_GLOBAL", "ARG_SCOPE_LOCAL"
    ]
    ARG_DIRECTIONS = [
        "ARG_DIR_UNDEF", "ARG_DIR_IN", "ARG_DIR_CONSTIN", "ARG_DIR_OUT", "ARG_DIR_INOUT"
    ]
    MEM_LOCATIONS = [
        "AT_MEM_UNDEF",
        "AT_MEM_L3_HRAM",
        "AT_MEM_L3_QSPIRAM",
        "AT_MEM_L3_OSPIRAM",
        "AT_MEM_L3_HFLASH",
        "AT_MEM_L3_QSPIFLASH",
        "AT_MEM_L3_OSPIFLASH",
        "AT_MEM_L3_MRAMFLASH",
        "AT_MEM_L2",
        "AT_MEM_L1",
    ]

    def __init__(self,
                 arg_type: str, arg_name: str, arg_scope: str,
                 arg_dir: str, home_location: Optional[str] = None,
                 exec_location: Optional[str] = None,
                 const_info: Optional[ConstantInfo] = None,
                 comment: Optional[str] = None,
                 extern_pointer=None):
        assert arg_type in self.ARG_TYPES, "arg_type ({}) must be in {}".format(arg_type, self.ARG_TYPES)
        self._arg_type = arg_type
        self._arg_name = arg_name
        assert arg_scope in self.ARG_SCOPES, "arg_scope ({}) must be in {}".format(arg_scope, self.ARG_SCOPES)
        self._arg_scope = arg_scope
        assert arg_dir in self.ARG_DIRECTIONS, "arg_dir ({}) must be in {}".format(arg_dir, self.ARG_DIRECTIONS)
        self._arg_dir = arg_dir
        assert home_location is None or home_location in self.MEM_LOCATIONS, \
            "home_location ({}) must be in {}".format(home_location, self.MEM_LOCATIONS)
        self._home_location = home_location
        assert exec_location is None or exec_location in self.MEM_LOCATIONS, \
            "exec_location ({}) must be in {}".format(exec_location, self.MEM_LOCATIONS)
        self._exec_location = exec_location
        self._const_info = const_info
        self._comment = comment
        self._extern_pointer = extern_pointer

    @property
    def comment(self):
        return self._comment

    @comment.setter
    def comment(self, val):
        self._comment = val

    @property
    def const_info(self):
        return self._const_info

    @property
    def arg_name(self):
        return self._arg_name

    def __str__(self):
        tcarg = "TCArgInfoA" if self._extern_pointer else "TCArgInfo"
        return str.format('{}("{}", "{}", {}, {}, {}, {}, {})',
                          tcarg,
                          self.ARG_TYPES[self._arg_type],
                          self._arg_name,
                          self._arg_scope,
                          self._arg_dir,
                          self._home_location or self.MEM_LOCATIONS[0],
                          self._exec_location or self.MEM_LOCATIONS[0],
                          self._const_info or "0")


class GlobalResetArgInfo(TCArgInfo):

    def __init__(self, arg_name: str, home_location: Optional[str] = None,
                 exec_location: Optional[str] = None,
                 comment=None):
        super(GlobalResetArgInfo, self).__init__(
            "bool", arg_name, "ARG_SCOPE_ARG",
            "ARG_DIR_IN",
            home_location=home_location,
            exec_location=exec_location,
            comment=comment)

class GlobalInitializerArgInfo(TCArgInfo):

    def __init__(self, arg_type: str, arg_name: str, home_location: Optional[str] = None,
                 exec_location: Optional[str] = None,
                 is_inout=False,
                 comment=None):
        super(GlobalInitializerArgInfo, self).__init__(
            arg_type, arg_name, "ARG_SCOPE_GLOBAL",
            "ARG_DIR_INOUT" if is_inout else "ARG_DIR_IN",
            home_location=home_location,
            exec_location=exec_location,
            comment=comment)

class LocalInitializerArgInfo(TCArgInfo):

    def __init__(self, arg_type: str, arg_name: str, home_location: Optional[str] = None,
                 exec_location: Optional[str] = None,
                 is_inout=False,
                 comment=None):
        super(LocalInitializerArgInfo, self).__init__(
            arg_type, arg_name, "ARG_SCOPE_LOCAL",
            "ARG_DIR_INOUT" if is_inout else "ARG_DIR_IN",
            home_location=home_location,
            exec_location=exec_location,
            comment=comment)

class GlobalArgInfo(TCArgInfo):

    def __init__(self, arg_type: str, arg_name: str, home_location: Optional[str] = None,
                 exec_location: Optional[str] = None,
                 const_info: Optional[ConstantInfo] = None,
                 comment=None):
        super(GlobalArgInfo, self).__init__(
            arg_type, arg_name, "ARG_SCOPE_GLOBAL",
            "ARG_DIR_CONSTIN", home_location=home_location,
            exec_location=exec_location,
            const_info=const_info,
            comment=comment)


class LocalArgInfo(TCArgInfo):
    def __init__(self, arg_type: str, arg_name: str, home_location: Optional[str] = None,
                 exec_location: Optional[str] = None):
        super(LocalArgInfo, self).__init__(
            arg_type, arg_name, "ARG_SCOPE_LOCAL",
            "ARG_DIR_INOUT", home_location=home_location,
            exec_location=exec_location)


class OutputArgInfo(TCArgInfo):
    def __init__(self, arg_type: str, arg_name: str, home_location: Optional[str] = None,
                 exec_location: Optional[str] = None, allocate=False,
                 extern_output_pointer=None):
        scope = "ARG_SCOPE_ARG_ALLOC" if allocate else "ARG_SCOPE_ARG"
        super(OutputArgInfo, self).__init__(
            arg_type, arg_name, scope,
            "ARG_DIR_OUT", home_location=home_location,
            exec_location=exec_location,
            extern_pointer=extern_output_pointer)


class InputArgInfo(TCArgInfo):
    def __init__(self, arg_type: str, arg_name: str, home_location: Optional[str] = None,
                 exec_location: Optional[str] = None, allocate=False, is_inout=False,
                 extern_input_pointer=None):
        scope = "ARG_SCOPE_ARG_ALLOC" if allocate else "ARG_SCOPE_ARG"
        direction = "ARG_DIR_INOUT" if is_inout else "ARG_DIR_IN"
        super(InputArgInfo, self).__init__(
            arg_type, arg_name, scope,
            direction, home_location=home_location,
            exec_location=exec_location,
            extern_pointer=extern_input_pointer)
