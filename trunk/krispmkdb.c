/*
 * $Id: krispmkdb.c,v 1.15 2010-06-18 13:53:21 oops Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <krisp.h>
#include <krdb.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#define FILEBUF 1024

#ifdef HAVE_GETOPT_LONG
static struct option long_options [] = { // {{{
	/* Options without arguments: */
	{ "help",       no_argument,       NULL, 'h' },
	{ "assume-yes", no_argument,       NULL, 'y' },

	/* Options accepting an argument: */
	{ "table-name", required_argument, NULL, 't' },
	{ "csv-file",   required_argument, NULL, 'c' },
	{ 0, 0, 0, 0 }
}; // }}}
#endif

void usage (void) { // {{{
	fprintf (stderr, "krispmkdb v%s: Make libkrisp database utility\n", KRISP_VERSION);
	fprintf (stderr, "USAGE: krispmkdb [option] database\n");
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "    -c, --csv-file=PATH        input data file with tab csv format\n");
	fprintf (stderr, "    -t, --table-name=NAME      table name\n");
	fprintf (stderr, "    -y, --assum-yes            Assume yes\n");
	fprintf (stderr, "    -h, --help                 print this message\n");

	exit (1);
} // }}}

int check_csv (char * file) { // {{{
	struct stat	f;
	size_t		r;

	f.st_size = 0;
	r = stat (file, &f);

	if ( r == -1 ) {
		fprintf (stderr, "ERROR: Can't open csv format file (%s)\n", file);
		return 1;
	}

	if ( f.st_size < 1 ) {
		 fprintf (stderr, "ERROR: %s size is zero\n", file);
		return 1;
	}

	return 0;
} // }}}

void back (size_t no) { // {{{
	int	go = 0;
	int	i;

	if ( no == 0 )
		return;

	if ( no < 10 )
		go = 0;
	else
		while ( (no /= 10) )
			go++;

	go++;
	for (i=0; i<go; i++ )
		fprintf (stderr, "%c", 0x08);
} // }}}

short put_data (KR_API * db, char * csv) { // {{{
	FILE *	fp;
	char	sql[2048];
	char	tmp[FILEBUF + 1];
	char **	buf;
	size_t	r, i;
	char *	quote_string;

	if ( (fp = fopen (csv, "rb")) == NULL ) {
		fprintf (stderr, "ERROR: Can't open %s in read mode\n", csv);
		return 1;
	}

	memset (tmp, 0, FILEBUF + 1);
	i = 0;
	fprintf (stderr, "Loading %s to %s table .. ", csv, db->table);
	while ( fgets (tmp, FILEBUF, fp) != NULL ) {
		back (i);

		for ( r=strlen (tmp) - 1; r>=0; r-- ) {
			if ( tmp[r] != '\r' && tmp[r] != '\n' )
				break;
			tmp[r] = 0;
		}
		r = parseDummyData (&buf, tmp, '\t');

		/*
		printf ("## r      : %d\n", r);
		printf ("## buf[0] : %s\n", buf[0]);
		printf ("## buf[1] : %s\n", buf[1]);
		printf ("## buf[2] : %s\n", buf[2]);
		 */

		if ( r != 3 || buf == NULL ) {
			fprintf (stderr, "\nERROR: Data format error. (Require 3 fields)\n");
			free (buf);
			return 1;
		}

		quote_string = kr_dbQuote_f (buf[2]);
		sprintf (
				sql,
				"INSERT INTO %s VALUES (%s, %s, '%s')",
				db->table,
				buf[0],
				buf[1],
				quote_string
		);
		free (quote_string);

		if ( kr_dbExecute (db, sql) ) {
			fprintf (stderr, "\nERROR: failed to insert data (%s)\n", db->err);
			fprintf (stderr, "%s\n", sql);
			free (buf);
			return 1;
		}

		memset (tmp, 0, FILEBUF + 1);
		free (buf);
		buf = NULL;

		fprintf (stderr, "%d", ++i);
	}
	fprintf (stderr, "\nLoad success!");

	return 0;
} // }}}

int main (int argc, char ** argv) {
	KR_API *	db;
	int			opt, r;
	int			assume = 0;
	int			exists_table = 0;
	int			delete_table = 0;
	char *		database = NULL;
	char *		table = NULL;
	char *		csv = NULL;
	char		err[1024];
	
#ifdef HAVE_GETOPT_LONG
	while ( (opt = getopt_long (argc, argv, "c:t:hy", long_options, (int *) 0)) != EOF ) {
#else
	while ( (opt = getopt (argc, argv, "c:t:hy")) != EOF ) {
#endif
		switch (opt) {
			case 'c' :
				csv = optarg;
				break;
			case 't' :
				table = optarg;
				if ( strlen (table) > 63 ) {
					fprintf (stderr, "ERROR: Length of table name is less than 64 characters.\n");
					exit (1);
				}
				break;
			case 'y' :
				assume++;
				break;
			default :
				usage ();
		}
	}

	if ( argc - optind < 1 || argc == 1 )
		usage ();

	database = argv[optind];

	if ( database == NULL || table == NULL || csv == NULL )
		usage ();

	if ( check_csv (csv) )
		return 1;

	if ( kr_open (&db, database, err) == false ) {
		fprintf (stderr, "ERROR: DB connect failed (%s)\n", err);
		return 1;
	}

	db->table = table;
	// Get existed table list
	if ( kr_dbQuery (db, "SELECT tbl_name FROM sqlite_master WHERE type = 'table'") ) {
		fprintf (stderr, "ERROR: %s\n", db->err);
		kr_close (db);
		return 1;
	}

	while ( ! (r = kr_dbFetch (db) ) ) {
		if ( strcmp (db->rowdata[0], table) == 0 )
			exists_table = 1;

		kr_dbFree (db);
		db->rows++;

		// If table is exists, force finalize sqlite vm
		if ( exists_table )
			db->final = 1;
	}

	if ( exists_table ) {
		char	yesno;

		if ( assume ) {
			delete_table++;
			goto pass_assume;
		}

reassume:

		printf ("%s table is already exists. Are you want to drop this table? [Y/N] ", table);
		scanf ("%c", &yesno);

		switch (yesno) {
			case 'Y' :
			case 'y' :
				delete_table++;
				break;
			case 'N' :
			case 'n' :
				break;
			default :
				goto reassume;
		}
	}

pass_assume:

	if ( delete_table ) {
		char	sql[256];
		sprintf (sql, "DELETE FROM '%s'", table);
		if ( kr_dbExecute (db, sql) ) {
			fprintf (stderr, "ERROR: failed delete %s table (%s)\n", table, db->err);
			kr_close (db);
			return 1;
		}
	}

	// Create table if table is not exist or drop table
	if ( ! exists_table ) {
		char	sql[256];

		sprintf (
			sql,
			"CREATE TABLE '%s' "
			"( "
			"    start unsigned integer NOT NULL DEFAULT '0', "
			"    end   unsigned integer NOT NULL DEFAULT '0', "
			"    data  varchar NOT NULL DEFAULT '', "
			"    PRIMARY KEY (start DESC), "
			"    UNIQUE (end) "
			")",
			table
		);

		if ( kr_dbExecute (db, sql) ) {
			fprintf (stderr, "ERROR: failed to create %s table (%s)\n", table, db->err);
			kr_close (db);
			return 1;
		}
	}

	r = put_data (db, csv);
	kr_close (db);

	return r;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
