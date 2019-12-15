
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "mpi.h"

#include "tree_gather.h"

#define EXIT() \
    ({ \
        if (!rank) \
        { \
            fprintf(stderr, usage); \
            fflush(stderr); \
        } \
        MPI_Finalize(); \
        return 0; \
     })

char* usage = "Options:\n"
    "\t--gather-method  (mpi|tree|itree)\n"
    "\t--display-result=<filename or blank for stdout>\n"
    "\t--data-per-node  <int>\n"
    "\t--num-loops      <int>\n";

int main(int argc, char** argv)
{
    int rank, size, i, j,
        num_loops = -1, data_per_node = -1;
    bool display_result = false;
    double *global_buffer;
    double *local_buffer;
    char gather_method[32];
    FILE* outfile;
    double start, end;
    double *elapsed_times;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *offsets = malloc(sizeof(int) * size);
    int *cnts = malloc(sizeof(int) * size);
    strcpy(gather_method, "mpi");
    outfile = stdout;

    if (rank == 0)
        fprintf(outfile, "\n");
    
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
            char *substr = malloc(sizeof(char)*64);
            if ((substr = strstr(argv[i], "=")) != NULL)
            {
                outfile = fopen(++substr, "w+");
            }
            display_result = true;
        }
        else
        {
            EXIT();
        }
    }

    if (num_loops == -1)
        num_loops = 5;

    if (data_per_node == -1)
        data_per_node = 5;

    global_buffer = malloc(sizeof(double) * size * data_per_node);
    local_buffer = malloc(sizeof(double) * data_per_node);
    elapsed_times = malloc(sizeof(double) * num_loops);

    for (i=0; i<size; i++)
        cnts[i] = data_per_node;

    if (!rank)
    {
        for (i=0; i<size; i++)
            fprintf(outfile, "cnts[%i] = %i\n", i, cnts[i]);
        fprintf(outfile, "\n");
    }

    offsets[0] = 0;
    for (j=1; j < size; j++)
        offsets[j] = offsets[j-1] + cnts[j-1];

    if (!rank)
    {
        for (i=0; i<size; i++)
            fprintf(outfile, "offset[%i] = %i\n", i, offsets[i]);
        fprintf(outfile, "\n");
        fprintf(outfile, "Looping %d times.\n", num_loops);
        fprintf(outfile, "Using gather method: %s.\n", gather_method);
        fprintf(outfile, "Looping %d times.\n", num_loops);
        fprintf(outfile, "Using %d data points per node.\n", data_per_node);
        fprintf(outfile, "size %d\n\n", size);
        fflush(outfile);
    }

    for (i=0; i<num_loops; i++)
    {
        memset(global_buffer, 0, sizeof(double) * size * data_per_node);
        memset(local_buffer, 0, sizeof(double) * data_per_node);
        end = 0;    start = 0;

        for (j=0; j<cnts[rank]; j++)
            local_buffer[j] = (double)(rank + 1);

        if (strcmp(gather_method, "mpi") == 0)
        {
            start = MPI_Wtime();
            MPI_Gatherv(
                local_buffer,
                cnts[rank],
                MPI_FLOAT,
                global_buffer,
                cnts,
                offsets,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);
            end = MPI_Wtime();
        }
        else if (strcmp(gather_method, "tree") == 0)
        {
            start = MPI_Wtime();
            tree_gatherv_d(
                local_buffer,
                cnts[rank],
                MPI_FLOAT,
                global_buffer,
                cnts,
                offsets,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);
            end = MPI_Wtime();
        }
        else if (strcmp(gather_method, "itree") == 0)
        {
            start = MPI_Wtime();
            tree_gatherv_d_async(
                local_buffer,
                cnts[rank],
                MPI_FLOAT,
                global_buffer,
                cnts,
                offsets,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);
            end = MPI_Wtime();
        }

        if (display_result && rank == 0)
        {
            if (i == 0 && outfile != stdout)
            {
                for (j=0; j<size*data_per_node; j++)
                {
                    fprintf(outfile, "global_buffer[%i] = %lf\n", j, global_buffer[j]);
                }
            }
            elapsed_times[i] = end - start;
            fprintf(outfile, "Elapsed time for iteration %i is %.15f sec\n", i, elapsed_times[i]);
            fflush(outfile);
        }
    }
    free(global_buffer);        free(local_buffer);

    if (rank == 0 && display_result)
    {
        double avg = 0.f;
        for (i=0; i<num_loops; i++)
            avg += elapsed_times[i]; 
        avg /= num_loops;
        fprintf(outfile, "Average time: %.15f seconds.\n", avg);
        fflush(outfile);
    }

    if (outfile != stdout)
        fclose(outfile);

    MPI_Finalize();
    return 0;
}
