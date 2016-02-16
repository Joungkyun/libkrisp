## Process this file with automake to produce Makefile.in
# Copyright (C) 2013 JoungKyun.Kim <http://oops.org>
#
# This file is part of libipcalc
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# $Id$

AM_CFLAGS = -Wall
AM_CPPFLAGS = -I$(top_srcdir)

bin_PROGRAMS = krisplookup@COMPAT_VERSION@
krisplookup@COMPAT_VERSION@_SOURCES = krisplookup.c
krisplookup@COMPAT_VERSION@_LDADD = ../libkrisp@COMPAT_VERSION@.la $(LIBICONV)

bin_PROGRAMS += krispmkdb@COMPAT_VERSION@
krispmkdb@COMPAT_VERSION@_SOURCES = krispmkdb.c
krispmkdb@COMPAT_VERSION@_LDADD = ../krdb.o ../krispapi.o ../krisp.o
