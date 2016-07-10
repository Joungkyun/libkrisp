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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <krispapi.h>

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

KRISP_API void krisp_safecpy (char * stor, char * str, int size) { // {{{
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
#ifdef HAVE_LIBPTHREAD
	if ( ! db->threadsafe )
		return;

	if ( db->verbose )
		fprintf (stderr, "DEBUG: Thread Mutex is locked\n");
	pthread_mutex_lock (&(db->mutex));
#endif

	return;
} // }}}

void krisp_mutex_unlock (KR_API * db) { // {{{
#ifdef HAVE_LIBPTHREAD
	if ( ! db->threadsafe )
		return;

	pthread_mutex_unlock (&(db->mutex));
	if ( db->verbose )
		fprintf (stderr, "DEBUG: Thread Mutex is unlocked\n");
#endif

	return;
} // }}}

void krisp_mutex_destroy (KR_API * db) { // {{{
#ifdef HAVE_LIBPTHREAD
	if ( ! db->threadsafe )
		return;

	pthread_mutex_destroy (&(db->mutex));
	if ( db->verbose )
		fprintf (stderr, "DEBUG: Thread Mutex destory\n");
#endif

	return;
} // }}}

bool check_database_mtime (KR_API *db) { // {{{
	struct stat	f;
	time_t		current;

	if ( db->db_time_stamp_interval < 1 )
		return false;

	current = time (NULL);
	if ( (current - db->db_stamp_checked) < db->db_time_stamp_interval )
		return false;

	if ( stat (db->database, &f) == -1 )
		return false;

	if ( db->db_time_stamp == f.st_mtime )
		return false;

	db->db_time_stamp = f.st_mtime;
	db->db_stamp_checked = current;

	return true;
} // }}}

KRISP_API void initStruct (KRNET_API * n) { // {{{
	memset (n->err, 0, 1);
	n->netmask = 0;
	n->start   = 0;
	n->end     = 0;
	strcpy (n->icode, "--");
	strcpy (n->iname, "N/A");
	strcpy (n->ccode, "--");
	strcpy (n->cname, "N/A");
} // }}}

KRISP_API void initRawStruct (RAW_KRNET_API * n, bool mfree) { // {{{
	memset (n->err, 0, 1);
	//memset (n->ip, 0, 1);
	n->start = 0;
	n->end   = 0;
	n->size  = 0;
	if ( mfree && n->dummy != NULL )
		free (n->dummy);
	n->dummy = NULL;
	if ( mfree && n->dummydata != NULL )
		free (n->dummydata);
	n->dummydata = NULL;
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

	longip = ip2long (n->ip);

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
					n->dummydata = (char *) malloc (sizeof (char) * (strlen (db->rowdata[r]) + 1));
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
	kr_dbFinalize (db);

	if ( r == -1 ) // SQL Error
		return -1;
	else if ( ! db->rows ) // No result
		return 1;
	else if ( n->dummydata == NULL ) // No result
		return 1;

	return 0;
} // }}}

int getRange (KR_API * db, KRNET_REQ_RANGE * n) { // {{{
	char sql[128] = { 0, };
	int r, i;
	char * search;

	r = strlen (n->data);
	search = (char *) malloc (sizeof (char) * (r + 3));
	r = 0;

	if ( n->code == KRISP_GET_COUNTRY )
		sprintf (search, "%s|%%", n->data);
	else
		sprintf (search, "%%|%s", n->data);

	// get row numbers;
	sprintf (
		sql,
		"SELECT count(*) FROM %s WHERE data LIKE '%s'",
		db->table, search
	);
	if ( n->verbose )
		fprintf (stderr, "DEBUG: %s\n", sql);

	if ( kr_dbQuery (db, sql) )
		return -1;

	memset (sql, 0, 128);

	if ( ! kr_dbFetch (db) )
		n->count = strtolong ((char *) db->rowdata[0]);
	kr_dbFree (db);
	kr_dbFinalize (db);

	// no result
	if ( n->count == 0 )
		return 1;

	sprintf (
		sql,
		"SELECT start, end FROM %s WHERE data LIKE '%s'",
		db->table, search
	);
	if ( n->verbose )
		fprintf (stderr, "DEBUG: %s\n", sql);

	SAFEFREE (search);

	if ( kr_dbQuery (db, sql) )
		return -1;

	n->ranges = (KRNET_RANGE *) malloc (sizeof (KRNET_RANGE) * n->count);

	i = 0;
	while ( ! kr_dbFetch (db) ) {
		for ( r=0; r<2; r++ ) {
			switch (r) {
				case 0 :
					n->ranges[i].start = strtolong ((char *) db->rowdata[r]);
					break;
				case 1 :
					n->ranges[i].end = strtolong ((char *) db->rowdata[r]);
					break;
			}
		}
		i++;
		kr_dbFree (db);
	}
	kr_dbFinalize (db);

	return 0;
} // }}}

short parseDummyData (char *** d, char * s, char delemeter) { // {{{
	char *	buf;
	int		rlen;
	short	len = 0, i;

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
	*d = (char **) malloc (sizeof (char *) * (len + 1));
	if ( *d == NULL ) {
		return 0;
	}
	(*d)[len] = NULL;

	while ( (buf = strrchr (s, delemeter)) != NULL ) {
		(*d)[--len] = buf + 1;
		*buf = 0;
	}
	(*d)[--len] = s;

	return (short) rlen;
} // }}}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
