/*
 * libkrisp sqlite3 frontend API
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define L strlen

void kr_dbError (KR_API *db) {
	if ( ! sqlite3_errcode (db->c) )
		return;

	memset (db->err, 0, 1024);
	sprintf (db->err, "%s (Table: %s)", sqlite3_errmsg (db->c), db->table);
}

void kr_dbErrorClear (KR_API *db) {
	memset (db->err, 0, 1024);
}

int kr_dbFree (KR_API *db) {
	int i;

	if ( db->verbose )
		fprintf (stderr, "DEBUG: **** db colname/rowdata free\n");

	for ( i=0; i<db->cols; i++ ) {
		free ((void *) db->colname[i]);
		free ((void *) db->rowdata[i]);
	}

	free (db->colname);
	free (db->rowdata);

	return 0;
}

bool kr_dbConnect (KR_API *db) {
	char * errmsg;

#if SQLITE_VERSION_NUMBER >= 3005000
	if ( sqlite3_libversion_number () >= 3005000 ) {
		db->r = sqlite3_open_v2 (
			db->database,
			&db->c,
			(db->threadsafe && sqlite3_threadsafe ()) ?
				SQLITE_OPEN_READONLY|SQLITE_OPEN_FULLMUTEX :
				SQLITE_OPEN_READONLY,
			NULL
		);
	} else
#endif
		db->r = sqlite3_open (db->database, &db->c);

	if ( db->r ) {
		kr_dbError (db);
		return false;
	}

	sqlite3_busy_timeout (db->c, 500);
	sqlite3_exec (
		db->c,
		"PRAGMA synchronous=OFF; PRAGMA count_changes=OFF; PRAGMA temp_store=memory; journal_mode=OFF",
		NULL, NULL, &errmsg
	);
	sqlite3_free (errmsg);

	return true;
}

int kr_dbQuery (KR_API *db, char * sql) {
	db->final = 0;
	db->rows  = 0;
	db->cols  = 0;

	if ( db->verbose )
		fprintf (stderr, "DEBUG: **** db prepare\n");

	db->r = sqlite3_prepare (db->c, sql, L (sql), &db->vm, NULL);

	if ( db->r != SQLITE_OK ) {
		kr_dbError (db);
		if ( db->verbose )
			fprintf (stderr, "DEBUG: **** db prepare result (%d) : %s\n", db->r, db->err);
		kr_dbFinalize (db);
		return 1;
	}

	return 0;
}

int kr_dbFetch (KR_API *db) {
	int i;
	char *colname;
	char *rowdata;

	if ( db->verbose )
		fprintf (stderr, "DEBUG: **** db fetch\n");

	if ( db->final ) {
		db->final = 0;
		db->r = SQLITE_OK;
		goto finalize;
	}

	db->r = sqlite3_step (db->vm);

	switch (db->r) {
		case SQLITE_ROW: // 100 - sqlite3_step() has another row ready
			db->cols = sqlite3_column_count(db->vm);

			db->colname = (const char **) malloc (sizeof (char *) * db->cols);
			db->rowdata = (const char **) malloc (sizeof (char *) * db->cols);

			for ( i=0; i<db->cols; i++ ) {
				colname = (char *) sqlite3_column_name (db->vm, i);
				rowdata = (char *) sqlite3_column_text (db->vm, i);

				db->colname[i] = (char *) malloc (sizeof (char) * (L (colname) + 1));
				db->rowdata[i] = (char *) malloc (sizeof (char) * (L (rowdata) + 1));
				strcpy ((char *) db->colname[i], colname);
				strcpy ((char *) db->rowdata[i], rowdata);
			}
			db->rows++;
			break;
		case SQLITE_BUSY:   // 5   - The database file is locked
		case SQLITE_ERROR:  // 1   - SQL error or missing database
		case SQLITE_MISUSE: // 21  - Library used incorrectly
		case SQLITE_DONE:   // 101 - sqlite3_step() has finished executing
		default:
finalize:
			if ( db->verbose )
				fprintf (stderr, "DEBUG: **** db fetch step result : %d\n", db->r);

			kr_dbFinalize (db);
			if ( db->r != SQLITE_OK ) {
				kr_dbError (db);
				return -1;
			}

			return 1;
	}

	return 0;
}

int kr_dbExecute (KR_API *db, char *sql) {
	short r;

	if ( kr_dbQuery (db, sql) )
		return 1;

	while ( ! (r = kr_dbFetch (db)) ) { } // empty actions
	if ( r == -1 )
		return 1;

	return 0;
}

void kr_dbFinalize (KR_API *db) {
	if ( db->verbose )
		fprintf (stderr, "DEBUG: **** db finalize\n");
	if ( db->vm ) {
		if ( db->verbose )
			fprintf (stderr, "DEBUG: **** db finalize act\n");
		db->r = sqlite3_finalize (db->vm);
	}

	db->vm = NULL;
}

void kr_dbClose (KR_API *db) {
	if ( db->verbose )
		fprintf (stderr, "DEBUG: **** db close\n");

	if ( db->c != NULL )
		sqlite3_close (db->c);
	memset (db->err, 0, 1024);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
