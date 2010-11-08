/* $Id: acconfig.h,v 1.4 2010-11-08 06:11:50 oops Exp $ */
#ifndef KRISP_CONFIG_H
#define KRISP_CONFIG_H

#ifdef PACKAGE_BUGREPORT
    #undef KR_PACKAGE_BUGREPORT
#endif

#ifdef PACKAGE_NAME
    #undef KR_PACKAGE_NAME
#endif

#ifdef PACKAGE_STRING
    #undef KR_PACKAGE_STRING
#endif

#ifdef PACKAGE_TARNAME
    #undef KR_PACKAGE_TARNAME
#endif

#ifdef PACKAGE_VERSION
    #undef KR_PACKAGE_VERSION
#endif


/* config.h:  a general config file */

@TOP@

@BOTTOM@

/*
 *  * Compotable C Type
 *   */

#if ! defined C_ULONG_DEFINED
	typedef unsigned long   ulong;
#endif

#endif /*  KRISP_CONFIG_H */
