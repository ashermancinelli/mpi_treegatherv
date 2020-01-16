#! /usr/bin/env python

from datetime import datetime
import sys
import glob

def main():
    filenames = glob.glob('slurm.*.out')

    for fn in filenames:
        print(f'On filename "{fn}"')
        f = open(fn, 'r')
        line = f.readlines()[-1]
        if 'FINAL TIME' in line:
            continue
        else:
            time = line.split('(')[-1].split(')')[0]
            t1, t2 = time.split(';')
            t1 = datetime.strptime(t1[:-3], '%H:%M:%S.%f')
            t2 = datetime.strptime(t2[:-3], '%H:%M:%S.%f')
            final_time = abs(t1 - t2)
            with open(fn, 'a') as wf:
                wf.writelines([f'FINAL TIME: {final_time}\n'])

        f.close()

if __name__ == '__main__':
    main()
