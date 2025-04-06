SUBDIRS = s

test:
	cd $(SUBDIRS) && $(MAKE) $@
#	for var in $(SUBDIRS); do { cd $${var} && $(MAKE) $@; } || exit 1; cd ..; done

#here will add
.PHONY: test
