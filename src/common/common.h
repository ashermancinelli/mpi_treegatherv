#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <assert.h>

#include "mpi.h"

#define GREEN       "\033[0;32m"
#define RED         "\033[1;31m"
#define BLUE        "\033[1;34m"
#define RESET       "\033[0m"

#define DEFAULT_PERSISTENT false
#define DEFAULT_DISPLAY_BUF false
#define DEFAULT_NUM_LOOPS 5
#define DEFAULT_DATA_PER_NODE 256
#define DEFAULT_DATA_PER_PROC 256
#define DEFAULT_METHOD MPI
#define DEFAULT_OUTFILE stdout

#define UNUSED(x) (void)(x)
#define MAX_MPI_BITS    10
#define MAX_MPI_RANKS   70

#define TESTCASE(code, name)\
  printf("%s-- TEST %-25s in %-20s: %15s%s\n", code?GREEN:RED, name, __FILE__, code?"PASS":"FAIL", RESET);

#endif
