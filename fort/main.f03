program main
    use mpi_f08
    implicit none

    integer :: rank, comm_size, ierror, i, DATA_LEN=10, n_loops=5, loop
    integer, dimension(:), allocatable          ::  counts
    integer, dimension(:), allocatable          ::  offsets
    double precision                            ::  average
    double precision, dimension(:), allocatable ::  global_buf
    double precision, dimension(:), allocatable ::  local_buf

    character(len=32) :: arg, gather_method, c
    
    call MPI_INIT(ierror)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, comm_size, ierror)
    call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierror)

    allocate(counts(comm_size))
    allocate(offsets(comm_size))
    allocate(local_buf(DATA_LEN))
    allocate(global_buf(DATA_LEN * comm_size))

    do i = 1, comm_size
        counts(i) = DATA_LEN
    end do

    offsets(1) = 0
    do i = 2, comm_size
        offsets(i) = offsets(i-1) + counts(i-1)
    end do

    do i = 1, DATA_LEN
        local_buf(i) = rank + 1
    end do

    do i = 1, DATA_LEN*comm_size
        global_buf(i) = 0
    end do

    do loop = 1, n_loops
    
        if (gather_method .eq. 'tree') then
            call tree_gatherv(local_buf, counts(rank+1), MPI_DOUBLE_PRECISION, &
                global_buf, counts, offsets, MPI_DOUBLE_PRECISION, &
                0, MPI_COMM_WORLD, ierror)
        end if

        if (gather_method .eq. 'mpi') then
            call MPI_Gatherv(local_buf, counts(rank+1), MPI_DOUBLE_PRECISION, &
                global_buf, counts, offsets, MPI_DOUBLE_PRECISION, &
                0, MPI_COMM_WORLD, ierror)
        end if

    end do

    if (rank .eq. 0) then
        do i=1, DATA_LEN*comm_size
            write (*,*) 'global_buf(', i, ') = ', global_buf(i)
        end do
    end if

    deallocate(counts)    
    deallocate(offsets)
    call MPI_FINALIZE(ierror)

end program
