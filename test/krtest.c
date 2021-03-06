/*
 * Copyright (c) 2016 JoungKyun.Kim <http://oops.org> all rights reserved
 *
 * This file is part of libkrisp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as published
 * by the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU LESSER GENERAL PUBLIC LICENSE for more details.
 *
 * You should have received a copy of the GNU LESSER GENERAL PUBLIC LICENSE
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <ipcalc.h>
#include <krisp.h>
#include <sys/time.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#define PNAME "krtest"

#ifdef HAVE_GETOPT_LONG
static struct option long_options [] = { // {{{
	/* Options without arguments: */
	{ "help",     no_argument,       NULL, 'h' },
	{ "verbose",  no_argument,       NULL, 'v' },

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
	struct timeval	tp;
	long			sec = 0L;
	double			ret = 0;
	double			msec = 0.0;

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


int main (int argc, char ** argv) {
	KR_API *		db;
	KRNET_API		isp;
	char *			ip;
	int				opt;
	char *			datafile = NULL;
	bool			verbose = false, dverbose = false;
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
				verbose = true;
				break;
			default:
				usage (PNAME);
		}
	}

	dverbose = (verbose == true) ? true : false;

	if ( argc - optind < 1 || argc == 1 ) {
		usage (PNAME);
		return 1;
	}

	/* database open */
	if ( kr_open (&db, datafile, err) == false ) {
		fprintf (stderr, "ERROR Connect: %s\n", err);
		kr_close (&db);
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
			fprintf (stderr, "ERROR: %s\n", isp.err);
			kr_close (&db);
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
	kr_close (&db);

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
