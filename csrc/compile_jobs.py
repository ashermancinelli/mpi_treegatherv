#!/usr/bin/env python

import hashlib
import os
from datetime import datetime
import re
import pickle as P
import os.path
import sys

combinations = [
        [2,	    1,	30],
        [2,	    2,	30],
        [4,	    1,	60],
        [4,	    2,	60],
        # [4,	    4,	60],
        # [8,	    1,	80],
        # [8,	    2,	80],
        # [8,	    4,	250],
        # [12,	1,	250],
        # [12,	2,	250],
        # [12,	4,	250],
        # [16,	1,	250],
        # [16,	2,	80],
        # [16,	4,	80],
        # [24,	1,	100],
        # [24,	2,	100],
        # [24,	4,	100],
        # [24,	6,	100],
        # [36,	2,	120],
        # [36,	4,	120],
        # [48,	2,	120],
        # [48,	4,	120],
        # [48,	6,	120],
        # [72,	4,	140],
        # [72,	6,	140],
        # [96,	4,	140],
        # [96,	6,	140],
        # [144,	6,	150],
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
        assert isinstance(sub_dict, dict), 'Can only sub with a dictionary.'
        for k, v in sub_dict.items():
            self.script = re.sub(r'\{\{\s?' + k + r'\s?\}\}', str(v), self.script)

    def __str__(self):
        return self.script

class SlurmTime:
    def __init__(self, start=None):
        self.n = int()
        if start is not None:
            if isinstance(start, str):
                self.n = self.slurm_time_to_n(start)
            elif isinstance(start, (int, float)):
                self.n = int(start)

    def n_to_slurm_time(self, t):
        assert isinstance(t, (int, float)), 'Input must be numbery'
        return f'{t // 60}'.zfill(2) + ':' + f'{t % 60}'.zfill(2) + ':00'

    def slurm_time_to_n(self, t):
        assert isinstance(t, str), 'Slurm time must be str'
        time = t.split(':')
        return (int(time[0]) * 60) + int(time[1])

    def __add__(self, other):
        if isinstance(other, str):
            return SlurmTime(self.n + self.slurm_time_to_n(start))
        elif isinstance(other, (int, float)):
            return SlurmTime(self.n + other)

    def __str__(self):
        return self.n_to_slurm_time(self.n)

def rerun_cancelled_jobs():
    '''
    if input('Are there any jobs still running? If so, problems may occur. (y/n) > ') == 'y':
        print('Exiting. Please wait for jobs to continue running.')
        return
    '''
    fn = 'hashdict.pkl'
    hashdict = dict()
    if os.path.isfile(fn):
        hashdict = P.load(open(fn, 'rb'))

    script = RunScript(open('sbatch_template', 'r').read())
    for h in hashdict.keys():
        if not 'status' in hashdict[h].keys():
            print('Please run status.py before this program.')
            return
        if hashdict[h]['status']['error']:

            script.reset()

            # remove leftovers from previous runs
            os.system('[ -f outputs/slurm.%s.err ] && rm outputs/slurm.%s.err' % (h, h))
            os.system('[ -f outputs/slurm.%s.out ] && rm outputs/slurm.%s.out' % (h, h))
            os.system('[ -f time/start.%s.time ] && rm time/start.%s.time' % (h, h))
            os.system('[ -f time/end.%s.time ] && rm time/end.%s.time' % (h, h))
            os.system('[ -f scripts/sbatch-%s.sh ] && rm scripts/sbatch-%s.sh' % (h, h))

            if 'due to time limit'.upper() in hashdict[h]['status']['message']:
                print('Rerunning hash %s with time limit increased by 30 minutes.' % h)

                time = SlurmTime(hashdict[h]['time'])

                time += 30

                hashdict[h]['time'] = time

            else:
                continue
                print('Some error encountered that did not have to do with time limit.'
                        ' Simply rerunning this job.')

            script.sub(dict(hash=h, **hashdict[h]))
            del hashdict[h]['status']
            # open('scripts/sbatch-%s.sh' % h, 'w+').write(str(script))
            # os.system('sbatch scripts/sbatch-%s.sh' % h)
            print('sbatch scripts/sbatch-%s.sh' % h)
            print(str(script)); exit()

    # P.dump(hashdict, open(fn, 'wb'))

def run_new_jobs():
    fn = 'hashdict.pkl'

    hashdict = dict()
    if os.path.isfile(fn):
        hashdict = P.load(open(fn, 'rb'))

    script = RunScript(open('sbatch_template', 'r').read())

    i = 0
    for mpi_mod in mpi_versions:
        for n, N, t in combinations:
            for mem_exp in range(19, 23):
                for method in methods:
                    script.reset()

                    _hash = hashlib.md5(str(datetime.now()).encode()).hexdigest()

                    # Convert from minutes to slurm format
                    time = SlurmTime(t)

                    '''
                    TODO set the time value based on some heuristic or 
                    guess at how long it will take
                    '''

                    config = {
                            'time':         str(time),
                            'nodes':		N,
                            'procs':		n,
                            'method':	    method,
                            'mpi':		    mpi_mod,
                            'data':		    '2 ^ %i' % mem_exp,
                            }

                    hashdict[_hash] = config

                    i += 1
                    script.sub(dict(hash=_hash, **config))

                    open('scripts/sbatch-%s.sh' % _hash, 'w').write(str(script))
                    os.system('sbatch scripts/sbatch-%s.sh' % _hash)

    P.dump(hashdict, open(fn, 'wb'))
    print('Total jobs submitted: %i' % i)

if __name__ == '__main__':
    args = sys.argv

    if '-h' in args or '--help' in args or len(args) == 1:
        print('Usage:')
        print('python3 compile_jobs.py [rerun|main]')
        print('\trerun: find all jobs that were stopped for time limit and rerun.')
        print('\tmain: run all specified combinations of new jobs.')
        exit(1)

    if 'rerun' in args:
        rerun_cancelled_jobs()

    if 'main' in args:
        run_new_jobs()
