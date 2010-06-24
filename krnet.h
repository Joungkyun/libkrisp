/*
 * $Id: krnet.h,v 1.6 2010-06-24 16:52:38 oops Exp $
 */

#ifndef KRNET_H
#define KRNET_H

short	_kr_valid_ip_address (char *);
ulong	_kr_ip2long (char *);
char *	_kr_long2ip (ulong);

short	_kr_long2prefix (ulong);
ulong	_kr_prefix2long (short);
short	_kr_guess_prefix (ulong, ulong);
ulong	_kr_guess_netmask (ulong, ulong);
//ulong	_kr_netmask (ulong, ulong);
ulong	_kr_network (ulong, ulong);
ulong	_kr_broadcast (ulong, ulong);
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
