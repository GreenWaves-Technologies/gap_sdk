#
# Copyright (C) 2019 GreenWaves Technologies
#

import pytablewriter
import os

class Rst_file(object):
    def __init__(self, name, path):
        self.file = open(path, "w")
        self.name = name

    def close(self):
        self.file.close()

    def get_file(self):
        return self.file

    def dump_title(self, title, depth, link=None):

        if depth == 1:
            title_char = '#'
        elif depth == 2:
            title_char = '*'
        elif depth == 3:
            title_char = '='
        elif depth == 4:
            title_char = '-'
        elif depth == 5:
            title_char = '^'
        else:
            title_char = '"'

        self.file.write('\n')
        if link is not None:
            self.file.write('.. _%s:\n' % link)
            self.file.write('\n')
        self.file.write(title + '\n')
        self.file.write(title_char * len(title) + '\n')
        self.file.write('\n')


class Cmdfield(object):

    def get_row(self):
        if self.width == 1:
            bit = '%d' % self.offset
        else:
            bit = '%d:%d' % (self.offset + self.width - 1, self.offset)

        return [bit, self.name, self.desc]

    def dump_to_rst(self, rst):
        rst.append(self.get_row())

class Cmd(object):

    def dump_to_cmdlist_rst(self, cmdmap, table):
        table.append([':ref:`%s<%s__%s>`' % (self.name, cmdmap.name, self.name), self.width, self.code, self.desc])

    def dump_to_rst(self, rst):
        writer = pytablewriter.RstGridTableWriter()
        writer.table_name = '\n%s:align: center\n%s:widths: 15 45 90' % (writer.indent_string, writer.indent_string)
        writer.header_list = ['Bit #', 'Name', 'Description']

        table = []
        for name, field in self.fields.items():
            field.dump_to_rst(table)

        writer.value_matrix = table

        writer.stream = rst.get_file()
        writer.write_table()


class Cmdmap(object):

    def dump_to_cmdlist_rst(self, rst, writer):
        writer = pytablewriter.RstGridTableWriter()
        writer.table_name = '\n%s:align: center\n%s:widths: 45 15 15 80' % (writer.indent_string, writer.indent_string)
        writer.header_list = ['Command name', 'Width', 'Command code', 'Description']

        table = []
        for name, cmd in self.commands.items():
            cmd.dump_to_cmdlist_rst(self, table)

        writer.value_matrix = table

        writer.stream = rst.get_file()
        writer.write_table()

        for name, cmd in self.commands.items():
            rst.dump_title(cmd.name, 6, link='%s__%s' % (self.name, cmd.name))
            cmd.dump_to_rst(rst)



class Regfield(object):

    def get_row(self):
        if self.width == 1:
            bit = '%d' % self.bit
        else:
            bit = '%d:%d' % (self.bit + self.width - 1, self.bit)

        return [bit, self.access, self.name, self.reset_txt, self.desc]

    def dump_to_rst(self, rst):
        rst.append(self.get_row())



class Register(object):

    def dump_to_rst(self, rst):
        if self.desc is not None:
            rst.file.write(self.desc)
            rst.file.write('\n')
            rst.file.write('\n')

        writer = pytablewriter.RstGridTableWriter()
        writer.table_name = '\n%s:align: center\n%s:widths: 13 12 45 24 85' % (writer.indent_string, writer.indent_string)
        writer.header_list = ['Bit #', 'R/W', 'Name', 'Reset', 'Description']

        table = []
        for name, field in self.fields.items():
            field.dump_to_rst(table)

        writer.value_matrix = table

        writer.stream = rst.get_file()
        writer.write_table()

        if os.environ.get('DUMP_C_HEADERS_TO_RST'):
            st.dump_title('Generated headers', 6)

            self.dump_c_headers_to_rst(rst=rst)

            rst.file.write('|\n')

    def dump_to_reglist_rst(self, regmap, table):
        table.append([':ref:`%s<%s__%s>`' % (self.name, regmap.name, self.name), '0x%X' % self.offset, self.width, self.desc])


class Regmap(object):

    def dump_to_rst(self, rst, pretty_name):

        if self.input_file is not None:
            rst.file.write('.. \n')
            rst.file.write('   Input file: %s\n' % self.input_file)

        if pretty_name is not None:
            rst.dump_title('Register map for %s' % pretty_name, 5)
        else:
            rst.dump_title('Register map', 5)

        rst.dump_title('Overview', 6)

        rst.file.write('\nRefer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.\n\n')

        writer = pytablewriter.RstGridTableWriter()
        writer.table_name = '\n%s:align: center\n%s:widths: 40 12 12 90' % (writer.indent_string, writer.indent_string)
        writer.header_list = ['Name', 'Offset', 'Width', 'Description']

        table = []
        for name, register in self.registers.items():
            register.dump_to_reglist_rst(self, table)

        writer.value_matrix = table

        writer.stream = rst.get_file()
        writer.write_table()


        if os.environ.get('DUMP_C_HEADERS_TO_RST'):
            rst.dump_title('Generated headers', 6)

            self.dump_regs_to_rst(rst=rst)

            rst.file.write('|\n')


        for name, register in self.registers.items():
            rst.dump_title(register.name, 6, link='%s__%s' % (self.name, register.name))
            register.dump_to_rst(rst)

        if len(self.cmdmaps) != 0:
            for name, cmdmap in self.cmdmaps.items():
                rst.dump_title(name, 5)

                cmdmap.dump_to_cmdlist_rst(rst, writer)



def dump_to_rst(regmap, name, rst_path, pretty_name):
    rst_file = Rst_file(name, rst_path)
    regmap.dump_to_rst(rst_file, pretty_name)
    rst_file.close()
