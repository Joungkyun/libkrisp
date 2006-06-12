/*
 * $Id: krispcommon.h,v 1.4 2006-06-12 19:47:50 oops Exp $
 */

#ifndef COMMON_H
#define COMMON_H

#ifdef HAVE_CONFIG_H
#include <krisp-config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if defined(HAVE_LIBSQLITE3)
#include <sqlite3.h>
#else
#include <sqlite.h>
#endif

/*
 * GeoIP extension start
 */
#ifdef HAVE_LIBGEOIP
#ifdef HAVE_GEOIP_H 
#include <GeoIP.h>
#define INCLUDE_GEOIP_HEADER_OK
#endif
#endif

#ifndef INCLUDE_GEOIP_HEADER_OK
typedef struct GeoIPTag {
	FILE *GeoIPDatabase;
	char *file_path;
	unsigned char *cache;
	unsigned char *index_cache;
	unsigned int *databaseSegments;
	char databaseType;
	time_t mtime;
	int flags;
	char record_length;
	int record_iter; /* used in GeoIP_next_record */
} GeoIP;

#define GEOIP_API
GEOIP_API GeoIP* GeoIP_new(int flags);
GEOIP_API int GeoIP_id_by_name (GeoIP* gi, const char *host);
extern const char GeoIP_country_code[247][3];
extern const char * GeoIP_country_name[247];

typedef enum { 
	GEOIP_STANDARD = 0,
	GEOIP_MEMORY_CACHE = 1,
	GEOIP_CHECK_CACHE = 2,
	GEOIP_INDEX_CACHE = 4,
} GeoIPOptions;
#endif
/*
 * GeoIP extension end
 */

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
	GeoIP *			gi;		/* GeoIP resource */
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
