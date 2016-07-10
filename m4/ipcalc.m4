dnl Copyright (C) 2016 JoungKyun.Kim <http://oops.org>
dnl
dnl This file is part of libipcalc
dnl
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU Lesser General Public License as published
dnl by the Free Software Foundation, either version 3 of the License, or
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

AC_DEFUN([AX_IPCALC_WITH],
[
	if test "x$with_ipcalc" != "x"; then
		if test "x$with_ipcalc" = "xyes"; then
			ipcalcconfig="/usr/bin/ipcalc-config"
		else
			ipcalcconfig="$with_ipcalc"
		fi
	else
		ipcalcconfig="/usr/bin/ipcalc-config"
	fi

	AC_MSG_CHECKING(for ipcalc-config path)
	if test ! -f "$ipcalcconfig" || test ! -x "$ipcalcconfig"; then
		if test $ipcalc_check_failed -eq 1; then
			AC_MSG_ERROR(can't find $ipcalcconfig)
		else
			ipcalcconfig=
			AC_MSG_RESULT([not found])
		fi
	else
		AC_MSG_RESULT([$ipcalcconfig])
	fi
])

AC_DEFUN([AX_IPCALC_HEADER],
[
	#
	# ipcalc header check
	#
	IPCALC_INCLUDES="`$ipcalcconfig --includes 2> /dev/null`"
	if test -z "$IPCALC_INCLUDES"; then
		IPCALC_INCLUDES="`$ipcalcconfig --defs 2> /dev/null | sed 's/[[ ]]*-D[[^ ]]*//g'`"
	fi

	ret_cppflags=0
	for check_cppflags in $CPPFLAGS
	do
		test -z "$IPCALC_INCLUDES" && break

		if test "$check_cppflags" = "$IPCALC_INCLUDES"; then
			ret_cppflags=1
			break
		fi
	done
	test $ret_cppflags -eq 0 && CPPFLAGS="$IPCALC_INCLUDES $CPPFLAGS"
	AC_CHECK_HEADERS(ipcalc.h)
	if test "$ac_cv_header_ipcalc_h" != "yes"; then
		AC_MSG_ERROR([You must need ipcalc.h header file])
	fi
])

AC_DEFUN([AX_IPCALC_LD],
[
	#
	# ipcalc library check
	#
	IPCALC_LDFLAGS="`$ipcalcconfig --ldflags 2> /dev/null`"

	#
	# for compatibility
	#
	if test -z "$IPCALC_LDFLAGS"; then
		IPCALC_LDFLAGS="`$ipcalcconfig --libs 2> /dev/null | sed 's/[[[:space:]]]*-l.*//g'`"
	fi

	ipcalc_OLD_LDFLAGS="$LDFLAGS"
	LDFLAGS="$IPCALC_LDFLAGS"
	AC_CHECK_LIB(
		ipcalc, ip2long, [
			AC_DEFINE(
				[HAVE_LIBIPCALC], 1,
				[Define to 1 if you have the `ipcalc' library (-lipcalc).])
			LIBS="`$ipcalcconfig --libs 2> /dev/null` $LIBS"
		],[
			if test $ipcalc_check_failed -eq 1; then
				AC_MSG_ERROR([Can't not found `ipcalc' library (-lipcalc)])
			fi
		]
	)
	LDFLAGS="$ipcalc_OLD_LDFLAGS"

	ret_ldflags=0
	for check_ldflags in $LDFLAGS
	do
		test -z "$IPCALC_LDFLAGS" && break

		if test "$check_ldflags" = "$IPCALC_LDFLAGS"; then
			ret_ldflags=1
			break
		fi
	done

	test $ret_ldflags -eq 0 && LDFLAGS="$IPCALC_LDFLAGS $LDFLAGS"
])

AC_DEFUN([AX_IPCALC_LIBTOOL],
[
	#
	# ipcalc library check
	#
	AC_MSG_CHECKING(for ip2long in -lipcalc)

	IPCALC_LIBS="`$ipcalcconfig --link-libtool 2> /dev/null`"
	if test -z "$IPCALC_LIBS"; then
		IPCALC_LIBS="`$ipcalcconfig --libs 2> /dev/null`"
	fi

	if test "x$IPCALC_LIBS" = "x"; then
		AC_MSG_RESULT([no])
		if test $ipcalc_check_failed -eq 1; then
			AC_MSG_ERROR([Can't not found ipcalc libtool library (libipcalc.la)])
		fi
	else
		AC_MSG_RESULT([yes])
		LIBS="$IPCALC_LIBS $LIBS"
		AC_DEFINE(
			[HAVE_LIBIPCALC], 1,
			[Define to 1 if you have the `ipcalc' library (-lipcalc).])
	fi
])

dnl
dnl Usage AX_IPCALC
dnl     AX_IPCALC([library type],[stop on failed])
dnl         library type : ld or libtool
dnl         stop on failed : 1 => stop, 0 => continue
dnl
dnl     AX_IPCALC([library type])
dnl     AX_IPCALC(1)
dnl
dnl     no argument on AX_IPCALC
dnl         library type   = ld
dnl         stop on failed = 0
dnl
AC_DEFUN([AX_IPCALC],
[
	ax_ipcalc_argc=$#
	ipcalc_check_failed=0

	if test $ax_ipcalc_argc -gt 0; then
		ax_ipcalc_first_arg=$1
		ax_ipcalc_second_arg=$2
	fi

	if test $ax_ipcalc_argc -gt 0; then
		if test $ax_ipcalc_argc -eq 2; then
			if test "$ax_ipcalc_first_arg" = "libtool"; then
				ipcalc_ld_mode="libtool"
			elif test "$ax_ipcalc_first_arg" = "ld"; then
				ipcalc_ld_mode="ld"
			else
				AC_MSG_ERROR([Unsupported link type on AX IPCALC macro])
			fi

			if test $ax_ipcalc_second_arg -eq 1; then
				ipcalc_check_failed=1
			fi
		elif test $ax_ipcalc_argc -eq 1; then
			if test "$ax_ipcalc_first_arg" = "libtool"; then
				ipcalc_ld_mode="libtool"
			elif test "$ax_ipcalc_first_arg" = "ld"; then
				ipcalc_ld_mode="ld"
			elif test $ax_ipcalc_first_arg -eq 1 || test $ax_ipcalc_first_arg -eq 0 ; then
				ipcalc_check_failed=$ax_ipcalc_first_arg
			else
				AC_MSG_ERROR([Unsupported argument value on AX IPCALC macro])
			fi
		else
			AC_MSG_ERROR([So many arguments on AX IPCALC macro])
		fi
	fi

	AC_ARG_WITH(ipcalc,
		[  --with-ipcalc=PATH      path of ipcalc-config [[default: /usr/bin/ipcalc-config]]])

	if test "x$with_ipcalc" != "xno"; then
		AX_IPCALC_WITH

		if test "$ipcalc_ld_mode" = "libtool"; then
			AX_IPCALC_LIBTOOL
		else
			AX_IPCALC_LD
		fi
		AX_IPCALC_HEADER
	else
		if test $ipcalc_check_failed -eq 1; then
			AC_MSG_ERROR([--without-ipcalc doesn't permit])
		fi
	fi
])
