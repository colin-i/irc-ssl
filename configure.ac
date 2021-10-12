
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])

AC_INIT([sirc], [0.1])
#AC_SUBST([PROJ], 'sirc')

AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_AUX_DIR([build-aux])

# compile with -x c++
AC_ARG_ENABLE(cpp,
  AS_HELP_STRING([--enable-cpp],[enable cpp, default: yes]),
  [case "${enableval}" in yes) cpp=true ;; no) cpp=false ;; *) AC_MSG_ERROR([bad value ${enableval} for --enable-cpp]) ;; esac],
  [cpp=true])
AM_CONDITIONAL(CPP, test x"$cpp" = x"true")

#

#AC_DEFINE_UNQUOTED(HOMEDIR,[$(echo "\"$(cd ~; pwd)/\"")],[~ cannot be "q q"])
AC_PROG_CC
AC_CHECK_HEADERS(libgen.h fcntl.h netdb.h netinet/in.h openssl/ssl.h pthread.h signal.h stdio.h stdlib.h string.h sys/socket.h time.h unistd.h)
#limits.h

start_cflgs="$CFLAGS"

PKG_CHECK_MODULES([GLIB], [glib-2.0],[
      CFLAGS="$GLIB_CFLAGS"
      AC_MSG_CHECKING([g_memdup2])
      AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
            #include <glib.h>
            #pragma clang diagnostic error "-Wimplicit-function-declaration"
            int main(){g_memdup2(NULL,0);}
         ]])],
         [
            AC_MSG_RESULT(yes)
            AC_DEFINE(FN_G_MEMDUP2, 1, [use g_memdup2])
         ],
         [
            AC_MSG_RESULT(no)
         ]
      )
      #not using GLIB_CFLAGS
])

PKG_CHECK_MODULES([GTK], [gtk+-3.0],[

CFLAGS="$GTK_CFLAGS"
X_PROGRAM="#include<gtk/gtk.h>"
AC_MSG_CHECKING([gtk/gtk.h])
AC_COMPILE_IFELSE(
   [AC_LANG_SOURCE([$X_PROGRAM])],
   [
      AC_MSG_RESULT([yes])
      AC_DEFINE(HAVE_GTK_GTK_H, 1, [3])
   ],
   [AC_MSG_RESULT([no])
    AC_SUBST([GTK_CFLAGS], '')]
)
#CFLAGS="$start_cflgs"

], [AC_MSG_ERROR("pkg check modules failed for gtk")])

CFLAGS="-Werror=unknown-warning-option -Weverything"
AC_MSG_CHECKING([-Weverything])
AC_COMPILE_IFELSE(
   [AC_LANG_SOURCE([])],
   [AC_MSG_RESULT([yes])
	start_cflgs="$start_cflgs -Weverything -Werror"],
   [AC_MSG_RESULT([no])]
)

CFLAGS="$start_cflgs"

AS_IF([test x"$cpp" = x"false"],[
  AC_CHECK_HEADERS(stdbool.h stddef.h)])

#

AM_INIT_AUTOMAKE

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

##set -v
#the connections. no need to remove them between ./configure
#if [[ ! -e "./.${PROJ}info" ]]; then echo -n >"./.${PROJ}info"; fi
#sudo make will change ~
#echo ~>./bash_home
