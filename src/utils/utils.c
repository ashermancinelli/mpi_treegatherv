
#include "utils.h"

#define EXIT() \
    if (rank==0) \
    { \
      fprintf(stderr, "%s%s%s", RED, usage, RESET); \
      fflush(stderr); \
    } \
    MPI_Finalize(); \
    exit(1);

char* usage = "Usage:\n"
  "\t--gather-method      (mpi|my-mpi|my-mpi-persist|tree|itree|itree-persist)\n"
  "\t--output-file        <filename or blank for stdout>\n"
  "\t--display-buf        (for sanity checking)\n"
  "\t--data-per-node      <int>\n"
  "\t--num-loops          <int>\n"
  "\t--persistent         (use persistent communication)";

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

void parse_args(int * _argc, char *** _argv, struct options * opts)
{
  set_default_opts(opts);
  if (*_argc == 1) return;
  int argc = *_argc;
  char ** argv = *_argv;
  int rank=opts->rank;

  int i;
  for (i=1; i<argc; i++)
  {
    if (strcmp(argv[i], "--help")==0 || strcmp(argv[i], "-h")==0)
    {
      EXIT();
    }
    else if (strcmp(argv[i], "--persistent") == 0)
    {
      opts->persistent = true;
    }
    else if (strcmp(argv[i], "--output-file") == 0)
    {
      if (i == argc+1) 
      {
        EXIT();
      }
      opts->outfile = fopen(argv[++i], "w");
    }
    else if (strcmp(argv[i], "--gather-method") == 0)
    {
      if (i == argc+1) 
      {
        EXIT();
      }

      char * gather_method = strdup(argv[++i]);

      if (strcmp(gather_method, "mpi") == 0)
        opts->method = MPI;
      else if (strcmp(gather_method, "my-mpi") == 0)
        opts->method = MYMPI;
      else if (strcmp(gather_method, "tree") == 0)
        opts->method = TREE;
      else if (strcmp(gather_method, "itree") == 0)
        opts->method = ITREE;
      else
      {
        EXIT();
      }
    }
    else if (strcmp(argv[i], "--num-loops") == 0)
    {
      if (i == argc+1) 
      {
        EXIT();
      }

      opts->num_loops = atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "--data-per-node") == 0)
    {
      if (i == argc+1) 
      {
        EXIT();
      }

      opts->data_per_node = atoi(argv[++i]);
    }
    else if (strstr(argv[i], "--display-buf") != NULL)
    {
      opts->display_buf = true;
    }
    else
    {
      if (!opts->rank)
        fprintf(opts->outfile, "%scommand '%s' not found.%s\n", RED, argv[i], RESET);
      {
        EXIT();
      }
    }
  }
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
