
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])

AC_INIT([sirc], [0.1])
AC_SUBST([PROJ], 'sirc')

AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_AUX_DIR([build-aux])

# compile with -x c++
AC_ARG_ENABLE(cpp,
  AS_HELP_STRING([--enable-cpp],[enable cpp, default: yes]),
  [case "${enableval}" in yes) cpp=true ;; no) cpp=false ;; *) AC_MSG_ERROR([bad value ${enableval} for --enable-cpp]) ;; esac],
  [cpp=true])
AM_CONDITIONAL(CPP, test x"$cpp" = x"true")

#
AC_DEFINE_UNQUOTED(HOMEDIR,[$(echo "\"$(cd ~; pwd)/\"")],[~ cannot be "q q"])
AC_PROG_CC
AC_CHECK_HEADERS(libgen.h limits.h fcntl.h netdb.h netinet/in.h openssl/ssl.h stdio.h stdlib.h string.h sys/socket.h unistd.h)
PKG_CHECK_MODULES([GTK], [gtk+-3.0],[

user_was_set_cflg="$CFLAGS"
CFLAGS="$GTK_CFLAGS"
X_PROGRAM="#include<gtk/gtk.h>"
AC_MSG_CHECKING([gtk/gtk.h])
AC_COMPILE_IFELSE(
   [AC_LANG_SOURCE([$X_PROGRAM])],
   [AC_MSG_RESULT([yes]) AC_DEFINE(HAVE_GTK_GTK_H, 1, [3])],
   [AC_MSG_RESULT([no])
    AC_SUBST([GTK_CFLAGS], '')]
)
CFLAGS="$user_was_set_cflg"

], [AC_MSG_ERROR("pkg check modules failed for gtk")])

AS_IF([test x"$cpp" = x"false"],[
  AC_CHECK_HEADERS(stdbool.h stddef.h)])
#

AM_INIT_AUTOMAKE

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
