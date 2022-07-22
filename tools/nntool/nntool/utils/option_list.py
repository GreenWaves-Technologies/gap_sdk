# Copyright (C) 2022  GreenWaves Technologies, SAS

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

from typing import Any
from nntool.utils.json_serializable import JsonSerializable

OPTION_TYPES = {
    'int': int,
    'str': str
}


class OptionList(dict, JsonSerializable):
    def __init__(self, *args, valid_options=None, **kwargs):
        if valid_options is None:
            valid_options = {}
        self._valid_options = {k.upper(): OPTION_TYPES[v]
                               if isinstance(v, str) else v
                               for k, v in valid_options.items()}
        super().__init__(*args, **kwargs)

    def __setattr__(self, name: str, value: Any) -> None:
        if name.startswith('_'):
            return super().__setattr__(name, value)
        if hasattr(self, f'_{name}'):  # subclasses can define new properties
            return super().__setattr__(f'_{name}', value)
        name = name.upper()
        if name not in self._valid_options:
            return super().__setattr__(name, value)
        if value is None:
            if name in self:
                del self[name]
        else:
            self[name] = value

    def __getattr__(self, name: str) -> Any:
        if name.startswith('_'):
            raise AttributeError()  # @IgnoreException
        name = name.upper()
        if name not in self._valid_options:
            raise AttributeError()
        return self.get(name, None)

    @property
    def valid_options(self):
        return self._valid_options

    def update_valid_options(self, other):
        if isinstance(other, OptionList):
            self._valid_options.update(other.valid_options)
        elif isinstance(other, dict):
            self._valid_options.update(
                {k.upper(): v for k, v in other.items()})

    def extend(self, *others, name_filter=None, overwrite=False):
        """Extends this optionlist with others.
        Valid options are merged and conflicts in settings are detected.

        Args:
            name_filter (callable, optional): Don't merge if it returns False. Defaults to None.
            overwrite (bool, optional): Overwrite all options. Defaults to False.

        Raises:
            ValueError: Option sets have conflicting values
        """
        for other in others:
            if other is None:
                continue
            self.update_valid_options(other)
            for name in other.keys():
                if name_filter is not None and not name_filter(name):
                    continue
                mine = self.get(name)
                if mine is None or overwrite:
                    self[name] = other[name]
                elif self[name] != other[name]:
                    raise ValueError(
                        "incompatibility between the two option sets")

    def _encapsulate(self):
        return {
            'options': dict(self),
            'valid_options': self._valid_options
        }

    @classmethod
    def _dencapsulate(cls, val):
        return cls(val['options'], valid_options=val['valid_options'])

    def __str__(self):
        return ",".join(["{}={}".format(k, v) for k, v in self.items()])
