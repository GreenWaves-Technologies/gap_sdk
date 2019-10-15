# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

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
