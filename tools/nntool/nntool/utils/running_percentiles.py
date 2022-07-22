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

import numpy as np

from nntool.utils.json_serializable import JsonSerializable

class RunningPercentile:
    def __init__(self, percentile=0.5, step=0.1):
        self.percentile = percentile
        self.step = step
        self.step_up = 1.0 - percentile
        self.step_down = percentile
        self.x = None

    def push(self, observation):
        observation = np.atleast_1d(observation)
        if self.x is None:
            self.x = observation.item(0)

        if self.percentile  == 0:
            self.x = np.min(np.minimum(self.x, observation))
        elif self.percentile == 1:
            self.x = np.max(np.maximum(self.x, observation))
        else:

            for sample in observation.flatten():
                if self.x > sample:
                    self.x -= self.step * self.step_up
                elif self.x < sample:
                    self.x += self.step * self.step_down
                if abs(sample - self.x) < self.step:
                    self.step /= 2.0

class RunningPercentiles(JsonSerializable):
    def __init__(self, percentiles):
        self._percentiles = {x: RunningPercentile(x, step=1 if x==0 or x==1 else 0.1) for x in percentiles}

    def _encapsulate(self):
        return [
            {
                'percentile': elem.percentile,
                'x': elem.x,
            }
        for elem in self._percentiles.values()]

    @classmethod
    def _dencapsulate(cls, val):
        percentiles = [elem['percentile'] for elem in val]
        new_rp = cls(percentiles)
        for elem in val:
            getattr(new_rp, '_percentiles')[elem['percentile']].x = elem['x']
        return new_rp

    def push(self, observation):
        for elem in self._percentiles.values():
            elem.push(observation)

    def __getitem__(self, idx):
        return list(self._percentiles.values())[idx].x

    @property
    def percentiles(self):
        return [elem.x for elem in self._percentiles.values()]
