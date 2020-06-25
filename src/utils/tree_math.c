
#include "utils.h"

int cmp(const void * a, const void * b)
{
  if (*(double*)a > *(double*)b) return 1;
  else if (*(double*)a < *(double*)b) return -1;
  else return 0;  
}

void sort(void* base, size_t n, size_t size)
{
  qsort(base, n, size, cmp);
}

void incr(double *ar, int len)
{
  int i;
  for (i=0; i<len; i++) ar[i]++;
}

void set_default_opts(struct options * opts)
{
  opts->persistent = DEFAULT_PERSISTENT;
  opts->display_buf = DEFAULT_DISPLAY_BUF;
  opts->num_loops = DEFAULT_NUM_LOOPS;
  opts->data_per_node = DEFAULT_DATA_PER_NODE;
  opts->data_per_proc = DEFAULT_DATA_PER_PROC;
  opts->method = DEFAULT_METHOD;
  opts->outfile = DEFAULT_OUTFILE;
}

char* method_to_str(enum gather_method m)
{
  switch (m)
  {
    case MPI:
      return strdup("MPI");
    case MYMPI:
      return strdup("MYMPI");
    case TREE:
      return strdup("TREE");
    case ITREE:
      return strdup("ITREE");
  }
  return strdup("INVALID METHOD");
}

size_t node_data_count(
        int rank,
        int size,
        int* cnts,
        int iteration)
{
    size_t count = 0;
    int i, rightmost_child=0;

    // How many partners will this node have?
    for (i=iteration; i>0; i--)
    {
        rightmost_child <<= 1;
        rightmost_child++;
    }
    rightmost_child += rank;

    // Sum the counts of each child the node
    // recieves from
    for (i=rank; i<=rightmost_child && i<size; i++)
        count += cnts[i];

    return count;
}
