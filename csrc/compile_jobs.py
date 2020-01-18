#!/usr/bin/env python

import hashlib
import os
from datetime import datetime
import re

combinations = [
    [2,	    1,	30],
    [2,	    2,	30],
	[4,	    1,	60],
	[4,	    2,	60],
	[4,	    4,	60],
	[8,	    1,	80],
	[8,	    2,	80],
	[8,	    4,	80],
]
'''
	[12,	1,	80],
	[12,	2,	80],
	[12,	4,	80],
	[16,	1,	80],
	[16,	2,	80],
	[16,	4,	80],
	[24,	1,	100],
	[24,	2,	100],
	[24,	4,	100],
	[24,	6,	100],
	[36,	2,	120],
	[36,	4,	120],
	[48,	2,	120],
	[48,	4,	120],
	[48,	6,	120],
	[72,	4,	140],
	[72,	6,	140],
	[96,	4,	140],
	[96,	6,	140],
	[144,	6,	150],
]
'''

mpi_versions = [
    'openmpi/3.1.0',
    'intelmpi/5.1.3.181',
    'mvapich2/2.3.2',
]

methods = [
    'mpi',
    'tree',
    'itree',
]

class RunScript:

    def __init__(self, template):
        self.template = template
        self.script = template

    def reset(self):
        self.script = self.template

    def sub(self, sub_dict):
        for k, v in sub_dict.items():
            self.script = re.sub(r'\{\{\s?' + k + r'\s?\}\}', str(v), self.script)
            print(f'Subbing {k} for {v}')

    def __str__(self):
        return self.script

def main():
    script = RunScript(open('sbatch_template', 'r').read())
    for mpi_mod in mpi_versions:
        for n, N, t in combinations:
            for mem_exp in range(19, 23):
                for method in methods:
                    script.reset()

                    _hash = hashlib.md5(str(datetime.now()).encode()).hexdigest()

                    # Convert from minutes to slurm format
                    time = f'{t // 60}:{t % 60}:00'

                    '''
                    TODO set the time value based on some heuristic or 
                    guess at how long it will take
                    '''
                    script.sub({
                        'hash':		    _hash,
                        'time':         time,
                        'nodes':		N,
                        'procs':		n,
                        'method':	    method,
                        'mpi':		    mpi_mod,
                        'data':		    '2 ^ %i' % mem_exp,
                    })
                    open('sbatch-%s.sh' % _hash, 'w+').write(str(script))
                    os.system('sbatch sbatch-%s.sh' % _hash)

if __name__ == '__main__':
    main()
