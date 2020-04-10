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

def summarize_stats(stats):
    summary = {'mean': 0, 'std': 0, 'amin': float("Infinity"),\
        'amax':float('-Infinity'), 'lmin': float("-Infinity"),\
        'lmax':float('Infinity'), 'min': 0, 'max': 0,\
        'range': float("-Infinity")}
    for v in stats:
        for k in ['mean', 'std', 'min', 'max']:
            summary[k] += v[k]
        summary['amin'] = min(summary['amin'], v['min'])
        summary['amax'] = max(summary['amax'], v['max'])
        summary['lmin'] = max(summary['lmin'], v['min'])
        summary['lmax'] = min(summary['lmax'], v['max'])
        summary['range'] = max(summary['range'], v['range'])
    for k in ['mean', 'std', 'min', 'max']:
        summary[k] /= len(stats)
    for v in stats:
        v['precision'] = v['range']/summary['range']
    return summary
