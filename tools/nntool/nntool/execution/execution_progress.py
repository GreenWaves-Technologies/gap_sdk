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


class ExecutionProgress(object):
    __instance = None
    def __new__(cls):
        if ExecutionProgress.__instance is None:
            ExecutionProgress.__instance = object.__new__(cls)
        return ExecutionProgress.__instance

    def __init__(self):
        if not hasattr(self, 'listeners'):
            self.listeners = []

    @classmethod
    def progress(cls, step_idx, name):
        inst = cls()
        for func in inst.listeners:
            func(step_idx, name)

    @classmethod
    def start(cls):
        inst = cls()
        for func in inst.listeners:
            func(None, "start")

    @classmethod
    def end(cls):
        inst = cls()
        for func in inst.listeners:
            func(None, "end")

    @classmethod
    def listen(cls, func):
        inst = cls()
        inst.listeners.append(func)

    @classmethod
    def unlisten(cls, func):
        inst = cls()
        inst.listeners.remove(func)
