/*
 * $Id: krisp.h,v 1.30 2010-09-10 11:09:14 oops Exp $
 */

#ifndef KR_ISP_H
#define KR_ISP_H

#include <krispcommon.h>

/*
 * KRISP user level function prototype
 */

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

extern void _safecpy (char *, char *, int);
#define SAFECPY_256(dest, src) _safecpy(dest, src, 256)
#define SAFECPY_512(dest, src) _safecpy(dest, src, 512)
#define SAFECPY_1024(dest, src) _safecpy(dest, src, 1024)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
