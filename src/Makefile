
all:
	for dir in common utils lib driver; do $(MAKE) -C $$dir; done

check: all
	$(MAKE) -C lib check
	$(MAKE) -C utils check

clean:
	for i in lib utils driver; do $(MAKE) -C $$i clean; done
