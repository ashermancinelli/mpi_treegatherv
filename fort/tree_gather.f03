subroutine tree_gatherv(sendbuf, sendcnt, sendtype, &
        recvbuf, recvcnts, displs, recvtype, &
        root, comm, ierror)
    use mpi_f08_types, only : MPI_Datatype, MPI_Comm
    implicit none

    type(*), dimension(..)                      ::  sendbuf, recvbuf
    integer, intent(in)                         ::  sendcnt, root
    type(MPI_Datatype), intent(in)              ::  sendtype, recvtype
    type(MPI_Comm), intent(in)                  ::  comm
    integer, dimension(..)                      ::  displs, recvcnts
    integer, optional, intent(inout)            ::  ierror

    integer ::  comm_size, rank

    call MPI_Comm_size(comm, comm_size, ierror)
    call MPI_Comm_rank(comm, rank, ierror)

    if (rank .eq. 0) then
        print *, 'Rank 0 working.'
    end if

end subroutine tree_gatherv
