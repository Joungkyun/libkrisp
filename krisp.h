/*
 * $Id: krisp.h,v 1.16 2010-06-08 03:05:09 oops Exp $
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
 * Valid IPv4 address
 * short valid_ipv4_addr (char *ip)
 */
#define valid_ipv4_addr (ip)

/*
 * Converts a string containing an (IPv4) Internet Protocol
 * dotted address into a proper address
 * ulong kr_ip2long (char * ip)
 */
#define kr_ip2long(v) ip2long(v)

/*
 * Converts IPv4 address into a string in Internet standard dotted format
 * The string is returned in a statically allocated buffer, which subsequent
 * calls will overwrite.
 * char * kr_long2ip (ulong ip)
 */
#define kr_long2ip(v) long2ip(v)

/*
 * convert prefix to long
 * ulong kr_prefix2long (short prefix)
 */
#define kr_prefix2long(v) prefix2long(v)

/*
 * convert long prefix to digit
 * short kr_long2prefix (ulong prefix)
 */
#define kr_long2prefix(v) long2prefix(v)

/*
 * return long type of subnet maak with start ip and end ip
 * ulong kr_netmask (ulong start, ulong end)
 */
#define kr_netmask(s, e) guess_netmask(s, e)

/*
 * return long type of network address
 * ulong kr_network (ulong ip, ulong mask)
 */
#define kr_network(ip, mask) _network(ip, mask)

/*
 * return long type of broadcast address
 * ulong kr_broadcast (ulong ip, ulong mask)
 */
#define kr_broadcast(ip, mask) _broadcast(ip, mask)

/*
 * return network prefix of subnet mask with start ip and end ip
 * short kr_prefix (ulogn start, ulong end)
 */
#define kr_prefix(s, e) guess_prefix(s, e);

#define SAFECPY_256(dest, src) _safecpy(dest, src, 256)
#define SAFECPY_512(dest, src) _safecpy(dest, src, 512)
#define SAFECPY_1024(dest, src) _safecpy(dest, src, 1024)

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
