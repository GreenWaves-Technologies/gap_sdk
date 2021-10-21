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

import gsystree as st
from tools.runner.runner import Runner_target
import runner.chips.gap9_v2 as gap9_v2
import json_tools as js
import os.path


class Gap9_runner_target(Runner_target):

    def __init__(self, parent, name, *kargs, **kwargs):
        super(Gap9_runner_target, self).__init__(parent, name, *kargs, **kwargs)

        self.add_properties({
            "runner": {
                "modules": {
                    "gvsoc": "gv.chips.gap9_v2"
                },
                "efuses": {
                    "enabled": True,
                    "content": {
                    }
                }
            }
        })

    def __get_efuse_path(self, work_dir=None):
        path = self.get_path().replace('/', '.') + '.' + 'efuse_preload.data'
        if work_dir is not None:
            return os.path.join(work_dir, path)
        else:
            return path

    def gen_stimuli(self, work_dir):
        runner = gap9_v2.Runner(None, js.import_config(self.get_config()), None)

        runner.gen_efuse_stim(self.__get_efuse_path(work_dir))

    def get_path(self, is_efuse=False, *kargs, **kwargs):
        if is_efuse:
            return self.__get_efuse_path()
        else:
            return Runner_target.get_path(self, *kargs, **kwargs)
