/*
 * $Id: sqlite3.c,v 1.6 2006-11-28 19:39:08 oops Exp $
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void kr_dbError (int code, const char *err) {
	memset (dberr, 0, 1024);
	if ( code )
		strcpy (dberr, err);
}

int kr_dbFree (KR_API *db) {
	int i;

	for ( i=0; i<db->cols; i++ ) {
		free ((void *) db->colname[i]);
		free ((void *) db->rowdata[i]);
	}

	free (db->colname);
	free (db->rowdata);

	return 0;
}

int kr_dbConnect (KR_API *db, char *file) {
	struct stat f;
	char _file[255];
	int l;

	if ( (db->r = sqlite3_open ((file != NULL) ? file : DBPATH, &db->c)) ) {
		kr_dbError (sqlite3_errcode(db->c), sqlite3_errmsg(db->c));
		return -1;
	}

	/* userdb database connect */
	if ( file != NULL ) {
		if ( (l = strlen (file) + 7) >= 255 )
			return 0;

		sprintf (_file, "%s-userdb", file);
		f.st_size = 0;
		if ( stat (_file, &f) == -1 )
			sprintf (_file, DBHPATH);
	} else
		strcpy (_file, DBHPATH);

	f.st_size = 0;
	if ( stat (_file, &f) == 0 )
		sqlite3_open (_file, &db->h);
	else
		db->h = NULL;

	return 0;
}

int kr_dbQuery (KR_API *db, char * sql, int t) {
	sqlite3 *c;

	switch (t) {
		case DBTYPE_USERDB :
			c = db->h;
			break;
		default:
			c = db->c;
	}

	db->r = sqlite3_prepare (c, sql, strlen (sql), &db->vm, NULL);

	if ( db->r != SQLITE_OK ) {
		kr_dbError (sqlite3_errcode(c), sqlite3_errmsg(c));
		return 1;
	}

	return 0;
}

int kr_dbFetch (KR_API *db, int t) {
	sqlite3 *c;
	int i;
	char colname[128] = { 0, };
	char rowdata[128] = { 0, };

	switch (t) {
		case DBTYPE_USERDB :
			c = db->h;
			break;
		default:
			c = db->c;
	}

	db->r = sqlite3_step (db->vm);

	switch (db->r) {
		case SQLITE_ROW:
			db->cols = sqlite3_column_count(db->vm);

			db->colname = (const char **) malloc (sizeof (char *) * db->cols);
			db->rowdata = (const char **) malloc (sizeof (char *) * db->cols);

			for ( i=0; i<db->cols; i++ ) {
				strcpy (colname, sqlite3_column_name (db->vm, i));
				strcpy (rowdata, sqlite3_column_text (db->vm, i));

				db->colname[i] = (char *) malloc (sizeof (char) * (strlen (colname) + 1));
				db->rowdata[i] = (char *) malloc (sizeof (char) * (strlen (rowdata) + 1));
				strcpy ((char *) db->colname[i], colname);
				strcpy ((char *) db->rowdata[i], rowdata);
			}
			db->rows++;
			break;
		case SQLITE_BUSY:
		case SQLITE_ERROR:
		case SQLITE_MISUSE:
		case SQLITE_DONE:
		default:
			if ( db->vm )
				db->r = sqlite3_finalize (db->vm);

			db->vm = NULL;

			if ( db->r != SQLITE_OK ) {
				kr_dbError (sqlite3_errcode(c), sqlite3_errmsg(c));
				return -1;
			}

			return 1;
	}

	return 0;
}

void kr_dbClose (KR_API *db) {
	if ( db->c != NULL )
		sqlite3_close (db->c);

	if ( db->h != NULL )
		sqlite3_close (db->h);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
