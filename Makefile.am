
AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = sirc
sirc_SOURCES = main.c
sirc_LDFLAGS = -lssl @GTK_LIBS@
sirc_CFLAGS = -DBDIR=\"$(DESTDIR)$(bindir)/\" @GTK_CFLAGS@
if CPP
sirc_CPPFLAGS = -x c++ -Wno-old-style-cast -Wno-c++98-compat -Wno-c++98-compat-pedantic
endif

install-exec-local:
	if [ ! -e "`cat ./bash_home`/.@PROJ@info" ]; then cp "./.@PROJ@info" "`cat ./bash_home`/" --preserve=ownership; fi

uninstall-local:
	if [ -e "`cat ./bash_home`/.@PROJ@info" ]; then rm "`cat ./bash_home`/.@PROJ@info"; fi
