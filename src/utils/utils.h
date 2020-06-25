
#ifndef _UTILS_H_
#define _UTILS_H_

#include "common.h"

#define GREEN       "\033[0;32m"
#define RED         "\033[1;31m"
#define BLUE        "\033[1;34m"
#define RESET       "\033[0m"

extern const char* usage;

#ifdef RELEASE
#define EXIT(rank) MPI_Finalize(); exit(0);
#else
#define EXIT(rank) \
    if (rank==0) \
    { \
      fprintf(stderr, "%s%s%s", RED, usage, RESET); \
      fflush(stderr); \
    } \
    MPI_Finalize(); \
    exit(0);
#endif

enum gather_method
{
  MPI,
  MYMPI,
  TREE,
  ITREE,
};

struct options
{
  bool persistent;
  bool display_buf;
  int num_loops;
  int data_per_node;
  int data_per_proc;
  int rank;
  int size;
  enum gather_method method;
  FILE* outfile;
};

extern void parse_args(int * argc, char *** argv, struct options * opts);
extern void options_print(struct options* opts);
extern void set_default_opts(struct options * opts);
extern int cmp(const void * a, const void * b);
extern void sort(void* base, size_t n, size_t size);
extern void incr(double *ar, int len);
extern char* method_to_str(enum gather_method m);
extern size_t node_data_count(int rank, int size, int* cnts, int iteration);

#endif
