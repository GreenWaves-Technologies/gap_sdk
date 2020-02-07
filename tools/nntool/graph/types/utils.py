# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
