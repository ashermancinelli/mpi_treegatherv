
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "mpi.h"

#include "tree_gather.h"

#define DEFAULT_KEEP_RATIO 0.8f

#define EXIT() \
    ({ \
        if (!rank) \
        { \
            fprintf(stderr, "%s%s%s", RED, usage, RESET); \
            fflush(stderr); \
        } \
        MPI_Finalize(); \
        return 0; \
     })

char* usage = "Usage:\n"
    "\t--gather-method      (mpi|my-mpi|my-mpi-persist|tree|itree|itree-persist)\n"
    "\t--output-file=<filename or blank for stdout>\n"
    "\t--display-buf        (for sanity checking)\n"
    "\t--data-per-node      <int>\n"
    "\t--keep-percentage    <int>\n"
    "\t--num-loops          <int>\n"
    "\t--persistent         (use persistent communication)";

int _MPI_Gatherv(
    double *sendbuf, int sendcnt,   MPI_Datatype sendtype,
    double *recvbuf, int *recvcnts, int *displs,
    MPI_Datatype recvtype, int root, MPI_Comm comm)
{
    return MPI_Gatherv(
            (void*)sendbuf, sendcnt, sendtype,
            (void*)recvbuf, recvcnts, displs,
            recvtype, root, comm);
}

static int cmp (const void * a, const void * b)
{
    if (*(double*)a > *(double*)b) return 1;
    else if (*(double*)a < *(double*)b) return -1;
    else return 0;  
}

void sort(void* base, size_t n, size_t size)
{
    qsort(base, n, size, cmp);
}

int main(int argc, char** argv)
{
    int rank, size, i, j,
        num_loops = -1, data_per_node = -1;
    bool display_buf = false;
    bool persistent = false;
    double *global_buffer;
    double *local_buffer;
    char gather_method[32];
    FILE* outfile;
    float keep_percentage = DEFAULT_KEEP_RATIO;
    MPI_Request reqs[MAX_MPI_RANKS];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *offsets = malloc(sizeof(int) * size);
    int *cnts = malloc(sizeof(int) * size);
    strcpy(gather_method, "mpi");
    outfile = stdout;

    int (*gatherv_function)(
        double *sendbuf, int sendcnt,   MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm);
    int (*persistent_gatherv_function)(
        double *sendbuf, int sendcnt, MPI_Datatype sendtype,
        double *recvbuf, int *recvcnts, int *displs,
        MPI_Datatype recvtype, int root, MPI_Comm comm,
        MPI_Request* reqs);

    for (i=0; i<argc; i++)
    {
        if (i == 0)
            continue;

        if (strcmp(argv[i], "--help")==0 ||
            strcmp(argv[i], "-h")==0)
            EXIT();
        else if (strcmp(argv[i], "--persistent") == 0)
        {
            persistent = true;
        }
        else if (strcmp(argv[i], "--gather-method") == 0)
        {
            if (i == argc+1) 
                EXIT();

            strcpy(gather_method, argv[++i]);

            if ((strcmp(gather_method, "mpi") != 0) &&
                    (strcmp(gather_method, "my-mpi") != 0) &&
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
        else if (strcmp(argv[i], "--keep-percentage") == 0)
        {
            if (i == argc+1) 
                EXIT();

            keep_percentage = (float)atoi(argv[++i]) / 100;
            if (keep_percentage > 1.f || keep_percentage < 0.f)
                EXIT();
        }
        else if (strcmp(argv[i], "--data-per-node") == 0)
        {
            if (i == argc+1) 
                EXIT();

            data_per_node = atoi(argv[++i]);
        }
        else if (strstr(argv[i], "--display-buf") != NULL)
        {
            display_buf = true;
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

    offsets[0] = 0;
    for (i=0; i<size; i++)
        cnts[i] = data_per_node;
    for (j=1; j < size; j++)
        offsets[j] = offsets[j-1] + cnts[j-1];
    for (j=0; j<cnts[rank]; j++)
        local_buffer[j] = (double)(rank + 1);
    for (i=0; i<MAX_MPI_RANKS; i++) reqs[i] = MPI_REQUEST_NULL;

    if (!rank)
    {
        if (display_buf)
        {
            fprintf(outfile, "Config:\n");
            for (i=0; i<size; i++)
                fprintf(outfile, "cnts[%i] = %i\n", i, cnts[i]);
            fprintf(outfile, "\n");
            for (i=0; i<size; i++)
                fprintf(outfile, "offset[%i] = %i\n", i, offsets[i]);
            fprintf(outfile, "\n");
        }
        fprintf(outfile, "Using gather method: %s.\n", gather_method);
        fprintf(outfile, "Looping %d times.\n", num_loops);
        fprintf(outfile, "Using %d data points per node.\n", data_per_node);
        fprintf(outfile, "Using %d procs.\n\n", size);
        fflush(outfile);
    }

    if (strcmp(gather_method, "mpi") == 0)
        gatherv_function = _MPI_Gatherv;

    else if (strcmp(gather_method, "tree") == 0)
        gatherv_function = tree_gatherv_d;

    else if (strcmp(gather_method, "itree") == 0)
        gatherv_function = tree_gatherv_d_async;

    else if (strcmp(gather_method, "itree") == 0 && persistent)
        persistent_gatherv_function = tree_gatherv_d_persistent;

    else if (strcmp(gather_method, "my-mpi") == 0)
        gatherv_function = my_mpi_gatherv;

    else if (strcmp(gather_method, "my-mpi") == 0 && persistent)
        persistent_gatherv_function = my_mpi_gatherv_persistent;

    // critical loop
    if (persistent)
    {
        if (!rank)
            fprintf(stdout, "Running critical loop with "
                "persistent communication.\n");
        for (i=0; i<num_loops; i++)
        {
            (*persistent_gatherv_function)(
                local_buffer,
                cnts[rank],
                MPI_DOUBLE,
                global_buffer,
                cnts,
                offsets,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD,
                reqs);
        }
    }
    else
    {
        if (!rank)
            fprintf(stdout, "Running critical loop with "
                "non-persistent communication.\n");
        for (i=0; i<num_loops; i++)
        {
            (*gatherv_function)(
                local_buffer,
                cnts[rank],
                MPI_DOUBLE,
                global_buffer,
                cnts,
                offsets,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);
        }
    }

    if (!rank && display_buf)
    {
        fprintf(outfile, "Display buffer: \n");
        for (i=0; i<data_per_node*size; i++)
            fprintf(outfile, "%.2f\n", global_buffer[i]);
        fprintf(outfile, "\n");
    }
    // \critical loop

    free(global_buffer);        free(local_buffer);

    if (outfile != stdout)
        fclose(outfile);

    MPI_Finalize();
    return 0;
}
