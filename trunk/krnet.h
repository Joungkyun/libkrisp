/*
 * $Id: krnet.h,v 1.4 2010-06-08 03:05:09 oops Exp $
 */

#ifndef KRNET_H
#define KRNET_H

short valid_address (char *);
ulong ip2long (char *);
char * long2ip (ulong);

short long2prefix (ulong);
ulong prefix2long (short);
short guess_prefix (ulong, ulong);
ulong guess_netmask (ulong, ulong);
//ulong _netmask (ulong, ulong);
ulong _network (ulong, ulong);
ulong _broadcast (ulong, ulong);
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
