<?php
/*
 * Generate GeoIP legacy datebase (GeoIPCountryWhois.csv)
 *
 * This script creates a GeoIP legacy database with GeoIP2 database.
 *
 * Copyright (C) 2018 JoungKyun.Kim <http://oops.org>
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


require_once 'ipcalc.php';

function usage ($prog) {
	echo <<<EOF
Usage: {$prog} GEOIP2_CSV_DIR

EOF;
	exit (1);
}

define ('LC_ID', 0);
define ('LC_CCODE', 4);
define ('LC_CNAME', 5);

function location_db ($f) {
	$fd = fopen ($f, 'rb');

	while ( ! feof ($fd) ) {
		$line = fgets ($fd, 2048);
		if ( strncmp ($line, 'geoname_id', 10) == 0 )
			continue;

		$r = preg_split ('/,/', trim (str_replace ('"', '', $line)));
		if ( ! $r[0] )
			continue;

		$ret[$r[0]] = (object) [
			'code' => $r[4] ? $r[4] : $r[2],
			'name' => $r[5] ? $r[5] : $r[3],
		];
	}
	fclose ($fd);

	return $ret;
}

function range_db ($f, $loc) {
	$fd = fopen ($f, 'rb');

	$l = new stdClass;
	while ( ! feof ($fd) ) {
		$line = fgets ($fd);

		if ( strncmp ($line, 'network,', 8) ==  0 )
			continue;
		
		#network, geoname_id, registered_country_geoname_id, represented_country_geoname_id, is_anonymous_proxy, is_satellite_provider
		$r = preg_split ('/,/', $line);

		list ($ip, $mask) = preg_split ('!/!', $r[0]);
		$mask = IPCALC::prefix2mask ($mask);
		$network = IPCALC::network ($ip, $mask);
		$broadcast = IPCALC::broadcast ($ip, $mask);

		$geoid = $r[3] ? $r[3] : ($r[2] ? $r[2] : $r[1]);

		$l->code = $loc[$geoid]->code;
		$l->name = $loc[$geoid]->name;

		if ( $r[4] == 1 ) {
			$l->code = 'A1';
			$l->name = 'Anonymous Proxy';
		} else if ( $r[5] == 1 ) {
			$l->code = 'A2';
			$l->name = 'Satellite Provider';
		}

		printf (
			'"%s","%s","%s","%s","%s","%s"' . PHP_EOL,
			$network, $broadcast, IPCALC::ip2long ($network), IPCALC::ip2long ($broadcast), $l->code, $l->name
		);
	}
}

if ( ! is_dir ($argv[1]) ) {
	echo "GeoIP2 cvs directory not found\n";
	exit (1);
}

$curdir = getcwd ();
chdir ($argv[1]);

# IP range csv
$range = "GeoLite2-Country-Blocks-IPv4.csv";
$desc  = "GeoLite2-Country-Locations-en.csv";

if ( ! file_exists ($range) || ! file_exists ($desc) ) {
	echo "Can not find $range or $desc\n";
	exit (1);
}


$ldb = location_db ($desc);

#print_r ($ldb);

$rdb = range_db ($range, $ldb);

chdir ($curdir);

exit (0);
?>
