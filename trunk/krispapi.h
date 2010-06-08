/*
 * $Id: krispapi.h,v 1.2 2010-06-08 03:05:09 oops Exp $
 *
 * Notice: if return value of API that has '_f' prefix is not NULL,
 *         it is must free memory!
 */

#ifndef KR_API_H
#define KR_API_H

#include <krversion.h>
#include <krispcommon.h>
#include <krdb.h>
#include <krnet.h>

/*
 * Init information structure
 */
void initStruct (KRNET_API *);

/*
 * Init raw database data structure
 */
void initRawStruct (RAW_KRNET_API *, int);
#define initStruct_ex initRawStruct

/*
 * Fill none isp data
 */
void kr_noneData (KRNET_API *);

/*
 * Get ISP information of each network from db
 */
int getISPinfo (KR_API *, RAW_KRNET_API *);

/*
 * Seperate pipe data that has isp information
 *
 */
int parseDummyData (char ***, char *, char delemeter);

/*
 * Convert character numeric to int numeric
 */
int chartoint (char);

/*
 * Convert numeric strings to long numeric
 */
ulong strtolong (char *);

/*
 * Safe copy character
 */
void _safecpy (char *, char *, int);
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
