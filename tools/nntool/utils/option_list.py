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

from utils.json_serializable import JsonSerializable

OPTION_TYPES = {
    'int': int,
    'str': str
}


class OptionList(JsonSerializable):
    def __init__(self, valid_options=None, init=None):
        if init:
            super(OptionList, self).__setattr__('_options', init['options'])
            super(OptionList, self).__setattr__('_valid_options', {
                k.upper(): OPTION_TYPES[v] for k, v in init['valid_options'].items()
            })
        else:
            super(OptionList, self).__setattr__('_valid_options', {})
            if valid_options is not None:
                self._valid_options.update({k.upper():v for k, v in valid_options.items()})
            super(OptionList, self).__setattr__('_options', {})

    def __setattr__(self, name, value):
        # if _option or _valid_options are not present then we are being deepcopied
        if not hasattr(self, '_options') or not hasattr(self, '_valid_options'):
            super(OptionList, self).__setattr__(name, value)
        upper_name = name.upper()
        if upper_name in self._valid_options:
            if value is None:
                if upper_name in self._options:
                    del self._options[upper_name]
                return
            elif not isinstance(value, self._valid_options[upper_name]):
                value = self._valid_options[upper_name](value)
            self._options[upper_name] = value
        else:
            super(OptionList, self).__setattr__(name, value)

    def __getattr__(self, name):
        upper_name = name.upper()
        valid_options = super(OptionList, self).__getattribute__('_valid_options')
        options = super(OptionList, self).__getattribute__('_options')
        if upper_name in valid_options:
            return options.get(upper_name)
        else:
            raise AttributeError()

    def __len__(self):
        return len(self._options)

    def extend(self, *others, name_filter=None, overwrite=False):
        for other in others:
            if other is None:
                continue
            self.update_valid_options(other)
            for name in other.set_options:
                if name_filter is not None and not name_filter(name):
                    continue
                mine = self._options.get(name)
                if mine is None or overwrite:
                    setattr(self, name, getattr(other, name))
                elif getattr(self, name) != getattr(other, name):
                    raise ValueError("incompatibility between the two option sets")

    @property
    def set_options(self):
        return self._options.keys()

    @property
    def valid_options(self):
        return self._valid_options

    def update_valid_options(self, other):
        if isinstance(other, OptionList):
            self._valid_options.update(other.valid_options)
        elif isinstance(other, dict):
            self._valid_options.update({k.upper():v for k, v in other.items()})

    def _encapsulate(self):
        return {
            'options': self._options,
            'valid_options': {k: getattr(v, '__name__') for k, v in self._valid_options.items()}
        }

    @classmethod
    def _dencapsulate(cls, val):
        return cls(init=val)

    def __str__(self):
        return ",".join(["{}={}".format(k, v) for k, v in self._options.items()])
