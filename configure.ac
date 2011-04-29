# Process this file with autoconf to produce a configure script.
# Configure template for krisp library
#
# $Id: configure.ac,v 1.48 2010-11-08 06:08:11 oops Exp $

#
AC_PREREQ(2.59)
AC_INIT([libkrisp], [3.1.3], [http://oops.org])
AC_CONFIG_AUX_DIR([tools])
AM_INIT_AUTOMAKE([-Wall -Werror -Wno-override foreign no-dependencies])
AM_MAINTAINER_MODE

AC_CONFIG_SRCDIR([krisp.h])
AC_CONFIG_HEADER([krisp-config.h krispcommon.h])
AC_PREFIX_DEFAULT([/usr/local])

AC_DEFUN([AC_DATAROOTDIR_CHECKED])

AC_SUBST(SONAME_INFO)
AC_SUBST(SONAME_VERSION)
AC_SUBST(SONAME_MAJOR)
AC_SUBST(SONAME_MINOR)
AC_SUBST(SONAME_REVISION)

SONAME_MAJOR=2
SONAME_MINOR=0
SONAME_REVISION=1
SONAME_VERSION=$SONAME_MAJOR.$SONAME_MINOR.$SONAME_REVISION

SONAME_VALUE=$(($SONAME_MAJOR + $SONAME_MINOR))
SONAME_INFO="$SONAME_VALUE:$SONAME_REVISION:$SONAME_MINOR"

AX_COMPAT_OPTION

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_AWK
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_LIB_RPATH

AX_GL_VISIBILITY

AC_PATH_PROG(PERL, perl)
if test ! -f "$ac_cv_path_PERL"; then
	AC_MSG_ERROR(Can't not found perl)
fi

AC_PATH_PROG(DATE, date)
if test ! -f "$ac_cv_path_DATE"; then
	AC_MSG_ERROR(Can't not found date)
fi

AC_SUBST(PACKAGE_DATE)
PACKAGE_DATE=$($DATE +"%a %b %d %Y")

# Checks for libraries.
AC_CHECK_LIB(dl, dlopen,,)

AM_ICONV
AX_IPCALC(libtool, 1)
AX_SQLITE

AC_CHECK_LIB(pthread, pthread_create,,,)
if test "x$ac_cv_lib_pthread_pthread_create" != "x"; then
	CPPFLAGS="$CPPFLAGS -D_REENTRANT"
fi

if test "x$prefix" = "xNONE" ; then
	prefix=$ac_default_prefix
fi
DBPATH="$(eval echo ${datadir}/krisp)$COMPAT_VERSION"
AC_SUBST(DBPATH)
AC_DEFINE_UNQUOTED([DBPATH],["$DBPATH"],[define krisp database locate.])

# Checks for header files.
AC_HEADER_STDC

AC_CHECK_HEADERS([arpa/inet.h getopt.h netinet/in.h stdio.h stdlib.h \
					string.h sys/socket.h iconv.h \
					stdbool.h pthread.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
#AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([getopt_long])

# Type check
AC_CHECK_TYPE([ulong], [AC_DEFINE([C_ULONG_DEFINED], [1], [Define to 1 if the ulong type exists in C.])])

AX_STATIC_BINDIR

AC_CONFIG_FILES([
	Makefile
	man/Makefile
	scripts/Makefile
	test/Makefile
	utils/Makefile
	scripts/krisp.pc
	scripts/krisp-config
	m4/krisp.m4
])
AC_OUTPUT
