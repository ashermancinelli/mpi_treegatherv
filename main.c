
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mpi.h"

#include "tree_gather.h"

#define EXIT() \
    ({ \
        fprintf(stderr, usage); \
        MPI_Finalize(); \
        return 0; \
     })
        

char* usage = "Options:\n"
    "\t--gather-method  (mpi|tree|itree)\n"
    "\t--num-loops      <int>\n";

int main(int argc, char** argv)
{
    int rank, size, i,
        num_loops = -1, data_per_node = -1;
    bool display_result = false;
    double *global_buffer;
    double *local_buffer;
    char gather_method[32];
    FILE* outfile;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *offsets = malloc(sizeof(int) * size);
    int *cnts = malloc(sizeof(int) * size);

    if (rank == 0)
        fprintf(stdout, "\n");
    
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
                    (strcmp(gather_method, "tree") != 0) &&
                    (strcmp(gather_method, "itree") != 0))
                EXIT();
        }
        else if (strcmp(argv[i], "--num-loops") == 0)
        {
            if (i == argc+1) 
                EXIT();

            num_loops = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--data-per-node") == 0)
        {
            if (i == argc+1) 
                EXIT();

            data_per_node = atoi(argv[++i]);
        }
        else if (strstr(argv[i], "--display-result") != NULL)
        {
            char *substr;
            if ((substr = strstr(argv[i], "=")) != NULL)
            {
                outfile = fopen(++substr, "w+");
            }
            else
                outfile = stdout;
            display_result = true;
        }
    }

    if (num_loops == -1)
        num_loops = 5;

    if (data_per_node == -1)
        data_per_node = 5;

    for (i=0; i<size; i++)
        cnts[i] = data_per_node;

    if (!rank)
    {
        fprintf(outfile, "Using gather method: %s.\n", gather_method);
        fprintf(outfile, "Looping %d times.\n", num_loops);
        fprintf(outfile, "Using %d data points per node.\n", data_per_node);
        fflush(outfile);
    }

    for (i=0; i<num_loops; i++)
    {
        offsets[0] = 0;
        for (i=1; i < size; i++)
            offsets[i] = offsets[i-1] + cnts[i-1];

        global_buffer = malloc(sizeof(double) * size * data_per_node);
        local_buffer = malloc(sizeof(double) * data_per_node);
        
        for (i=0; i<cnts[rank]; i++)
            local_buffer[i] = rank + 1;

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
        else if (strcmp(gather_method, "tree") == 0)
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
        else if (strcmp(gather_method, "itree") == 0)
        {
            tree_gatherv_d_async(
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

#       ifdef __DEBUG
            if (display_result)
            {
                if (rank == 0)
                {
                    fprintf(outfile, "Printing out final global buffer on first iteration.\n");
                    for (i=0; i<size*data_per_node; i++)
                    {
                        fprintf(outfile, "global_buffer[%i] = %.1f\n", i, global_buffer[i]);
                    }
                }
                fflush(outfile);
            }
#       endif

        free(local_buffer);     free(global_buffer);
    }

    free(offsets);      free(cnts);

    if (outfile != stdout)
        fclose(outfile);

    MPI_Finalize();
    return 0;
}
