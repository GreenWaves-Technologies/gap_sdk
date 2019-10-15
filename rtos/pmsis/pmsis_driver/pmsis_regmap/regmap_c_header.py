#
# Copyright (C) 2018 ETH Zurich, University of Bologna
# and GreenWaves Technologies
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



c_head_pattern = """/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT. */
/*
 * Copyright (C) 2019 ETH Zurich, University of Bologna
 * and GreenWaves Technologies
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


class Header(object):

    def __init__(self, name, path):
        self.file = open(path, 'w')
        self.name = name
        self.file.write(c_head_pattern)
        def_name = path.replace('/', '_').replace('.', '_').upper()
        self.file.write('#ifndef __%s__\n' % def_name)
        self.file.write('#define __%s__\n' % def_name)
        self.file.write('\n')

    def close(self, path):
        def_name = path.replace('/', '_').replace('.', '_').upper()
        self.file.write('\n')
        self.file.write('#endif /* __%s__ */\n' % def_name)



class Constant(object):

    def dump_to_header(self, header):
        header.file.write('#define %s_%s %s\n' % (header.name.upper(), self.name.upper(), self.value))



class Regfield(object):

    def dump_to_header(self, header, reg_name):
        field_name = '%s_%s' % (reg_name, self.name.upper())
        access_str = ''
        #if self.access is not None:
        #  access_str = ' (access: %s)' % self.access
        #if self.desc != '' or access_str != '':

        header.file.write('/* %s%s */\n' % (self.desc .replace('\n', '\n  '), access_str))
        header.file.write('#define %-60s 0x%x\n' % (field_name + '_MASK', ((1<<self.width)-1)<<self.bit))
        header.file.write('#define %-60s %d\n' % (field_name + '_SHIFT', self.bit))
        header.file.write('#define %-60s ((val) << %d)\n\n' % (field_name + '(val)', self.bit))



class Register(object):

    def dump_to_header(self, header):
        header.file.write('\n')
        if self.desc != '':
            header.file.write('// %s\n' % self.desc.replace('\n', ' '))
        header.file.write('#define %-40s 0x%x\n' % ('%s_%s_OFFSET' % (header.name.upper(), self.name.upper()), self.offset))

    def dump_fields_to_header(self, header):
        header.file.write('\n/*! @name %s */\n' % self.name)
        for name, field in self.fields.items():
            reg_name = '%s_%s' % (header.name.upper(), self.name.upper())
            field.dump_to_header(reg_name=reg_name, header=header)

    def dump_macros(self, header=None):
        reg_name = '%s_%s' % (header.name.upper(), self.name.upper())
        for name, field in self.fields.items():
            field.dump_macros(header, reg_name)
    
    def dump_struct(self, header):
        header.file.write('\n/*! @name %s_%s_t */\n' % ( header.name, self.name.lower()))
        header.file.write('\n')
        header.file.write('typedef union {\n')
        header.file.write('  struct {\n')

        current_index = 0
        current_pad = 0
        for name, field in self.fields.items():
            if current_index < field.bit:
                header.file.write('    unsigned int padding%d:%-2d;\n' % (current_pad, field.bit - current_index))
                current_pad += 1

            current_index = field.bit + field.width

            header.file.write('    unsigned int %-16s:%-2d; // %s\n' % (field.name.lower(), field.width, field.desc.replace('\n', ' ')))

        header.file.write('  };\n')
        header.file.write('  unsigned int raw;\n')
        header.file.write('} __attribute__((packed)) %s_%s_t;\n' % (header.name.lower(), self.name.lower()))


class Regmap(object):

    def dump_regfields_to_header(self, header, udma_ip_type):
        header.file.write('\n/* ----------------------------------------------------------------------------\n')
        header.file.write('   -- %s Peripheral Access Layer --\n' % header.name.upper())
        header.file.write('   ---------------------------------------------------------------------------- */\n')
        header.file.write('\n/** %s_Type Register Layout Typedef */\n' % header.name.upper())
        header.file.write('typedef struct %s\n{\n' % header.name.lower())

        elmt_count = 0
        nb_channels = 0
        channel_reg_nb = 4

        current_index = 0
        current_pad = 0
        if(udma_ip_type is not None):
            if("CMD" in udma_ip_type):
                nb_channels = 3*channel_reg_nb
                header.file.write('  udma_core_cmd_t %s; // core channels, cmd type\n' % (header.name.lower()))
            else:
                nb_channels = 2*channel_reg_nb
                header.file.write('  udma_core_t %s; // core channels, 2 channels type\n' % (header.name.lower()))
        for name, register in self.registers.items():
            if(elmt_count < nb_channels):
                elmt_count +=1
                current_index +=32
            else:
                if(current_index < register.offset):
                    header.file.write('  unsigned int padding%d:%-2d;\n' % (current_pad, register.offset - current_index))
                    current_pad += 1;
                else:
                    desc = '/**< %s */' % register.desc
                    header.file.write('  volatile uint32_t %s; %s\n' % (register.name.lower(), desc))
                    current_index+=32

        header.file.write('} __attribute__((packed)) %s_t;\n\n' % (header.name.lower()))

        header.file.write('\n/* ----------------------------------------------------------------------------\n')
        header.file.write('   -- %s Register Bitfield Access --\n' % header.name.upper())
        header.file.write('   ---------------------------------------------------------------------------- */\n')

        for name, register in self.registers.items():
            register.dump_struct(header=header)
            register.dump_fields_to_header(header=header)

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
                header.file.write('#define %-40s 0x%x\n' % ('%s_%s_OFFSET' % (header.name.upper(), name.upper()), group.offset))

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

    def dump_to_header(self, header, udma_ip_type):
        if(udma_ip_type is not None):
            if('CMD' in udma_ip_type):
                header.file.write('#include \"udma_core_cmd_periph.h\"')
            else:
                header.file.write('#include \"udma_core_periph.h\"')
        self.dump_regfields_to_header(header, udma_ip_type)
        self.dump_groups_to_header(header)
        self.dump_constants_to_header(header)

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

def dump_to_header(regmap, cmdmap, name, header_path, udma_ip_type):
    header_file = Header(name, header_path)
    regmap.dump_to_header(header_file, udma_ip_type)
    cmdmap.dump_to_header(header_file)
    header_file.close(header_path)
