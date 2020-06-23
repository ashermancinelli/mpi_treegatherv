
all: test_parse_args test_tree_math
	$(BINDIR)/test_tree_math || exit 1
	$(BINDIR)/test_parse_args || exit 1
	@echo
	@echo Utils tests passed
	@echo

test_parse_args: parse_args.c
	$(CC)				$<			$(BUILDDIR)/*.o -o $(BINDIR)/$@		$(CFLAGS)

test_tree_math: tree_math.c
	$(CC)				$<			$(BUILDDIR)/*.o -o $(BINDIR)/$@		$(CFLAGS)

