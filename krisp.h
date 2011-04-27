/*
 * $Id: krisp.h,v 1.9 2006-06-12 19:47:50 oops Exp $
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
int kr_open (struct db_argument *, char *);

/*
 * close krisp database
 */
void kr_close (struct db_argument *);

/*
 * search isp information of given ip address
 */
int kr_search (struct netinfos *isp, struct db_argument *db);



/* -------------------{{{ not use on user level---------------------- */


/*
 *
 * not use on user level
 *
 */

/*
 * get netmask of each A class ip from db
 */
int kr_netmask (struct db_argument *, char *, struct netmasks *);

/*
 * get ISP information of each network from db
 */
int getISPinfo (struct db_argument *, char *, struct netinfos *);

/*
 * free memory that used db query function
 */
void kr_free_array (char **);

/*
 * init information structure
 */
void initStruct (struct netinfos *n);

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
