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
#include <sys/stat.h>

#include <krispapi.h>
#include <krversion.h>

KRISP_API char * krisp_version (void) { // {{{
	return KRISP_VERSION;
} // }}}

KRISP_API char * krisp_uversion (void) { // {{{
	return KRISP_UVERSION;
} // }}}

KRISP_API void kr_close (KR_API **db) { // {{{
	if ( *db == NULL )
		return;

	krisp_mutex_destroy (*db);

	if ( (*db)->c != NULL )
		kr_dbClose (*db);
	free (*db);
	*db = NULL;
} // }}}

bool _kr_open (KR_API **db, char *file, char *err, bool safe) { // {{{
	struct stat		f;

	memset (err, 0, 1);

	*db = (KR_API *) malloc (sizeof (KR_API));
	if ( *db == NULL ) {
		SAFECPY_1024 (err, "kr_open:: failed memory allocation");
		return false;
	}

	memset ((*db)->err, 0, 1024);
	memset ((*db)->database, 0, 256);

	(*db)->c = NULL;
	(*db)->vm = NULL;

	(*db)->rowdata = NULL;
	(*db)->colname = NULL;
	(*db)->table = NULL;
	(*db)->old_table = NULL;

#ifdef HAVE_LIBSQLITE
	(*db)->dberr = NULL;
#endif

	(*db)->rows = 0;
	(*db)->cols = 0;

	(*db)->verbose = false;
#ifdef HAVE_LIBPTHREAD
	(*db)->threadsafe = safe;
	if ( (*db)->threadsafe == true )
		pthread_mutex_init (&((*db)->mutex), NULL);
#endif

	SAFECPY_256 ((*db)->database, (file == NULL ) ? DEFAULT_DATABASE : file);

	f.st_size = 0;
	if ( stat ((*db)->database, &f) == -1 ) {
#ifdef _WIN32
		char * WinDIR = getenv ("windir");
		char originalData[256] = { 0, };
		int WinCnt;

		if ( WinDIR == NULL )
			WinDIR = "C:\\Windows";

		char * WinDefData[2] = { "", "\\System32" };
		SAFECPY_256(originalData, (*db)->database);
		for ( WinCnt=0; WinCnt<2; WinCnt++ ) {
			memset ((*db)->database, 0, 256);
			sprintf ((*db)->database, "%s%s\\krisp.dat", WinDIR, WinDefData[WinCnt]);

			if ( stat ((*db)->database, &f) != -1 )
				goto endWinDataCheck;
		}

		SAFECPY_256((*db)->database, originalData);
#endif
		sprintf (err, "kr_open:: Can't find database (%s)", (*db)->database);
		(*db)->c = NULL;
		kr_close (db);
		return false;
	}

#ifdef _WIN32
endWinDataCheck:
#endif

	if ( f.st_size < 1 ) {
		sprintf (err, "kr_open:: %s size is zero", (*db)->database);
		kr_close (db);
		return false;
	}

	(*db)->db_time_stamp_interval = 0;
	(*db)->db_time_stamp = f.st_mtime;
	(*db)->db_stamp_checked = time (NULL);

	if ( kr_dbConnect (*db) == false ) {
		SAFECPY_1024 (err, (*db)->err);
		kr_close (db);
		return false;
	}

	return true;
} // }}}

KRISP_API bool kr_open_safe (KR_API **db, char *file, char *err) { // {{{
	return _kr_open (db, file, err, true);
} // }}}

KRISP_API bool kr_open (KR_API **db, char *file, char *err) { // {{{
	return _kr_open (db, file, err, false);
} // }}}

/*
 * if return 1, db error
 */
KRISP_API int kr_search (KRNET_API *isp, KR_API *db) { // {{{
	RAW_KRNET_API	raw;
	int				r;
	char			err[1024];

	if ( db == NULL ) {
		SAFECPY_1024 (isp->err, "kr_search:: KR_API *db is null");
		return 1;
	}

	if ( isp->verbose != 0 && isp->verbose != 1 )
		isp->verbose = 0;
	db->verbose = isp->verbose;
	raw.verbose = isp->verbose;

	krisp_mutex_lock (db);

	// check database mtime
	if ( isp->verbose )
		fprintf (stderr, "DEBUG: Check changed %s\n", db->database);

	if ( check_database_mtime (db) == true ) {
		kr_dbClose (db);

		if ( isp->verbose )
			fprintf (stderr, "DEBUG: *** db reconnect\n");

		if ( kr_dbConnect (db) == false ) {
			SAFECPY_1024 (isp->err, db->err);
			return 1;
		}
	}

	db->table = "krisp";

	memset (raw.ip, 0, 1);
	initRawStruct (&raw, false);
	initStruct (isp);

	if ( valid_ip_address (isp->ip, err) ) {
		SAFECPY_1024 (isp->err, err);
		krisp_mutex_unlock (db);
		return 1;
	}

	strcpy (raw.ip, isp->ip);
	if ( (r = getISPinfo (db, &raw)) != 0 ) {
		// SQL error
		if ( r == -1 ) {
			SAFECPY_1024 (isp->err, db->err);
			krisp_mutex_unlock (db);
			return 1;
		}

		goto jumpNet;
	}

	isp->start = raw.start;
	isp->end   = raw.end;
	isp->netmask = guess_netmask (raw.start, raw.end);

	if ( isp->verbose ) {
		char sip[16] = { 0, };
		fprintf (stderr, "DEBUG: IP    => %-15s (%lu)\n", isp->ip, ip2long (isp->ip));
		fprintf (stderr, "DEBUG: START => %-15s (%lu)\n", long2ip_r (isp->start, sip), isp->start);
		fprintf (stderr, "DEBUG: END   => %-15s (%lu)\n", long2ip_r (isp->end, sip), isp->end);
		fprintf (stderr, "DEBUG: MASK  => %-15s (%lu)\n", long2ip_r (isp->netmask, sip), isp->netmask);
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
KRISP_API int kr_search_ex (KRNET_API_EX *raw, KR_API *db) { // {{{
	int		r;
	char	err[1024];

	if ( db == NULL ) {
		SAFECPY_1024 (raw->err, "kr_search:: KR_API *db is null");
		return 1;
	}

	if ( raw->verbose != 0 && raw->verbose != 1 )
		raw->verbose = 0;
	db->verbose = raw->verbose;

	krisp_mutex_lock (db);

	// check database mtime
	if ( raw->verbose )
		fprintf (stderr, "DEBUG: Check changed %s\n", db->database);

	if ( check_database_mtime (db) == true ) {
		kr_dbClose (db);

		if ( raw->verbose )
			fprintf (stderr, "DEBUG: ** db reconnec\n");

		if ( kr_dbConnect (db) == false ) {
			SAFECPY_1024 (raw->err, db->err);
			return 1;
		}
	}

	initRawStruct (raw, false);

	if ( valid_ip_address (raw->ip, err) ) {
		SAFECPY_1024 (raw->err, err);
		db->table = "krisp";
		krisp_mutex_unlock (db);
		return 0;
	}

	if ( (r = getISPinfo (db, raw)) != 0 ) {
		// SQL error
		if ( r == -1 ) {
			SAFECPY_1024 (raw->err, db->err);
			db->table = "krisp";
			krisp_mutex_unlock (db);
			return 1;
		}
	}

	if ( raw->verbose ) {
		ulong netmask = guess_netmask (raw->start, raw->end);
		char sip[16] = { 0, };
		fprintf (stderr, "DEBUG: IP    => %-15s (%lu)\n", raw->ip, ip2long (raw->ip));
		fprintf (stderr, "DEBUG: START => %-15s (%lu)\n", long2ip_r (raw->start, sip), raw->start);
		fprintf (stderr, "DEBUG: END   => %-15s (%lu)\n", long2ip_r (raw->end, sip), raw->end);
		fprintf (stderr, "DEBUG: MASK  => %-15s (%lu)\n", long2ip_r (netmask, sip), netmask);
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
	krisp_mutex_unlock (db);
	return 0;
} // }}}

/*
 * if return 1, db error
 */
KRISP_API int kr_range (KRNET_REQ_RANGE * range, KR_API * db) { // {{{
	int				r;

	if ( db == NULL ) {
		SAFECPY_1024 (range->err, "kr_range:: KR_API *db is null");
		return 1;
	}

	if ( range->verbose != 0 && range->verbose != 1 )
		range->verbose = 0;
	db->verbose = range->verbose;

	krisp_mutex_lock (db);

	// check database mtime
	if ( range->verbose )
		fprintf (stderr, "DEBUG: Check changed %s\n", db->database);

	if ( check_database_mtime (db) == true ) {
		kr_dbClose (db);

		if ( range->verbose )
			fprintf (stderr, "DEBUG: *** db reconnect\n");

		if ( kr_dbConnect (db) == false ) {
			SAFECPY_1024 (range->err, db->err);
			return 1;
		}
	}

	db->table = "krisp";

	// Error process
	if ( (r = getRange (db, range)) ) {
		if ( r == -1 ) {
			SAFECPY_1024 (range->err, db->err);
			krisp_mutex_unlock (db);
			return 1;
		}
	}

	krisp_mutex_unlock (db);

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
