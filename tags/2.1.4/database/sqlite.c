/*
 * $Id: sqlite.c,v 1.5.2.2 2010-06-05 11:43:58 oops Exp $
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void kr_dbError (KR_API *db) {
	memset (db->err, 0, 1024);
	memset (dberr, 0, 1024);
	if ( db->error != NULL ) {
		strcpy (db->err, db->error);
		strcpy (dberr, db->err);
	}
	sqlite_freemem (db->error);
}

int kr_dbFree (KR_API *db) {
	return 0;
}

int kr_dbConnect (KR_API *db, char *file) {
	struct stat f;
	char _file[255];
	int l;

	db->c = sqlite_open ((file != NULL) ? file : DBPATH, 0644, &db->err);

	if ( db->c == NULL ) {
		kr_dbError (db);
		return -1;
	}

	/* userdb database connect */
	if ( file != NULL ) {
		if ( (l = strlen (file) + 7) >= 255 )
			return 0;

		sprintf (_file, "%s-userdb", file);
	} else
		strcpy (_file, DBHPATH);

	f.st_size = 0;
	if ( stat (_file, &f) == 0 )
		db->h = sqlite_open (_file, 0644, &db->err);
	else
		db->h = NULL;

	return 0;
}

int kr_dbQuery (KR_API *db, char * sql, int t) {
	sqlite *c;

	switch (t) {
		case DBTYPE_USERDB :
			c = db->h;
			break;
		default:
			 c = db->c;
	}

	db->r = sqlite_compile (c, sql, NULL, &db->vm, &db->err);

	if ( db->r != SQLITE_OK ) {
		kr_dbError (db);
		return 1;
	}

	return 0;
}

int kr_dbFetch (KR_API *db, int t) {
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
			if ( db->vm )
				db->r = sqlite_finalize (db->vm, &db->err);

			db->vm = NULL;

			if ( db->r != SQLITE_OK ) {
				kr_dbError (db);
				return -1;
			}

			return 1;
	}

	return 0;
}

void kr_dbClose (KR_API *db) {
	if ( db->c != NULL )
		sqlite_close (db->c);

	if ( db->h != NULL )
		sqlite_close (db->h);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
