/*
 * $Id: krispcommon.h,v 1.33 2010-06-18 12:59:01 oops Exp $
 */

#ifndef KR_COMMON_H
#define KR_COMMON_H

#ifdef HAVE_CONFIG_H
#include <krisp-config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#if defined(HAVE_LIBSQLITE3)
#include <sqlite3.h>
#else
#include <sqlite.h>
#endif

#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#else
typedef enum {
	false = 0,
	true  = 1
} bool;

#define false false
#define true true
#endif

#define set_true(x) (x = true)
#define set_false(x) (x = false)
#define switch_bool(x) ((x == true) ? false : true)

typedef struct db_argument {
#if defined(HAVE_LIBSQLITE3)
	sqlite3	*		c;		// db resource
	sqlite3_stmt *	vm;		// sqlite vm
#else
	sqlite *		c;		// db resource
	sqlite_vm *		vm;		// sqlite vm
#endif
	bool			verbose;
#ifdef HAVE_PTHREAD_H
	bool			threadsafe;
#endif
	short			r;		// execute result code
	short			final;  // force finalize
	int				rows;	// vm rows
	int				cols;	// number of columns
#if defined(HAVE_LIBSQLITE)
	char *			dberr;	// vm error message
#endif
	char 			err[1024];
	char *			table;
	char *			old_table;
	const char **	rowdata;
	const char **	colname;
} KR_API;

typedef struct raw_netinfos {
	bool			verbose;
	char			ip[16];
	ulong			start;
	ulong			end;
	short			size;
	char			err[1024];
	char **			dummy;
	char *			dummydata;
} RAW_KRNET_API;

#define KRNET_API_EX RAW_KRNET_API

typedef struct netinfos {
	bool			verbose;
	char			ip[256];
	ulong			netmask;
	ulong			start;
	ulong			end;
	char			icode[64];
	char			iname[64];
	char			ccode[4];
	char			cname[64];
	char			err[1024];
} KRNET_API;

struct cinfo {
	ulong	ip;
	ulong	mask;
	ulong	network;
	ulong	bcast;
};

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
