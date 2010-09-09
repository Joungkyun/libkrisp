/*
 * $Id: krisp.h,v 1.27 2010-09-09 19:23:46 oops Exp $
 */

#ifndef KR_ISP_H
#define KR_ISP_H

#include <krversion.h>
#include <krispcommon.h>
#include <krispapi.h>

/*
 * KRISP user level function prototype
 */

/*
 * print krisp version
 */
char * krisp_version (void);
char * krisp_uversion (void);

/*
 * open krisp database
 * int kr_open (KR_API **db, char *database, char *err);
 */
KR_LOCAL_API bool _kr_open (KR_API **db, char *file, char *err, bool safe);
bool kr_open (KR_API **, char *, char *);
bool kr_open_safe (KR_API **, char *, char *);

/*
 * close krisp database
 */
void kr_close (KR_API **);

/*
 * search isp information of given ip address
 */
int kr_search (KRNET_API *, KR_API *);
int kr_search_ex (KRNET_API_EX *, KR_API *);

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
