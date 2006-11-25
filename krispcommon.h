/*
 * $Id: krispcommon.h,v 1.22 2006-11-25 19:55:53 oops Exp $
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

#define DBTYPE_KRISP  0
#define DBTYPE_HOSTIP 1

/*
 * GeoIP extension start
 */
#ifdef HAVE_LIBGEOIP
#ifdef HAVE_GEOIP_H 
#include <GeoIP.h>
#include <GeoIPCity.h>
void _GeoIP_setup_dbfilename (void);
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

typedef enum { 
	GEOIP_STANDARD = 0,
	GEOIP_MEMORY_CACHE = 1,
	GEOIP_CHECK_CACHE = 2,
	GEOIP_INDEX_CACHE = 4,
} GeoIPOptions;

typedef enum {
	GEOIP_COUNTRY_EDITION     = 1,
	GEOIP_REGION_EDITION_REV0 = 7,
	GEOIP_CITY_EDITION_REV0   = 6,
	GEOIP_ORG_EDITION         = 5,
	GEOIP_ISP_EDITION         = 4,
	GEOIP_CITY_EDITION_REV1   = 2,
	GEOIP_REGION_EDITION_REV1 = 3,
	GEOIP_PROXY_EDITION       = 8,
	GEOIP_ASNUM_EDITION       = 9,
	GEOIP_NETSPEED_EDITION    = 10,
	GEOIP_DOMAIN_EDITION      = 11
} GeoIPDBTypes;

typedef struct GeoIPRecordTag {
	char *country_code;
	char *country_code3;
	char *country_name;
	char *region;
	char *city;
	char *postal_code;
	float latitude;
	float longitude;
	int dma_code;
	int area_code;
} GeoIPRecord;

#define GEOIP_API
GEOIP_API GeoIP* GeoIP_new(int flags);
GEOIP_API void GeoIP_delete(GeoIP* gi);
GEOIP_API int GeoIP_id_by_name (GeoIP* gi, const char *host);
GEOIP_API int GeoIP_db_avail(int type);
GEOIP_API char *GeoIP_org_by_name (GeoIP* gi, const char *host);
GeoIPRecord * GeoIP_record_by_name (GeoIP* gi, const char *host);
void GeoIPRecord_delete (GeoIPRecord *gir);
extern const char GeoIP_country_code[247][3];
extern const char * GeoIP_country_name[247];
#endif

typedef struct GeoIPvarTag {
	GeoIP *		gid;
	GeoIP *		gic;
	GeoIP *		gip;
} GeoIPvar;
/*
 * GeoIP extension end
 */

struct netmasks {
	int				nums;
	char **			mask;
};

typedef struct db_argument {
#if defined(HAVE_LIBSQLITE3)
	sqlite3			*c;		/* db resource */
	sqlite3			*h;		/* hostip db resource */
	sqlite3_stmt	*vm;	/* sqlite vm */
#else
	sqlite			*c;		/* db resource */
	sqlite			*h;		/* hostip db resource */
	sqlite_vm		*vm;	/* sqlite vm */
#endif
	int				r;		/* execute result code */
	int				rows;	/* vm rows */
	int				cols;	/* number of columns */
	char *			err;	/* vm error message */
	const char **	rowdata;
	const char **	colname;
	GeoIPvar *		gi;		/* GeoIP resource */
} KR_API;

typedef struct netinfos {
	char			key[16];
	char			ip[256];
	char			netmask[16];
	char			network[16];
	char			broadcast[16];
	char			icode[128];
	char			iname[128];
#ifdef HAVE_LIBGEOIP
	char			gcode[4];
	char			gname[128];
#endif
	char			gcity[64];
	char			gregion[4];
} KRNET_API;

struct cinfo {
	unsigned long	ip;
	unsigned long	mask;
	unsigned long	network;
	unsigned long	broad;
};

typedef struct hostips {
	char			city[64];
	char			region[32];
} HOSTIP;

extern short hostip;

#ifndef GEOCITYVAR
#ifdef HAVE_LIBGEOIP
/* set 1, search GeoIPCity database if enabled search GeoIPCity
 * default value is 0 */
extern short geocity;
extern short geocity_type;
extern short geoisp_type;
extern short geo_type;
#endif
#endif

#ifndef DBERRVAR
extern char dberr[1024];
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
