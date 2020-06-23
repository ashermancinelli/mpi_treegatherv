
all: test_parse_args test_tree_math
	@echo
	$(BINDIR)/test_tree_math || exit 1
	@echo
	$(BINDIR)/test_parse_args || exit 1
	@echo
	@echo Utils tests passed
	@echo
	if [ -f filename ]; then rm filename; fi

test_parse_args: parse_args.c
	@echo
	$(CC)				$<			$(BUILDDIR)/*.o -o $(BINDIR)/$@		$(CFLAGS)

test_tree_math: tree_math.c
	@echo
	$(CC)				$<			$(BUILDDIR)/*.o -o $(BINDIR)/$@		$(CFLAGS)

