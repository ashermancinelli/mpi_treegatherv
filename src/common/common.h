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

#define DEFAULT_PERSISTENT false
#define DEFAULT_DISPLAY_BUF true
#define DEFAULT_NUM_LOOPS 5
#define DEFAULT_DATA_PER_NODE 1024
#define DEFAULT_DATA_PER_PROC 1024
#define DEFAULT_METHOD MPI
#define DEFAULT_OUTFILE stdout

#endif
