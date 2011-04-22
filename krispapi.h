/*
 * $Id: krispapi.h,v 1.12 2010-11-08 06:15:37 oops Exp $
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
	#define KR_LOCAL_API
#else
	#if defined(__GNUC__) && __GNUC__ >= 4
		#define KR_LOCAL_API  __attribute__ ((visibility("hidden")))
	#else
		#define KR_LOCAL_API
	#endif
#endif
#include <krdb.h>

#include <ipcalc.h>

KR_LOCAL_API bool _kr_open (KR_API **db, char *file, char *err, bool safe);

/*
 * Fill none isp data
 */
KR_LOCAL_API void kr_noneData (KRNET_API *);

/*
 * Get ISP information of each network from db
 */
KR_LOCAL_API int getISPinfo (KR_API *, RAW_KRNET_API *);

/*
 * Seperate pipe data that has isp information
 *
 */
KR_LOCAL_API short parseDummyData (char ***, char *, char delemeter);

/*
 * Convert character numeric to int numeric
 */
KR_LOCAL_API int chartoint (char);

/*
 * Convert numeric strings to long numeric
 */
KR_LOCAL_API ulong strtolong (char *);

/*
 * for thread safe
 */
KR_LOCAL_API void krisp_mutex_lock (KR_API *);
KR_LOCAL_API void krisp_mutex_unlock (KR_API *);
KR_LOCAL_API void krisp_mutex_destroy (KR_API *);

/*
 * for check krisp database changing
 */
KR_LOCAL_API bool check_database_mtime (KR_API *);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
