dnl Copyright (C) 2011 JoungKyun.Kim <http://oops.org>
dnl
dnl This file is part of libkrisp
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
dnl $Id: Makefile.am,v 1.6 2011-04-12 16:36:35 oops Exp $

AC_SUBST(IPCALC_LDFLAGS)
AC_SUBST(IPCALC_CPPFLAGS)

AC_DEFUN([AX_IPCALC],
[
	AC_ARG_WITH(ipcalc,
		[  --with-ipcalc=PATH      path of ipcalc-config [[default: /usr/bin/ipcalc-config]]])

	if test "x$with_ipcalc" != "xno"; then
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
			AC_MSG_ERROR([can't find $ipcalcconfig])
		else
			AC_MSG_RESULT([$ipcalcconfig])
		fi

		#
		# ipcalc library check
		#
		IPCALC_LDFLAGS="`$ipcalcconfig --ldflags 2> /dev/null`"
		if test -z "$IPCALC_LDFLAGS"; then
			tmp_IPCALC_LDFALGS="`$ipcalcconfig --libs 2> /dev/null`"
			for tmp_ip_ld in $tmp_IPCALC_LDFALGS
			do
				echo "$tmp_ip_ld" | grep -- "-L" >& /dev/null
				test $? -ne 0 && continue
				IPCALC_LDFLAGS="$IPCALC_LDFLAGS $tmp_ip_ld"
			done
		fi

		ipcalc_OLD_FLAGS="$LDFLAGS"
		LDFLAGS="$IPCALC_LDFLAGS"
		AC_CHECK_LIB(
			ipcalc, ip2long, [
			AC_DEFINE(
				[HAVE_LIBIPCALC], 1,
				[Define to 1 if you have the `ipcalc' library (-lipcalc).]
			)
			LIBS="`$ipcalcconfig --libs` $LIBS"],[
			AC_MSG_ERROR([Can't not found ipcalc library (-lipcalc)])]
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

		#
		# ipcalc header check
		#
		IPCALC_INCLUDES="`$ipcalcconfig --includes 2> /dev/null`"
		if test -z "$IPCALC_INCLUDES"; then
			IPCALC_INCLUDES="`$ipcalcconfig --defs 2> /dev/null`"
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
	fi
])
