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

#include <krisp.h>

int main (void) {
	KR_API *		db;
	KRNET_API_EX	isp;
	char *			ip;
	int				ret = 0;
	bool			verbose = false;
	char			err[1024];

	/* database open */
	if ( kr_open_safe (&db, NULL, err) == false ) {
		printf ("FAIL: kr_open failed (%s)\n", err);
		return 1;
	}

	db->db_time_stamp_interval = 0;

	isp.verbose = false;

	SAFECPY_256 (isp.ip, "1.1.1.1");
	db->table = "krisp";
	if ( kr_search_ex (&isp, db) ) {
		printf ("FAIL: kr_search_ex search failed\n");
		kr_close (&db);
		return 1;
	}

	if ( strcmp (isp.dummy[0], "AU") == 0 )
		printf ("PASS: 1.1.1.1 == AU\n");
	else {
		printf ("FAIL: 1.1.1.1 != AU (%s)\n", isp.dummy[0]);
		ret = 1;
	}

	initStruct_ex (&isp, true);

	/* database close */
	kr_close (&db);

	return ret;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
