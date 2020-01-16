#!/usr/bin/env python

import hashlib
import os
from datetime import datetime

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

def main():
    template = open('sbatch_template', 'r').read()
    for mem_exp in range(19, 23):
        for n, N in combinations:
            for mpi_mod in mpi_versions:
                for method in methods:
                    _hash = hashlib.md5(str(datetime.now()).encode()).hexdigest()
                    runscript = template.replace('###HASH', _hash)
                    runscript = runscript.replace('###NODES', str(N))
                    runscript = runscript.replace('###PROCS', str(n))
                    runscript = runscript.replace('###METHOD', str(method))
                    runscript = runscript.replace('###MPI', str(mpi_mod))
                    runscript = runscript.replace('###DATA', '2 ^ %i' % mem_exp)
                    open('sbatch-%s.sh' % _hash, 'w+').write(runscript)
                    return

if __name__ == '__main__':
    main()
