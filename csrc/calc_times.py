#! /usr/bin/env python

from datetime import datetime
import sys
import glob

def main():
    filenames = glob.glob('*.txt')

    for fn in filenames:
        with open(filename, 'r') as f:
            lines = f.readlines()

    d2 = datetime.strptime('11:10:03.691067241'[:-3], '%H:%M:%S.%f')

if __name__ == '__main__':
    main()
