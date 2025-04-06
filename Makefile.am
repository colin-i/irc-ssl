SUBDIRS = s

test:
	z="$(SUBDIRS)"; \
	for var in "$${z[@]}"; do \
		cd $${var}; \
		$(MAKE) $@; \
		cd ..; \
	done

#here will add
.PHONY: test
