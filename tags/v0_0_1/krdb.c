/*
 * $Id: krdb.c,v 1.1.1.1 2006-06-08 19:31:52 oops Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#if defined(HAVE_LIBSQLITE3)
#include <sqlite3.h>
#else
#include <sqlite.h>
#endif

#include <krispcommon.h>
#include <krdb.h>

char dberr[1024];

#if defined(HAVE_LIBSQLITE3)
#include <database/sqlite3.c>
#else
#include <database/sqlite.c>
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
