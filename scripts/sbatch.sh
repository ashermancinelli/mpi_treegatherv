#! /usr/bin/env bash
#SBATCH -A ops
#SBATCH -t 10:00
#SBATCH -N 1
#SBATCH -n 2
#SBATCH -p newell
#SBATCH -o %j.txt
#SBATCH -e %j.txt

method="itree"
data_per_node=384
num_procs=2
num_nodes=2
num_loops_sn="1 * (10 ^ 3)"
num_loops=$( echo "$num_loops_sn" | bc)
mpi_version='openmpi/3.1.5'
installd=$(pwd)/install;

set -x
test -f start.time && rm start.time
test -f end.time && rm end.time
module purge
module load gcc/7.4.0
module load $mpi_version
which mpirun
which mpicc
make -j 4 install BUILD_TYPE=release PREFIX=$installd
ldd $installd/treegather.bin

touch start.time
mpirun -np $num_procs $installd/bin/treegather.bin \
    --gather-method $method \
    --num-loops $num_loops \
    --data-per-node $data_per_node
touch end.time

echo
time=$(ls --full-time start.time end.time \
    | cut -f7 -d' ' \
    | tr '\n' ' ' \
    | sed 's/$/\n/' \
    | $installd/bin/bench.bin)
printf "MPI $mpi_version\nDATA/PROC $data_per_node\nMETHOD $method\nNUM PROCS $num_procs\nNUM NODES $num_nodes\nNUM LOOPS\n$num_loops_sn\nTIME $time\n"
echo
rm *.time
