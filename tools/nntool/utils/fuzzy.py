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

class Fuzzy():
    def __init__(self, val):
        self._val = val

    @classmethod
    def maybe_yes(cls, maybe=False):
        if maybe:
            return cls.maybe
        return cls.yes

    @classmethod
    def yes(cls):
        return cls("yes")

    @classmethod
    def no(cls):
        return cls("no")

    @classmethod
    def maybe(cls):
        return cls("maybe")

    @property
    def is_yes(self):
        return self._val == "yes"

    @property
    def is_maybe(self):
        return self._val == "maybe"

    @property
    def is_no(self):
        return self._val == "no"

    @classmethod
    def all(cls, gen):
        is_maybe = False
        for elem in gen:
            if elem.is_no:
                return cls.no()
            if elem.is_maybe:
                is_maybe = True
        return cls.maybe_yes(maybe=is_maybe)

    def __bool__(self):
        return self.is_maybe or self.is_yes
