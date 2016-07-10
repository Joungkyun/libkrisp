<?php
/*
 * Copyright (C) 2016 JoungKyun.Kim <http://oops.org>
 * 
 * This file is part of libkrisp
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

require_once '../lib/ValidIP.php';

define ('NETWORK_',   0);
define ('CITY_',      1);
define ('LATITUDE_',  2);
define ('LONGITUDE_', 3);

define ('NETWORK',   0);
define ('BROAD',     1);
define ('CITY',      2);
define ('REGION',    3);
define ('LATITUDE',  4);
define ('LONGITUDE', 5);

$f = './hip_ip4_city_lat_lng.csv';
$SAVE = './hostip.csv-' . date ('Ymd');

$stderr = fopen ('php://stderr', 'w');
$fp = fopen ($f, 'rb');
if ( ! is_resource ($fp) ) {
	fprintf ($stderr, "ERROR: %s open error\n", $f);
	fclose ($stderr);
	exit (1);
}

$save = fopen ($SAVE, 'wb');
if ( ! is_resource ($save) ) {
	fprintf ($stderr, "ERROR: %s open error for writing\n", $SAVE);
	fclose ($fp);
	fclose ($stderr);
	exit (1);
}

$i = 0;
$o = '';
error_log ("* Read line", 0);
while ( ($line_r = fgets ($fp, 1024)) !== false ) {
	fprintf ($stderr, "  ==> %d\r", $i++);
	$line = explode (',', trim ($line_r));
	if ( ValidIP::valid ($line[NETWORK_]) === false )
		continue;

	$line[CITY_] = urldecode ($line[CITY_]);
	$pos = strpos ($line[CITY_], ',');
	if ( $pos === false )
		$region = '';
	else {
		$region = trim (substr ($line[CITY_], $pos + 1));
		$line[CITY_] = substr ($line[CITY_], 0, $pos);
	}

	$l = array (
		$line[NETWORK_],
		$line[NETWORK_] + 255,
		$line[CITY_],
		$region,
		$line[LATITUDE_],
		$line[LONGITUDE_]
	);

	if ( ! is_array ($o) ) {
		$o = $l;
		continue;
	}

	if ( $o[BROAD] + 1 == $l[NETWORK] && $o[CITY] == $l[CITY] ) {
		$o[BROAD] = $l[BROAD];
		continue;
	}

	if ( $o[NETWORK] == $l[NETWORK] ) {
		if ( ! $conflict[$o[NETWORK]] )
			$conflict[$o[NETWORK]] = 2;
		else
			$conflict[$o[NETWORK]] += 1;
		$o = $l;
		continue;
	}

	$DATA = sprintf (
		"%s\t%s\t%s|%s|%s|%s\n",
		$o[NETWORK],
		$o[BROAD],
		$o[CITY],
		$o[REGION],
		$o[LATITUDE],
		$o[LONGITUDE]
	);
	$DATA = iconv ('iso-8859-1', 'utf8', $DATA);
	fwrite ($save, $DATA, strlen ($DATA));
	$o = $l;
}
fprintf ($stderr, "\n");

$DATA = sprintf (
	"%s\t%s\t%s|%s|%s|%s\n",
	$o[NETWORK],
	$o[BROAD],
	$o[CITY],
	$o[REGION],
	$o[LATITUDE],
	$o[LONGITUDE]
);
$DATA = iconv ('iso-8859-1', 'utf8', $DATA);
fwrite ($save, $DATA, strlen ($DATA));

fclose ($save);
fclose ($fp);
fclose ($stderr);

print_r ($conflict);
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
?>
