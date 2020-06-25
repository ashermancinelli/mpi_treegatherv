#! /usr/bin/env bash
#SBATCH -A ops
#SBATCH -t 30:00
#SBATCH -N 1
#SBATCH -n 2
#SBATCH -p dl
#SBATCH -o %j.txt
#SBATCH -e %j.txt

method="itree"
data_per_node=2048
num_procs=2
num_nodes=1
num_loops=1000
mpi_version='openmpi/3.1.3'
gcc_version='gcc/7.3.0'
# installd=$(pwd)/install;
installd=/people/manc568/projects/mpi_treegatherv/../builds/treegather/gcc7.3.0_openmpi3.1.3/
hash=$(date | md5sum | awk '{print$1}')

quit()
{
  echo
  echo Exiting after an error...
  echo
  exit 1
}

usage()
{
  cat <<EOD
  Usage:
    -nn       num nodes
    -np       num procs
    -nl       num loops
    -mpi      mpi module to be loaded
EOD
}

while [ $# -gt 0 ]
do
  case $1 in
    -nn)
      num_nodes=$2
      shift; shift
      ;;
    -np)
      num_procs=$2
      shift; shift
      ;;
    -nl)
      num_loops=$2
      shift; shift
      ;;
    -mpi)
      mpi_version=$2
      shift; shift
      ;;
    -showme)
      echo method         :=		$method
      echo data_per_node  :=		$data_per_node
      echo num_procs      :=		$num_procs
      echo num_nodes      :=		$num_nodes
      echo num_loops      :=		$num_loops
      echo mpi_version    :=		$mpi_version
      echo installd       :=		$installd
      exit 0
      ;;
    *)
      echo
      echo Offending option: $1
      echo
      usage
      exit 1
      ;;
  esac
done

set -x
test -f "start.$hash.time" && rm "start.$hash.time"
test -f "end.$hash.time" && rm "end.$hash.time"
module purge || quit
module load $gcc_version || quit
module load $mpi_version || quit
which mpirun || quit
which mpicc || quit
ldd $installd/bin/treegather.bin || quit

touch start.time
mpirun -np $num_procs $installd/bin/treegather.bin \
  --gather-method $method \
  --num-loops $num_loops \
  --data-per-node $data_per_node || quit
touch end.time

echo
time=$(ls --full-time start.time end.time \
  | cut -f7 -d' ' \
  | tr '\n' ' ' \
  | sed 's/$/\n/' \
  | $installd/bin/bench.bin)
set +x
printf "MPI $mpi_version\nDATA/PROC $data_per_node\nMETHOD $method\nNUM PROCS $num_procs\nNUM NODES $num_nodes\nNUM LOOPS\n$num_loops_sn\nTIME $time\n"
echo
# rm *.time
