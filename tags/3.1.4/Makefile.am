## Process this file with automake to produce Makefile.in
# Copyright (C) 2013 JoungKyun.Kim <http://oops.org>
#
# This file is part of libkrisp
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

DISTCLEANFILES =
ACLOCAL_AMFLAGS = -I m4

includedir = @includedir@/krisp@COMPAT_VERSION@

EXTRA_DIST = Migration THREADSAFE autogen libkrisp.spec \
			 contrib/database/README \
			 contrib/database/geo-krisp.php \
			 contrib/database/geocity.php \
			 contrib/database/geoip-geoisp.php \
			 contrib/database/hostip.php \
			 contrib/database/krisp-geoip.php \
			 contrib/database/krisp-geoisp.php \
			 contrib/database/vaccum.php \
			 contrib/krnic/README \
			 contrib/krnic/krisp-data.php \
			 contrib/krnic/krnic-data.php \
			 contrib/lib/ValidIP.php \
			 contrib/lib/fix-eng-gu.php \
			 database/sqlite.c \
			 database/sqlite3.c \
			 db/CurrentVersions \
			 db/README \
			 db/krisp.csv \
			 db/table \
			 debbuild-sh \
			 debian/changelog \
			 debian/compat \
			 debian/control \
			 debian/copyright \
			 debian/krisp.dirs \
			 debian/krisp.install \
			 debian/libkrisp-dev.dirs \
			 debian/libkrisp-dev.install \
			 debian/libkrisp2.dirs \
			 debian/libkrisp2.install \
			 debian/patches/01-remove-rpath.patch \
			 debian/rules

docdir = $(datadir)/doc/$(PACKAGE)@COMPAT_VERSION@-$(VERSION)
dist_doc_DATA = Changes README LICENSE

m4dir = $(datadir)/aclocal
dist_m4_DATA = m4/krisp@COMPAT_VERSION@.m4

if COMPAT
BUILT_SOURCE = m4/krisp@COMPAT_VERSION@.m4
DISTCLEANFILES += m4/krisp@COMPAT_VERSION@.m4

m4/krisp@COMPAT_VERSION@.m4:
	mv m4/krisp.m4 m4/krisp@COMPAT_VERSION@.m4
endif


SUBDIRS = . man scripts test utils

SONAME_MAJOR    = @SONAME_MAJOR@
SONAME_MINOR    = @SONAME_MINOR@
SONAME_REVISION = @SONAME_REVISION@
SONAME_INFO     = @SONAME_INFO@
SONAME_VERSION  = @SONAME_VERSION@

AM_CFLAGS = -Wall
AM_LDFLAGS = -version-info $(SONAME_INFO)

include_HEADERS = krisp.h krispcommon.h krversion.h
noinst_HEADERS = krdb.h krispapi.h

BUILT_SOURCES = krispcommon.h

lib_LTLIBRARIES = libkrisp@COMPAT_VERSION@.la
libkrisp@COMPAT_VERSION@_la_SOURCES = \
	krdb.c krisp.c krispapi.c

DISTCLEANFILES += *.bz2
dist-hook:
	$(PERL) -pi -e \
			"s/\@PACKAGE_VERSION\@/$(VERSION)/g" \
			$(distdir)/$(PACKAGE).spec
	$(PERL) -pi -e \
			"s/\@PACKAGE_DATE\@/$(PACKAGE_DATE)/g" \
			$(distdir)/$(PACKAGE).spec

test-install:
	$(MAKE) DESTDIR=`pwd`/z install
