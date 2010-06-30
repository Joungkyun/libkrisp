/*
 * $Id: krnet.c,v 1.5 2010-06-07 11:31:26 oops Exp $
 */

#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <krnet.h>

/*
 * check valid ip
 */
short valid_address (char * addr) { // {{{
	struct hostent *	hp;
	struct in_addr		s;

	hp = gethostbyname (addr);

	if ( hp && *(hp->h_addr_list) ) {
		memcpy(&s.s_addr, *(hp->h_addr_list), sizeof(s.s_addr));
		strcpy (addr, inet_ntoa (s));
	}

	if ( inet_addr (addr) == -1 )
		return 1;

	return 0;
} // }}}

/*
 * Converts a string containing an (IPv4) Internet Protocol
 * dotted address into a proper address
 */
ulong ip2long (char *ip) { // {{{
	struct in_addr s;

	if ( ! strlen (ip) || ! inet_aton (ip, &s) )
		return 0;

	return ntohl (s.s_addr);
} // }}}

/*
 * Converts IPv4 address into a string in Internet standard dotted format
 * The string is returned in a statically allocated buffer, which subsequent
 * calls will overwrite.
 */
char * long2ip (ulong num) { // {{{
	struct in_addr	addr;

	/*
	* if num is charactor type, add follow line
	* n = strtoul (num, NULL, 0);
	*/
	addr.s_addr = htonl (num);
	return inet_ntoa (addr);
} // }}}

#define IPBITS (sizeof(u_int32_t) * 8)
/*
 * convert long netmask to decimical prefix
 * from ipcalc from initscripts on redhat
 */
short long2prefix (ulong mask) { // {{{
	int i;
	int count = IPBITS;

	for (i = 0; i < IPBITS; i++) {
		if (!(mask & ((2 << i) - 1)))
			count--;
	}

	return count;
} // }}}

/*
 * convert network prefix to long mask
 * from ipcalc from initscripts on redhat
 */
ulong prefix2long (short n) { // {{{
	short	l = 32;
	short   shift = 30;
	ulong	v = 0;

	n = 32 - n;

	while (l > n) {
		if ( shift < 0 )
			v += 1;
		else
			v += (2 << shift);

		shift--;
		l--;
	}

	return v;
} // }}}

/*
 * return netmask prefix with start ip and end ip
 */
short guess_prefix (ulong s, ulong e) { // {{{
	ulong	n;
	short	prefix = 0;

	// If end is bigger than start, reverse between start and end.
	if ( e < s ) {
		n = s;
		s = e;
		e = n;
	}

	n = e - s;
	if ( n == 0 )
		return 32;

	while ( ! (n & 0x80000000) ) {
		n <<= 1;
		prefix++;
	}

	// If brocast is smaller than end, decrease prefix.
	n = 0;
	while ( e > n ) {
		if ( n > 0 )
			prefix--;

		n = broadcast (s, prefix2long (prefix));
	}

	return prefix;
} // }}}

/*
 * return netmask with start ip and end ip
 */
ulong guess_netmask (ulong s, ulong e) { // {{{
	return prefix2long (guess_prefix (s, e));
} // }}}

/*
 * return netmask with network and broadcast address
 */
ulong netmask (ulong net, ulong bcast) { // {{{
	return net + 0x80000000 - bcast;
} // }}}

/*
 * return network
 */
ulong network (ulong ip, ulong mask) { // {{{
	return ip & mask;
} // }}}

/*
 * return broadcast
 */
ulong broadcast (ulong ip, ulong mask) { // {{{
	return (ip & mask) | ~(mask);
} // }}}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */