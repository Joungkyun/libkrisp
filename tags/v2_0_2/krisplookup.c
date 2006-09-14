/*
 * $Id: krisplookup.c,v 1.24 2006-09-13 13:37:50 oops Exp $
 */

#include <krisp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#define PNAME "krisplookup"

#ifdef HAVE_GETOPT_LONG
static struct option long_options [] = {
	/* Options without arguments: */
	{ "help", no_argument, NULL, 'h' },

	/* Options accepting an argument: */
	{ "datafile", required_argument, NULL, 'f' },
	{ 0, 0, 0, 0 }
};
#endif

void usage (char *prog) {
	fprintf (stderr, "%s v%s: Resolved korea range ISP inforamtion\n", prog, krisp_version ());
	fprintf (stderr, "Usage: %s [option] ip-address\n", prog);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "         -f path, --datafile=path     set user define database file\n");
	fprintf (stderr, "         -h , --help                  print this message\n");
#ifdef HAVE_LIBGEOIP
	fprintf (stderr, "         -c , --city                  search geoip city database\n\n");
#else
	fprintf (stderr, "         -c , --city                  dummy option [not use]\n\n");
#endif

	exit (1);
}

int main (int argc, char ** argv) {
	KR_API *db;
	KRNET_API isp;
	struct stat f;
	char * ip;
	int opt;
	char *datafile = NULL;

#ifdef HAVE_GETOPT_LONG
	while ( (opt = getopt_long (argc, argv, "cf:h", long_options, (int *) 0)) != EOF ) {
#else
	while ( (opt = getopt (argc, argv, "cf:h")) != EOF ) {
#endif
		switch (opt) {
			case 'c' :
#ifdef HAVE_LIBGEOIP
				geocity = 1;
#endif
				break;
			case 'f' :
				datafile = optarg;
				break;
			default:
				usage (PNAME);
		}
	}

	if ( argc - optind < 1 || argc == 1 ) {
		usage (PNAME);
		return 1;
	}

	f.st_size = 0;
	if ( stat ((datafile != NULL) ? datafile : DBPATH, &f) == -1 ) {
		fprintf (stderr, "ERROR: Can't find data file (%s)\n",
				(datafile != NULL) ? datafile : DBPATH);
		return 1;
	}

	if ( f.st_size < 1 ) {
		fprintf (stderr, "ERROR: %s size is zero\n",
				(datafile != NULL) ? datafile : DBPATH);
		return 1;
	}

	db = (KR_API *) malloc (sizeof (KR_API));

	/* database open */
	if ( kr_open (db, (datafile != NULL) ? datafile : NULL) ) {
		fprintf (stderr, "ERROR Connect: %s\n", dberr);
		return 1;
	}

	ip = argv[optind];

	if ( strlen (ip) > 255 ) {
		strncpy (isp.ip, ip, 255);
		isp.ip[255] = 0;
	} else {
		strcpy (isp.ip, ip);
	}
	kr_search (&isp, db);

	printf ("%s (%s): %s (%s)\n", ip, isp.ip, isp.iname, isp.icode);
	printf ("SUBNET : %s\n", isp.netmask);
	printf ("NETWORK: %s\n", isp.network);
	printf ("BCAST  : %s\n", isp.broadcast);

#ifdef HAVE_LIBGEOIP
	printf ("NATION : %s (%s)\n", isp.gname, isp.gcode);
	if ( geocity )
		printf ("CITY   : %s\n", isp.gcity);
#endif

	/* database close */
	kr_close (db);
	free (db);

	return 0;
}
