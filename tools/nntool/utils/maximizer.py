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

from functools import lru_cache
import math

class Maximizer():
    def __init__(self, func, var_min, var_max, func_change=None, int_step=False):
        self._func = func
        self._func_change = func_change
        self._var_max = var_max
        self._var_min = var_min
        self._args = tuple()
        self._int_step = int_step

    @lru_cache(maxsize=128, typed=False)
    def func(self, var):
        return self._func(var, *self._args)

    def change_dir(self, cur, step, cur_dir):
        step /= 2
        if self._int_step:
            step = int(min(math.floor(step + 0.5), 1))
        if cur_dir=='down':
            cur = min(cur + step, self._var_max)
            cur_dir = 'up'
        else:
            cur = max(cur - step, self._var_min)
            cur_dir = 'down'
        if self._func_change:
            self._func_change(cur, step, cur_dir, *self._args)

        return cur, step, cur_dir

    def step(self, cur, step, cur_dir):
        if cur_dir =='down':
            cur = max(cur - step, self._var_min)
        else:
            cur = min(cur + step, self._var_max)
        return cur, step

    def run(self, var_start, *args, stop_margin=None, dir_start='down', progress=None, start_step=None):
        #pylint: disable=no-member
        if stop_margin is None:
            stop_margin = 1 if self._int_step else 0.5
        self.func.cache_clear()
        if start_step:
            var_step = start_step
        else:
            if dir_start == "down":
                var_step = (var_start - self._var_min) / 2 if start_step is None else start_step
            else:
                var_step = (self._var_max - var_start) / 2 if start_step is None else start_step
            if self._int_step:
                var_step = int(max(math.floor(var_step + 0.5), 1))

        var_cur = int(var_start) if self._int_step else var_start
        cur_dir = dir_start
        best = tuple()
        self._args = args
        while True:
            if progress:
                progress(var_cur, var_step, cur_dir)
            # if self._int_step:
            #     var_cur = int(min(max(math.floor(var_cur + 0.5), self._var_min), self._var_max))

            res = self.func(var_cur)
            if res is None:
                if not best and var_cur == self._var_max:
                    return None
                if self._int_step and best and best[1] == var_cur + 1:
                    return best
                if cur_dir == "down":
                    var_cur, var_step, cur_dir = self.change_dir(var_cur, var_step, cur_dir)
                else:
                    var_cur, var_step = self.step(var_cur, var_step, cur_dir)
            else:
                if not best or res > best[0]:
                    best = (res, var_cur)
                if self._int_step:
                    if (stop_margin > 1 and var_step <= stop_margin) or var_cur == self._var_min:
                        return best
                    if cur_dir == 'up' and var_step == 1:
                        return best
                else:
                    if var_step <= stop_margin:
                        return best
                if cur_dir == "down":
                    var_cur, var_step = self.step(var_cur, var_step, cur_dir)
                else:
                    var_cur, var_step, cur_dir = self.change_dir(var_cur, var_step, cur_dir)
