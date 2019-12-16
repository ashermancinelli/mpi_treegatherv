#! /usr/bin/env bash

infile=$1

if [ -f $infile ]
then
    echo Found $infile
else
    echo Could not find $infile.
    exit 1
fi

avg=$(grep -i average $infile)

mpi=$(echo $avg | grep mpi | awk -F' ' 'BEGIN {
    count=0;
    avg=0;
}

{
    count++;
    avg += $3;
}

END {
    printf avg/count;
}')

tree=$(echo $avg | grep tree | awk -F' ' 'BEGIN {
    count=0;
    avg=0;
}

{
    count++;
    avg += $3;
}

END {
    printf avg/count;
}')

itree=$(echo $avg | grep itree | awk -F' ' 'BEGIN {
    count=0;
    avg=0;
}

{
    count++;
    avg += $3;
}

END {
    printf avg/count;
}')

echo itree $itree
echo mpi $mpi
echo tree $tree
