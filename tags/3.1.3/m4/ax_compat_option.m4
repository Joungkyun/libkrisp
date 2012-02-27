dnl 
dnl                       Written by JoungKyun.Kim
dnl            Copyright (c) 2011 JoungKyun.Kim <http://oops.org>
dnl 
dnl ----------------------------------------------------------------------------
dnl Redistribution and use in source and binary forms, with or without
dnl modification, are permitted provided that the following conditions are met:
dnl 
dnl     * Redistributions of source code must retain the above copyright notice,
dnl       this list of conditions and the following disclaimer.
dnl 
dnl     * Redistributions in binary form must reproduce the above copyright
dnl       notice, this list of conditions and the following disclaimer in the
dnl       documentation and/or other materials provided with the distribution.
dnl 
dnl     * Neither the name of JoungKyun.Kim nor the url of oops.org
dnl       nor the names of their contributors may be used to endorse or
dnl       promote products derived from this software without specific prior
dnl       written permission.
dnl 
dnl THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
dnl AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
dnl IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
dnl ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
dnl LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
dnl CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
dnl SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
dnl INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
dnl CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
dnl ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
dnl POSSIBILITY OF SUCH DAMAGE.
dnl ----------------------------------------------------------------------------
dnl  This file is part of olibc.
dnl
dnl  $Id$
dnl 

AC_SUBST(COMPAT_VERSION)

AC_DEFUN([AX_COMPAT_OPTION],
[
	if test -z "$PERL"; then
		AC_PATH_PROG(PERL, perl)
		if test ! -f "$ac_cv_path_PERL"; then
			AC_MSG_ERROR(Can't not found perl)
		fi
	fi

	AC_ARG_WITH(compat, [  --with-compat=VERSION   build compat library])
	if test "x$with_compat" != "x" && test "$with_compat" != "no" ; then
		 AC_MSG_CHECKING(for compat library version)
		 if test "$with_compat" = "yes"; then
			AC_MSG_ERROR(Don't specify compat library version)
		fi

		compatv=`echo $with_compat | $PERL -pe 's/[[0-9]]//g'`
		if test -n "$compatv"; then
			AC_MSG_ERROR(compat version is only numeric)
		fi

		AC_MSG_RESULT([$with_compat])
		COMPAT_VERSION=$with_compat
	fi
	AM_CONDITIONAL(COMPAT, test "x$with_compat" != "x" && test "$with_compat" != "no")
])
