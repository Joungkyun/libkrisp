/*
 * $Id: krisplookup.c,v 1.4 2006-06-10 19:41:56 oops Exp $
 */

#include <stdio.h>
#include <krisp.h>

#ifdef HAVE_LIBGEOIP
#include <GeoIP.h>
GeoIP *gi = NULL;
#endif

extern char dberr[1024];

void usage (char *prog) {
	fprintf (stderr, "krisplookup v%s: Resoved korea range ISP inforamtion\n", krisp_version ());
	fprintf (stderr, "Usage: %s ip-address\n", prog);

	exit (1);
}

int main (int argc, char ** argv) {
	struct db_argument db;
	struct netinfos isp;
	char * ip;

	if ( argc != 2 ) {
		usage(argv[0]);
	}
	ip = argv[1];

	/* database open */
	if ( kr_open (&db, NULL) ) {
		fprintf (stderr, "ERROR Connect: %s\n", dberr);
		return 1;
	}

#ifdef HAVE_LIBGEOIP
	gi = GeoIP_new (GEOIP_MEMORY_CACHE);
#endif

	search (ip, &isp, &db);

	printf ("%s(%s) %s %s %s %s",
			ip, isp.ip, isp.serv, isp.network, isp.broadcast, isp.org);

#ifdef HAVE_LIBGEOIP
	printf (" %s %s", isp.code, isp.nation);
#endif
	printf ("\n");

	/* database close */
	kr_close (&db);

	return 0;
}
