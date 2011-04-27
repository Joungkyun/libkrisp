/*
 * $Id: sqlite.c,v 1.1.1.1 2006-06-08 19:31:53 oops Exp $
 */

void kr_dbError (struct db_argument *db) {
	memset (dberr, 0, 1024);
	if ( db->err != NULL )
		strcpy (dberr, db->err);
	sqlite_freemem (db->err);
}

int kr_dbFree (struct db_argument *db) {
	return 0;
}

int kr_dbConnect (struct db_argument *db) {
	db->c = sqlite_open (DBPATH, 0644, &db->err);

	if ( db->c == NULL ) {
		kr_dbError (db);
		return -1;
	}

	return 0;
}

int kr_dbQuery (struct db_argument *db, char * sql) {
	db->r = sqlite_compile (db->c, sql, NULL, &db->vm, &db->err);

	if ( db->r != SQLITE_OK ) {
		kr_dbError (db);
		return 1;
	}

	return 0;
}

int kr_dbFetch (struct db_argument *db) {
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

void kr_dbClose (struct db_argument *db) {
	if ( db->c != NULL )
		sqlite_close (db->c);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
