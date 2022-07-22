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

class HandlerOptions():
    HANDLER_OPTIONS = []

    @staticmethod
    def handler_option(name, val_type=str, default="", desc="description missing", shortcut=None):
        def deco(cls):
            # check if this class actually defines HANDLER_OPTIONS
            if 'HANDLER_OPTIONS' not in cls.__dict__:
                value = []
                setattr(cls, 'HANDLER_OPTIONS', value)
            else:
                value = getattr(cls, 'HANDLER_OPTIONS')
            value.append({'name': name, 'val_type': val_type, 'default': default,
                          'desc': desc, 'shortcut': shortcut})
            return cls

        return deco

    @classmethod
    def get_all_handler_options(cls):
        options = {}
        for subclass in cls.__subclasses__():
            for option in getattr(subclass, 'HANDLER_OPTIONS'):
                options[option['name']] = option
        return options

    @classmethod
    def get_default_handler_options(cls):
        options = {}
        for subclass in cls.__subclasses__():
            for option in getattr(subclass, 'HANDLER_OPTIONS'):
                options[option['name']] = option['default']
        return options


#pylint: disable=invalid-name
handler_option = HandlerOptions.handler_option
