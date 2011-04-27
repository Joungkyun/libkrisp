/*
 * $Id: sqlite.c,v 1.8 2010-06-24 17:24:34 oops Exp $
 *
 * libkrisp sqlite2 frontend API
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

KR_LOCAL_API void kr_dbError (KR_API *db) {
	memset (db->err, 0, 1024);
	if ( db->dberr != NULL )
		sprintf (db->err, "%s (Table: %s)", db->dberr, db->table);
		//strcpy (db->err, db->dberr);
	sqlite_freemem (db->dberr);
}

KR_LOCAL_API void kr_dbErrorClear (KR_API *db) {
	memset (db->err, 0, 1024);
}

KR_LOCAL_API int kr_dbFree (KR_API *db) {
	return 0;
}

KR_LOCAL_API bool kr_dbConnect (KR_API *db, char *file) {
	db->c = sqlite_open ((file != NULL) ? file : DBPATH, 0644, &db->dberr);

	if ( db->c == NULL ) {
		kr_dbError (db);
		return false;
	}

	return true;
}

KR_LOCAL_API int kr_dbQuery (KR_API *db, char * sql) {
	db->final = 0;
	db->rows  = 0;
	db->cols  = 0;
	db->r = sqlite_compile (db->c, sql, NULL, &db->vm, &db->dberr);

	if ( db->r != SQLITE_OK ) {
		kr_dbError (db);
		kr_dbFinalize (db);
		return 1;
	}

	return 0;
}

KR_LOCAL_API int kr_dbFetch (KR_API *db) {
	if ( db->final ) {
		db->final = 0;
		db->r = SQLITE_OK;
		goto finalize;
	}

	db->r = sqlite_step (db->vm, &db->cols, &db->rowdata, &db->colname);

	switch (db->r) {
		case SQLITE_ROW:
			db->rows++;
			break;
		case SQLITE_BUSY:
		case SQLITE_ERROR:
		case SQLITE_MISUSE:
		case SQLITE_DONE:
		default:
finalize:
			kr_dbFinalize (db);

			if ( db->r != SQLITE_OK ) {
				kr_dbError (db);
				return -1;
			}

			return 1;
	}

	return 0;
}

KR_LOCAL_API int kr_dbExecute (KR_API *db, char *sql) {
	short r;

	if ( kr_dbQuery (db, sql) )
		return 1;

	while ( ! (r = kr_dbFetch (db)) ) { } // empty actions
	if ( r == -1 )
		return 1;

	return 0;
}

KR_LOCAL_API void kr_dbFinalize (KR_API *db) {
	if ( db->vm )
		db->r = sqlite_finalize (db->vm, &db->dberr);

	db->vm = NULL;
}

KR_LOCAL_API void kr_dbClose (KR_API *db) {
	if ( db->c != NULL )
		sqlite_close (db->c);
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
