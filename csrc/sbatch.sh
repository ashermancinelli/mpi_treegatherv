#! /usr/bin/env bash
#SBATCH -A ops
#SBATCH -t 10:00
#SBATCH -n 24
#SBATCH -p phi
#SBATCH -o ouputs/%j.txt
#SBATCH -e errors/%j.txt

method="itree"
data_per_node=384

cmds=(                          \
    'module purge'              \
    'module load gcc/7.3.0'     \
    'module load openmpi/3.1.0' \
    'which mpirun'              \
    'which mpicc'               \
    'make opt'                  \
    'ldd treegather.bin'        \
)

# printing out commands ran to build and run the program
for cmd in "${cmds[@]}"
do
    echo 'bash> ' $cmd
    echo
    eval $cmd
done

touch start.time

data_per_node=384
n_trials=1

mpirun -np 24 ./treegather.bin      \
    --gather-method $method         \
    --num-loops 1                   \
    --display-time                  \
    --data-per-node $data_per_node  \
    --keep-percentage 100

touch end.time
echo "Ending"

echo
time=$(ls --full-time *.time)
echo
