#include "utils.h"

const char* usage  = "Usage:\n"
  "\t--gather-method      (mpi|my-mpi|tree|itree)\n"
  "\t--output-file        <filename or blank for stdout>\n"
  "\t--display-buf        Display buffers before/after run\n"
  "\t--data-per-node      <int>\n"
  "\t--num-loops          <int>\n"
  "\t--persistent         Use persistent communication";

void options_print(struct options* opts)
{
  if (opts->rank == 0)
  {
    printf("Options:\n\tpersist: %s\n\tdisplay_buf: %s\n\t"
        "num_loops: %d\n\t"
        "data_per_node: %d\n\t"
        "data_per_proc: %d\n\t"
        "size: %d\n\t"
        "gather_method: %s\n",
        opts->persistent?"true":"false",
        opts->display_buf?"true":"false",
        opts->num_loops,
        opts->data_per_node,
        opts->data_per_proc,
        opts->size,
        method_to_str(opts->method));
  }
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
      EXIT(rank);
    }
    else if (strcmp(argv[i], "--persist") == 0)
    {
      opts->persistent = true;
    }
    else if (strcmp(argv[i], "--output-file") == 0)
    {
      if (i == argc+1) 
      {
        EXIT(rank);
      }
      opts->outfile = fopen(argv[++i], "w");
    }
    else if (strcmp(argv[i], "--gather-method") == 0)
    {
      if (i == argc+1) 
      {
        EXIT(rank);
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
        EXIT(rank);
      }
    }
    else if (strcmp(argv[i], "--num-loops") == 0)
    {
      if (i == argc+1) 
      {
        EXIT(rank);
      }

      opts->num_loops = atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "--data-per-node") == 0)
    {
      if (i == argc+1) 
      {
        EXIT(rank);
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
        EXIT(rank);
      }
    }
  }

  assert(!(opts->method == MPI && opts->persistent)
      && "Invalid configuration. No persistent version of MPI algorithm available.");
  assert(!(opts->method == TREE && opts->persistent)
      && "Invalid configuration. No persistent version of TREE algorithm available.");
}
