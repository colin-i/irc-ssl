
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])

AC_INIT([sirc], [0.1])
#AC_SUBST([PROJ], 'sirc')

AC_CONFIG_SRCDIR([s/main.c])
AC_CONFIG_AUX_DIR([build-aux])

# compile with -x c++
AC_ARG_ENABLE(cpp,
  AS_HELP_STRING([--enable-cpp],[enable cpp, default: yes]),
  [case "${enableval}" in yes) cpp=true ;; no) cpp=false ;; *) AC_MSG_ERROR([bad value ${enableval} for --enable-cpp]) ;; esac],
  [cpp=true])
AM_CONDITIONAL(CPP, test x"$cpp" = x"true")

#

# Check for CFLAGS
AC_MSG_CHECKING([whether configure should try to set CFLAGS])
if test "x${CFLAGS+set}" = xset; then
enable_cflags_setting=no
else
enable_cflags_setting=yes
fi
AC_MSG_RESULT($enable_cflags_setting)

#AC_DEFINE_UNQUOTED(HOMEDIR,[$(echo "\"$(cd ~; pwd)/\"")],[~ cannot be "q q"])
AC_PROG_CC
AC_CHECK_HEADERS(libgen.h fcntl.h pthread.h stdlib.h string.h time.h unistd.h)
#limits.h

AC_CHECK_HEADERS(openssl/ssl.h, [
	AC_SEARCH_LIBS([SSL_new],[ssl])
])

#__imp_socket
AC_CHECK_HEADERS(windows.h, [
	AC_SEARCH_LIBS([__imp_WSAStartup],[ws2_32])
], [
	AC_CHECK_HEADERS(netdb.h netinet/in.h signal.h sys/socket.h)
])

#is not with define _GNU_SOURCE
AC_CHECK_HEADERS(stdio.h, [
	AC_MSG_CHECKING([function getdelim])
	AC_COMPILE_IFELSE(
		[AC_LANG_SOURCE([[
			#include <stdio.h>
			void main(){FILE*nonnull;getdelim(0,0,0,nonnull);}
		]])],
		[
			AC_MSG_RESULT([yes])
			AC_DEFINE(HAVE_GETDELIM, 1)
		],
		[
			AC_MSG_RESULT([no])
		]
	)
])
AC_CHECK_HEADERS(sys/stat.h, [
	AC_MSG_CHECKING([mkdir without mode_t])
	AC_COMPILE_IFELSE(
		[AC_LANG_SOURCE([[
			#include <sys/stat.h>
			void main(){mkdir("a");}
		]])],
		[
			AC_MSG_RESULT([yes])
			AC_DEFINE(MKDIR_1ARG, 1)
		],
		[
			AC_MSG_RESULT([no])
		]
	)
])

start_cflgs="${CFLAGS}"

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
], [
	#there is a headless build, but still lazy about @GTK_LIBS@ at link
	#GTK_LIBS=" -lgtk-3 -lgdk-3 -lz -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 "
	AC_MSG_CHECKING([whether GTK_LIBS is set])
	if test "x${GTK_LIBS+set}" = xset; then
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
		AC_MSG_ERROR("pkg check modules failed for gtk")
	fi
])

CFLAGS="-Werror=unknown-warning-option -Weverything"
AC_MSG_CHECKING([-Weverything])
AC_COMPILE_IFELSE(
   [AC_LANG_SOURCE([])],
   [AC_MSG_RESULT([yes])
	start_cflgs="${start_cflgs} -Weverything -Werror"],
   [AC_MSG_RESULT([no])]
)

CFLAGS="${start_cflgs}"

AS_IF([test x"$cpp" = x"false"],[
  AC_CHECK_HEADERS(stdbool.h stddef.h)])

#back at CFLAGS
test "x${enable_cflags_setting}" = xyes && CFLAGS="-O3 -s"

#

AM_INIT_AUTOMAKE

AC_CONFIG_FILES([Makefile s/Makefile])

AC_OUTPUT

##set -v
#the connections. no need to remove them between ./configure
#if [[ ! -e "./.${PROJ}info" ]]; then echo -n >"./.${PROJ}info"; fi
#sudo make will change ~
#echo ~>./bash_home
