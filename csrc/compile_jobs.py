#!/usr/bin/env python

import hashlib
import os
from datetime import datetime
import re

combinations = [
    [2, 1],
    [2, 2],
	[4, 1],
	[4, 2],
	[4, 4],
	[8, 1],
	[8, 2],
	[8, 4],
	[12, 1],
	[12, 2],
	[12, 4],
	[16, 1],
	[16, 2],
	[16, 4],
	[24, 1],
	[24, 2],
	[24, 4],
	[24, 6],
	[36, 2],
	[36, 4],
	[48, 2],
	[48, 4],
	[48, 6],
	[72, 4],
	[72, 6],
	[96, 4],
	[96, 6],
	[144, 6],
]

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
    for mem_exp in range(19, 23):
        for n, N in combinations:
            for mpi_mod in mpi_versions:
                for method in methods:
                    script.reset()

                    _hash = hashlib.md5(str(datetime.now()).encode()).hexdigest()

                    '''
                    TODO set the time value based on some heuristic or 
                    guess at how long it will take
                    '''
                    script.sub({
                        'hash':		    _hash,
                        'time':		    '20:00',
                        'nodes':		N,
                        'procs':		n,
                        'method':	    method,
                        'mpi':		    mpi_mod,
                        'data':		    '2 ^ %i' % mem_exp,
                    })
                    open('sbatch-%s.sh' % _hash, 'w+').write(str(script))
                    os.system('sbatch sbatch-%s.sh' % _hash)
                    return

if __name__ == '__main__':
    main()
