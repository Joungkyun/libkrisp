/*
 * $Id: thread_sample2.c,v 1.2 2010-06-15 18:55:15 oops Exp $
 */

#include <krisp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include <sys/time.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#define THREAD_SIZE 5

pthread_t threads[THREAD_SIZE];

void * thread_main (void *);
ulong prand (void);

int main (void) { // {{{
	int	i, r;
	int	status;

	for ( i=0; i<THREAD_SIZE; i++ ) {
		pthread_create (&threads[i], NULL, &thread_main, (void *) i);
		printf ("%d, %ld\n", i, threads[i]);
	}

	for ( i=0; i<THREAD_SIZE; i++ ) {
		r = pthread_join (threads[i], (void **) &status);
		if ( r == 0 )
			printf("Completed join with thread %d status= %d\n",i, status);
		else
			printf("ERROR; return code from pthread_join() is %d, thread %d\n", r, i);
	}

	return 0;
} // }}}

void * thread_main (void * arg) { // {{{
	int			tno, r;
	char *		ip;
	KR_API *	db;
	KRNET_API	isp;
	char		err[1024];

	tno = (int) arg;

	ip = long2ip (prand ());

	/* database open */
	if ( (r = kr_open (&db, NULL, err)) > 0 ) {
		fprintf (stderr, "ERROR Connect: %s\n", err);
		kr_close (db);
		return 1;
	}

	isp.verbose = 0;
	SAFECPY_256 (isp.ip, ip);

	if ( kr_search (&isp, db) ) {
		fprintf (stderr, "ERROR: %s\n", isp.err);
	} else
		printf ("--> Thread %d : %15s => %s\n", tno, isp.ip, isp.icode);

	kr_close (db);
	pthread_exit ((void *) 0);
} // }}}

ulong prand (void) { // {{{
	int c;
	ulong p;

retry:
	c = time (0) % 2;
	p = random ();
	if ( c == 0 )
		p *= 2;

	if ( p < 16777216 )
		goto retry;

	return p;
} // }}}
