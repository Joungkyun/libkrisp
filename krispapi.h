/*
 * $Id: krispapi.h,v 1.12 2010-11-08 06:15:37 oops Exp $
 *
 * Notice: if return value of API that has '_f' prefix is not NULL,
 *         it is must free memory!
 */

#ifndef KR_API_H
#define KR_API_H

#include <krversion.h>
#include <krispcommon.h>
#include <krdb.h>

#include <ipcalc.h>

/*
 * Init information structure
 */
void initStruct (KRNET_API *);

/*
 * Init raw database data structure
 */
void initRawStruct (RAW_KRNET_API *, bool);
#define initStruct_ex initRawStruct

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
 * Safe copy character
 */
void _safecpy (char *, char *, int);
#endif

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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
