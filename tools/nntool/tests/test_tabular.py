import io
from utils.tabular import Fmt, TabularColumn, Tabular,\
    CSVRenderer, TextTableRenderer, ExcelRenderer, TabularColumnGroup

def test_format():
    fmt1 = Fmt("s")
    assert str(fmt1) == "s"
    fmt2 = Fmt("0.7f")
    assert str(fmt2) == "0.7f"
    fmt3 = Fmt(">010.7f")
    assert str(fmt3) == ">010.7f"

def test_csv():
    tab = Tabular()
    tab.add_row([TabularColumn('test1'), TabularColumn('test2'), TabularColumn('test3', fmt="^d")])
    tab.add_row(["1", "2", 3])
    output = io.StringIO()
    tab.render(CSVRenderer(output))
    output = output.getvalue()
    assert output == 'test1,test2,test3\r\n1,2,3\r\n'

def test_textable():
    tab = Tabular()
    tab.add_row([TabularColumn('test1'), TabularColumn('test2'), TabularColumn('test3', fmt="^d")])
    tab.add_row(["1", "2", 3])
    renderer = TextTableRenderer(maxwidth=80)
    tab.render(renderer)
    output = renderer.get_output()
    assert output == ('+-------+-------+-------+\n'
                      '| test1 | test2 | test3 |\n'
                      '+=======+=======+=======+\n'
                      '| 1     | 2     |   3   |\n'
                      '+-------+-------+-------+')

def test_excel():
    tab = Tabular()
    tab.add_row([
        TabularColumn('test1 is very long'),
        TabularColumnGroup('group',
                           [TabularColumn('test2'),
                            TabularColumn('test3', fmt="^d")])
    ])
    tab.add_row(["1", "2", 3])
    renderer = ExcelRenderer("/tmp/test.xls")
    tab.render(renderer)
