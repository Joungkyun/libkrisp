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

Class IPv6 {
	#
	# https://stackoverflow.com/questions/18276757/php-convert-ipv6-to-number
	#
	static function ip2long($ip) {
		$ip_n = inet_pton($ip);
		$bin = '';
		for ($bit = strlen($ip_n) - 1; $bit >= 0; $bit--) {
			$bin = sprintf('%08b', ord($ip_n[$bit])) . $bin;
		}

		if (function_exists('gmp_init')) {
			return gmp_strval(gmp_init($bin, 2), 10);
		} elseif (function_exists('bcadd')) {
			$dec = '0';
			for ($i = 0; $i < strlen($bin); $i++) {
				$dec = bcmul($dec, '2', 0);
				$dec = bcadd($dec, $bin[$i], 0);
			}
			return $dec;
		} else {
			trigger_error('GMP or BCMATH extension not installed!', E_USER_ERROR);
		}
	}

	static function long2ip($dec) {
		if (function_exists('gmp_init')) {
			$bin = gmp_strval(gmp_init($dec, 10), 2);
		} elseif (function_exists('bcadd')) {
			$bin = '';
			do {
				$bin = bcmod($dec, '2') . $bin;
				$dec = bcdiv($dec, '2', 0);
			} while (bccomp($dec, '0'));
		} else {
			trigger_error('GMP or BCMATH extension not installed!', E_USER_ERROR);
		}

		$bin = str_pad($bin, 128, '0', STR_PAD_LEFT);
		$ip = array();
		for ($bit = 0; $bit <= 7; $bit++) {
			$bin_part = substr($bin, $bit * 16, 16);
			$ip[] = dechex(bindec($bin_part));
		}
		$ip = implode(':', $ip);
		return inet_ntop(inet_pton($ip));
	}

	#
	# https://raw.githubusercontent.com/djamps/php-ipv6-calculator/master/ipcalc.php
	#
	function network ($ip, $mask) {
		$host_c = inet_pton($ip);
		$mask_c = self::_cdr2Char($mask, strlen($host_c));

		$net_c = $host_c & $mask_c; // Supernet network address
		$bcst_c = $net_c | ~$mask_c; // Supernet broadcast

		return (object) array (
			'network' => inet_ntop ($net_c),
			'broadcast' => inet_ntop ($bcst_c),
		);
	}

	private function _cdr2Bin ($cdrin,$len=4){
		if ( $len > 4 || $cdrin > 32 ) { // Are we ipv6?
			return str_pad(str_pad("", $cdrin, "1"), 128, "0");
		} else {
			return str_pad(str_pad("", $cdrin, "1"), 32, "0");
		}
	}

	private function _cdr2Char ($cdrin,$len=4){
		$hex = self::_bin2Hex(self::_cdr2Bin($cdrin,$len));
		return self::_hex2Char($hex);
	}

	private function _hex2Char($hex){
		return pack('H*',$hex);
	}

	private function _bin2Hex($bin){
		$hex='';
		for($i=strlen($bin)-4;$i>=0;$i-=4)
			$hex.=dechex(bindec(substr($bin,$i,4)));
		return strrev($hex);
	}
}

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
	$v6 = preg_match ('/IPv6/', $f);
	$fd = fopen ($f, 'rb');

	$l = new stdClass;
	while ( ! feof ($fd) ) {
		$line = fgets ($fd);

		if ( ! trim ($line) || strncmp ($line, 'network,', 8) ==  0 )
			continue;
		
		#network, geoname_id, registered_country_geoname_id, represented_country_geoname_id, is_anonymous_proxy, is_satellite_provider
		$r = preg_split ('/,/', $line);

		list ($ip, $mask) = preg_split ('!/!', $r[0]);
		if ( $v6 ) {
			$r1 = IPv6::network ($ip, $mask);
			$network = $r1->network;
			$broadcast = $r1->broadcast;
			$lnetwork = IPv6::ip2long ($r1->network);
			$lbroadcast = IPv6::ip2long ($r1->broadcast);
		} else {
			$mask = IPCALC::prefix2mask ($mask);
			$network = IPCALC::network ($ip, $mask);
			$broadcast = IPCALC::broadcast ($ip, $mask);
			$lnetwork = IPCALC::ip2long ($network);
			$lbroadcast = IPCALC::ip2long ($broadcast);
		}

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

		#fprintf (STDERR, "%s - %s - %s - %s\n", $line, $ip, $mask, $network);
		if ( $v6 ) {
			printf (
				'"%s", "%s", "%s", "%s", "%s", "%s"' . PHP_EOL,
				$network, $broadcast, $lnetwork, $lbroadcast, $l->code, $l->name
			);
		} else {
			printf (
				'"%s","%s","%s","%s","%s","%s"' . PHP_EOL,
				$network, $broadcast, $lnetwork, $lbroadcast, $l->code, $l->name
			);
		}
	}
}

if ( ! is_dir ($argv[1]) ) {
	echo "GeoIP2 cvs directory not found\n";
	exit (1);
}

$curdir = getcwd ();
chdir ($argv[1]);

# IP range csv
$range = array (
	"GeoLite2-Country-Blocks-IPv4.csv",
	"GeoLite2-Country-Blocks-IPv6.csv"
);
$desc  = "GeoLite2-Country-Locations-en.csv";

if ( $argc == 3 && ($argv[2] == 'ipv6' || $argv[2] == 1) )
	$no = 1;
else
	$no = 0;

if ( ! file_exists ($range[$no]) || ! file_exists ($desc) ) {
	echo "Can not find $range or $desc\n";
	exit (1);
}

$ldb = location_db ($desc);

#print_r ($ldb);

$rdb = range_db ($range[$no], $ldb);

chdir ($curdir);

exit (0);
?>
