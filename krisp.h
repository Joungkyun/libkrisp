/*
 * Copyright (c) 2016 JoungKyun.Kim <http://oops.org> all rights reserved
 *
 * This file is part of libkrisp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as published
 * by the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU LESSER GENERAL PUBLIC LICENSE for more details.
 *
 * You should have received a copy of the GNU LESSER GENERAL PUBLIC LICENSE
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef KR_ISP_H
#define KR_ISP_H

#include <krversion.h>
#if defined _WIN32  && ! defined HAVE_CONFIG_H
#include <krispcommon.h.in>
#else
#include <krispcommon.h>
#endif

/*
 * KRISP user level function prototype
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Init information structure
 */
extern void initStruct (KRNET_API *);

/*
 * Init raw database data structure
 */
extern void initRawStruct (RAW_KRNET_API *, bool);
#define initStruct_ex initRawStruct

/*
 * print krisp version
 */
extern char * krisp_version (void);
extern char * krisp_uversion (void);

/*
 * open krisp database
 * int kr_open (KR_API **db, char *database, char *err);
 */
extern bool kr_open (KR_API **, char *, char *);
extern bool kr_open_safe (KR_API **, char *, char *);

/*
 * close krisp database
 */
extern void kr_close (KR_API **);

/*
 * search isp information of given ip address
 */
extern int kr_search (KRNET_API *, KR_API *);
extern int kr_search_ex (KRNET_API_EX *, KR_API *);
extern int kr_range (KRNET_REQ_RANGE *, KR_API *);

extern void krisp_safecpy (char *, char *, int);
#define SAFECPY_256(dest, src) krisp_safecpy(dest, src, 256)
#define SAFECPY_512(dest, src) krisp_safecpy(dest, src, 512)
#define SAFECPY_1024(dest, src) krisp_safecpy(dest, src, 1024)
#ifndef SAFEFREE
	#define	SAFEFREE(x) if ( x != NULL ) { free (x); x = NULL; }
#endif

#ifdef __cplusplus
};
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
