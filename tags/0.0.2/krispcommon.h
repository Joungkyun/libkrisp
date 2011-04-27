/*
 * $Id: krispcommon.h,v 1.1.1.1 2006-06-08 19:31:52 oops Exp $
 */

#ifndef COMMON_H
#define COMMON_H

#ifdef HAVE_CONFIG_H
#include <krisp-config.h>
#endif

#if defined(HAVE_LIBSQLITE3)
#include <sqlite3.h>
#else
#include <sqlite.h>
#endif

struct netmasks {
	int				nums;
	char **			mask;
};

struct db_argument {
#if defined(HAVE_LIBSQLITE3)
	sqlite3			*c;		/* db resource */
	sqlite3_stmt	*vm;	/* sqlite vm */
#else
	sqlite			*c;		/* db resource */
	sqlite_vm		*vm;	/* sqlite vm */
#endif
	int				r;		/* execute result code */
	int				rows;	/* vm rows */
	int				cols;	/* number of columns */
	char *			err;	/* vm error message */
	const char **	rowdata;
	const char **	colname;
};

struct netinfos {
	char			key[16];
	char			ip[16];
	char			netmask[16];
	char			network[16];
	char			broadcast[16];
	char			org[128];
	char			serv[128];
#ifdef HAVE_LIBGEOIP
	char			code[4];
	char			nation[128];
#endif
};

struct cinfo {
	unsigned long	ip;
	unsigned long	mask;
	unsigned long	network;
	unsigned long	broad;
};

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
