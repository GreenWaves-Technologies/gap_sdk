"""A lightweight python templating engine.  Templet version 3.3

Lightweight templating idiom using @stringfunction and @unicodefunction.

Each template function is marked with the attribute @stringfunction
or @unicodefunction.  Template functions will be rewritten to expand
their document string as a template and return the string result.
For example:

    from templet import stringfunction

    @stringfunction
    def myTemplate(animal, body):
      "the $animal jumped over the $body."

    print myTemplate('cow', 'moon')

The template language understands the following forms:

    $myvar - inserts the value of the variable 'myvar'
    ${...} - evaluates the expression and inserts the result
    ${[...]} - evaluates the list comprehension and inserts all the results
    ${{...}} - executes enclosed code; use 'out.append(text)' to insert text

In addition the following special codes are recognized:
    $$ - an escape for a single $
    $ (at the end of the line) - a line continuation
    $( $. - translates directly to $( and $. so jquery does not need escaping
    $/ $' $" - also passed through so the end of a regex does not need escaping

Template functions are compiled into code that accumulates a list of
strings in a local variable 'out', and then returns the concatenation
of them.  If you want do do complicated computation, you can append
to the 'out' variable directly inside a ${{...}} block, for example:

    @stringfunction
    def myrow(name, values):
      '''
      <tr><td>$name</td><td>${{
         for val in values:
           out.append(string(val))
      }}</td></tr>
      '''

Generated code is arranged so that error line numbers are reported as
accurately as possible.

Templet is by David Bau and was inspired by Tomer Filiba's Templite class.
For details, see http://davidbau.com/templet

Templet is posted by David Bau under BSD-license terms.

Copyright (c) 2012, David Bau
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    3. Neither the name of Templet nor the names of its contributors may
       be used to endorse or promote products derived from this software
       without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""

# pylint: disable=invalid-name, unused-argument


import sys
import re
import inspect

class _TemplateBuilder(object):
    __pattern = re.compile(r"""\$         # Directives begin with a $
        (?![.(/'"])(                    # $. $( $/ $' $" do not require escape
        \$                            | # $$ is an escape for $
        [^\S\n]*\n                    | # $\n is a line continuation
        [_a-z][_a-z0-9]*              | # $simple Python identifier
        \{(?![\[{])[^\}]*\}            | # ${...} expression to eval
        \{\[.*?\]\}                   | # ${[...]} list comprehension to eval
        \{\{.*?\}\}                   | # ${{...}} multiline code to exec
      )((?<=\}\})[^\S\n]*\n|)           # eat trailing newline after }}
    """, (re.IGNORECASE | re.VERBOSE | re.DOTALL))

    def __init__(self, *args):
        self.defn, self.start, self.constpat, self.emitpat, self.listpat, self.finish = args
        self.code = self.extralines = self.simple = None

    def __realign(self, s, spaces=''):
        """Removes any leading empty columns of spaces and an initial empty line"""
        lines = s.splitlines()
        if lines and not lines[0].strip():
            del lines[0]
        lspace = [len(l) - len(l.lstrip()) for l in lines if l.lstrip()]
        margin = len(lspace) and min(lspace)
        return '\n'.join((spaces + l[margin:]) for l in lines)

    def __addcode(self, line, lineno, simple):
        offset = lineno - self.extralines - len(self.code)
        if offset <= 0 and simple and self.simple and self.code:
            self.code[-1] += ';' + line
        else:
            self.code.append('\n' * (offset - 1) + line)
            self.extralines += max(0, offset - 1)
        self.extralines += line.count('\n')
        self.simple = simple

    def build(self, template, filename, lineno, docline):
        self.code = ['\n' * (lineno - 1) + self.defn, ' ' + self.start]
        self.extralines, self.simple = max(0, lineno - 1), True
        lineno += docline + (re.match(r'\s*\n', template) and 1 or 0)
        for i, part in enumerate(self.__pattern.split(self.__realign(template))):
            if i % 3 == 0 and part:
                self.__addcode(' ' + self.constpat % repr(part), lineno, True)
            elif i % 3 == 1:
                if not part:
                    raise SyntaxError('Unescaped $ in %s:%d' % (filename, lineno))
                elif part == '$':
                    self.__addcode(' ' + self.constpat % '"%s"' % part, lineno, True)
                elif part.startswith('{{'):
                    self.__addcode(self.__realign(part[2:-2], ' '),
                                   lineno + (re.match(r'\{\{\s*\n', part) and 1 or 0), False)
                elif part.startswith('{['):
                    self.__addcode(' ' + self.listpat % part[2:-2], lineno, True)
                elif part.startswith('{'):
                    self.__addcode(' ' + self.emitpat % part[1:-1], lineno, True)
                elif not part.endswith('\n'):
                    self.__addcode(' ' + self.emitpat % part, lineno, True)
            lineno += part.count('\n')
        self.code.append(' ' + self.finish)
        return '\n'.join(self.code)

def _templatefunction(func, listname, stringtype):
    globals_, locals_ = sys.modules[func.__module__].__dict__, {}
    filename, lineno = func.__code__.co_filename, func.__code__.co_firstlineno
    if func.__doc__ is None:
        raise SyntaxError('No template string at %s:%d' % (filename, lineno))
    try: # scan source code to find the docstring line number (2 if not found)
        docline, (source, _) = 2, inspect.getsourcelines(func)
        for lno, line in enumerate(source):
            if re.match('(?:|[^#]*:)\\s*[ru]?[\'"]', line):
                docline = lno
                break
# pylint: disable=broad-except
    except Exception:
        docline = 2
    sig = inspect.signature(func)
    builder = _TemplateBuilder(
        'def %s%s:' % (func.__name__, str(sig)),
        '%s = []' % listname,
        '%s.append(%%s)' % listname,
        '%s.append(%s(%%s))' % (listname, stringtype),
        '%s.extend(map(%s, [%%s]))' % (listname, stringtype),
        'return "".join(%s)' % listname)
    code_str = builder.build(func.__doc__, filename, lineno, docline)
    code = compile(code_str, filename, 'exec')
# pylint: disable=exec-used
    exec(code, globals_, locals_)
    return locals_[func.__name__]

def stringfunction(func):
    """Function attribute for string template functions"""
    return _templatefunction(func, listname='out', stringtype='str')


##############################################################################
# When executed as a script, run some testing code.
if __name__ == '__main__':
    ok = True
    def expect(actual, expected):
        # pylint: disable=global-statement
        global ok
        if expected != actual:
            print("error - expect: %s, got:\n%s" % (repr(expected), repr(actual)))
            ok = False

    @stringfunction
    def testBasic(name):
        "Hello $name."

    expect(testBasic('Henry'), "Hello Henry.")

    @stringfunction
    def testReps(a, count=5): r"""
        ${{ if count == 0: return '' }}
        $a${testReps(a, count - 1)}"""

    expect(
        testReps('foo'),
        "foofoofoofoofoo")

    @stringfunction
    def testList(a): r"""
        ${[testBasic(x) for x in a]}"""

    expect(
        testList(['David', 'Kevin']),
        "Hello David.Hello Kevin.")

    @stringfunction
    def testmyrow(name, values):
        '''
        <tr><td>$name</td><td>${{
        for val in values:
            out.append(str(val))
        }}</td></tr>
        '''

    expect(
        testmyrow('prices', [1, 2, 3]),
        "<tr><td>prices</td><td>123</td></tr>\n")

    try:
        GOT_EXCEPTION = ''
        def dummy_for_line():
            pass
        @stringfunction
        def testsyntaxerror():
            # extra line here
            # another extra line here
            '''
            some text
            $a$<'''
    except SyntaxError as ex:
        GOT_EXCEPTION = str(ex).split(':')[-1]

    expect(GOT_EXCEPTION, str(dummy_for_line.__code__.co_firstlineno + 8))

    try:
        GOT_LINE = 0
        def dummy_for_line2():
            pass
        @stringfunction
        def testruntimeerror(a):
            '''
            some $a text
            ${{
                out.append(a) # just using up more lines
            }}
            some more text
            $b text $a again'''
        expect(testruntimeerror.__code__.co_firstlineno,
               dummy_for_line2.__code__.co_firstlineno + 2)
        testruntimeerror('hello')
    except NameError as ex:
        import traceback
        _, GOT_LINE, _, _ = traceback.extract_tb(sys.exc_info()[2], 10)[-1]

    expect(GOT_LINE, dummy_for_line2.__code__.co_firstlineno + 10)

# pylint: disable=exec-used
    exec("""if True:
        @stringfunction
        def testnosource(a):
            "${[c for c in reversed(a)]} is '$a' backwards." """)
# pylint: disable=undefined-variable
    expect(testnosource("hello"), "olleh is 'hello' backwards.")

    ERROR_LINE = None
    try:
        exec("""if True:
        @stringfunction
        def testnosource_error(a):
            "${[c for c in reversed a]} is '$a' backwards." """)
    except SyntaxError as ex:
        ERROR_LINE = re.search('line [0-9]*', str(ex)).group(0)

    expect(ERROR_LINE, 'line 4')

    if ok:
        print("OK")
    else:
        print("FAIL")
