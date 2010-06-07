/*
 * $Id: krisp.h,v 1.15 2010-06-07 11:31:26 oops Exp $
 */

#ifndef KR_ISP_H
#define KR_ISP_H

#include <krversion.h>
#include <krispcommon.h>
#include <krispapi.h>

/*
 * KRISP user level function prototype
 */

/*
 * print krisp version
 */
char * krisp_version (void);
char * krisp_uversion (void);

/*
 * open krisp database
 */
int kr_open (KR_API **, char *);

/*
 * close krisp database
 */
void kr_close (KR_API *);

/*
 * search isp information of given ip address
 */
int kr_search (KRNET_API *, KR_API *);
int kr_search_ex (KRNET_API_EX *, KR_API *);

/*
 * KRISP network conversion api
 */

/*
 * Converts a string containing an (IPv4) Internet Protocol
 * dotted address into a proper address
 */
ulong kr_ip2long (char *);

/*
 * Converts IPv4 address into a string in Internet standard dotted format
 * The string is returned in a statically allocated buffer, which subsequent
 * calls will overwrite.
 */
char * kr_long2ip (ulong);

/*
 * convert prefix to long
 */
ulong kr_prefix2long (short);

/*
 * return long type of subnet maak with start ip and end ip
 */
ulong kr_netmask (ulong start, ulong end);

/*
 * return long type of network address
 */
ulong kr_network (ulong ip, ulong mask);

/*
 * return long type of broadcast address
 */
ulong kr_broadcast (ulong ip, ulong mask);

/*
 * return network prefix of subnet mask with start ip and end ip
 */
short kr_prefix (ulong, ulong);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
