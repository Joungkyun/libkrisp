/*
 * $Id: krtest.c,v 1.5 2010-06-15 17:58:55 oops Exp $
 */

#include <krisp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include <sys/time.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#define PNAME "krtest"

#ifdef HAVE_GETOPT_LONG
static struct option long_options [] = { // {{{
	/* Options without arguments: */
	{ "help", no_argument, NULL, 'h' },
	{ "verbose", no_argument, NULL, 'v' },

	/* Options accepting an argument: */
	{ "datafile", required_argument, NULL, 'f' },
	{ 0, 0, 0, 0 }
}; // }}}
#endif

void usage (char *prog) { // {{{
	fprintf (stderr, "%s v%s: Benchmakr libkrisp\n", prog, krisp_version ());
	fprintf (stderr, "Usage: %s [option] <loop no(int)|unlimit(string)>\n", prog);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "         -f path, --datafile=path     set user define database file\n");
	fprintf (stderr, "         -h , --help                  print this message\n");
	fprintf (stderr, "         -v , --verbose               verbose message\n\n");

	exit (1);
} // }}}

/*
 * From olibc http://cvs.oops.org/?cvsroot=OOPS-Project&module=olibc
 */
double microtime (void) { // {{{
	struct timeval tp;
	long sec = 0L;
	double ret = 0;
	double msec = 0.0;

	if ( gettimeofday ((struct timeval *) &tp, '\0') == 0) {
		msec = (double) (tp.tv_usec / 1000000.00);
		sec = tp.tv_sec;

		if (msec >= 1.0) msec -= (long) msec;
		ret = sec + msec;
	}

	return ret;
} // }}}

ulong prand (void) { // {{{
	int c;
	ulong p;

retry:
	c = time (0) % 2;
	p = random ();
	if ( c == 0 )
		p *= 2;

	if ( p < 16777216 )
		goto retry;

	return p;
} // }}}


int main (int argc, char **argv) {
	KR_API *		db;
	KRNET_API		isp;
	struct stat		f;
	char *			ip;
	int				opt, r;
	char *			datafile = NULL;
	short			verbose = 0, dverbose = 0;
	int				loop = 0, unlimit =0;
	int				i;
	double			t1, t2;
	char			err[1024];

#ifdef HAVE_GETOPT_LONG
	while ( (opt = getopt_long (argc, argv, "f:hv", long_options, (int *) 0)) != EOF ) {
#else
	while ( (opt = getopt (argc, argv, "f:hv")) != EOF ) {
#endif
		switch (opt) {
			case 'f' :
				datafile = optarg;
				break;
			case 'v' :
				verbose++;
				break;
			default:
				usage (PNAME);
		}
	}

	dverbose = (verbose > 1) ? 1 : 0;

	if ( argc - optind < 1 || argc == 1 ) {
		usage (PNAME);
		return 1;
	}

	/* database open */
	if ( (r = kr_open (&db, datafile, err)) > 0 ) {
		fprintf (stderr, "ERROR Connect: %s\n", err);
		kr_close (db);
		return 1;
	}

	isp.verbose = dverbose;
	db->verbose = dverbose;
	if ( strcmp (argv[optind], "unlimit") )
		loop = atoi (argv[optind]);
	else {
		loop = 10;
		unlimit++;
	}
	srand (time (0));

	t1 = microtime ();

	for ( i=0; i<loop; ) {
		ip = long2ip (prand ());

		SAFECPY_256 (isp.ip, ip);
		if ( kr_search (&isp, db) ) {
			fprintf (stderr, "ERROR: %s\n", db->err);
			kr_close (db);
			return 1;
		}

		if ( verbose )
			printf ("--> %15s : %s\n", ip, isp.icode);

		if ( ! unlimit )
			i++;
	}

	t2 = microtime ();
	fprintf (stderr, "%.4f sec\n", t2 - t1);

	/* database close */
	kr_close (db);

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
