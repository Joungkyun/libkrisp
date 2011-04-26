/* $Id: acconfig.h,v 1.5 2010-11-08 06:38:58 oops Exp $ */
#ifndef KRISP_CONFIG_H
#define KRISP_CONFIG_H

/* config.h:  a general config file */

@TOP@

@BOTTOM@

/*
 *  * Compotable C Type
 *   */

#if ! defined C_ULONG_DEFINED
#	if ! defined ulong
		typedef unsigned long   ulong;
#	endif
#endif

#endif /*  KRISP_CONFIG_H */
