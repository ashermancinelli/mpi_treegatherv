
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
    float *buffer = calloc(100, sizeof(int));
    float *sub_buffer = calloc(10, sizeof(int));
    char gather_method[32];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
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

                fprintf(stdout, "Using gather method: %s\n", gather_method);
            }
        }
    }

    
    if (size != 10)
    {
        if (rank == 0)
            EXIT();
    }
    
    if (rank == 0)
    {
        fprintf(stdout, "Running\n");
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}
