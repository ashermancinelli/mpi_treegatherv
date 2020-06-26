
check: all
	@echo
	@echo Gather Algorithms Integration Tests:
	@echo
	@$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method mpi 		\
			--num-loops 10 		|| exit 1
	@echo
	@echo Test mpi: pass
	@echo
	@$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method my-mpi \
			--num-loops 10 		|| exit 1
	@echo
	@echo Test my mpi: pass
	@echo
	@$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method tree 	\
			--num-loops 10 		|| exit 1
	@echo
	@echo Test tree: pass
	@echo
	@$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method itree 	\
			--num-loops 10 		|| exit 1
	@echo
	@echo Test itree: pass
	@echo
	@$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method my-mpi \
			--num-loops 10 --persist		|| exit 1
	@echo
	@echo Test my mpi persist: pass
	@echo
	@$(MPIRUN) $(MPIRUN_ARGS) $(BIN_NAME) --data-per-node 1024 --gather-method itree 	\
			--num-loops 10 --persist		|| exit 1
	@echo
	@echo Test itree persist: pass
	@echo
	@echo
	@echo Gather Algorithms Integration Tests passed
	@echo
	@$(MAKE) -C src check
	@$(MAKE) -C tools check
