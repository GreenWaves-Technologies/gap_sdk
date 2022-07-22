#
# Copyright (C) 2019 GreenWaves Technologies
#


from prettytable import PrettyTable


class Regfield(object):

    def dump_to_table(self, table):
        if self.width == 1:
            bit = '%d' % self.bit
        else:
            bit = '%d:%d' % (self.bit + self.width - 1, self.bit)

        row = ['', '', '', ''] + [bit, self.access, self.name, self.desc]

        table.add_row(row)



class Register(object):

    def dump_to_table(self, table, dump_regfields=True):

        if self.get_offset() is not None:
            row = [self.get_full_name(), '0x%x' % self.get_offset(), self.width, self.desc]
            if dump_regfields:
                row += ['', '', '', '']

            table.add_row(row)

        if dump_regfields:
            for name, field in self.fields.items():
                field.dump_to_table(table)



class Regmap(object):

    def dump_to_table(self, dump_regfields=True):
        rows = ['Name', 'Offset', 'Width', 'Description']
        if dump_regfields:
            rows += ['Field bit #', 'Field R/W', 'Field name', 'Field description']

        table = PrettyTable(rows)
        table.align = 'l'

        for name, register in self.registers.items():
            register.dump_to_table(table, dump_regfields=dump_regfields)

        print (table)
