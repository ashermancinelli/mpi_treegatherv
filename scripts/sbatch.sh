#! /usr/bin/env bash
#SBATCH -A ops
#SBATCH -t 10:00
#SBATCH -N 2
#SBATCH -n 2
#SBATCH -p phi
#SBATCH -o %j.txt
#SBATCH -e %j.txt

method="itree"
data_per_node=384
num_procs=2
num_nodes=2
num_loops_sn="3 * (10 ^ 8)"
# num_loops_sn="10"
num_loops=$( echo "$num_loops_sn" | bc)
mpi_version='openmpi/3.1.0'

cmds=(                          				\
    "test -f start.time && rm start.time"   	\
    "test -f end.time && rm end.time"       	\
    "module purge"              				\
    "module load gcc/7.3.0"     				\
    "module load $mpi_version"  				\
    "which mpirun"              				\
    "which mpicc"               				\
    "make opt"                  				\
    "ldd treegather.bin"        				\
)

# printing out commands ran to build and run the program
for cmd in "${cmds[@]}"
do
    echo 'bash> ' $cmd
    echo
    eval $cmd
done

touch start.time
mpirun -np $num_procs ./treegather.bin \
    --gather-method $method \
    --num-loops $num_loops \
    --data-per-node $data_per_node
touch end.time

echo
time=$(ls --full-time start.time end.time \
    | cut -f7 -d' ' \
    | tr '\n' ';' \
    | sed 's/;//2')
printf "MPI($mpi_version) DATA/PROC($data_per_node) METHOD($method) NUM PROCS($num_procs) NUM NODES($num_nodes) NUM LOOPS($num_loops_sn)\nTIME($time)\n"
echo
