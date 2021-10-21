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
from typing import List

# valid_keys is a dictionary with keys
# completer - function which takes one argument which is the partial or complete value
# choices - string choices for values
# type - function that takes string value and returns value
# if the dictionary contains a key * then all keys without an entry will match this

class kwargs_append_action(Action):
    """
    argparse action to split an argument into KEY=VALUE form where VALUE is an int
    and append to a dictionary.
    """

    def __init__(self, option_strings,
                 *args,
                 kwargs_valid_keys=None,
                 **kwargs):
        self._valid_keys = kwargs_valid_keys
        super(kwargs_append_action, self).__init__(option_strings=option_strings,
                                                   *args,
                                                   **kwargs)

    def __call__(self, parser, args, values, option_string=None):
        def fn(x):
            print(x)
            vals = x.split('=')
            if self._valid_keys:
                if vals[0] not in self._valid_keys:
                    if '*' in self._valid_keys:
                        type_arg = self._valid_keys['*'].get('type')
                    else:
                        raise ArgumentError(
                            self, f'{vals[0]} is not a valid key')
                else:
                    type_arg = self._valid_keys[vals[0]].get('type')
                if type_arg:
                    try:
                        vals[1] = type_arg(vals[1])
                    except ValueError:
                        raise ArgumentError(
                            self, f'{vals[0]} does not have a valid value')
            return vals
        try:
            d = dict(map(fn, values))
        except ValueError as ex:
            raise ArgumentError(
                self, f'Could not parse argument "{values}" as k1=v1 k2=v2 ... format')
        attr = getattr(args, self.dest)
        if attr is None:
            attr = {}
            setattr(args, self.dest, attr)
        attr.update(d)

    @classmethod
    def get_completer_method(cls, valid_keys):
        def completer(parser, text: str, line: str, begidx: int, endidx: int, *, complete_blank: bool = False) -> List[str]:
            parts = text.split('=')
            if len(parts) == 1:
                parser.allow_appended_space=False
                choices = []
                parts_upper = parts[0].upper()
                for k in valid_keys:
                    if k == '*':
                        continue
                    kupper = k.upper()
                    if kupper == parts_upper:
                        choices.append(f'{k}=')
                    elif kupper.startswith(parts_upper):
                        choices.append(k)
                return choices
            elif len(parts) == 2:
                if parts[0] not in valid_keys:
                    if '*' in valid_keys:
                        params = valid_keys['*']
                    else:
                        parser.allow_appended_space=False
                        return []
                else:
                    params = valid_keys[parts[0]]
                if 'completer' in params:
                    choices = params['completer'](parts[1])
                else:
                    choices = params.get('choices')
                    if choices is None:
                        if 'type' in params and params['type'] == bool:
                            choices = ['True', 'False']
                        else:
                            parser.allow_appended_space=bool(parts[1])
                            return [text]
                if parts[1]:
                    choices = [choice for choice in choices if choice.upper().startswith(parts[1].upper())]
                return [f'{parts[0]}={choice}' for choice in choices]
            return []
        return completer
