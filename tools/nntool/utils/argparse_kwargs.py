# Copyright (C) 2021  GreenWaves Technologies, SAS

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

from argparse import Action, ArgumentError
from distutils.util import strtobool
from typing import List

from cmd2 import CompletionError
from interpreter.nntool_shell_base import NODE_SELECTOR_HELP

# valid_keys is a dictionary with keys
# completer - function which takes one argument which is the partial or complete value
# choices - string choices for values
# type - function that takes string value and returns value
# if the dictionary contains a key * then all keys without an entry will match this
# or a function returning a dictionary of this form

class kwargs_append_action(Action):
    """
    argparse action to split an argument into KEY=VALUE form where VALUE is an int
    and append to a dictionary.
    """

    def __init__(self, option_strings,
                 *args,
                 kwargs_valid_keys=None,
                 kwargs_first_arg_completer=None,
                 kwargs_first_arg_mapper=None,
                 **kwargs):
        self._valid_keys = kwargs_valid_keys
        self._first_arg_mapper = kwargs_first_arg_mapper
        super(kwargs_append_action, self).__init__(option_strings=option_strings,
                                                   *args,
                                                   **kwargs)

    def __call__(self, parser, args, values, option_string=None):
        def mapper(valid_keys):
            def mapperfn(x):
                vals = x.split('=')
                if valid_keys:
                    if vals[0] not in valid_keys:
                        if '*' in valid_keys:
                            type_arg = valid_keys['*'].get('type')
                        else:
                            raise ArgumentError(
                                self, f'{vals[0]} is not a valid key')
                    else:
                        type_arg = valid_keys[vals[0]].get('type')
                    if type_arg:
                        try:
                            if type_arg == bool and isinstance(vals[1], str):
                                vals[1] = strtobool(vals[1])
                            vals[1] = type_arg(vals[1])
                        except ValueError as ex:
                            raise ArgumentError(
                                self, f'{vals[0]} does not have a valid value') from ex
                return vals
            return mapperfn

        if self._first_arg_mapper:
            first_arg = self._first_arg_mapper(args.shell, values[0])
            if not first_arg:
                raise ArgumentError(self, f"{values[0]} is not valid")
            values = values[1:]
            if callable(self._valid_keys):
                this_valid_keys = self._valid_keys(first_arg)
            else:
                this_valid_keys = self._valid_keys[first_arg]
        else:
            this_valid_keys = self._valid_keys

        try:
            vals = dict(map(mapper(this_valid_keys), values))
        except ValueError as ex:
            raise ArgumentError(
                self, f'Could not parse argument "{values}" as k1=v1 k2=v2 ... format') from ex
        attr = getattr(args, self.dest)
        if self._first_arg_mapper:
            if attr is None:
                attr = []
                setattr(args, self.dest, attr)
            attr.append((first_arg, vals))
        else:
            if attr is None:    
                attr = {}
                setattr(args, self.dest, attr)
            attr.update(vals)

    @classmethod
    def get_completer_method(cls, valid_keys, kwargs_first_arg_completer=None, kwargs_first_arg_mapper=None):
        def completer(parser, text: str, line: str, begidx: int, endidx: int, *args, complete_blank: bool = False) -> List[str]:
            if kwargs_first_arg_completer:
                tokens = line[:begidx].strip().split(' ')
                if tokens[-1].startswith('--'):
                   return kwargs_first_arg_completer(parser, text, line, begidx, endidx)
                start_idx = next((idx for idx, tok in reversed(list(enumerate(tokens))) if tok.startswith('--')), None)
                if start_idx is None or start_idx + 1 >= len(tokens):
                    return []
                first_arg = tokens[start_idx + 1]
                mapped_first_arg = kwargs_first_arg_mapper(parser, first_arg)
                if not mapped_first_arg:
                    raise CompletionError(f"{first_arg} is not valid")
                if callable(valid_keys):
                    this_valid_keys = valid_keys(mapped_first_arg)
                else:
                    this_valid_keys = {}
                    for elem in mapped_first_arg:
                        this_valid_keys.update(valid_keys[elem])
            else:
                this_valid_keys = valid_keys

            parts = text.split('=')
            if len(parts) == 1:
                parser.allow_appended_space=False
                choices = []
                parts_upper = parts[0].upper()
                for k in this_valid_keys:
                    if k == '*':
                        continue
                    kupper = k.upper()
                    if kupper == parts_upper:
                        choices.append(f'{k}=')
                    elif kupper.startswith(parts_upper):
                        choices.append(k)
                return choices
            elif len(parts) == 2:
                if parts[0] not in this_valid_keys:
                    if '*' in this_valid_keys:
                        params = this_valid_keys['*']
                    else:
                        parser.allow_appended_space=False
                        return []
                else:
                    params = this_valid_keys[parts[0]]
                if 'completer' in params:
                    choices = params['completer'](parts[1])
                else:
                    choices = params.get('choices')
                    if choices is None:
                        if 'type' in params and params['type'] == bool:
                            choices = ['true', 'false']
                        else:
                            parser.allow_appended_space=bool(parts[1])
                            return [text]
                if parts[1]:
                    choices = [choice for choice in choices if str(choice).upper().startswith(parts[1].upper())]
                return [f'{parts[0]}={choice}' for choice in choices]
            return []
        return completer
