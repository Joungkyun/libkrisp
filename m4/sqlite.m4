dnl Copyright (C) 2016 JoungKyun.Kim <http://oops.org>
dnl
dnl This file is part of libkrisp
dnl
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU Lesser General Public License as published
dnl by the Free Software Foundation, either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl

AC_DEFUN([AX_SQLITE],
[
	AC_ARG_WITH(sqlite,
		[  --with-sqlite=PATH      prefix of sqlite or sqlite3[[default: /usr]]])

	if test "x$with_sqlite" != "xno"; then
		if test "x$with_sqlite" != "x"; then
			if test "x$with_sqlite" = "xyes"; then
				sqlite_prefix="/usr"
			else
				sqlite_prefix="$with_sqlite"
			fi
		else
			sqlite_prefix="/usr"
		fi

		sqlite_incdir="$sqlite_prefix/include"
		sqlite_cppflags="-I$sqlite_incdir"
		if test -f "$sqlite_prefix/lib64/libsqlite3.so" || test -f "$sqlite_prefix/lib64/libsqlite.so"; then
			sqlite_libdir="$sqlite_prefix/lib64"
		elif test -f "$sqlite_prefix/lib64/libsqlite3.a" || test -f "$sqlite_prefix/lib64/libsqlite.a"; then
			sqlite_libdir="$sqlite_prefix/lib64"
		else
			sqlite_libdir="$sqlite_prefix/lib"
		fi

		sqlite_ldflags="-L$sqlite_libdir"

		#
		# sqlite library check
		#
		sqlite_OLD_FLAGS="$LDFLAGS"
		LDFLAGS="$sqlite_ldflags"

		AC_CHECK_LIB(
			sqlite3, sqlite3_open, [
				AC_DEFINE(
					[HAVE_LIBSQLITE3], 1,
					[Define to 1 if you have the `sqlite3' library (-lsqlite3).]
				)
				LIBS="-lsqlite3 $LIBS"
			],
		)

		if test "$ac_cv_lib_sqlite3_sqlite3_open" != "yes"; then
			AC_CHECK_LIB(
				sqlite, sqlite_open, [
					AC_DEFINE(
						[HAVE_LIBSQLITE], 1,
						[Define to 1 if you have the `sqlite' library (-lsqlite).]
					)
					LIBS="-lsqlite $LIBS"
				],[
					AC_MSG_ERROR([Error.. you must need sqlite3 or sqlite library!])
				]
			)
		fi

		LDFLAGS="$sqlite_OLD_LDFLAGS"

		ret_ldflags=0
		for check_ldflags in $LDFLAGS
		do
			test -z "$sqlite_prefix" && break
	
			if test "$check_ldflags" = "$sqlite_ldflags"; then
				ret_ldflags=1
				break
			fi
		done

		test $ret_ldflags -eq 0 && LDFLAGS="$sqlite_ldflags $LDFLAGS"

		#
		# sqlite header check
		#
		ret_cppflags=0
		for check_cppflags in $CPPFLAGS
		do
			test -z "$sqlite_prefix" && break

			if test "$check_cppflags" = "$sqlite_cppflags"; then
				ret_cppflags=1
				break
			fi
		done
		test $ret_cppflags -eq 0 && CPPFLAGS="$sqlite_cppflags $CPPFLAGS"
		AC_CHECK_HEADERS(sqlite3.h sqlite.h)
		if test "$ac_cv_header_sqlite3_h" != "yes" && test "$ac_cv_header_sqlite_h" != "yes"; then
			AC_MSG_ERROR([You must need sqlite3.h or sqlite.h header file!])
		fi
	fi
])
