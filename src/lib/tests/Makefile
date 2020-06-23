
all: test_tree_gather
	$(BINDIR)/test_tree_gather || exit 1
	@echo
	@echo Tree gather tests passed
	@echo

test_tree_gather: tree_gather.c
	$(CC)				$<			$(BUILDDIR)/*.o -o $(BINDIR)/$@		$(CFLAGS)
