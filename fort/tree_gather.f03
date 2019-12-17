subroutine tree_gatherv(sendbuf, sendcnt, sendtype, &
        recvbuf, recvcnts, displs, recvtype, &
        root, comm, ierror)
    use mpi_f08
    implicit none

    type(*), dimension(..)                      ::  sendbuf, recvbuf
    integer, intent(in)                         ::  sendcnt, root
    type(MPI_Datatype), intent(in)              ::  sendtype, recvtype
    type(MPI_Comm), intent(in)                  ::  comm
    integer, dimension(..)                      ::  displs, recvcnts
    integer, optional, intent(inout)            ::  ierror

    integer ::  comm_size, rank, i, cnt

    call MPI_Comm_size(comm, comm_size, ierror)
    call MPI_Comm_rank(comm, rank, ierror)

    if (rank .eq. 0) then
        print *, 'Rank 0 recieving.'
        do i = 0, comm_size
            cnt = recvcnts(i)
            call MPI_RECV(recvbuf + displs(i), cnt, &
                recvtype, i, 0, MPI_STATUS_IGNORE)
        end do
    else
        write (*,*) 'Rank ', rank, ' sending to root.'
        call MPI_SEND(sendbuf, sendcnt, sendtype, 0, 0, comm)
    end if

end subroutine tree_gatherv
