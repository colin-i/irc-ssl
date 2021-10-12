
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

#install-exec-local:
#	if [ ! -e "`cat ./bash_home`/.@PROJ@info" ]; then cp "./.@PROJ@info" "`cat ./bash_home`/" --preserve=ownership; fi

#uninstall-local:
#	if [ -e "`cat ./bash_home`/.@PROJ@info" ]; then rm "`cat ./bash_home`/.@PROJ@info"; fi
