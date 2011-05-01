## Process this file with automake to produce Makefile.in
# Copyright (C) 2011 JoungKyun.Kim <http://oops.org>
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

COMPAT_VERSION = @COMPAT_VERSION@

bin_SCRIPTS = krisp$(COMPAT_VERSION)-config
pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA = krisp$(COMPAT_VERSION).pc

if COMPAT
DISTCLEANFILES = *.pc *-config
krisp$(COMPAT_VERSION)-config:
	mv krisp-config krisp$(COMPAT_VERSION)-config

krisp$(COMPAT_VERSION).pc:
	mv krisp.pc krisp$(COMPAT_VERSION).pc
endif
