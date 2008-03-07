/*
 * $Id: krisp.c,v 1.62 2008-03-07 10:02:01 oops Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifdef HAVE_LIBGEOIP
#define GEOCITYVAR
#endif

#include <krispcommon.h>
#include <krdb.h>
#include <krisp.h>

#ifdef HAVE_LIBGEOIP
/* set 1, search GeoIPCity database if enabled search GeoIPCity */
short geocity      = 0;
short geocity_type = GEOIP_INDEX_CACHE | GEOIP_CHECK_CACHE;
short geoisp_type  = GEOIP_INDEX_CACHE | GEOIP_CHECK_CACHE;
short geo_type     = GEOIP_MEMORY_CACHE | GEOIP_CHECK_CACHE;
#endif

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

	if ( kr_dbQuery (db, sql, DBTYPE_KRISP) )
		return 1;

	n->nums = 0;
	db->rows = 0;
	db->cols = 0;
	while ( ! (r = kr_dbFetch (db, DBTYPE_KRISP) ) ) {
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

	if  ( kr_dbQuery (db, sql, DBTYPE_KRISP) )
		return 1;

	db->rows = 0;
	db->cols = 0;
	while ( ! (r = kr_dbFetch (db, DBTYPE_KRISP) ) ) {
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

int getHostIP (KR_API *db, char *ip, USERDB *h) {
	char sql[64] = { 0, };
	char net[16] = { 0, };
	int r;
	struct cinfo cp;
	char *city;
	char *reg;

	memset (h->ccode, 0, sizeof (h->ccode));
	memset (h->cname, 0, sizeof (h->cname));
	memset (h->icode, 0, sizeof (h->icode));
	memset (h->iname, 0, sizeof (h->iname));
	memset (h->city, 0, sizeof (h->city));
	memset (h->region, 0, sizeof (h->region));
	h->flag = 0;

	if ( db->h == NULL )
		return 1;

	cp.ip = ip2long (ip);
	cp.mask = ip2long ("255.255.255.0");
	cp.network = cp.ip & cp.mask;
	sprintf (net, "%lu", cp.network);

	sprintf (sql, "SELECT * FROM userdb WHERE longip = '%s'", net);

	if ( kr_dbQuery (db, sql, DBTYPE_USERDB) )
		return 1;

	db->rows = 0;
	db->cols = 0;
	while ( ! (r = kr_dbFetch (db, DBTYPE_USERDB)) ) {
		for ( r=0; r<db->cols; r++ ) {
			switch (r) {
				case 1 :
					strcpy (h->ccode, db->rowdata[r] ? db->rowdata[r] : "");
					break;
				case 2 :
					strcpy (h->cname, db->rowdata[r] ? db->rowdata[r] : "");
					break;
				case 3 :
					strcpy (h->icode, db->rowdata[r] ? db->rowdata[r] : "");
					break;
				case 4 :
					strcpy (h->iname, db->rowdata[r] ? db->rowdata[r] : "");
					break;
				case 5 :
					strcpy (h->city, db->rowdata[r] ? db->rowdata[r] : "");
					break;
				case 6 :
					strcpy (h->region, db->rowdata[r] ? db->rowdata[r] : "");
					break;
				case 7 :
					h->flag = (! strncmp (db->rowdata[r], "0", 1) ) ? 0 : 1;
					break;
				default:
					continue;
			}
		}
		kr_dbFree (db);
	}

	if ( ! strlen (h->icode) && strlen (h->iname) )
		strcpy (h->icode, h->iname);

	if ( strlen (h->city) ) {
		city = (char *) strdup (h->city);
		if ( (reg = strchr (city, ',')) != NULL ) {
			strcpy (h->region, reg + 1);
			city[reg - city] = 0;
			strcpy (h->city, city);
		}
		free (city);
	}

	if ( r == -1 )
		return 1;

	return 0;
}

void kr_close (KR_API *db) {
#ifdef HAVE_LIBGEOIP
	if ( db->gi != NULL ) {
		int i;
		if ( db->gi->gid != NULL ) {
			GeoIP_delete (db->gi->gid);

			if ( db->gi->gic != NULL )
				GeoIP_delete (db->gi->gic);

			if ( db->gi->gip != NULL )
				GeoIP_delete (db->gi->gip);

			free (db->gi);

			for ( i = 1; i <= 11 ; i++ )
				free (GeoIPDBFileName[i]);
			free (GeoIPDBFileName);
			GeoIPDBFileName = NULL;
		}
	}
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
	strcpy (n->ccode, "--");
	strcpy (n->cname, "N/A");
	strcpy (n->city, "N/A");
	strcpy (n->region, "N/A");
}

int kr_search (KRNET_API *isp, KR_API *db) {
	int r = 0, i;
	char * aclass;
	char * aclass_tmp;

	struct netmasks n;
	struct cinfo cp;

	struct hostent *hp;
	struct in_addr s;

	USERDB h;

	initStruct (isp);

	hp = gethostbyname(isp->ip);

	if ( hp && *(hp->h_addr_list) ) {
		memcpy(&s.s_addr, *(hp->h_addr_list), sizeof(s.s_addr));
		strcpy (isp->ip, inet_ntoa (s));
	}

	if ( inet_addr (isp->ip) == -1 ) {
		strcpy (isp->icode, "--");
		strcpy (isp->iname, "N/A");
		strcpy (isp->ccode, "--");
		strcpy (isp->cname, "N/A");
		strcpy (isp->city, "N/A");
		strcpy (isp->region, "N/A");
		return 0;
	}

	aclass = (char *) strdup (isp->ip);
	aclass_tmp = (char *) strchr (aclass, '.');
	if ( aclass_tmp == NULL ) {
		strcpy (isp->icode, "--");
		strcpy (isp->iname, "N/A");
		strcpy (isp->ccode, "--");
		strcpy (isp->cname, "N/A");
		strcpy (isp->city, "N/A");
		strcpy (isp->region, "N/A");
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
			/*
			 * if ip is big broadcast, this is miss match
			 */
			if ( ip2long (isp->ip) <= ip2long (isp->broadcast) ) {
				r = 1;
				break;
			}
		}
	}

	kr_free_array (n.mask);

geoip_section:
#ifdef HAVE_LIBGEOIP
	if ( db->gi != NULL ) {
		int country_id = 0;

		if ( db->gi->gid == NULL )
			goto geoispend;

		country_id = GeoIP_id_by_name (db->gi->gid, isp->ip);
		strcpy (isp->ccode,
				GeoIP_country_code[country_id] ? GeoIP_country_code[country_id] : "--");
		strcpy (isp->cname,
				GeoIP_country_name[country_id] ? GeoIP_country_name[country_id] : "N/A");

		/* manipulated geoip null data */
		if ( ! strcmp (isp->ccode, "--") && strlen (isp->icode) ) {
			strcpy (isp->ccode, "KR");
			strcpy (isp->cname, "Korea, Republic of");
		}
geoispend:

		/* check city information
		 * GEOIP_CITY_EDITION_REV0 2
		 * GEOIP_CITY_EDITION_REV1 6
		 */
		if ( db->gi->gic == NULL )
			goto geocityend;

		if ( GeoIP_db_avail (GEOIP_CITY_EDITION_REV0) || GeoIP_db_avail (GEOIP_CITY_EDITION_REV1) ) {
			GeoIPRecord *gir;
			gir = GeoIP_record_by_name (db->gi->gic, isp->ip);

			if ( gir != NULL && gir->city )
				strcpy (isp->city, strlen (gir->city) ? gir->city : "N/A");
			else
				strcpy (isp->city, "N/A");

			if ( gir != NULL && gir->region )
				strcpy (isp->region, strlen (gir->region) ? gir->region : "N/A");
			else
				strcpy (isp->region, "N/A");

			if ( gir != NULL )
				GeoIPRecord_delete (gir);
		}
	}
geocityend:

#else
	strcpy (isp->ccode, strlen (isp->icode) ? "KR" : "");
	strcpy (isp->cname, strlen (isp->icode) ? "Korea, Republic of" : "");
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
				strcpy (isp->icode, strlen (g_isp) ? g_isp : "N/A");
				strcpy (isp->iname, strlen (g_isp) ? g_isp : "N/A");
				free ((char *) g_isp);
			}
		} else {
#endif
			strcpy (isp->icode, "--");
			strcpy (isp->iname, "N/A");
#ifdef HAVE_LIBGEOIP
		}
#endif
	}

	/* get HostIP */
	getHostIP (db, isp->ip, &h);

	if ( h.flag ) {
		if ( strlen (h.ccode) )
			strcpy (isp->ccode, h.ccode);

		if ( strlen (h.cname) )
			strcpy (isp->cname, h.cname);

		if ( strlen (h.icode) )
			strcpy (isp->icode, h.icode);

		if ( strlen (h.iname) ) {
			if ( ! strcmp (isp->icode, "--") )
				strcpy (isp->icode, h.iname);

			strcpy (isp->iname, h.iname);
		}

		if ( strlen (h.city) ) {
			strcpy (isp->city, h.city);
			strcpy (isp->region, strlen (h.region) ? h.region : "N/A");
		}
	} else {
		if ( ! strcmp (isp->ccode, "--") && strlen (h.ccode) )
			strcpy (isp->ccode, h.ccode);

		if ( ! strcmp (isp->cname, "N/A") && strlen (h.cname) )
			strcpy (isp->cname, h.cname);

		if ( ! strcmp (isp->icode, "--") && strlen (h.icode) )
			strcpy (isp->icode, h.icode);

		if ( ! strcmp (isp->iname, "N/A") && strlen (h.iname) ) {
			if ( ! strcmp (isp->icode, "--") )
				strcpy (isp->icode, h.iname);

			strcpy (isp->iname, h.iname);
		}

		if ( ! strcmp (isp->city, "N/A") && strlen (h.city) ) {
			strcpy (isp->city, h.city);
			strcpy (isp->region, strlen (h.region) ? h.region : "N/A");
		}
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
