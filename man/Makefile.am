## Process this file with automake to produce Makefile.in
# Copyright (C) 2011 JoungKyun.Kim <http://oops.org>
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

# $Id: Makefile.am,v 1.6 2011-04-12 16:36:35 oops Exp $

if ! COMPAT
EXTRA_DIST = ko/kr_open.3 ko/kr_close.3 ko/kr_search_ex.3 ko/kr_search.3 \
			 ko/krisplookup.1 ko/krisp_version.3 ko/krispmkdb.1
dist_man_MANS = kr_open.3 kr_close.3 kr_search_ex.3 kr_search.3 \
				krisplookup.1 krisp_version.3 krispmkdb.1

ko_man1dir = $(mandir)/ko/man1
ko_man1_DATA = ko/krisplookup.1 ko/krispmkdb.1
ko_man3dir = $(mandir)/ko/man3
ko_man3_DATA = ko/kr_open.3 ko/kr_close.3 ko/kr_search_ex.3 \
			   ko/kr_search.3 ko/krisp_version.3
endif
