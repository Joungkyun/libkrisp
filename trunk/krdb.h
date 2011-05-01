/*
 * $Id$
 */

#ifndef KRDB_H
#define KRDB_H

#ifndef DBPATH
#include <krisp-config.h>
#endif

#define DEFAULT_DATABASE DBPATH "/krisp.dat"

void	kr_dbError (KR_API *);
void	kr_dbErrorClear (KR_API *);
int		kr_dbFree (KR_API *);
bool	kr_dbConnect (KR_API *);
int		kr_dbQuery (KR_API *, char *);
int		kr_dbFetch (KR_API *);
int		kr_dbExecute (KR_API *, char *);
void	kr_dbFinalize (KR_API *);
void	kr_dbClose (KR_API *);
char *	kr_dbQuote_f (char *);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
