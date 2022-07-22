#
# Copyright (C) 2019 GreenWaves Technologies
#


import os.path

c_head_pattern = """
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2020 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

"""

def get_c_desc(name):
    if name is None:
        return name
    else:
        return name.replace('\n', ' ').encode('ascii', 'ignore').decode('ascii')

def get_c_name(name):
    return name.replace('/', '_').replace('.', '_').replace(' ', '_').encode('ascii', 'ignore').decode('ascii')

def get_c_name_no_digit_first(name):
    result = name.replace('/', '_').replace('.', '_').replace(' ', '_').encode('ascii', 'ignore').decode('ascii')
    if result[0].isdigit():
        result = '_' + result

    return result


class Header(object):

    def __init__(self, name, def_name, path, inc_stdint=False, inc_utils=False):
        try:
            os.makedirs(os.path.dirname(path))
        except:
            pass
        self.file = open(path, 'w')
        self.name = name
        self.file.write(c_head_pattern)
        def_name = get_c_name(def_name).upper()
        self.file.write('#ifndef __ARCHI_%s__\n' % def_name)
        self.file.write('#define __ARCHI_%s__\n' % def_name)
        self.file.write('\n')
        self.file.write('#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)\n')
        self.file.write('\n')
        if inc_stdint:
            self.file.write('#include <stdint.h>\n')
        if inc_utils:
            self.file.write('#include "archi/gap_utils.h"\n')
        self.file.write('\n')
        self.file.write('#endif\n')
        self.file.write('\n')

    def close(self):
        self.file.write('\n')
        self.file.write('#endif\n')



class Constant(object):

    def dump_to_header(self, header):
        header.file.write('#define %s_%s %s\n' % (get_c_name(header.name).upper(), get_c_name(self.name).upper(), self.value))



class Regfield(object):

    def __dump_file(self, file, msg, rst=False):
        indent = '' if not rst else '        '
        file.write('%s%s' % (indent, msg))

    def dump_to_header(self, header, reg_name, rst=False):
        field_name = '%s_%s' % (get_c_name(reg_name), get_c_name(self.name).upper())
        access_str = ''
        if self.access is not None:
          access_str = ' (access: %s)' % self.access
        if self.desc != '' or access_str != '':
          self.__dump_file(header.file, '// %s%s\n' % (get_c_desc(self.desc), access_str), rst)
        self.__dump_file(header.file, '#define %-60s %d\n' % (field_name + '_BIT', self.bit), rst)
        self.__dump_file(header.file, '#define %-60s %d\n' % (field_name + '_WIDTH', self.width), rst)
        self.__dump_file(header.file, '#define %-60s 0x%x\n' % (field_name + '_MASK', ((1<<self.width)-1)<<self.bit), rst)
        reset = self.reset

        if reset is None and self.reg_reset is not None:
          reset = (self.reg_reset >> self.bit) & ((1<<self.width) - 1)

        if reset is not None:
            self.__dump_file(header.file, '#define %-60s 0x%x\n' % (field_name + '_RESET', reset), rst)

    def dump_macros(self, header, reg_name=None, rst=False, reg=None):
        self.__dump_file(header.file, '\n', rst)
        field_name = '%s_%s' % (get_c_name(reg_name), get_c_name(self.name).upper())
        if reg is not None and self.width == reg.width:
            self.__dump_file(header.file, '#define %-50s (value)\n' % (field_name + '_GET(value)'), rst)
            self.__dump_file(header.file, '#define %-50s (value)\n' % (field_name + '_GETS(value)'), rst)
            self.__dump_file(header.file, '#define %-50s (field)\n' % (field_name + '_SET(value,field)'), rst)
        else:
            self.__dump_file(header.file, '#define %-50s (GAP_BEXTRACTU((value),%d,%d))\n' % (field_name + '_GET(value)', self.width, self.bit), rst)
            self.__dump_file(header.file, '#define %-50s (GAP_BEXTRACT((value),%d,%d))\n' % (field_name + '_GETS(value)', self.width, self.bit), rst)
            self.__dump_file(header.file, '#define %-50s (GAP_BINSERT((value),(field),%d,%d))\n' % (field_name + '_SET(value,field)', self.width, self.bit), rst)
        self.__dump_file(header.file, '#define %-50s ((val) << %d)\n' % (field_name + '(val)', self.bit), rst)

    def dump_access_functions(self, reg, header=None, rst=False):
        reg_name = '%s_%s' % (get_c_name(header.name), get_c_name(reg.name))
        reg_c_name = get_c_name(reg_name).upper()
        field_c_name = get_c_name(self.name)

        indent = '' if not rst else '        '
        header.file.write("\n")

        if self.width == 32:
            set_implem = '\n{\n    GAP_WRITE(base, %s_OFFSET, value);\n}\n' % (reg_c_name) if not rst else ';'
            getu_implem = '\n{\n    return GAP_READ(base, %s_OFFSET);\n}\n' % (reg_c_name) if not rst else ';'
            get_implem = '\n{\n    return GAP_READ(base, %s_OFFSET);\n}\n' % (reg_c_name) if not rst else ';'
        else:
            set_implem = '\n{\n    GAP_WRITE(base, %s_OFFSET, GAP_BINSERT(GAP_READ(base, %s_OFFSET), value, %d, %d));\n}\n' % (reg_c_name, reg_c_name, self.width, self.bit) if not rst else ';'
            getu_implem = '\n{\n    return GAP_BEXTRACTU(GAP_READ(base, %s_OFFSET), %d, %d);\n}\n' % (reg_c_name, self.width, self.bit) if not rst else ';'
            get_implem = '\n{\n    return GAP_BEXTRACT(GAP_READ(base, %s_OFFSET), %d, %d);\n}\n' % (reg_c_name, self.width, self.bit) if not rst else ';'

        header.file.write("%sstatic inline __attribute__((always_inline)) void %s_%s_set(uint32_t base, uint32_t value)%s\n" % (indent, reg_name.lower(), field_c_name.lower(), set_implem))
        header.file.write("%sstatic inline __attribute__((always_inline)) uint32_t %s_%s_get(uint32_t base)%s\n" % (indent, reg_name.lower(), field_c_name.lower(), getu_implem))
        header.file.write("%sstatic inline __attribute__((always_inline)) int32_t %s_%s_gets(uint32_t base)%s\n" % (indent, reg_name.lower(), field_c_name.lower(), get_implem))




class Register(object):

    def __dump_toggle_header(self, rst, name):
        rst.file.write('\n')
        rst.file.write('.. toggle-header::\n')
        rst.file.write('    :header: *%s*\n' % name)
        rst.file.write('\n')
        rst.file.write('    .. code-block:: c\n')
        rst.file.write('\n')

    def dump_c_headers_to_rst(self, rst):
        self.__dump_toggle_header(rst, 'Register map C offsets')
        self.dump_to_header(rst, rst=True)

        self.__dump_toggle_header(rst, 'Register accessors')
        self.dump_access_functions(rst, rst=True)

        self.__dump_toggle_header(rst, 'Register fields defines')
        self.dump_fields_to_header(rst, rst=True)

        self.__dump_toggle_header(rst, 'Register fields macros')
        self.dump_macros(rst, rst=True)

        self.__dump_toggle_header(rst, 'Register fields structures')
        self.dump_struct(rst, rst=True)

        self.__dump_toggle_header(rst, 'GVSOC registers')
        self.dump_vp_class(rst, rst=True)

        rst.file.write('\n')

    def dump_to_header(self, header, rst=False):

        if self.offset is not None:
            indent = '' if not rst else '        '

            self.__dump_file(header.file, '\n', rst)
            if self.desc != '':
                self.__dump_file(header.file, '%s// %s\n' % (indent, get_c_desc(self.desc)), rst)

            self.__dump_file(header.file, '%s#define %-40s 0x%x\n' % (indent, '%s_%s_OFFSET' % (get_c_name(header.name).upper(), get_c_name(self.name).upper()), self.offset), rst)

    def dump_fields_to_header(self, header, rst=False):

        for name, field in self.fields.items():
            self.__dump_file(header.file, '\n', rst)
            reg_name = '%s_%s' % (get_c_name(header.name).upper(), get_c_name(self.name).upper())
            field.dump_to_header(reg_name=reg_name, header=header, rst=rst)

    def dump_struct(self, header, rst=False):

        self.__dump_file(header.file, '\n', rst)
        self.__dump_file(header.file, 'typedef union {\n', rst)
        self.__dump_file(header.file, '  struct {\n', rst)

        current_index = 0
        current_pad = 0
        for name, field in self.fields.items():
            if current_index < field.bit:
                self.__dump_file(header.file, '    unsigned int padding%d:%-2d;\n' % (current_pad, field.bit - current_index), rst)
                current_pad += 1

            current_index = field.bit + field.width

            if field.width <= 32:
                self.__dump_file(header.file, '    unsigned int %-16s:%-2d; // %s\n' % (get_c_name_no_digit_first(field.name).lower(), field.width, get_c_desc(field.desc)), rst)
            else:
                self.__dump_file(header.file, '    unsigned long long int %-16s:%-2d; // %s\n' % (get_c_name_no_digit_first(field.name).lower(), field.width, get_c_desc(field.desc)), rst)

        self.__dump_file(header.file, '  };\n', rst)

        if current_index <= 32:
            self.__dump_file(header.file, '  unsigned int raw;\n', rst)
        else:
            self.__dump_file(header.file, '  unsigned int raw[%d];\n' % ((current_index + 31) / 32), rst)

        self.__dump_file(header.file, '} __attribute__((packed)) %s_%s_t;\n' % (get_c_name(header.name).lower(), get_c_name(self.name).lower()), rst)

    def __dump_file(self, file, msg, rst=False):
        indent = '' if not rst else '        '
        file.write('%s%s' % (indent, msg))

    def get_vp_name(self, header):
        return 'vp_%s_%s' % (get_c_name(header.name).lower(), get_c_name(self.name).lower())

    def dump_vp_class(self, header, rst=False):
        width = self.width

        try:
            if width > 1 and width < 8:
                width = 8
            elif width > 8 and width < 16:
                width = 16
            elif width > 16 and width < 32:
                width = 32
            elif width > 32 and width < 64:
                width = 64
        except:
            width = 32

        self.__dump_file(header.file, '\n', rst)
        self.__dump_file(header.file, 'class %s : public vp::reg_%d\n' % (self.get_vp_name(header), width), rst)
        self.__dump_file(header.file, '{\n', rst)
        self.__dump_file(header.file, 'public:\n', rst)

        reg_name = '%s_%s' % (get_c_name(header.name).upper(), get_c_name(self.name).upper())
        for name, field in self.fields.items():
            field_name = '%s_%s' % (get_c_name(reg_name), get_c_name(field.name).upper())
            implem = ' { this->set_field(value, %s_BIT, %s_WIDTH); }' % (field_name, field_name) if not rst else ';'
            self.__dump_file(header.file, '    inline void %s_set(uint%d_t value)%s\n' % (get_c_name_no_digit_first(field.name).lower(), width, implem), rst)
            implem = ' { return this->get_field(%s_BIT, %s_WIDTH); }' % (field_name, field_name) if not rst else ';'
            self.__dump_file(header.file, '    inline uint%d_t %s_get()%s\n' % (width, get_c_name_no_digit_first(field.name).lower(), implem), rst)

        if not rst:

            reg_code = '        this->hw_name = "%s";\n' % (self.name)
            reg_code += '        this->offset = 0x%x;\n' % (self.offset if self.offset is not None else 0)
            reg_code += '        this->width = %d;\n' % (self.width if self.width is not None else 32)
            reg_code += '        this->do_reset = %d;\n' % (self.do_reset)
            reg_code += '        this->write_mask = 0x%x;\n' % (self.get_write_mask() & 0xFFFFFFFF)
            if self.reset is not None:
                reg_code += '        this->reset_val = 0x%x;\n' % (self.reset)
            for field in self.get_fields():
                reg_code += '        this->regfields.push_back(new vp::regfield("%s", %d, %d));\n' % (field.name, field.bit, field.width)

            self.__dump_file(header.file, '    %s()\n    {\n%s    }\n' % (self.get_vp_name(header), reg_code))

        self.__dump_file(header.file, '};\n', rst)

    def dump_macros(self, header=None, rst=False):
        reg_name = '%s_%s' % (get_c_name(header.name).upper(), get_c_name(self.name).upper())
        for name, field in self.fields.items():
            field.dump_macros(header, reg_name, rst=rst, reg=self)

    def dump_regfields_access_functions(self, header=None, rst=False):
        for name, field in self.fields.items():
            field.dump_access_functions(reg=self, header=header)


    def dump_access_functions(self, header=None, rst=False):
        reg_name = '%s_%s' % (get_c_name(header.name), get_c_name(self.name))

        if self.offset is not None:
            indent = '' if not rst else '        '
            header.file.write("\n")

            get_implem = '\n{\n    return GAP_READ(base, %s_OFFSET);\n}\n' % (get_c_name(reg_name).upper()) if not rst else ';'
            set_implem = '\n{\n    GAP_WRITE(base, %s_OFFSET, value);\n}\n' % (get_c_name(reg_name).upper()) if not rst else ';'

            header.file.write("%sstatic inline __attribute__((always_inline)) uint32_t %s_get(uint32_t base)%s\n" % (indent, reg_name.lower(), get_implem));
            header.file.write("%sstatic inline __attribute__((always_inline)) void %s_set(uint32_t base, uint32_t value)%s\n" % (indent, reg_name.lower(), set_implem));



class Regmap(object):

    def __dump_file(self, file, msg, rst=False):
        indent = '' if not rst else '        '
        file.write('%s%s' % (indent, msg))

    def __dump_toggle_header(self, rst, name):
        rst.file.write('\n')
        rst.file.write('.. toggle-header::\n')
        rst.file.write('    :header: *%s*\n' % name)
        rst.file.write('\n')
        rst.file.write('    .. code-block:: c\n')
        rst.file.write('\n')

    def dump_vp_class(self, header, rst=False):
        self.__dump_file(header.file, '\n', rst)
        self.__dump_file(header.file, 'class vp_regmap_%s : public vp::regmap\n' % (get_c_name(self.name).lower()), rst)
        self.__dump_file(header.file, '{\n', rst)
        self.__dump_file(header.file, 'public:\n', rst)

        for register in self.registers.values():
            if register.offset is not None:
                self.__dump_file(header.file, '    %s %s;\n' % (register.get_vp_name(header), register.name.lower()), rst)

        if not rst:
            reg_init_code = ''
            for register in self.registers.values():
                if register.offset is not None:
                    reg_init_code += '        this->registers.push_back(&this->%s);\n' % (register.name.lower())

            self.__dump_file(header.file, '    vp_regmap_%s()\n    {\n%s    }\n' % (get_c_name(self.name).lower(), reg_init_code))

        self.__dump_file(header.file, '};\n', rst)

    def dump_regs_to_rst(self, rst):
        self.__dump_toggle_header(rst, 'Register map C offsets')
        for name, register in self.registers.items():
            register.dump_to_header(rst, rst=True)

        self.__dump_toggle_header(rst, 'Register accessors')
        for name, register in self.registers.items():
            register.dump_access_functions(rst, rst=True)

        self.__dump_toggle_header(rst, 'Register fields defines')
        for name, register in self.registers.items():
            register.dump_fields_to_header(rst, rst=True)

        self.__dump_toggle_header(rst, 'Register fields macros')
        for name, register in self.registers.items():
            register.dump_macros(rst, rst=True)

        self.__dump_toggle_header(rst, 'Register map structure')
        self.dump_regmap_to_header(rst, rst=True)

        self.__dump_toggle_header(rst, 'Register fields structures')
        for name, register in self.registers.items():
            register.dump_struct(rst, rst=True)

        self.__dump_toggle_header(rst, 'GVSOC registers')
        self.dump_vp_class(rst, rst=True)

        rst.file.write('\n')

    def dump_regs_to_header(self, header):

        header.file.write('\n')
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('//\n')
        header.file.write('// REGISTERS\n')
        header.file.write('//\n')

        for name, register in self.registers.items():
            register.dump_to_header(header)

    def dump_regfields_to_header(self, header):
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('//\n')
        header.file.write('// REGISTERS FIELDS\n')
        header.file.write('//\n')

        for name, register in self.registers.items():
            register.dump_fields_to_header(header=header)

    def dump_structs_to_header(self, header):
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('//\n')
        header.file.write('// REGISTERS STRUCTS\n')
        header.file.write('//\n')
        header.file.write('\n')
        header.file.write('#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)\n')

        for name, register in self.registers.items():
            register.dump_struct(header=header)

        header.file.write('\n')
        header.file.write('#endif\n')

    def dump_cmds_to_header(self, header):
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('//\n')
        header.file.write('// COMMANDS\n')
        header.file.write('//\n')
        header.file.write('\n')
        header.file.write('#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)\n')

        for name, cmdmap in self.cmdmaps.items():
            cmdmap.dump_to_header(header=header)

        header.file.write('\n')
        header.file.write('#endif\n')

    def dump_vp_structs_to_header(self, header):
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('//\n')
        header.file.write('// REGISTERS STRUCTS\n')
        header.file.write('//\n')
        header.file.write('\n')
        header.file.write('#ifdef __GVSOC__\n')

        for name, register in self.registers.items():
            register.dump_vp_class(header=header)

        header.file.write('\n')

        self.dump_vp_class(header=header)

        header.file.write('\n')
        header.file.write('#endif\n')


    def dump_regmap_to_header(self, header, rst=False):
        if not rst:
            header.file.write('\n')
            header.file.write('\n')
            header.file.write('\n')
            header.file.write('//\n')
            header.file.write('// REGISTERS GLOBAL STRUCT\n')
            header.file.write('//\n')
            header.file.write('\n')
            header.file.write('#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)\n')

            header.file.write('\n')

        self.__dump_file(header.file, ('/** %s_Type Register Layout Typedef */\n' % header.name.upper()), rst)
        self.__dump_file(header.file, 'typedef struct {\n', rst)

        cur_idx = 0
        prev_idx = 0
        padding = 0
        padding_idx = 0
        for name, register in self.registers.items():
            if register.offset is None:
                continue

            cur_idx = register.offset
            if (cur_idx != 0):
                if (cur_idx > (prev_idx + 4)):
                    padding = ((cur_idx - prev_idx) / 4) - 1
            desc = ''
            if register.desc != '':
                desc = ' // %s' % register.desc
            if (padding != 0):
                desc_reserved = " // Reserved/Not used."
                self.__dump_file(header.file, '    volatile uint32_t reserved_%d[%d]; %s\n' % (padding_idx, padding, get_c_desc(desc_reserved)), rst)
                padding_idx = padding_idx + 1
                padding = 0
            self.__dump_file(header.file, '    volatile uint32_t %s; %s\n' % (get_c_name(register.name).lower(), get_c_desc(desc)), rst)
            prev_idx = cur_idx

        self.__dump_file(header.file, '} __attribute__((packed)) %s_t;\n' % (get_c_name(self.name).lower()), rst)

        self.__dump_file(header.file, ('/** %s_Type Register Layout Typedef */\n' % header.name.upper()), rst)
        self.__dump_file(header.file, 'typedef struct {\n', rst)

        if not rst:
            header.file.write('\n')


        cur_idx = 0
        prev_idx = 0
        padding = 0
        padding_idx = 0
        for name, register in self.registers.items():
            if register.offset is None:
                continue

            cur_idx = register.offset
            if (cur_idx != 0):
                if (cur_idx > (prev_idx + 4)):
                    padding = ((cur_idx - prev_idx) / 4) - 1
            desc = ''
            if register.desc != '':
                desc = ' // %s' % register.desc
            if (padding != 0):
                desc_reserved = " // Reserved/Not used."
                self.__dump_file(header.file, '    volatile uint32_t reserved_%d[%d]; %s\n' % (padding_idx, padding, get_c_desc(desc_reserved)), rst)
                padding_idx = padding_idx + 1
                padding = 0
            self.__dump_file(header.file, '    volatile %s_%s_t %s; %s\n' % (get_c_name(header.name).lower(), get_c_name(register.name).lower(), get_c_name(register.name).lower(), get_c_desc(desc)), rst)
            prev_idx = cur_idx

        self.__dump_file(header.file, '} __attribute__((packed)) %s_struct_t;\n' % (get_c_name(self.name).lower()), rst)



        if not rst:
            header.file.write('\n')
            header.file.write('#endif  /* LANGUAGE_ASSEMBLY || __ASSEMBLER__ */\n')


    def dump_accessors_to_header(self, header):
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('//\n')
        header.file.write('// REGISTERS ACCESS FUNCTIONS\n')
        header.file.write('//\n')
        header.file.write('\n')
        header.file.write('#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)\n')

        for name, register in self.registers.items():
            register.dump_access_functions(header=header)


        header.file.write('\n')
        header.file.write('#endif\n')


    def dump_regfields_accessors_to_header(self, header):
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('//\n')
        header.file.write('// REGISTERS FIELDS ACCESS FUNCTIONS\n')
        header.file.write('//\n')
        header.file.write('\n')
        header.file.write('#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)\n')

        for name, register in self.registers.items():
            register.dump_regfields_access_functions(header=header)


        header.file.write('\n')
        header.file.write('#endif\n')



    def dump_macros_to_header(self, header):
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('\n')
        header.file.write('//\n')
        header.file.write('// REGISTERS FIELDS MACROS\n')
        header.file.write('//\n')
        header.file.write('\n')
        header.file.write('#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)\n')

        for name, register in self.registers.items():
            register.dump_macros(header=header)


        header.file.write('\n')
        header.file.write('#endif\n')

    def dump_groups_to_header(self, header):

        for name, group in self.regmaps.items():

            header.file.write('\n')
            header.file.write('\n')
            header.file.write('\n')

            header.file.write('//\n')
            header.file.write('// GROUP %s\n' % name)
            header.file.write('//\n')

            if group.offset is not None:
                header.file.write('\n')
                header.file.write('#define %-40s 0x%x\n' % ('%s_%s_OFFSET' % (get_c_name(header).name.upper(), get_c_name(name).upper()), group.offset))

            group.dump_to_header(header)


    def dump_constants_to_header(self, header):

        if len(self.constants) != 0:
            header.file.write('\n')
            header.file.write('\n')
            header.file.write('\n')
            header.file.write('//\n')
            header.file.write('// CUSTOM FIELDS\n')
            header.file.write('//\n')

        for name, constant in self.constants.items():
            constant.dump_to_header(header=header)

    def dump_to_header(self, header, header_path):
        header.file.write('#include "%s_regs.h"\n' % (header_path))
        header.file.write('#include "%s_regfields.h"\n' % (header_path))
        header.file.write('#include "%s_structs.h"\n' % (header_path))
        header.file.write('#include "%s_regmap.h"\n' % (header_path))
        header.file.write('#include "%s_accessors.h"\n' % (header_path))
        header.file.write('#include "%s_regfields_accessors.h"\n' % (header_path))
        header.file.write('#include "%s_macros.h"\n' % (header_path))
        header.file.write('#include "%s_groups.h"\n' % (header_path))
        header.file.write('#include "%s_constants.h"\n' % (header_path))


class Cmdfield(object):
    def dump_to_header(self, header):
        if('ARG' not in self.value):
            header.file.write('(0x%x << 0x%x)' % (int(self.value,0), self.offset))
        else:
            header.file.write('(%s << 0x%x)' % (self.name.lower(), self.offset))

class Cmd(object):
    def dump_to_header(self, header):
        self.dump_cmdfield_to_header(header=header)

    def dump_cmdfield_to_header(self, header):
        header.file.write('#define %s_%s(' % (header.name.upper(),self.name.upper()))
        first = 1
        for name, cmdfield in self.fields.items():
            if( first == 0 and ('ARG' in cmdfield.value)):
                header.file.write(',')
            if('ARG' in cmdfield.value):
                header.file.write('%s' % cmdfield.name.lower())
                first = 0
        header.file.write(') ')
        
        header.file.write('(')
        first = 1
        for name, cmdfield in self.fields.items():
            if(first != 1):
                header.file.write('|')
            first= 0 
            cmdfield.dump_to_header(header=header)
        header.file.write(')\n')

class Cmdmap(object):
    def dump_cmd_to_header(self, header):
        header.file.write('\n/* ----------------------------------------------------------------------------\n')
        header.file.write('\n CMD IDs and macros\n')
        header.file.write('\n----------------------------------------------------------------------------*/\n')
        for name, command in self.commands.items():
            command.dump_to_header(header)

    def dump_to_header(self, header):
        self.dump_cmd_to_header(header)




def dump_to_header(regmap, name, header_path):
    header_file = Header(name, name, header_path + '.h')
    regmap.dump_to_header(header_file, os.path.basename(header_path))
    header_file.close()

    header_file = Header(name, name + '_regs', header_path + '_regs.h')
    regmap.dump_regs_to_header(header_file)
    header_file.close()

    header_file = Header(name, name + '_regfield', header_path + '_regfields.h')
    regmap.dump_regfields_to_header(header_file)
    header_file.close()

    header_file = Header(name, name + '_structs', header_path + '_structs.h')
    regmap.dump_structs_to_header(header_file)
    header_file.close()

    header_file = Header(name, name + '_cmd', header_path + '_cmd.h')
    regmap.dump_cmds_to_header(header_file)
    header_file.close()

    header_file = Header(name, name + '_gvsoc', header_path + '_gvsoc.h', inc_stdint=True)
    regmap.dump_vp_structs_to_header(header_file)
    header_file.close()

    header_file = Header(name, name + '_regmap', header_path + '_regmap.h', inc_stdint=True)
    regmap.dump_regmap_to_header(header_file)
    header_file.close()

    header_file = Header(name, name + '_accessors', header_path + '_accessors.h', inc_stdint=True, inc_utils=True)
    regmap.dump_accessors_to_header(header_file)
    header_file.close()

    header_file = Header(name, name + '_regfields_accessors', header_path + '_regfields_accessors.h', inc_stdint=True, inc_utils=True)
    regmap.dump_regfields_accessors_to_header(header_file)
    header_file.close()

    header_file = Header(name, name + '_macros', header_path + '_macros.h', inc_utils=True)
    regmap.dump_macros_to_header(header_file)
    header_file.close()

    header_file = Header(name, name + '_groups', header_path + '_groups.h')
    regmap.dump_groups_to_header(header_file)
    header_file.close()

    header_file = Header(name, name + '_constants', header_path + '_constants.h')
    regmap.dump_constants_to_header(header_file)
    header_file.close()
