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
dnl  $Id: ax_static_bindir.m4,v 1.1 2011-04-12 17:00:15 oops Exp $
dnl 

AC_SUBST(static_bindir)

AC_DEFUN([AX_STATIC_BINDIR],
[
	if test -n "`echo $bindir | grep exec_prefix`"; then
		if test "$exec_prefix" = "$prefix"; then
			if test "$prefix" = "NONE"; then
				static_bindir="$ac_default_prefix/bin"
			else
				static_bindir="$prefix/bin"
			fi
		else
			if test "$exec_prefix" = "NONE"; then
				if test "$prefix" = "NONE"; then
					static_bindir="$ac_default_prefix/bin"
				else
					static_bindir="$prefix/bin"
				fi
			else
				static_bindir="$exec_prefix/bin"
			fi
		fi
	else
		static_bindir="$bindir"
	fi
])
