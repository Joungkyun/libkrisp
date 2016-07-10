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
 *
 * Notice: if return value of API that has '_f' prefix is not NULL,
 *         it is must free memory!
 */

#ifndef KR_API_H
#define KR_API_H

#ifdef HAVE_CONFIG_H
    #include <krisp-config.h>
#endif

#include <krversion.h>
#include <krisp.h>

#if defined _WIN32 || defined __CYGWIN__
	#ifdef HAVE_DLL_EXPORT
		#define KRISP_API __declspec(dllexport)
	#else
		#define KRISP_API __declspec(dllimport)
	#endif
#else
	#if HAVE_VISIBILITY
		#define KRISP_API __attribute__ ((visibility("default")))
	#else
		#define KRISP_API
	#endif
#endif
#include <krdb.h>

#include <ipcalc.h>

#define KRISP_GET_COUNTRY 0
#define KRISP_GET_ISP     1

bool _kr_open (KR_API **db, char *file, char *err, bool safe);

/*
 * Fill none isp data
 */
void kr_noneData (KRNET_API *);

/*
 * get ip address range of Country or ISP
 */
int getRange (KR_API *, KRNET_REQ_RANGE *);

/*
 * Get ISP information of each network from db
 */
int getISPinfo (KR_API *, RAW_KRNET_API *);

/*
 * Seperate pipe data that has isp information
 *
 */
short parseDummyData (char ***, char *, char delemeter);

/*
 * Convert character numeric to int numeric
 */
int chartoint (char);

/*
 * Convert numeric strings to long numeric
 */
ulong strtolong (char *);

/*
 * for thread safe
 */
void krisp_mutex_lock (KR_API *);
void krisp_mutex_unlock (KR_API *);
void krisp_mutex_destroy (KR_API *);

/*
 * for check krisp database changing
 */
bool check_database_mtime (KR_API *);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
