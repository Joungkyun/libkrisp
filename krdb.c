/*
 * $Id: krdb.c,v 1.16 2010-06-24 17:24:34 oops Exp $
 */

#include <krispapi.h>

#if defined(HAVE_LIBSQLITE3)
#include <sqlite3.h>
#else
#include <sqlite.h>
#endif

#include <krdb.h>

#if defined(HAVE_LIBSQLITE3)
#include <database/sqlite3.c>
#else
#include <database/sqlite.c>
#endif

char * kr_dbQuote_f (char * v) { // {{{
	int		no = 0;
	size_t	t, l, start;
	size_t	len;
	char *	buf;

	if ( v == NULL )
		return "";

	len = strlen (v);
	for ( t=0; t<len; t++ ) {
		if ( v[t] == 0x27 )
			no++;
	}

	t = len + no + 1;
	buf = (char *) malloc (sizeof (char) * t);
	memset (buf, 0, t);

	l = 0;
	start = 0;
	for ( t=0; t<len; t++ ) {
		if ( v[t] == '\'' ) {
			memcpy (buf + start, v + l, t - l);
			start += t - l;
			memset (buf + start++, '\'', 1);
			memset (buf + start++, '\'', 1);
			l = t + 1;
		}
	}
	memcpy (buf + start, v + l, strlen (v + l));

	return buf;
} // }}}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
