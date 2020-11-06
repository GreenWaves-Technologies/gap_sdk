#
# Copyright (C) 2019 GreenWaves Technologies
#

import regmap as rmap
import mistune as mt
from bs4 import BeautifulSoup
import bs4
import html2text as htt

# def get_name(field):
    # if field[0].isdigit():
        # field = '_' + field
    # return field.replace(' ', '_')
# 
# def get_description(field):
    # return field.replace('\n', ' ').replace('\r', ' ')

def is_header(n): return n.name[0] == 'h'

def header_name(level): return 'h' + str(level)

def header_node_level(hn):
    assert hn.name[0] == 'h'
    return int(hn.name[1])

def build_header_hierarchy(node, hier = []):
    pass

def get_description(node):
    description_header_level = header_node_level(node)
    description_nodes = []
    for i in node.find_next_siblings():
        if is_header(i): break
        else:
            description_nodes.append(i)

    description_html = "".join(map(str, description_nodes))
    description_text = htt.html2text(description_html)
    return description_text


def get_register_node(current_node, reg_name):
    for register_header in current_node.find_next_siblings(header_name(header_node_level(current_node)+1)):
        name = register_header.decode_contents()

        if name == reg_name:
            return register_header

    return None

def get_register_field_table(current_node):
    for i in current_node.find_next_siblings('table'):
        headrow = i.thead.tr
        if headrow.th:
            s = headrow.th.decode_contents().lower()
            if 'field name' in s:
                # looks like a valid register table
                return i

    return None

def get_table_index(table, name):
    headrow = table.thead.tr
    r = []

    for field_el in headrow('th'):
        r.append(field_el.decode_contents().lower())

    if not name.lower() in r:
        return -1
    return r.index(name.lower())

def get_register_field_node(current_node, field_name):
    for register_header in current_node.find_next_siblings(header_name(header_node_level(current_node)+2)):
        name = register_header.decode_contents()

        if name == field_name:
            return register_header

    return None

def get_top_node(bs, current_node, name):
    for i in bs(header_name(header_node_level(current_node) +1)):
        if name in i.decode_contents().lower():
            return i

    return None

def get_nodes(current_node, level=1):
    return current_node.find_next_siblings(header_name(header_node_level(current_node)+level))

def get_node(current_node, name, level=1):
    for node in get_nodes(current_node, level):
        if node.decode_contents() == name:
            return node
    return None

def get_table(table_node):
    table = []
    for row in table_node.tbody('tr'):
        r = []
        for el in row('td'):
            r.append(el.decode_contents())
        table.append(r)

    return table

def get_table_node(current_node, name):
    for i in current_node.find_next_siblings('table'):
        headrow = i.thead.tr
        if headrow.th:
            s = headrow.th.decode_contents().lower()
            if name.lower() in s:
                return i
    
    return None

def import_md(regmap, path):
    with open(path, "rt") as fh:
        raw_text = fh.read()
        html_text = mt.markdown(raw_text)
        with open(path+'.html', 'wt') as fh2: fh2.write(html_text)
        bs = BeautifulSoup(html_text, "html.parser")

        block = {}

        most_headers_names = ['h'+str(i) for i in range(1, 10)]

        # get block name, first header
        name_header_node = bs(most_headers_names)[0]
        if name_header_node:
            block['name'] = name_header_node.decode_contents()
        else:
            raise RuntimeError("Cannot find block name header")

        # get block description
        # first header with 'description' in it
        description_header_node = None
        for i in bs(header_name(header_node_level(name_header_node) +1)):
            if 'description' in i.decode_contents().lower():
                description_header_node = i
                break

        if not description_header_node:
            raise RuntimeError("Cannot find a description header")
            
        # description nodes
        # between description header and next header of another hierarchy
        block['description'] = get_description(description_header_node)

        # register table
        # find an header named register
        # with the same hierarchy than description
        register_header_node = None
        for i in bs(header_name(header_node_level(name_header_node) +1)):
            if 'register' in i.decode_contents().lower():
                register_header_node = i
                break

        if not register_header_node:
            raise RuntimeError("Cannot find a register header")

        # find the register table, should be just after this header
        register_table = None
        for i in register_header_node.find_next_siblings('table'):
            headrow = i.thead.tr
            if headrow.th:
                s = headrow.th.decode_contents().lower()
                if 'register name' in s:
                    # looks like a valid register table
                    register_table = i
                    break
        
        # parse registers
        for register_row in register_table.tbody('tr'):
            r = []
            for register_el in register_row('td'):
                r.append(register_el.decode_contents())

            reg = regmap.add_register(
                rmap.Register(
                name=r[0],
                offset=int(r[1], 0)
                )
            )

            try:
                default_index = get_table_index(register_table, 'Properties')
                if default_index != -1:
                    for prop in r[default_index].split():
                        key, value = prop.split('=')
                        if key == 'template':
                            reg.template = value

                default_index = get_table_index(register_table, 'Size')
                if default_index != -1:
                    # Too many IPs has wrong register width, hard-code it to 32 bits
                    reg.width = 32 # int(r[default_index], 0)

                default_index = get_table_index(register_table, 'Default')
                if default_index != -1:
                    reg.reset = int(r[default_index], 0)

                desc_index = get_table_index(register_table, 'Description')
                if desc_index == -1:
                    desc_index = get_table_index(register_table, 'Short description')

                if desc_index != -1:
                    if len(r) > desc_index:
                        reg.desc = r[desc_index]

                do_reset_index = get_table_index(register_table, 'Reset')
                if do_reset_index != -1:
                    reg.do_reset = int(r[do_reset_index])

            except:
                print ('Caught error while parsing register (name: %s)' % r[0])
                raise

        for register in regmap.get_registers():
            node = get_register_node(register_header_node, register.get_field_template())

            if node is not None and register.desc is None:
                description = get_description(node)
                register.desc = description

            if node is not None:
                fields_table = get_register_field_table(node)

                if fields_table is not None:
                    for field_row in fields_table.tbody('tr'):
                        r = []
                        for field_el in field_row('td'):
                            r.append(field_el.decode_contents())

                        register.add_regfield(
                            rmap.Regfield(
                                name=r[0],
                                width=int(r[2], 0),
                                bit=int(r[1], 0),
                                access=r[3],
                                reset=int(r[5], 0)
                            )
                        )

                        desc_index = get_table_index(fields_table, 'Description')
                        if desc_index == -1:
                            desc_index = get_table_index(fields_table, 'Short description')

                        if desc_index != -1:
                            register.get_regfield(r[0]).desc = r[desc_index]

            for field in register.get_fields():
                field_node = get_register_field_node(node, field.name)

                if field_node is not None and field.desc is None:
                    description = get_description(field_node)
                    field.desc = description

        commands_top_node = get_top_node(bs, name_header_node, 'commands')

        if commands_top_node is not None:
            for command_node in get_nodes(commands_top_node, 1):
                cmdmap = rmap.Cmdmap(name=command_node.decode_contents())
                regmap.add_cmdmap(cmdmap)

                table_node = get_table_node(command_node, 'Command Name')

                if table_node is not None:

                    size_index = get_table_index(table_node, 'Size')
                    id_index = get_table_index(table_node, 'Command field')
                    if id_index == -1:
                        id_index = get_table_index(table_node, 'id')

                    for row in get_table(table_node):

                        if size_index != -1:
                            size = int(row[size_index], 0)
                        else:
                            size = 32

                        cmdmap.add_cmd(
                            rmap.Cmd(
                                name=row[0],
                                code=row[id_index],
                                width=size
                            )
                        )

                        desc_index = get_table_index(get_table_node(command_node, 'Command Name'), 'Description')
                        if desc_index == -1:
                            desc_index = get_table_index(get_table_node(command_node, 'Command Name'), 'Short description')

                        if desc_index != -1 and len(row) >= desc_index + 1:
                            cmdmap.get_cmd(row[0]).desc = row[desc_index]

                    for cmd in cmdmap.get_cmds():
                        node = get_node(command_node, cmd.name)

                        if node is not None:
                            if cmd.desc is None:
                                cmd.desc = get_description(node)

                            table = get_table_node(node, 'Field Name')

                        if node is not None and table is not None:

                            for row in get_table(table):
                                cmd.add_cmdfield(
                                    rmap.Cmdfield(
                                        name=row[0],
                                        width=int(row[2], 0),
                                        offset=int(row[1], 0),
                                        value=row[3]
                                    )
                                )

                                desc_index = get_table_index(get_table_node(node, 'Field Name'), 'Description')
                                if desc_index == -1:
                                    desc_index = get_table_index(get_table_node(node, 'Field Name'), 'Short description')

                                if desc_index != -1 and len(row) >= desc_index + 1:
                                    cmd.get_cmdfield(row[0]).desc = row[desc_index]

                                if cmd.get_cmdfield(row[0]).desc is None:
                                    desc_node = get_node(node, row[0], 2)
                                    if desc_node is not None:
                                        cmd.get_cmdfield(row[0]).desc = get_description(desc_node)

                        else:
                            cmd.add_cmdfield(
                                rmap.Cmdfield(
                                    value=cmd.code,
                                    width=cmd.width,
                                    name=cmd.name,
                                    offset=0
                                )
                            )