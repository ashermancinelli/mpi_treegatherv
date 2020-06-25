
all: test_tree_gather
	$(MPIRUN) $(MPIRUN_ARGS) $(BINDIR)/test_tree_gather || exit 1
	@echo
	@echo Tree gather tests passed
	@echo

test_tree_gather: tree_gather.c
	@echo
	$(CC)				$<			$(BUILDDIR)/*.o -o $(BINDIR)/$@		$(CFLAGS)

clean:
	if [ -f test_tree_gather ]; then rm test_tree_gather; fi
