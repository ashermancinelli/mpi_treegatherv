#! /usr/bin/env bash
#SBATCH -A ops
#SBATCH -t 2:59:00
#SBATCH -N 2
#SBATCH -n 2
#SBATCH -p dl
#SBATCH -o %j.txt
#SBATCH -e %j.txt

# 2<<19 1048576
# 2<<20 2097152
# 2<<21 4194304
# 2<<22 8388608
# 2<<23 16777216

# my-mpi and itree have persistent implementations as well
method="itree" # mpi my-mpi tree itree
data_per_node=1048576
num_nodes=2
num_procs=2
num_loops=1000000
persistent=true
mpi_version='openmpi/3.1.3'
gcc_version='gcc/7.3.0'
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

tstart=start."$hash".time
tend=end."$hash".time
test -f $tstart && rm $tstart
test -f $tend && rm $tend

set -x
module purge || quit
module load $gcc_version || quit
module load $mpi_version || quit
which mpirun || quit
which mpicc || quit
ldd $installd/bin/treegather.bin || quit

declare -a args=(
  -np $num_procs
  $installd/bin/treegather.bin
  --gather-method $method
  --num-loops $num_loops
  --data-per-node $data_per_node)
test -z "$persistent" || args+=(--persist)
echo "${args[@]}"

touch $tstart
mpirun "${args[@]}" || quit
touch $tend

echo
time=$(ls --full-time $tstart $tend \
  | cut -f7 -d' ' \
  | tr '\n' ' ' \
  | sed 's/$/\n/' \
  | $installd/bin/bench.bin)
set +x
cat <<EOD
RESULTS MPI $mpi_version
RESULTS GCC $gcc_version
RESULTS DATA_PER_PROC $data_per_node
RESULTS METHOD $method
RESULTS NUM_PROCS $num_procs
RESULTS NUM_NODES $num_nodes
RESULTS NUM_LOOPS $num_loops
RESULTS HASH $hash
RESULTS TIME $time
EOD
echo
# rm *.time
