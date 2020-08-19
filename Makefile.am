
AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = sirc
sirc_SOURCES = main.c
sirc_LDFLAGS = -lssl @GTK_LIBS@
sirc_CFLAGS = -DBDIR=\"$(DESTDIR)$(bindir)/\" @GTK_CFLAGS@ -Weverything -Werror
if CPP
sirc_CPPFLAGS = -x c++ -Wno-old-style-cast -Wno-c++98-compat -Wno-c++98-compat-pedantic
endif

allinfofilecommand:
	if [ ! -e ./.@PROJ@info ]; then touch ./.@PROJ@info; fi

all: all-am allinfofilecommand

setinfofilecommand:
	if [ ! -e ~/.@PROJ@info ]; then touch ~/.@PROJ@info; fi

install: install-am setinfofilecommand

reminfofilecommand:
	if [ -e ~/.@PROJ@info ]; then rm ~/.@PROJ@info; fi

uninstall: uninstall-am reminfofilecommand
