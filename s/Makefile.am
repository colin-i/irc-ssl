
AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = sirc
sirc_SOURCES = main.c
sirc_LDADD = -lssl @GTK_LIBS@

sirc_CFLAGS = @GTK_CFLAGS@
# -DBDIR=\"$(DESTDIR)$(bindir)/\"
#this is stucking at realpath when executing from /usr/bin in PATH
#"The running script must have executable permissions on all directories in the hierarchy, otherwise realpath() will return FALSE."

if CPP
sirc_CPPFLAGS = -x c++ -Wno-old-style-cast -Wno-c++98-compat -Wno-c++98-compat-pedantic
endif

CLEANFILES = ${bin_PROGRAMS}t

test:
	flag1=`echo "${DEFS}" | grep -o "\-DMKDIR_1ARG=1"`; \
	flag2=`echo "${DEFS}" | grep -o "\-DHAVE_WINDOWS_H=1"`; \
	text="$(CC) $${flag1} $${flag2} -I. ${sirc_SOURCES} ${sirc_LDADD} -o ${bin_PROGRAMS}t"; \
	echo $${text}; \
	$${text}

#here will add
.PHONY: test
