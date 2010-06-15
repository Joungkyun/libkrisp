/*
 * $Id: ip2long.c,v 1.2 2010-06-08 03:05:09 oops Exp $
 */

#include <krisp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#define PNAME "ip2long"
#define LONGIP 0
#define IPv4 1

#ifdef HAVE_GETOPT_LONG
static struct option long_options [] = { // {{{
	/* Options without arguments: */
	{ "help", no_argument, NULL, 'h' },
	{ "verbose", no_argument, NULL, 'v' },

	/* Options accepting an argument: */
	//{ "datafile", required_argument, NULL, 'f' },
	{ 0, 0, 0, 0 }
}; // }}}
#endif

void usage (char *prog) { // {{{
	fprintf (stderr, "%s v%s: Convert between IPv4 address and Long IP\n", prog, KRISP_VERSION);
	fprintf (stderr, "Usage: %s [option] ip-address|LongIP\n", prog);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "         -h, --help             print this message\n");
	fprintf (stderr, "         -v, --verbose          print both long ip and ipv4\n");

	exit (1);
} // }}}

int main (int argc, char **argv) {
	char 			ip[256] = { 0, };
	int				opt;
	short			verbose = 0;
	short			input;

#ifdef HAVE_GETOPT_LONG
	while ( (opt = getopt_long (argc, argv, "hv", long_options, (int *) 0)) != EOF ) {
#else
	while ( (opt = getopt (argc, argv, "hv")) != EOF ) {
#endif
		switch (opt) {
			case 'v' :
				verbose++;
				break;
			default:
				usage (PNAME);
		}
	}

	if ( argc - optind < 1 || argc == 1 ) {
		usage (PNAME);
		return 1;
	}

	SAFECPY_256 (ip, argv[optind]);
	input = (strchr (ip, '.') != NULL) ? IPv4 : LONGIP;

	if ( valid_address (ip) ) {
		fprintf (stderr, "ERROR: %s is not IPv4 nor Long IP address\n", ip);
		return 1;
	}

	if ( verbose )
		printf ("IPv4   : ");
	if ( verbose || input == LONGIP )
		printf ("%s\n", ip);

	if ( verbose )
		printf ("LONGIP : ");
	if ( verbose || input == IPv4 )
		printf ("%lu\n", ip2long (ip));

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