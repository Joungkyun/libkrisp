/*
 * $Id: krisp.c,v 1.76 2010-06-15 16:55:30 oops Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <krisp.h>

char * krisp_version (void) { // {{{
	return KRISP_VERSION;
} // }}}

char * krisp_uversion (void) { // {{{
	return KRISP_UVERSION;
} // }}}

int kr_open_safe (KR_API **db, char *file) { // {{{
	int r;

	r = kr_open (db, file);
	if ( r > 0 )
		return r;

#ifdef HAVE_PTHREAD_H
	(*db)->threadsafe = 1;
#endif

	return 0;
} // }}}

int kr_open (KR_API **db, char *file) { // {{{
	struct stat		f;
	char * data;

	*db = (KR_API *) malloc (sizeof (KR_API));
	if ( *db == NULL )
		return 2;

	data = (file == NULL) ? DBPATH : file;

	f.st_size = 0;
	if ( stat (data, &f) == -1 ) {
		sprintf ((*db)->err, "kr_open: Can't find data data (%s)", data);
		(*db)->c = NULL;
		return 1;
	}

	if ( f.st_size < 1 ) {
		sprintf ((*db)->err, "kr_open: %s size is zero", data);
		(*db)->c = NULL;
		return 1;
	}

	(*db)->verbose = 0;
#ifdef HAVE_PTHREAD_H
	(*db)->threadsafe = 0;
#endif

	if ( kr_dbConnect (*db, data) ) {
		(*db)->c = NULL;
		return 1;
	}

	return 0;
} // }}}

void kr_close (KR_API *db) { // {{{
	krisp_mutex_destroy (db);
	kr_dbClose (db);
	if ( db != NULL )
		free (db);
} // }}}

/*
 * if return 1, db error
 */
int kr_search (KRNET_API *isp, KR_API *db) { // {{{
	RAW_KRNET_API	raw;
	int				r;

	krisp_mutex_lock (db);
	db->table = "krisp";

	memset (raw.ip, 0, 1);
	initRawStruct (&raw, false);
	initStruct (isp);
	raw.verbose = isp->verbose;

	if ( valid_address (isp->ip) ) {
		initRawStruct (&raw, false);
		krisp_mutex_unlock (db);
		return 0;
	}

	strcpy (raw.ip, isp->ip);
	if ( (r = getISPinfo (db, &raw)) != 0 ) {
		initRawStruct (&raw, true);

		// SQL error
		if ( r == -1 ) {
			krisp_mutex_unlock (db);
			return 1;
		}

		goto jumpNet;
	}

	isp->start = raw.start;
	isp->end   = raw.end;
	isp->netmask = guess_netmask (raw.start, raw.end);

	if ( isp->verbose ) {
		fprintf (stderr, "DEBUG: IP    => %-15s (%lu)\n", isp->ip, ip2long (isp->ip));
		fprintf (stderr, "DEBUG: START => %-15s (%lu)\n", long2ip (isp->start), isp->start);
		fprintf (stderr, "DEBUG: END   => %-15s (%lu)\n", long2ip (isp->end), isp->end);
		fprintf (stderr, "DEBUG: MASK  => %-15s (%lu)\n", long2ip (isp->netmask), isp->netmask);
	}

jumpNet:

	/*
	 * Parsing data
	 */
	raw.size = parseDummyData (&raw.dummy, raw.dummydata, 0);

	if ( raw.size < 3 )
		goto goWrongData;

	// ISPCODE|ISPNAME
	strcpy (isp->ccode, (strlen (raw.dummy[0]) == 0) ? "--" : raw.dummy[0]);
	strcpy (isp->cname, (strlen (raw.dummy[1]) == 0) ? "N/A" : raw.dummy[1]);
	strcpy (isp->iname, (strlen (raw.dummy[3]) == 0) ? "N/A" : raw.dummy[3]);
	if ( strlen (raw.dummy[2]) )
		strcpy (isp->icode, raw.dummy[2]);
	else {
		if ( strcmp (isp->iname, "N/A") == 0 )
			strcpy (isp->icode, "--");
		else
			strcpy (isp->icode, isp->iname);
	}

goWrongData:

	if ( isp->verbose ) {
		fprintf (stderr, "DEBUG: TABLE  => %s\n", db->table);
		fprintf (stderr, "DEBUG: DATA   => %s\n", raw.dummydata);
		fprintf (stderr, "DEBUG: ISP    => %s (%s)\n", isp->iname, isp->icode);
		fprintf (stderr, "DEBUG: NATION => %s (%s)\n", isp->cname, isp->ccode);
	}

	// second argues is set ture, free dummy member
	initRawStruct (&raw, true);

	krisp_mutex_unlock (db);

	return 0;
} // }}}

/*
 * if return 1, db error
 */
int kr_search_ex (KRNET_API_EX *raw, KR_API *db) { // {{{
	int	r;

	initRawStruct (raw, false);

	if ( valid_address (raw->ip) ) {
		initRawStruct (raw, false);
		db->table = "krisp";
		return 0;
	}

	if ( (r = getISPinfo (db, raw)) != 0 ) {
		initRawStruct (raw, true);

		// SQL error
		if ( r == -1 ) {
			db->table = "krisp";
			return 1;
		}
	}

	if ( raw->verbose ) {
		ulong netmask = guess_netmask (raw->start, raw->end);
		fprintf (stderr, "DEBUG: IP    => %-15s (%lu)\n", raw->ip, ip2long (raw->ip));
		fprintf (stderr, "DEBUG: START => %-15s (%lu)\n", long2ip (raw->start), raw->start);
		fprintf (stderr, "DEBUG: END   => %-15s (%lu)\n", long2ip (raw->end), raw->end);
		fprintf (stderr, "DEBUG: MASK  => %-15s (%lu)\n", long2ip (netmask), netmask);
	}

	/*
	 * Parsing data
	 */
	raw->size = parseDummyData (&(raw->dummy), raw->dummydata, 0);

	if ( raw->verbose ) {
		fprintf (stderr, "DEBUG: TABLE  => %s\n", db->table);
		fprintf (stderr, "DEBUG: DATA   => %s\n", raw->dummydata);
	}

	db->table = "krisp";
	return 0;
} // }}}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
