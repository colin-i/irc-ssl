
AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = sirc
sirc_SOURCES = main.c
sirc_LDFLAGS = -lssl @GTK_LIBS@
sirc_CFLAGS = @GTK_CFLAGS@ -Weverything
if CPP
sirc_CPPFLAGS = -x c++ -Wno-old-style-cast -Wno-c++98-compat -Wno-c++98-compat-pedantic
#98compat & pedantic
endif
