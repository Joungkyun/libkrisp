/*
 * $Id: krispmkdb.c,v 1.9 2006-11-25 18:58:50 oops Exp $
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
#define DEFSAVE "./krisp-new.dat"

#ifdef HAVE_GETOPT_LONG
static struct option long_options [] = {
	/* Options without arguments: */
	{ "help", no_argument, NULL, 'h' },
			    
	/* Options accepting an argument: */
	{ "savepath", required_argument, NULL, 's' },
	{ 0, 0, 0, 0 }
};
#endif

void usage (void) {
	fprintf (stderr, "krmakedb v%s: Make libkrisp database utility\n", KRISP_VERSION);
	fprintf (stderr, "USAGE: krmakedb [option] ip_list_sql_file_path\n");
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "    -s path, --savepath=path   save data file path (def: ./krisp-new.dat)\n");
	fprintf (stderr, "    -h, --help                 print this message\n");

	exit (1);
}

char * getlist (char *file) {
	struct stat f;
	FILE *fp;
	size_t r;
	char tmp[FILEBUF+1], *buf;;

	f.st_size = 0;
	r = stat (file, &f);

	if ( r == -1 ) {
		fprintf (stderr, "ERROR: Can't open ISP ip list file %s\n", file);
		return NULL;
	}

	if ( f.st_size < 1 ) {
		fprintf (stderr, "ERROR: %s size is zero\n", file);
		return NULL;
	}

	if ( (fp = fopen (file, "rb")) == NULL ) {
		fprintf (stderr, "ERROR: Can't open %s in read mode\n", file);
		return NULL;
	}

	buf = (char *) malloc (sizeof (char) * (f.st_size + 1));
	memset (buf, 0, sizeof (char) * (f.st_size + 1));

	memset (tmp, 0, FILEBUF+1);
	while ( (r = fread (tmp, sizeof (char), FILEBUF, fp)) > 0 ) {
		strcat (buf, tmp);
		memset (tmp, 0, FILEBUF+1);
	}

	fclose (fp);

	return buf;
}

int main (int argc, char **argv) {
	KR_API *db;
	char *ipfile;
	char *savedb = NULL;
	char *buf, *sql, *sql_t;
	int r, len;
	char output[80];
	int opt;

#ifdef HAVE_GETOPT_LONG
	while ( (opt = getopt_long (argc, argv, "s:h", long_options, (int *) 0)) != EOF ) {
#else
	while ( (opt = getopt (argc, argv, "s:h")) != EOF ) {
#endif
		switch (opt) {
			case 's' :
				savedb = optarg;
				break;
			default:
				usage ();
				return 1;
		}
	}

	if ( argc - optind < 1 || argc == 1 ) {
		usage ();
		return 1;
	}

	ipfile = argv[optind];

	db = (KR_API *) malloc (sizeof (KR_API));

	if ( kr_open (db, (savedb != NULL ) ? savedb : DEFSAVE) ) {
		fprintf (stderr, "ERROR: DB connect failed (%s)\n", dberr);
		return 1;
	}
	
	if ( (buf = getlist (ipfile)) == NULL ) {
		kr_close (db);
		free (db);
		fprintf (stderr, "ERROR: %s is empty\n", ipfile);
		return 1;
	}

	//printf ("%s\n", buf);
	
	sql = buf;
	while ( 1 ) {
		sql_t = strchr (sql, '\n');
		if ( sql_t == NULL )
			break;

		sql[sql_t - sql] = 0;

		if ( (len = strlen (sql)) ) {
			if ( len > 77 ) {
				strncpy (output, sql, 77);
				output[77] = 0;
			} else {
				strcpy (output, sql);
			}
			printf ("=> %s\n", output);

			if ( kr_dbQuery (db, sql, DBTYPE_KRISP) ) {
				fprintf (stderr, "\nERROR: query failed (%s)\n", dberr);
				kr_close (db);
				free (buf);
				free (db);
				return 1;
			}

			while ( ! (r = kr_dbFetch (db, DBTYPE_KRISP) ) ) {
				// not actions
			}

			if ( r == -1 ) {
				fprintf (stderr, "\nERROR: fetch failed (%s)\n", dberr);
				kr_close (db);
				free (buf);
				free (db);
				return 1;
			}
		}

		sql = sql_t + 1;
	}

	kr_close (db);
	free (buf);
	free (db);

	return 0;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
