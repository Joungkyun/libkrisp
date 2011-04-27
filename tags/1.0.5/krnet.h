/*
 * $Id: krnet.h,v 1.2 2006-06-20 03:32:11 oops Exp $
 */

#ifndef KRNET_H
#define KRNET_H

unsigned long ip2long (char *ip);
char * long2ip (unsigned long num);
int makeNetworkInfo (char *ip, KRNET_API *n);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
