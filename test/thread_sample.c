/*
 * $Id: thread_sample.c,v 1.3 2010-06-15 17:05:27 oops Exp $
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

typedef struct {
	pthread_t threads[THREAD_SIZE];
	int done[THREAD_SIZE];
} tVar;

tVar th;

typedef struct {
	int no;
	KR_API * db;
} tArg;

void * thread_main (void *);
ulong prand (void);

int main (void) { // {{{
	int i = 0, r;
	KR_API * db;
	tArg * kr;

	/* database open */
	if ( (r = kr_open_safe (&db, NULL)) > 0 ) {
		if ( r == 2 )
			fprintf (stderr, "ERROR: kr_open:: failed memory allocation\n");
		else {
			fprintf (stderr, "ERROR Connect: %s\n", db->err);
			kr_close (db);
		}
		return 1;
	}

	for ( r=0; r<THREAD_SIZE; r++ ) {
		th.done[r] = 0;

		kr = (tArg *) malloc (sizeof (tArg));
		kr->db = db;
		kr->no = r;

		pthread_create (&th.threads[r], NULL, &thread_main, (void *) kr);
		printf ("%d, %ld\n", r, th.threads[r]);
		//usleep (1);
	}

	for ( ;; ) {
		int x = 0;
		for ( i=0; i<THREAD_SIZE; i++ ) {
			if ( ! th.done[i] )
				break;
			x++;
		}
		if ( x == THREAD_SIZE )
		   break;	
	}

	kr_close (db);

	return 0;
} // }}}

void * thread_main (void *arg) { // {{{
	int tno = 0;
	char * ip;
	KRNET_API isp;

	tno = (int) ((tArg *) arg)->no;

	pthread_detach (pthread_self ());

	ip = long2ip (prand ());

	isp.verbose = 0;
	((tArg *) arg)->db->verbose= 0;
	SAFECPY_256 (isp.ip, ip);

	if ( kr_search (&isp, ((tArg *) arg)->db) ) {
		fprintf (stderr, "ERROR: %s\n", ((tArg *) arg)->db->err);
	} else
		printf ("--> Thread %d : %15s => %s\n", tno, isp.ip, isp.icode);

	th.done[tno] = 1;
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
