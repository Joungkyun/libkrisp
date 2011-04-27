/*
 * $Id: sqlite3.c,v 1.2 2006-06-10 19:41:56 oops Exp $
 */

void kr_dbError (int code, const char *err) {
	memset (dberr, 0, 1024);
	if ( code )
		strcpy (dberr, err);
}

int kr_dbFree (struct db_argument *db) {
	int i;

	for ( i=0; i<db->cols; i++ ) {
		free ((void *) db->colname[i]);
		free ((void *) db->rowdata[i]);
	}

	free (db->colname);
	free (db->rowdata);

	return 0;
}

int kr_dbConnect (struct db_argument *db, char *file) {
	if ( (db->r = sqlite3_open ((file != NULL) ? file : DBPATH, &db->c)) ) {
		kr_dbError (sqlite3_errcode(db->c), sqlite3_errmsg(db->c));
		return -1;
	}

	return 0;
}

int kr_dbQuery (struct db_argument *db, char * sql) {
	db->r = sqlite3_prepare (db->c, sql, strlen (sql), &db->vm, NULL);

	if ( db->r != SQLITE_OK ) {
		kr_dbError (sqlite3_errcode(db->c), sqlite3_errmsg(db->c));
		return 1;
	}

	return 0;
}

int kr_dbFetch (struct db_argument *db) {
	int i;
	char colname[128] = { 0, };
	char rowdata[128] = { 0, };

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
				kr_dbError (sqlite3_errcode(db->c), sqlite3_errmsg(db->c));
				return -1;
			}

			return 1;
	}

	return 0;
}

void kr_dbClose (struct db_argument *db) {
	if ( db->c != NULL )
		sqlite3_close (db->c);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
