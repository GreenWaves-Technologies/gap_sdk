#
# Copyright (C) 2019 GreenWaves Technologies
#

import regmap as rmap
import marko
from marko import Markdown
from marko.ext.gfm import gfm, GFM
from marko.ast_renderer import ASTRenderer

class Node(object):

    def __init__(self, is_table=False):
        self.children = []
        self.level = None
        self.is_table = is_table
        self.tables = []

    def get_table(self, index=0):
        if index >= len(self.tables):
            return None

        return self.tables[index]

    def add_header(self, node):
        self.children.append(node)

    def add_children(self, node):
        self.children.append(node)

        if node.is_table:
            self.tables.append(node)

    def is_header(self):
        return self.level is not None

    def get_level(self):
        return self.level

    def set_header(self, level, title):
        self.level = level
        self.title = title

    def dump(self, indent=''):
        if self.is_header():
            print (indent + self.title)

        for child in self.children:
            child.dump('  ' + indent)

    def get(self, name):
        if self.is_header() and self.title == name:
            return self
        
        for child in self.children:
            ast = child.get(name)
            if ast is not None:
                return ast

        return None


class Document(Node):

    def __init__(self, md):
        super(Document, self).__init__()

        last_headers = [self] + [None] * 6
        last_header = None

        for child in md.children:
            node = parse_md(child)

            if node is not None:
                if node.is_header():
                    if last_headers[node.get_level() - 1] is not None:
                        last_headers[node.get_level() - 1].add_header(node)

                    last_headers[node.get_level()] = node
                    last_header = node

                else:
                    if last_header is not None:
                        last_header.add_children(node)


class Heading(Node):

    def __init__(self, md):
        super(Heading, self).__init__()

        self.set_header(md.level, md.children[0].children)


class Table(Node):

    def __init__(self, md):
        super(Table, self).__init__(is_table=True)

        nb_cols = md._num_of_cols

        self.table = []

        for md_row in md.children:

            row = [None]*nb_cols

            index = 0
            for cell in md_row.children:
                if len(cell.children) > 0:
                    row[index] = cell.children[0].children
                else:
                    row[index] = ''
                index += 1

            self.table.append(row)

        self.names = []

        for name in self.table[0]:
            self.names.append(name.lower())

    def get_index(self, names):
        for name in names:
            if name.lower() in self.names:
                return self.names.index(name.lower())

        return -1

    def get_elem(self, row, index):
        if index is None:
            return None

        return self.table[row+1][index]

    def get_elem_int(self, row, index):
        elem = self.get_elem(row, index)
        if elem is not None:
            return int(elem, 0)
        return None
    
    def get_size(self):
        return len(self.table) - 1



def parse_md(md):
    if type(md) == marko.block.Document:
        return Document(md)

    elif type(md) == marko.block.Heading:
        return Heading(md)

    elif type(md) == marko.ext.gfm.elements.Table:
        return Table(md)

    #print (type(md))


    return None



def import_md(regmap, path, registers=[]):
    
    with open(path, "rt") as fh:
        ast = parse_md(gfm.parse(fh.read()))

        registers_ast = ast.get('Registers')
        if registers_ast is None:
            raise RuntimeError("Didn't find section 'Registers'")

        table = registers_ast.get_table()
        if table is None:
            raise RuntimeError("Didn't find table in section 'Registers'")

        for reg_name in registers:
            reg = regmap.add_register(rmap.Register(name=reg_name))

        name_index = table.get_index(names=['Register Name', 'Name'])
        offset_index = table.get_index(names=['Offset', 'Address'])
        width_index = table.get_index(names=['Size'])
        properties_index = table.get_index(names=['Properties'])
        reset_index = table.get_index(names=['Default', 'Reset Value'])
        description_index = table.get_index(names=['Description', 'Short description'])
        do_reset_index = table.get_index(names=['Reset'])

        for index in range(0, table.get_size()):
            reg = regmap.add_register(rmap.Register(
                name=table.get_elem(index, name_index),
                offset=table.get_elem_int(index, offset_index)
            ))

            if properties_index != -1:
                for prop in table.get_elem(index, properties_index).split():
                    key, value = prop.split('=')
                    if key == 'template':
                        reg.template = value

            if width_index != -1:
                # Too many IPs has wrong register width, hard-code it to 32 bits
                reg.width = 32 #table.get_elem_int(index, width_index)

            if reset_index != -1:
                reg.reset = table.get_elem_int(index, reset_index)

            if description_index != -1:
                reg.desc = table.get_elem(index, description_index)

            if do_reset_index != -1:
                reg.do_reset = table.get_elem_int(index, do_reset_index)


        for register in regmap.get_registers():
            reg_name = register.get_field_template()
            register_ast = ast.get(reg_name)
            if register_ast is None:
                print ("WARNING: Didn't find section for register %s" % reg_name)

            else:
                fields_ast = register_ast.get('Fields')
                if fields_ast is not None:
                    table = fields_ast.get_table()
                else:
                    table = register_ast.get_table()

                if table is None:
                    print ("WARNING: Didn't find table in section %s" % reg_name)
                else:
                    name_index = table.get_index(names=['Field Name'])
                    bit_index = table.get_index(names=['Offset', 'Bit', 'Bit Position'])
                    width_index = table.get_index(names=['Size', 'Width'])
                    access_index = table.get_index(names=['Host Access Type', 'Access Type'])
                    reset_index = table.get_index(names=['Default', 'Reset Value'])
                    description_index = table.get_index(names=['Description', 'Short description'])
                    
                    for index in range(0, table.get_size()):
                        regfield = register.add_regfield(rmap.Regfield(
                            name=table.get_elem(index, name_index),
                            width=table.get_elem_int(index, width_index),
                            bit=table.get_elem_int(index, bit_index),
                            access=table.get_elem(index, access_index)
                        ))

                        if reset_index != -1:
                            regfield.reset = table.get_elem_int(index, reset_index)
                            regfield.reset_txt = table.get_elem(index, reset_index)
                        else:
                            regfield.reset_txt = "--"

                        if description_index != -1:
                            regfield.desc = table.get_elem(index, description_index)

        commands_ast = ast.get('Commands')
        if commands_ast is not None:
            for command_ast in commands_ast.children:
                cmdmap = rmap.Cmdmap(name=command_ast.title)
                regmap.add_cmdmap(cmdmap)
                
                table = command_ast.get_table();
                if table is None:
                    raise RuntimeError("Didn't find table in section 'Commands'")

                name_index = table.get_index(names=['Command Name'])
                size_index = table.get_index(names=['Size'])
                id_index = table.get_index(names=['Command field', 'id'])
                description_index = table.get_index(names=['Description', 'Short description'])

                for index in range(0, table.get_size()):
                    if  size_index != -1:
                        size = table.get_elem_int(index, size_index)
                    else:
                        size = 32

                    cmdmap.add_cmd(rmap.Cmd(
                        name=table.get_elem(index, name_index),
                        code=table.get_elem(index, id_index),
                        width=size,
                        desc=table.get_elem(index, description_index)
                    ))

                for cmd in cmdmap.get_cmds():
                    cmd_name = cmd.name
                    cmd_ast = ast.get(cmd_name)
                    if cmd_ast is None:
                        print ("WARNING: Didn't find section for command %s" % cmd_name)

                    else:
                        fields_ast = cmd_ast.get('Fields')

                    if fields_ast is not None:
                        table = fields_ast.get_table()
                    else:
                        table = cmd_ast.get_table()

                    if table is None:
                        print ("WARNING: Didn't find table in section %s" % cmd_name)
                    else:
                        name_index = table.get_index(names=['Field Name'])
                        bit_index = table.get_index(names=['Offset', 'Bit', 'Bit Position'])
                        width_index = table.get_index(names=['Size', 'Width'])
                        value_index = table.get_index(names=['Value'])
                        description_index = table.get_index(names=['Description', 'Short description'])

                        for index in range(0, table.get_size()):
                            cmd.add_cmdfield(rmap.Cmdfield(
                                name=table.get_elem(index, name_index),
                                width=table.get_elem_int(index, width_index),
                                offset=table.get_elem_int(index, bit_index),
                                value=table.get_elem(index, value_index),
                                desc=table.get_elem(index, description_index)
                            ))
