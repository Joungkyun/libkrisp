/*
 * $Id: thread_sample.c,v 1.18 2010-09-10 12:44:26 oops Exp $
 */

#include <ipcalc.h>
#include <krisp.h>
#include <sys/time.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#define THREAD_SIZE 5

pthread_t threads[THREAD_SIZE];

typedef struct {
	int			no;
	KR_API *	db;
} tArg;

void * thread_main (void *);
ulong prand (void);

int main (void) { // {{{
	int			i = 0, r;
	long		status;
	KR_API *	db;
	tArg *		kr;
	char		err[1024];

	/* database open */
	if ( kr_open_safe (&db, NULL, err) == false ) {
		fprintf (stderr, "ERROR Connect: %s\n", err);
		kr_close (&db);
		return 1;
	}

	for ( i=0; i<THREAD_SIZE; i++ ) {
		kr = (tArg *) malloc (sizeof (tArg));
		kr->db = db;
		kr->no = i;

		pthread_create (&threads[i], NULL, &thread_main, (void *) kr);
		printf ("%d, %ld\n", i, threads[i]);
	}

	for ( i=0; i<THREAD_SIZE; i++ ) {
		r = pthread_join (threads[i], (void **) &status);
		if ( r == 0 )
			printf ("Completed join with thread %d status=%d\n", i, (int) status);
		else
			printf ("ERROR; return code from pthread_join() is %d, thread %d\n", r, i);
	}

	kr_close (&db);

	return 0;
} // }}}

void * thread_main (void * arg) { // {{{
	int			tno;
	char *		ip;
	KRNET_API	isp;

	tno = (int) ((tArg *) arg)->no;

	ip = long2ip (prand ());

	isp.verbose = false;
	((tArg *) arg)->db->verbose= false;
	SAFECPY_256 (isp.ip, ip);

	if ( kr_search (&isp, ((tArg *) arg)->db) ) {
		fprintf (stderr, "ERROR: %s\n", isp.err);
	} else
		printf ("--> Thread %d : %15s => %s\n", tno, isp.ip, isp.icode);

	free (arg);
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
