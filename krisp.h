/*
 * $Id$
 */

#ifndef KRIP_H
#define KRIP_H

#include <krversion.h>
#include <krispcommon.h>
#include <krdb.h>

/*
 *
 * user level function prototype
 *
 */

/*
 * print krisp version
 */
char *krisp_version (void);
char *krisp_uversion (void);

/*
 * open krisp database
 */
int kr_open (KR_API *, char *);

/*
 * close krisp database
 */
void kr_close (KR_API *);

/*
 * search isp information of given ip address
 */
int kr_search (KRNET_API *isp, KR_API *db);

/*
 *  krisp GeoIP database open API
 */
GeoIPvar * krGeoIP_open (KR_API *db);


/* -------------------{{{ not use on user level---------------------- */


/*
 *
 * not use on user level
 *
 */

/*
 * get ISP information of each network from db
 */
int getISPinfo (KR_API *, char *, KRNET_API *);

/*
 * get USERDB information of each network from db
 */
int getHostIP (KR_API *, char *, USERDB *);

/*
 * init information structure
 */
void initStruct (KRNET_API *n);

/*
 * convert ip address to netlong type
 */
unsigned long ip2long (char *ip);

/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
