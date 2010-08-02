# Process this file with autoconf to produce a configure script.
# Configure template for krisp library
#
# $Id: configure.ac,v 1.32 2010-08-02 17:38:33 oops Exp $

#
AC_PREREQ(2.59)
AC_INIT([krisp], [3.0.1], [http://oops.org])
AC_CONFIG_SRCDIR([krisp.h])
AC_CONFIG_HEADER([krisp-config.h krispcommon.h])
AC_PREFIX_DEFAULT([/usr/local])

AC_SUBST(SONAME_INFO)
AC_SUBST(SONAME_VERSION)
AC_SUBST(SONAME_MAJOR)
AC_SUBST(SONAME_MINOR)
AC_SUBST(SONAME_REVISION)

SONAME_MAJOR=2
SONAME_MINOR=0
SONAME_REVISION=0
SONAME_VERSION=$SONAME_MAJOR.$SONAME_MINOR.$SONAME_REVISION

SONAME_VALUE=$(($SONAME_MAJOR + $SONAME_MINOR))
SONAME_INFO="$SONAME_VALUE:$SONAME_REVISION:$SONAME_MINOR"

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_AWK
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

AC_ARG_WITH(sqlite,
[  --with-sqlite=PATH      sqlite path [[default: /usr]]])

AC_ARG_WITH(libiconv,
[  --with-libiconv=PATH    libiconv prefix])

AC_PATH_PROG(MAKE, make)
if test ! -f "$ac_cv_path_MAKE"; then
	AC_MSG_ERROR(Can't not found make)
fi

AC_PATH_PROG(PERL, perl)
if test ! -f "$ac_cv_path_PERL"; then
	AC_MSG_ERROR(Can't not found perl)
fi

AC_PATH_PROG(STRIP, strip)
if test ! -f "$ac_cv_path_STRIP"; then
	AC_MSG_ERROR(Can't not found strip)
fi

AC_PATH_PROG(RM, rm)
if test ! -f "$ac_cv_path_RM"; then
	AC_MSG_ERROR(Can't not found rm)
fi

AC_PATH_PROG(MKDIR, mkdir)
if test ! -f "$ac_cv_path_MKDIR"; then
	AC_MSG_ERROR(Can't not found mkdir)
fi

AC_PATH_PROG(MV, mv)
if test ! -f "$ac_cv_path_MV"; then
	AC_MSG_ERROR(Can't not found mv)
fi

AC_CHECK_LIB(dl, dlopen,,)

if test "x$with_sqlite" != "x"; then
	SEARCHPATH="$dbpath /usr /usr/local /opt"

	for i in $SEARCHPATH
	do
		for j in sqlite3 sqlite
		do
			if test -f "$i/lib/lib${j}.so" -o -f "$i/lib/lib${j}.a"; then
				sqlitelib_tmp="L$i/lib"
			elif test -f -f "$i/lib64/lib${j}.so" -o -f "$i/lib64/lib${j}.a"; then
				sqlitelib_tmp="L$i/lib64"
			fi

			if test "x$sqlitelib_tmp" != "x"; then
				chk=`echo "$sqlitelib" | sed 's/-//g' | grep "$sqlitelib_tmp "`
				if test -z "$chk"; then
					sqlitelib="$sqlitelib -$sqlitelib_tmp "
				fi
			fi

			if test -f "$i/include/${j}.h"; then
				sqlitehead_tmp="I$i/include"
			fi

			if test "x$sqlitehead_tmp" != "x"; then
				chk=`echo "$sqlitehead" | sed 's/-//g' | grep "$sqlitehead_tmp "`
				if test -z "$chk" ; then
					sqlitehead="$sqlitehead -$sqlitehead_tmp "
				fi
			fi
		done
	done

	sqlitelib=`echo "$sqlitelib" | sed 's/^[ ]*\|[ ]*$//g'`
	sqlitehead=`echo "$sqlitehead" | sed 's/^[ ]*\|[ ]*$//g'`

	export LDFLAGS="$sqlitelib $LDFALSG"
	export CPPFLAGS="$sqlitehead $CPPFLAGS"
fi

AC_CHECK_LIB(sqlite3, sqlite3_open,,)

if test "$ac_cv_lib_sqlite3_sqlite3_open" = "yes"; then
	KR_DEFS="$KR_DEFS -DHAVE_LIBSQLITE3"
else
	AC_CHECK_LIB(
		sqlite, sqlite_open, ,
		[AC_MSG_ERROR([Can't not found sqlte library (-lsqlite)])]
	)
	KR_DEFS="$KR_DEFS -DHAVE_LIBSQLITE"
fi

if test "x$with_libiconv" != "x"; then
	if test -f "$with_libiconv/lib/libiconv.so" -o -f "$with_libiconv/lib/libiconv.a"; then
		export LDFLAGS="$LDFLAGS -L$with_libiconv/lib"
	fi

	if test -f "$with_libiconv/include/iconv.h"; then
		export CPPFLAGS="$CPPFLAGS -I$with_libiconv/include"
	fi

	AC_CHECK_LIB(
		iconv, libiconv_open,,
		[ AC_MSG_ERROR([Can not find libiconv library (-liconv)])]
	)
fi

AC_CHECK_LIB(pthread, pthread_create,,,)
if test "x$ac_cv_lib_pthread_pthread_create" != "x"; then
	KR_DEFS="$KR_DEFS -DHAVE_PTHREAD_H"
fi

DBPATH=$(eval echo ${datadir}/krisp)
AC_SUBST(DBPATH)

# Checks for header files.
AC_HEADER_STDC

AC_CHECK_HEADERS([arpa/inet.h getopt.h netinet/in.h stdio.h stdlib.h \
					string.h sys/socket.h sqlite3.h sqlite.h iconv.h \
					stdbool.h pthread.h])
AC_CHECK_HEADER([stdbool.h], [KR_DEFS="$KR_DEFS -DHAVE_STDBOOL_H"])

AC_SUBST(KR_DEFS)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([getopt_long])

AC_CONFIG_FILES([Makefile test/Makefile krisp-config krdb.h libkrisp.spec])
AC_OUTPUT
