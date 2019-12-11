
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#include "tree_gather.h"

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
    float *global_buffer;
    float *local_buffer;
    char gather_method[32];
    int offsets[10];

    // sum == 100
    int cnts[10] =
    {
        10, 20, 10,
        3,  2,  5,
        10, 5,  5,
        30
    };

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size != 10)
        EXIT();
    
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
        fprintf(stdout, "Using %i procs.\n", size);
        fflush(stdout);

        offsets[0] = 0;
        for (i=1; i < 10; i++)
            offsets[i] = offsets[i-1] + cnts[i-1];
    }

    global_buffer = malloc(sizeof(float) * 100);
    local_buffer = malloc(sizeof(float) * cnts[rank]);
    
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
        tree_gatherv_f(
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

    if (!rank)
    {
        for (i=0; i<100; i++)
        {
            fprintf(stdout, "local_buffer[%i] = %.1f\n", i, global_buffer[i]);
        }
    }

    MPI_Finalize();
    return 0;
}
