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
    itree = []

    found = False
    for l in lines:
        if ' mpi.' in l:
            n = mpi
        elif ' tree.' in l:
            n = tree
        elif ' itree.' in l:
            n = itree

        if 'sys' in l:
            l = l.replace('sys', '').strip().replace('0m', '').replace('s', '')
            n.append(l)

    mpi = list(map(float, mpi))
    tree = list(map(float, tree))
    itree = list(map(float, itree))

    avg_mpi = sum(mpi) / len(mpi)
    avg_tree = sum(tree) / len(tree)
    avg_itree = sum(itree) / len(itree)
    print('Results (average in seconds): \n\tTree: %.4f\n\tITree: %.4f\n\tMPI: %.4f\nover %d trials'
            % (avg_tree, avg_mpi, avg_itree, (len(mpi) + len(tree) + len(itree))/3))
