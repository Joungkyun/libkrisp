/*
 * Copyright (c) 2016 JoungKyun.Kim <http://oops.org> all rights reserved
 *
 * This file is part of libkrisp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as published
 * by the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU LESSER GENERAL PUBLIC LICENSE for more details.
 *
 * You should have received a copy of the GNU LESSER GENERAL PUBLIC LICENSE
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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
	int			i, r;
	long		status;
	KR_API *	db[THREAD_SIZE];
	tArg *		kr;
	char		err[1024];

	/* database open */
	for ( i=0; i<THREAD_SIZE; i++ ) {
		if ( kr_open (&db[i], NULL, err) == false ) {
			fprintf (stderr, "ERROR Connect: %s\n", err);

			for ( r=0; r<=i; r++ )
				kr_close (&db[r]);
			return 1;
		}
	}

	for ( i=0; i<THREAD_SIZE; i++ ) {
		kr = (tArg *) malloc (sizeof (tArg));
		kr->db = db[i];
		kr->no = i;

		pthread_create (&threads[i], NULL, &thread_main, (void *) kr);
		printf ("%d, %ld\n", i, threads[i]);
	}

	for ( i=0; i<THREAD_SIZE; i++ ) {
		r = pthread_join (threads[i], (void **) &status);
		if ( r == 0 )
			printf ("Completed join with thread %d status= %d\n", i, (int) status);
		else
			printf ("ERROR; return code from pthread_join() is %d, thread %d\n", r, i);
	}

	return 0;
} // }}}

void * thread_main (void * arg) { // {{{
	int tno = 0;
	char * ip;
	KRNET_API isp;

	tno = (int) ((tArg *) arg)->no;

	ip = long2ip (prand ());

	isp.verbose = false;
	((tArg *) arg)->db->verbose= false;
	SAFECPY_256 (isp.ip, ip);

	if ( kr_search (&isp, ((tArg *) arg)->db) ) {
		fprintf (stderr, "ERROR: %s\n", isp.err);
	} else
		printf ("--> Thread %d : %15s => %s\n", tno, isp.ip, isp.icode);

	kr_close (&((tArg *) arg)->db);
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
