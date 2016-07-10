## Process this file with automake to produce Makefile.in
# Copyright (C) 2016 JoungKyun.Kim <http://oops.org>
#
# This file is part of libkrisp
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

AM_CLFAGS = -Wall
AM_CPPFLAGS = -I$(top_srcdir)
LDADD = $(top_srcdir)/libkrisp@COMPAT_VERSION@.la

ctests = lookup
TESTS = $(ctests)
check_PROGRAMS = $(ctests)
TESTS_ENVIRONMENT = $(VALGRIND)

SAMPLESOURCES = krispexlookup krtest thread_sample thread_sample1 thread_sample2

#bin_PROGRAMS = krispexlookup krtest thread_sample \
#			   thread_sample1 thread_sample2
#krispexlookup_SOURCES = krispexlookup.c
#krtest_SOURCES = krtest.c
#thread_sample_SOURCES = thread_sample.c
#thread_sample1_SOURCES = thread_sample1.c
#thread_sample2_SOURCES = thread_sample2.c

sample:
	for bins in $(SAMPLESOURCES); \
	do \
		$(CC) -I.. -L../.libs -o $$bins $$bins.c -lkrisp; \
		LD_LIBRARY_PATH=../.libs ./$$bins 1; \
	done

CLEANFILES = $(SAMPLESOURCES)
