/*
 * Copyright (c) 2016 JoungKyun.Kim <http://oops.org> all rights reserved
 *
 * This file is part of libkrisp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as published
 * by the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU LESSER GENERAL PUBLIC LICENSE for more details.
 *
 * You should have received a copy of the GNU LESSER GENERAL PUBLIC LICENSE
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef KRDB_H
#define KRDB_H

#ifndef DBPATH
#include <krisp-config.h>
#endif

#define DEFAULT_DATABASE DBPATH "/krisp.dat"

void	kr_dbError (KR_API *);
void	kr_dbErrorClear (KR_API *);
int		kr_dbFree (KR_API *);
bool	kr_dbConnect (KR_API *);
int		kr_dbQuery (KR_API *, char *);
int		kr_dbFetch (KR_API *);
int		kr_dbExecute (KR_API *, char *);
void	kr_dbFinalize (KR_API *);
void	kr_dbClose (KR_API *);
char *	kr_dbQuote_f (char *);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
