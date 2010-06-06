/*
 * $Id: krisplookup.c,v 1.33.2.1 2010-06-05 10:56:25 oops Exp $
 */

#include <krisp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#define PNAME "krisplookup"

#ifdef HAVE_ICONV_H
#include <iconv.h>
#include <errno.h>
#define DB_CHARSET "EUC-KR"
#define FORCE_NOTHING 0
#define FORCE_UTF8 1
#define FORCE_EUCKR 2
#endif

extern short verbose;

#ifdef HAVE_GETOPT_LONG
static struct option long_options [] = {
	/* Options without arguments: */
	{ "help", no_argument, NULL, 'h' },
	{ "verbose", no_argument, NULL, 'v' },
	{ "onlyisp", no_argument, NULL, 'i' },
	{ "onlynation", no_argument, NULL, 'n' },

	/* Options accepting an argument: */
	{ "datafile", required_argument, NULL, 'f' },
	{ 0, 0, 0, 0 }
};
#endif

short confirm_local_charset (void) {
	char * lang_env = getenv ("LANG");
	char * lcharset = strrchr (lang_env + 1, '.');

	if ( ! strcmp (".eucKR", lcharset) || ! strcmp (".euckr", lcharset)
		|| ! strcmp (".euc-kr", lcharset) || ! strcmp (".EUC-KR", lcharset)
		|| ! strcmp (".euc-KR", lcharset) ) {
		return ( ! strcmp ("EUC-KR", DB_CHARSET) ) ? FORCE_NOTHING : FORCE_EUCKR;
	} else {
		return ( ! strcmp ("UTF8", DB_CHARSET) ) ? FORCE_NOTHING : FORCE_UTF8;
	}
}

void usage (char *prog) {
	fprintf (stderr, "%s v%s: Resolved korea range ISP inforamtion\n", prog, krisp_version ());
	fprintf (stderr, "Usage: %s [option] ip-address\n", prog);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "         -f path, --datafile=path     set user define database file\n");
	fprintf (stderr, "         -h , --help                  print this message\n");
	fprintf (stderr, "         -c , --city                  search geoip city database\n");
	fprintf (stderr, "         -i , --isp                   only print isp code\n");
	fprintf (stderr, "         -n , --nation                only print nation code\n\n");

	exit (1);
}

int main (int argc, char ** argv) {
	KR_API *db;
	KRNET_API isp;
	struct stat f;
	char * ip;
	int opt;
	char *datafile = NULL;
	short city = 0;
	short onlyisp = 0;
	short onlynation = 0;

#ifdef HAVE_GETOPT_LONG
	while ( (opt = getopt_long (argc, argv, "cf:hinv", long_options, (int *) 0)) != EOF ) {
#else
	while ( (opt = getopt (argc, argv, "cf:hinv")) != EOF ) {
#endif
		switch (opt) {
			case 'c' :
				city = 1;
#ifdef HAVE_LIBGEOIP
				geocity = 1;
#endif
				break;
			case 'f' :
				datafile = optarg;
				break;
			case 'i' :
				if ( onlynation > 0 ) {
					fprintf (stderr, "ERROR: Can't use -n option and -i option togather\n");
					return 1;
				}
				onlyisp++;
				break;
			case 'n' :
				if ( onlyisp > 0 ) {
					fprintf (stderr, "ERROR: Can't use -n option and -i option togather\n");
					return 1;
				}
				onlynation++;
				break;
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
		fprintf (stderr, "ERROR Connect: %s\n", db->err);
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

	if ( verbose )
		printf ("\n");

	if ( onlyisp ) {
		printf ("%s\n", isp.icode);
	} else if ( onlynation ) {
		printf ("%s\n", isp.ccode);
	} else {
#ifdef HAVE_ICONV_H
		short lcharset;
		iconv_t cd;
		char * ispname, * to;
		size_t flen, tlen;
		char * srcname = (char *) isp.iname;

		lcharset = confirm_local_charset ();
		flen = strlen (srcname);

		switch (lcharset) {
			case FORCE_UTF8 :
				tlen = flen * 4 + 1;
				cd = iconv_open ("UTF-8", "CP949");
				break;
			case FORCE_EUCKR :
				tlen = flen + 1;
				cd = iconv_open ("CP949", "UTF-8");
				break;
			default :
				tlen = 1;
				cd = (iconv_t)(-1);
		}	

		if ( cd == (iconv_t)(-1) ) {
			ispname = strdup (isp.iname);
			goto noconvert;
		}

		if ( (ispname = (char *) malloc (sizeof (char) * tlen)) == NULL ) {
			ispname = strdup (isp.iname);
			goto noconvert;
		}
		memset (ispname, 0, sizeof (char) * tlen);
		to = ispname;

		iconv (cd, &srcname, &flen, &to, &tlen);
		switch ( errno ) {
			case E2BIG :
			case EILSEQ :
			case EINVAL :
				strcpy (ispname, isp.iname);
				break;
		}

noconvert:
		printf ("%s (%s): %s (%s)\n", ip, isp.ip, ispname, isp.icode);
#else
		printf ("%s (%s): %s (%s)\n", ip, isp.ip, isp.iname, isp.icode);
#endif

#ifdef HAVE_ICONV_H
		if ( cd != (iconv_t)(-1) )
			iconv_close (cd);
		free (ispname);
#endif

		printf ("SUBNET : %s\n", isp.netmask);
		printf ("NETWORK: %s\n", isp.network);
		printf ("BCAST  : %s\n", isp.broadcast);
		printf ("NATION : %s (%s)\n", isp.cname, isp.ccode);
		if ( city ) {
			printf ("CITY   : %s", isp.city);
			if ( strlen (isp.region) && strcmp (isp.region, "N/A") )
				printf (", %s", isp.region);
			printf ("\n");
		}
	}

	/* database close */
	kr_close (db);
	free (db);

	return 0;
}
