/*
 * $Id: krispcommon.h,v 1.38 2010-06-30 18:40:34 oops Exp $
 */

#ifndef KR_COMMON_H
#define KR_COMMON_H

#ifdef HAVE_CONFIG_H
#include <krisp-config.h>
#endif

#if defined _WIN32 || defined __CYGWIN__
	#define KR_LOCAL_API
#else
	#if defined(__GNUC__) && __GNUC__ >= 4
		#define KR_LOCAL_API  __attribute__ ((visibility("hidden")))
	#else
		#define KR_LOCAL_API
	#endif
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
	char 			err[1024];
#ifdef HAVE_PTHREAD_H
	pthread_mutex_t	mutex;
#endif
#if defined(HAVE_LIBSQLITE3)
	sqlite3	*		c;		// db resource
	sqlite3_stmt *	vm;		// sqlite vm
#else
	sqlite *		c;		// db resource
	sqlite_vm *		vm;		// sqlite vm
#endif
	const char **	rowdata;
	const char **	colname;
	char *			table;
	char *			old_table;
#if defined(HAVE_LIBSQLITE)
	char *			dberr;	// vm error message
#endif
	int				rows;	// vm rows
	int				cols;	// number of columns
	bool			verbose;
#ifdef HAVE_PTHREAD_H
	bool			threadsafe;
#endif
	short			r;		// execute result code
	short			final;  // force finalize
} KR_API;

typedef struct raw_netinfos {
	char			err[1024];
	char			ip[16];
	char **			dummy;
	char *			dummydata;
	ulong			start;
	ulong			end;
	bool			verbose;
	short			size;
} RAW_KRNET_API;

#define KRNET_API_EX RAW_KRNET_API

typedef struct netinfos {
	char			err[1024];
	char			ip[256];
	char			icode[64];
	char			iname[64];
	char			cname[64];
	char			ccode[4];
	ulong			netmask;
	ulong			start;
	ulong			end;
	bool			verbose;
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
