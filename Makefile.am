
AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = sirc
sirc_SOURCES = main.c
sirc_LDFLAGS = -lssl `pkg-config --libs gtk+-3.0`
sirc_CFLAGS = -Weverything
if CPP
sirc_CPPFLAGS = -x c++
# -Wno-old-style-cast
endif
