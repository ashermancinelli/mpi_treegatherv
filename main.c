
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#include "tree_gather.h"

#define COUNT 384

#define EXIT() \
    ({ \
        fprintf(stderr, usage); \
        MPI_Finalize(); \
        return 0; \
     })
        

char* usage = "Options:\n"
    "\t--gather-method (mpi|tree)\n";

int main(int argc, char** argv)
{
    int rank, size, i;
    double *global_buffer;
    double *local_buffer;
    char gather_method[32];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *offsets = malloc(sizeof(int) * size);
    int *cnts = malloc(sizeof(int) * size);
    for (i=0; i<size; i++)
        cnts[i] = COUNT;
    
    for (i=0; i<argc; i++)
    {
        if (i == 0)
            continue;

        if (strcmp(argv[i], "--gather-method") == 0)
        {
            if (i == argc+1) 
                EXIT();

            strcpy(gather_method, argv[++i]);

            if ((strcmp(gather_method, "mpi") != 0) &&
                    (strcmp(gather_method, "tree") != 0))
                EXIT();
        }
    }

    if (!rank)
    {
        fprintf(stdout, "Using gather method: %s.\n", gather_method);
        fflush(stdout);
    }

    offsets[0] = 0;
    for (i=1; i < size; i++)
        offsets[i] = offsets[i-1] + cnts[i-1];

    global_buffer = malloc(sizeof(double) * size * COUNT);
    local_buffer = malloc(sizeof(double) * COUNT);
    
    for (i=0; i<cnts[rank]; i++)
        local_buffer[i] = rank;

    if (strcmp(gather_method, "mpi") == 0)
    {
        MPI_Gatherv(
            local_buffer,
            cnts[rank],
            MPI_FLOAT,
            global_buffer,
            cnts,
            offsets,
            MPI_FLOAT,
            0,
            MPI_COMM_WORLD);
    }
    else
    {
        tree_gatherv_d(
            local_buffer,
            cnts[rank],
            MPI_FLOAT,
            global_buffer,
            cnts,
            offsets,
            MPI_FLOAT,
            0,
            MPI_COMM_WORLD);
    }

#   ifdef __DEBUG
    if (!rank)
    {
        for (i=0; i<size*COUNT; i++)
        {
            fprintf(stdout, "global_buffer[%i] = %.1f\n", i, global_buffer[i]);
        }
    }
#   endif

    MPI_Finalize();
    return 0;
}
