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

from abc import ABC, abstractmethod

import csv
from texttable import Texttable
import xlsxwriter as xl

ALLOWED_TYPES = set(list('nbox%fFegwWdDsSl') +\
    ['t' + c for c in 'ieahgcts'])

class Fmt:
# borrowed from https://github.com/r1chardj0n3s/parse

    def __init__(self, fmt, extra_types=None):
        '''Pull apart the format [[fill]align][0][width][.precision][type]
        '''
        if callable(fmt):
            self._fmt = fmt
            return

        self._fill = self._align = ""
        if fmt[0] in '<>=^':
            self._align = fmt[0]
            fmt = fmt[1:]
        elif len(fmt) > 1 and fmt[1] in '<>=^':
            self._fill = fmt[0]
            self._align = fmt[1]
            fmt = fmt[2:]

        self._zero = False
        if fmt and fmt[0] == '0':
            self._zero = True
            fmt = fmt[1:]

        self._width = ''
        while fmt:
            if not fmt[0].isdigit():
                break
            self._width += fmt[0]
            fmt = fmt[1:]

        self._precision = ''
        if fmt.startswith('.'):
            # Precision isn't needed but we need to capture it so that
            # the ValueError isn't raised.
            fmt = fmt[1:]  # drop the '.'
            while fmt:
                if not fmt[0].isdigit():
                    break
                self._precision += fmt[0]
                fmt = fmt[1:]

        # the rest is the type, if present
        if fmt and fmt not in ALLOWED_TYPES and\
            (extra_types is None or\
                fmt not in extra_types):
            raise ValueError('format spec %r not recognised' % fmt)
        self._fmt = fmt

    @property
    def fill(self):
        return self._fill

    @property
    def width(self):
        return self._width

    @property
    def zero(self):
        return self._zero

    @property
    def align(self):
        return self._align

    @property
    def fmt(self):
        return self._fmt

    def apply(self, elem):
        if callable(self._fmt):
            return self._fmt(elem)
        if elem is None:
            return ""
        return ("{:"+str(self)+"}").format(elem)

    def applyfn(self):
        def format_fn(elem):
            if str(elem) == "":
                return elem
            return self.apply(elem)
        return format_fn

    def __str__(self):
        if self._precision:
            return "{}{}{}{}.{}{}".format(\
                self._fill, self._align, self._zero and '0' or '',\
                self._width, self._precision, self._fmt)

        return "{}{}{}{}{}".format(\
            self._fill, self._align, self._zero and '0' or '',\
            self._width, self._fmt)

    def __eq__(self, other):
        return self.__dict__ == other.__dict__

class AbstractTabularColumn(ABC):

    def __init__(self, show=True):
        self._show = show

    @property
    @abstractmethod
    def is_container(self):
        pass

    @property
    @abstractmethod
    def contents(self):
        pass

    @property
    def show(self):
        return self._show

    @show.setter
    def show(self, val):
        self._show = val

    def __eq__(self, other):
        return self.__dict__ == other.__dict__

class TabularColumnGroup(AbstractTabularColumn):

    def __init__(self, name, columns=None, header_fmt="^", header_valign="c", show=True):
        super().__init__(show)
        self._name = name
        self._header_fmt = Fmt(header_fmt)
        self._header_valign = header_valign
        if columns is None:
            self._columns = []
        else:
            self._columns = columns

    @property
    def is_container(self):
        return True

    @property
    def contents(self):
        return self._columns

    @property
    def name(self):
        return self._name

    @property
    def header_fmt(self):
        return self._header_fmt

    @property
    def header_valign(self):
        return self._header_valign

class TabularColumn(AbstractTabularColumn):

    def __init__(self, name, fmt="<s", valign="c",
                 header_fmt="^", header_valign="c", show=True):
        super().__init__(show)
        self._name = name
        self._fmt = Fmt(fmt)
        self._valign = valign
        self._header_fmt = Fmt(header_fmt)
        self._header_valign = header_valign

    @property
    def is_container(self):
        return False

    @property
    def contents(self):
        return [self]

    @property
    def name(self):
        return self._name

    @property
    def fmt(self):
        return self._fmt

    @property
    def valign(self):
        return self._valign

    @property
    def header_fmt(self):
        return self._header_fmt

    @property
    def header_valign(self):
        return self._header_valign

class TabularRenderer(ABC):

    @abstractmethod
    def render(self, table):
        pass

class Tabular:

    def __init__(self):
        self._rows = []
        self._current_header = None
        self._current_header_len = 0

    def add_row(self, row):
        if row:
            if isinstance(row[0], AbstractTabularColumn):
                new_header_len = sum([len(col.contents) for col in row])
                assert self._current_header is None or self._current_header_len == new_header_len,\
                    "new header must have the same length"
                self._current_header = row
                self._current_header_len = new_header_len
            else:
                assert self._current_header is not None, "no header defined"
                assert len(row) == self._current_header_len, "length missmatch"
        else:
            assert self._current_header is not None, "no header defined"

        self._rows.append(row)

    @property
    def rows(self):
        return self._rows

    def render(self, renderer):
        renderer.render(self)

    def __eq__(self, other):
        return self.__dict__ == other.__dict__

class CSVRenderer(TabularRenderer):

    def __init__(self, output, **kwargs):
        self._output = output
        self._fmtparams = kwargs

    def render(self, table):
        writer = csv.writer(self._output, **self._fmtparams)
        current_header = None
        for row in table.rows:
            if row and isinstance(row[0], AbstractTabularColumn):
                current_header = row
                headers = []
                for header in current_header:
                    for header_elem in header.contents:
                        if header_elem.show:
                            headers.append(header_elem.header_fmt.apply(header_elem.name\
                                .replace('\n', ' ').replace('\r', '')))
                        else:
                            headers.append("")
                writer.writerow(headers)
                continue
            if not row:
                writer.writerow([""]*0)
                continue
            orow = []
            row_elem_idx = 0
            for header in current_header:
                for header_elem in header.contents:
                    orow.append(header.fmt.apply(row[row_elem_idx]))
                    row_elem_idx += 1
            writer.writerow(orow)

def get_textable_align(fmt):
    if fmt.align == "<":
        return 'l'
    if fmt.align == ">":
        return 'r'
    if fmt.align == "^":
        return 'c'
    return 'l'

def get_textable_dtype(fmt):
    return fmt.applyfn()

def get_textable_valign(valign):
    return valign

class TextTableRenderer(TabularRenderer):

    def __init__(self, maxwidth=80, chars=None):
        self._maxwidth = maxwidth
        self._chars = chars
        self._output = None

    def render(self, table):

        writer = Texttable(self._maxwidth)
        if self._chars:
            writer.set_chars(self._chars)

        current_header = None

        for row in table.rows:
            if current_header is None and row and\
                isinstance(row[0], AbstractTabularColumn):
                current_header = row
                headers = []
                align = []
                valign = []
                dtype = []
                for header in current_header:
                    for header_elem in header.contents:
                        if header_elem.show:
                            headers.append(header_elem.header_fmt.apply(header_elem.name))
                        else:
                            headers.append("")
                        align.append(get_textable_align(header_elem.fmt))
                        valign.append(get_textable_valign(header_elem.valign))
                        dtype.append(get_textable_dtype(header_elem.fmt))

                writer.header(headers)
                writer.set_cols_align(align)
                writer.set_cols_dtype(dtype)
                writer.set_cols_valign(valign)
                continue
            if not row:
                writer.add_row([""]*0)
                continue
            writer.add_row(row)

        self._output = writer.draw()

    def get_output(self):
        return self._output

def get_excel_align(fmt):
    if fmt.align == "<":
        return 'left'
    if fmt.align == ">":
        return 'right'
    if fmt.align == "^":
        return 'center'
    return 'left'

def get_excel_valign(valign):
    if valign == "t":
        return 'top'
    if valign == "b":
        return 'vbottom'
    if valign == "c":
        return 'vcenter'
    return 'vcenter'

def excel_format(work_book, fmt, valign, header=False):
    return work_book.add_format({
        'bold': header,
        'align': get_excel_align(fmt),
        'valign': get_excel_valign(valign)
    })

class ExcelRenderer(TabularRenderer):

    def __init__(self, filename):
        self._filename = filename

    def render(self, table):
        work_book = xl.Workbook(self._filename)
        work_sheet = work_book.add_worksheet()

        xls_row = 0
        widths = None
        for row in table.rows:
            row_nl = 0
            if row and isinstance(row[0], AbstractTabularColumn):
                if widths is None:
                    widths = [0] * sum([len(elem.contents) for elem in row])
                col_idx = 0
                has_groups = False
                for header in row:
                    if isinstance(header, TabularColumnGroup):
                        has_groups = True
                        work_sheet.merge_range(\
                            xls_row, col_idx, xls_row, col_idx + len(header.contents) - 1,\
                            header.name, excel_format(work_book,
                                                      header.header_fmt,
                                                      header.header_valign,
                                                      True))
                        col_idx += len(header.contents)
                        nls = str(header.name).count("\n")
                        if nls > row_nl:
                            row_nl = nls
                    else:
                        col_idx += 1

                if has_groups:
                    xls_row += 1

                col_idx = 0
                formats = []
                for header in row:
                    for header_elem in header.contents:
                        if header_elem.show:
                            if len(header_elem.name) > widths[col_idx]:
                                widths[col_idx] = len(header_elem.name)
                            work_sheet.write(xls_row,
                                             col_idx,
                                             header_elem.name,
                                             excel_format(work_book,
                                                          header_elem.header_fmt,
                                                          header_elem.header_valign,
                                                          True))
                            nls = str(header_elem.name).count("\n")
                            if nls > row_nl:
                                row_nl = nls
                        formats.append(excel_format(work_book,
                                                    header_elem.fmt,
                                                    header_elem.valign))
                        col_idx += 1
                if row_nl > 0:
                    work_sheet.set_row(xls_row, 15 * (row_nl + 1))
                xls_row += 1
                continue

            if not row:
                xls_row += 1
                continue

            for row_elem_idx, row_elem in enumerate(row):
                if len(str(row_elem)) > widths[row_elem_idx]:
                    widths[row_elem_idx] = len(str(row_elem))
                work_sheet.write(xls_row, row_elem_idx, row_elem, formats[row_elem_idx])
                nls = str(row_elem).count("\n")
                if nls > row_nl:
                    row_nl = nls

            if row_nl > 0:
                work_sheet.set_row(xls_row, 15 * (row_nl + 1))
            xls_row += 1

        for col, width in enumerate(widths):
            work_sheet.set_column(col, col, width)

        work_book.close()
