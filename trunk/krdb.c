/*
 * $Id: krdb.c,v 1.9 2006-09-08 16:29:20 oops Exp $
 */

#include <krispcommon.h>

#if defined(HAVE_LIBSQLITE3)
#include <sqlite3.h>
#else
#include <sqlite.h>
#endif

#include <krdb.h>

char dberr[1024];

#if defined(HAVE_LIBSQLITE3)
#include <database/sqlite3.c>
#else
#include <database/sqlite.c>
#endif

GeoIPvar * krGeoIP_open (KR_API *db) {
	GeoIPvar * p;

#ifndef HAVE_LIBGEOIP
	return NULL;
#endif

	if ( (p = (GeoIPvar *) malloc (sizeof (GeoIPvar)) ) == NULL )
		return NULL;

	p->gid = GeoIP_new (GEOIP_MEMORY_CACHE | GEOIP_CHECK_CACHE);

	/*
	 * check city information
	 * GEOIP_CITY_EDITION_REV0 2
	 * GEOIP_CITY_EDITION_REV1 6
	 */
	if ( p->gid != NULL ) {
		_GeoIP_setup_dbfilename ();
		if ( GeoIP_db_avail (GEOIP_CITY_EDITION_REV0) )
			p->gic = GeoIP_open_type (GEOIP_CITY_EDITION_REV0,
										GEOIP_MEMORY_CACHE | GEOIP_CHECK_CACHE);
		else if (GeoIP_db_avail (GEOIP_CITY_EDITION_REV1) )
			p->gic = GeoIP_open_type (GEOIP_CITY_EDITION_REV1,
										GEOIP_MEMORY_CACHE | GEOIP_CHECK_CACHE);
		else
			p->gic = NULL;

		if ( GeoIP_db_avail (GEOIP_ISP_EDITION) )
			p->gip = GeoIP_open_type (GEOIP_ISP_EDITION,
										GEOIP_MEMORY_CACHE | GEOIP_CHECK_CACHE);
		else
			p->gip = NULL;
	} else {
		p->gic = NULL;
		p->gip = NULL;
		free (p);
		p = NULL;
	}

	return p;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
