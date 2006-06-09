/*
 * $Id: krnet.c,v 1.2 2006-06-09 13:57:03 oops Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <krispcommon.h>
#include <krnet.h>

/*
 * Converts a string containing an (IPv4) Internet Protocol dotted address into a proper address
 */
unsigned long ip2long (char *ip) {
	if ( ! strlen (ip) || inet_addr (ip) == INADDR_NONE ) {
		return -1;
	}

	return ntohl (inet_addr (ip));
}

/*
 * Converts IPv4 address into a string in Internet standard dotted format
 */
char * long2ip (unsigned long num) {
	struct in_addr addr;

	/*
	* if num is charactor type, add follow line
	* n = strtoul (num, NULL, 0);
	*/
	addr.s_addr = htonl (num);

	return inet_ntoa (addr);
}

int makeNetworkInfo (char *ip, struct netinfos *n) {
	struct in_addr addr, mddr, tddr;
	int tmp;

	if ( ! inet_aton (ip, &addr) )
		return 1;

	// input ip
	strcpy (n->ip, ip);

	addr.s_addr = ntohl (addr.s_addr);

	// make subnet mask
	if ( ((addr.s_addr & 0xFF000000) >> 24) <= 127 )
		strcpy (n->netmask, "255.0.0.0");
	else if (((addr.s_addr & 0xFF000000) >> 24) <= 191)
		strcpy (n->netmask, "255.255.0.0");
	else
		strcpy (n->netmask, "255.255.255.0");

	// make network
	if ( ! inet_aton (n->netmask, &mddr) )
		return 1;

	addr.s_addr = htonl (addr.s_addr);

	tmp = addr.s_addr & mddr.s_addr;
	tddr.s_addr = htonl (ntohl (tmp));
	strcpy (n->network, inet_ntoa (tddr));

	// make broadcast
	tmp = (addr.s_addr & mddr.s_addr) | (~mddr.s_addr);
	tddr.s_addr = htonl (ntohl (tmp));
	strcpy (n->broadcast, inet_ntoa (tddr));

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
