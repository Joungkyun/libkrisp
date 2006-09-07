/*
 * $Id: krisp.c,v 1.34 2006-09-07 12:52:47 oops Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <krispcommon.h>
#include <krdb.h>
#include <krisp.h>

extern char dberr[1024];

char *krisp_version (void) {
	return KRISP_VERSION;
}

char *krisp_uversion (void) {
	return KRISP_UVERSION;
}

int kr_open (KR_API *db, char *file) {
	if ( kr_dbConnect (db, file) ) {
		return 1;
	}

	db->gi = krGeoIP_open (db);

	return 0;
}

int kr_netmask (KR_API *db, char *aclass, struct netmasks *n) {
	char sql[64] = { 0, };
	char **masks;
	int r;
	int i;

	masks = (char **) malloc (sizeof (char *) * 32);
	n->mask = masks;
	*masks = NULL;

	sprintf (sql, "SELECT subnet FROM netmask WHERE net = '%s'", aclass);

	if ( kr_dbQuery (db, sql) )
		return 1;

	n->nums = 0;
	db->rows = 0;
	db->cols = 0;
	while ( ! (r = kr_dbFetch (db) ) ) {
		for ( i=0; i<db->cols; i++ ) {
			if ( n->nums > 31 && (n->nums % 32 ) == 0 ) {
				char ** new;
				new = (char **) realloc (n->mask, sizeof (char *) * (32 * ((n->nums / 32) + 1)));
				if ( new == NULL ) {
					kr_free_array (masks);
					strcpy (dberr, "masks relocation failed\n");
					return 1;
				}

				n->mask = new;
			}

			//*masks = (char *) malloc (sizeof (char) * 16);
			//strcpy (*masks++, db->rowdata[i]);
			*masks++ = (char *) strdup (db->rowdata[i]);
		}
		n->nums++;
		kr_dbFree (db);
		//printf ("### %d\n", n->nums);
	}
	*masks = NULL;

	if ( r == -1 )
		return 1;

	return 0;
}

int getISPinfo (KR_API *db, char *key, KRNET_API *n) {
	char sql[64] = { 0, };
	int r;

	sprintf (sql, "SELECT * FROM isp WHERE longip = '%s'", key);

	if  ( kr_dbQuery (db, sql) )
		return 1;

	db->rows = 0;
	db->cols = 0;
	while ( ! (r = kr_dbFetch (db) ) ) {
		for ( r=0; r<db->cols; r++ ) {
			switch (r) {
				case 0 :
					//strcpy (n->key, db->rowdata[r]);
					break;
				case 1 :
					strcpy (n->network, db->rowdata[r]);
					break;
				case 2 :
					strcpy (n->broadcast, db->rowdata[r]);
					break;
				case 3 :
					strcpy (n->netmask, db->rowdata[r]);
					break;
				case 4 :
					strcpy (n->iname, db->rowdata[r]);
					break;
				case 5 :
					strcpy (n->icode, db->rowdata[r]);
					break;
			}
		}
		kr_dbFree (db);
	}

	if ( r == -1 )
		return 1;

	return 0;
}

void kr_close (KR_API *db) {
#ifdef HAVE_LIBGEOIP
	if ( db->gi != NULL ) {
		if ( db->gi->gid != NULL )
			GeoIP_delete (db->gi->gid);

		if ( db->gi->gic != NULL )
			GeoIP_delete (db->gi->gic);

		if ( db->gi->gip != NULL )
			GeoIP_delete (db->gi->gip);
	}
	free (db->gi);
#endif

	kr_dbClose (db);
}

void kr_free_array (char **p) {
	char **pp;

	pp = p;

	while ( *pp != NULL )
		free (*pp++);

	free (p);
}

void initStruct (KRNET_API *n) {
	strcpy (n->key, "");
	strcpy (n->network, "");
	strcpy (n->broadcast, "");
	strcpy (n->netmask, "");
	strcpy (n->icode, "");
	strcpy (n->iname, "");
#ifdef HAVE_LIBGEOIP
	strcpy (n->gcode, "");
	strcpy (n->gname, "");
	strcpy (n->gcity, "");
#endif
}

int kr_search (KRNET_API *isp, KR_API *db) {
	int r = 0, i;
	char * aclass;
	char * aclass_tmp;

	struct netmasks n;
	struct cinfo cp;

	struct hostent *hp;
	struct in_addr s;

	initStruct (isp);

	hp = gethostbyname(isp->ip);

	if ( hp && *(hp->h_addr_list) ) {
		memcpy(&s.s_addr, *(hp->h_addr_list), sizeof(s.s_addr));
		strcpy (isp->ip, inet_ntoa (s));
	}

	if ( inet_addr (isp->ip) == -1 ) {
		strcpy (isp->icode, "--");
		strcpy (isp->iname, "N/A");
#ifdef HAVE_LIBGEOIP
		strcpy (isp->gcode, "--");
		strcpy (isp->gname, "N/A");
		strcpy (isp->gcity, "N/A");
#endif
		return 0;
	}

	aclass = (char *) strdup (isp->ip);
	aclass_tmp = (char *) strchr (aclass, '.');
	if ( aclass_tmp == NULL ) {
		strcpy (isp->icode, "--");
		strcpy (isp->iname, "N/A");
#ifdef HAVE_LIBGEOIP
		strcpy (isp->gcode, "--");
		strcpy (isp->gname, "N/A");
		strcpy (isp->gcity, "N/A");
#endif
		return 0;
	}
	aclass[aclass_tmp - aclass] = 0;

	if ( kr_netmask (db, aclass, &n) ) {
		r = 0;
		free (aclass);
		kr_free_array (n.mask);
		goto geoip_section;
	}

	free (aclass);

	if ( ! n.nums ) {
		r = 0;
		kr_free_array (n.mask);
		goto geoip_section;
	}

	cp.ip = ip2long (isp->ip);

	for ( i=0; i<n.nums; i++ ) {
		unsigned long compare;

		cp.mask = ip2long (n.mask[i]);
		cp.network = cp.ip & cp.mask;
		sprintf (isp->key, "%lu", cp.network);

		if ( getISPinfo (db, isp->key, isp) )
			continue;

		compare = cp.ip & ip2long (isp->netmask);

		if ( cp.network == compare ) {
			r = 1;
			break;
		}
	}

	kr_free_array (n.mask);

geoip_section:
#ifdef HAVE_LIBGEOIP
	if ( db->gi != NULL ) {
		int country_id = 0;

		country_id = GeoIP_id_by_name (db->gi->gid, isp->ip);
		strcpy (isp->gcode,
				GeoIP_country_code[country_id] ? GeoIP_country_code[country_id] : "--");
		strcpy (isp->gname,
				GeoIP_country_name[country_id] ? GeoIP_country_name[country_id] : "N/A");

		/* manipulated geoip null data */
		if ( ! strcmp (isp->gcode, "--") && strlen (isp->icode) ) {
			strcpy (isp->gcode, "KR");
			strcpy (isp->gname, "Korea, Republic of");
		}

		/* check city information
		 * GEOIP_CITY_EDITION_REV0 2
		 * GEOIP_CITY_EDITION_REV1 6
		 */
		_GeoIP_setup_dbfilename();
		if ( GeoIP_db_avail (GEOIP_CITY_EDITION_REV0) || GeoIP_db_avail (GEOIP_CITY_EDITION_REV1) ) {
			GeoIPRecord *gir;
			gir = GeoIP_record_by_name (db->gi->gic, isp->ip);

			if ( gir != NULL && gir->city) {
				if ( gir->region && ! atoi (gir->region) )
					sprintf (isp->gcity, "%s %s", gir->region, gir->city);
				else
					strcpy (isp->gcity, gir->city);
			} else
				strcpy (isp->gcity, "UnKnown");
		}
	}
#endif

	if ( r == 0 || ! strlen (isp->icode) ) {
#ifdef HAVE_LIBGEOIP
		if ( GeoIP_db_avail (GEOIP_ISP_EDITION) ) {
			const char * g_isp;
			g_isp = GeoIP_org_by_name(db->gi->gip, isp->ip);
			if ( g_isp == NULL ) {
				strcpy (isp->icode, "--");
				strcpy (isp->iname, "N/A");
			} else {
				strcpy (isp->icode, "--");
				strcpy (isp->iname, g_isp);
			}
		} else {
#endif
			strcpy (isp->icode, "--");
			strcpy (isp->iname, "N/A");
#ifdef HAVE_LIBGEOIP
		}
#endif
	}

	return 0;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
