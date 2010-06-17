/*
 * $Id: krisplookup.c,v 1.43 2010-06-17 17:36:37 oops Exp $
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
#define LOCALEALIAS "/usr/share/locale/locale.alias"
#define DB_CHARSET "UTF8"
#define FORCE_NOTHING 0
#define FORCE_UTF8 1
#define FORCE_EUCKR 2
#endif

#ifdef HAVE_GETOPT_LONG
static struct option long_options [] = { // {{{
	/* Options without arguments: */
	{ "help",       no_argument,       NULL, 'h' },
	{ "verbose",    no_argument,       NULL, 'v' },
	{ "onlyisp",    no_argument,       NULL, 'i' },
	{ "onlynation", no_argument,       NULL, 'n' },

	/* Options accepting an argument: */
	{ "datafile",   required_argument, NULL, 'f' },
	{ 0, 0, 0, 0 }
}; // }}}
#endif

#ifdef HAVE_ICONV_H
char * confirm_local_charset (void) { // {{{
	char *	lang_env = getenv ("LANG");
	char *	lcharset = strrchr (lang_env + 1, '.');
	char	buf[1024];
	char *	tmp;

	if ( lcharset == NULL ) {
		struct stat		f;
		FILE *			fp;
		char *			line;

		f.st_size = 0;
		if ( stat (LOCALEALIAS, &f) == -1 )
			return NULL;
		
		if ( (fp = fopen (LOCALEALIAS, "r")) == NULL )
			return NULL;

		while ( fgets (buf, 1024, fp) != NULL ) {
			if ( buf[0] < 97 && buf[0] > 122 )
				continue;

			if ( (line = strstr (buf, lang_env)) != NULL ) {
				if ( (tmp = strchr (buf, '.')) != NULL )
					return tmp + 1;
			}
		}

		return NULL;
	}

	if ( ! strcasecmp (".utf-8", lcharset) || ! strcasecmp (".utf8", lcharset) )
		return "UTF8";

	return lcharset + 1;
} // }}}
#endif

void usage (char *prog) { // {{{
	fprintf (stderr, "%s v%s: Resolved korea range ISP inforamtion\n", prog, krisp_version ());
	fprintf (stderr, "Usage: %s [option] ip-address\n", prog);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "         -f path, --datafile=path     set user define database file\n");
	fprintf (stderr, "         -h , --help                  print this message\n");
	fprintf (stderr, "         -i , --isp                   only print isp code\n");
	fprintf (stderr, "         -n , --nation                only print nation code\n\n");

	exit (1);
} // }}}

int main (int argc, char ** argv) {
	KR_API *		db;
	KRNET_API		isp;
	char *			ip;
	int				opt;
	char *			datafile = NULL;
	short			onlyisp = 0;
	short			onlynation = 0;
	bool			verbose = false;
	char			err[1024];

#ifdef HAVE_GETOPT_LONG
	while ( (opt = getopt_long (argc, argv, "f:hinv", long_options, (int *) 0)) != EOF ) {
#else
	while ( (opt = getopt (argc, argv, "f:hinv")) != EOF ) {
#endif
		switch (opt) {
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
				set_true (verbose);
				break;
			default:
				usage (PNAME);
		}
	}

	if ( argc - optind < 1 || argc == 1 ) {
		usage (PNAME);
		return 1;
	}

	/* database open */
	if ( kr_open (&db, datafile, err) == false ) {
		fprintf (stderr, "ERROR Connect: %s\n", err);
		kr_close (db);
		return 1;
	}

	isp.verbose = verbose;
	db->verbose = verbose;
	ip = argv[optind];

	SAFECPY_256 (isp.ip, ip);
	if ( kr_search (&isp, db) ) {
		fprintf (stderr, "ERROR: %s\n", isp.err);
		kr_close (db);
		return 1;
	}

	if ( verbose == true )
		fprintf (stderr, "\n");

	if ( onlyisp ) {
		printf ("%s\n", isp.icode);
	} else if ( onlynation ) {
		printf ("%s\n", isp.ccode);
	} else {
#ifdef HAVE_ICONV_H
		char *	lcharset;
		iconv_t	cd;
		char *	ispname, * to;
		size_t	flen, tlen;
		char *	srcname = (char *) isp.iname;

		lcharset = confirm_local_charset ();
		flen = strlen (srcname);

		if ( lcharset == NULL || ! strcmp (DB_CHARSET, lcharset) ) {
			tlen = 1;
			cd = (iconv_t)(-1);
		} else {
			tlen = ! strcmp (lcharset, "UTF8") ? flen * 4 + 1 : flen + 1;
			cd = iconv_open (lcharset, DB_CHARSET);
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

		printf ("SUBNET    : %s\n", kr_long2ip (isp.netmask));
		printf (
				"NETWORK   : %s\n",
				kr_long2ip (kr_network (isp.start, isp.netmask))
		);
		printf (
				"BROADCAST : %s\n",
				kr_long2ip (kr_broadcast(isp.start, isp.netmask))
		);
		printf ("DB RANGE  : %s - ", kr_long2ip (isp.start));
		printf ("%s\n", kr_long2ip (isp.end));
		printf ("NATION    : %s (%s)\n", isp.cname, isp.ccode);
	}

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
