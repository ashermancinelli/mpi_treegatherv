#! /usr/bin/env python3

import sys

if __name__ == '__main__':
    assert len(sys.argv) > 1, 'Must pass filename as first arg.'

    fn = sys.argv[1]
    print('Filename: ', fn)

    lines = []
    with open(fn, 'r') as f:
        lines = f.readlines()

    mpi = []
    tree = []

    found = False
    for l in lines:
        if ' MPI ' in l:
            found = True

        if 'sys' in l:
            l = l.replace('sys', '').strip().replace('0m', '').replace('s', '')
            if found:
                mpi.append(l)
            else:
                tree.append(l)

    mpi = list(map(float, mpi))
    tree = list(map(float, tree))

    avg_mpi = sum(mpi) / len(mpi)
    avg_tree = sum(tree) / len(tree)
    print('Results (average in seconds): \n\tTree: %.4f\n\tMPI: %.4f\nover %d trials'
            % (avg_tree, avg_mpi, (len(mpi) + len(tree))/2))
