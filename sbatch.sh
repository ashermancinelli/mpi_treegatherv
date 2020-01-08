#! /usr/bin/env bash
#SBATCH -A ops
#SBATCH -t 1:00:00
#SBATCH -n 24
#SBATCH -p phi

echo "Starting:"
date

echo 'Purging modules...'
module purge

echo 'Loading gcc 7.3.0'
module load gcc/7.3.0

#echo 'Loading intelmpi/5.0.1.035'
#module load intelmpi/5.0.1.035
echo 'Loading OMPI 3.1.0'
module load openmpi/3.1.0

echo 'Building opt'
make opt

n_loops=300
data_per_node=2048
n_trials=10

printf "\n\tNumber of loops: $n_loops"
printf "\tData per node: $data_per_node"
printf "\tNumber of trials: $n_trials\n"

for method in itree tree mpi persistent
do
    echo
    echo "***$method***"
    echo

    for i in `seq 1 $n_trials`
    do
        mpirun -np 24 ./treegather.bin      \
            --gather-method $method         \
            --num-loops $n_loops            \
            --display-time                  \
            --data-per-node $data_per_node
    done
done

echo "Ending:"
date

