/*
 * $Id: krispapi.c,v 1.10 2010-06-24 16:52:38 oops Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <krispapi.h>

#ifdef HAVE_PTHREAD_H
pthread_mutex_t krisp_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void initStruct (KRNET_API * n) { // {{{
	memset (n->err, 0, 1);
	n->netmask = 0;
	n->start   = 0;
	n->end     = 0;
	strcpy (n->icode, "--");
	strcpy (n->iname, "N/A");
	strcpy (n->ccode, "--");
	strcpy (n->cname, "N/A");
} // }}}

void initRawStruct (RAW_KRNET_API * n, bool mfree) { // {{{
	memset (n->err, 0, 1);
	//memset (n->ip, 0, 1);
	n->start = 0;
	n->end   = 0;
	n->size  = 0;
	if ( mfree && n->dummydata != NULL )
		free (n->dummydata);
	n->dummydata = NULL;
	if ( mfree && n->dummy != NULL )
		free (n->dummy);
	n->dummy = NULL;
} // }}}

void kr_noneData (KRNET_API * n) { // {{{
	strcpy (n->icode, "--");
	strcpy (n->iname, "N/A");
	strcpy (n->ccode, "--");
	strcpy (n->cname, "N/A");
} // }}}

int getISPinfo (KR_API * db, RAW_KRNET_API * n) { // {{{
	ulong	longip;
	char	sql[128] = { 0, };
	int		r;

	longip = _kr_ip2long (n->ip);

	sprintf (
		sql,
		"SELECT * FROM %s WHERE start <= '%lu' ORDER By start DESC LIMIT 1",
		db->table,
		longip
	);
	if ( n->verbose )
		fprintf (stderr, "DEBUG: %s\n", sql);

	if  ( kr_dbQuery (db, sql) )
		return -1;

	while ( ! (r = kr_dbFetch (db) ) ) {
		for ( r=0; r<db->cols; r++ ) {
			switch (r) {
				case 0 :
					n->start = strtolong ((char *) db->rowdata[r]);
					break;
				case 1 :
					n->end = strtolong ((char *) db->rowdata[r]);
					if ( longip > n->end ) {
						kr_dbFinalize (db);
						kr_dbFree (db);
						return 1;
					}
					break;
				case 2 :
					n->dummydata = (char *) malloc (sizeof (char) * strlen (db->rowdata[r]) + 1);
					if ( n->dummydata != NULL )
						strcpy (n->dummydata, db->rowdata[r]);
					break;
			}
		}
		kr_dbFree (db);

		/* row is must none or 1 */
		/*
		if ( db->rows == 1 )
			db->final = 1;
		*/
	}

	if ( r == -1 ) // SQL Error
		return -1;
	else if ( ! db->rows ) // No result
		return 1;
	else if ( n->dummydata == NULL ) // No result
		return 1;

	return 0;
} // }}}

int parseDummyData (char *** d, char * s, char delemeter) { // {{{
	int		len = 0;
	int		i, rlen;
	char *	buf;

	if ( s == NULL ) {
		*d = NULL;
		return 0;
	}

	if ( delemeter == 0 )
		delemeter = '|';

	rlen = strlen (s);

	for ( i=0; i<rlen; i++ )
		if ( s[i] == delemeter )
			len++;

	if ( len == 0 ) {
		*d = (char **) malloc (sizeof (char *) * 2);
		if ( *d == NULL )
			return 0;

		(*d)[0] = s;
		(*d)[1] = NULL;
		return 1;
	}

	len++;
	rlen = len;
	*d = (char **) malloc (sizeof (char *) * len + 1);
	if ( *d == NULL ) {
		return 0;
	}
	(*d)[len] = NULL;

	while ( (buf = strrchr (s, delemeter)) != NULL ) {
		(*d)[--len] = buf + 1;
		*buf = 0;
	}
	(*d)[--len] = s;

	return rlen;
} // }}}

int chartoint (char c) { // {{{
	if (c > 47 && c < 58)
		return c - 48;

	return -1;
} // }}}

ulong strtolong (char * s) { // {{{
	int		len, i = 0, minus = 0, bufno = 0;
	ulong	x = 1, res = 0;

	/* removed blank charactor */
	len = strlen (s);

	/* minus value check */
	if ( s[0] == '-' ) minus = 1;

	for ( i = len - 1; i > -1; i-- ) {
		bufno = chartoint (s[i]);

		if ( bufno == 0 ) {
			x *= 10;
			continue;
		}

		if ( bufno > 0 ) {
			res += bufno * x;
			x *= 10;
		}
	}

	// buffer overflow
	if (minus) res *= -2;

	return res;
} // }}}

void _safecpy (char * stor, char * str, int size) { // {{{
	size--;
	if ( strlen (str) > size ) {
		memcpy (stor, str, size);
	} else {
		size = strlen (str);
		memcpy (stor, str, size);
	}
	stor[size] = 0;
} // }}}

void krisp_mutex_lock (KR_API * db) { // {{{
#ifdef HAVE_PTHREAD_H
	if ( ! db->threadsafe )
		return;

	if ( db->verbose )
		fprintf (stderr, "DEBUG: Thread Mutex is locked\n");
	pthread_mutex_lock (&krisp_mutex);
#endif

	return;
} // }}}

void krisp_mutex_unlock (KR_API * db) { // {{{
#ifdef HAVE_PTHREAD_H
	if ( ! db->threadsafe )
		return;

	pthread_mutex_unlock (&krisp_mutex);
	if ( db->verbose )
		fprintf (stderr, "DEBUG: Thread Mutex is unlocked\n");
#endif

	return;
} // }}}

void krisp_mutex_destroy (KR_API * db) { // {{{
#ifdef HAVE_PTHREAD_H
	if ( ! db->threadsafe )
		return;

	pthread_mutex_destroy (&krisp_mutex);
	if ( db->verbose )
		fprintf (stderr, "DEBUG: Thread Mutex destory\n");
#endif

	return;
} // }}}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
