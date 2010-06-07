#!/usr/bin/php
<?php
#
# $Id: geo-krisp.php,v 1.1 2010-06-07 11:31:27 oops Exp $
#
# convert GeoIP database to KRISP database
#

define ('T_GEOIP', 0);
define ('T_GEOIPISP', 1);

define ('START_LONG', 0);
define ('END_LONG', 1);
define ('NATION_CODE', 2);
define ('ISPNAME', 2);
define ('NATION', 3);

function banner () { // {{{
	global $argv;

	echo <<<EOF
 * Convert GeoIP database to KRISP data format (TXT, CSV, SQL)
 *

EOF;
} // function banner () }}}

Class GeoIP_KRISP // {{{
{
	static public $mode;
	static public $stderr;
	static private $today;

	function __construct () {
		$this->stderr = fopen ('php://stderr', 'w');
		$this->today = date ('Ymd');
		$this->mode = T_GEOIP;

		self::$stderr = &$this->stderr;
		self::$today  = &$this->today;
		self::$mode   = &$this->mode;
	}

	function GeoIP ($f) {
		if ( ! file_exists ($f) || ! is_file ($f) )
			return 1;

		fprintf (self::$stderr, ' * Make GeoIP database .. ');

		$p = fopen ($f, 'rb');
		if ( ! is_resource ($p) )
			return 2;

		$fname = (self::$mode == T_GEOIP) ? 'geoip' : 'geoisp';

		$csvH = './' . $fname . '.csv-' . self::$today;

		$csv = fopen ($csvH, 'wb');
		if ( ! is_resource ($csv) ) {
			if ( is_resource ($p) )
				fclose ($p);
			return 3;
		}

		$num = 1;
		$O = '';
		while ( ! feof ($p) ) {
			self::progress ($num++);

			$r = trim (fgets ($p, 512));
			$r = iconv ('iso-8859-1', 'utf8', $r);

			if ( self::$mode == T_GEOIP) {
				if ( ! preg_match ('/^"/', $r) || ! preg_match ('/"$/', $r) )
					continue;

				self::removeQuote ($r);
				$v = explode ('","', $r);

				$C = array ($v[2], $v[3], $v[4], $v[5]);

				if ( ! is_array ($O) ) {
					$O = $C;
					continue;
				}

				if ( ($O[END_LONG] + 1) == $C[START_LONG] && $O[NATION_CODE] == $C[NATION_CODE] ) {
					$O[END_LONG] = $C[END_LONG];
					continue;
				}

				$DATA = sprintf (
					"%s\t%s\t%s|%s\n",
					$O[START_LONG],
					$O[END_LONG],
					$O[NATION_CODE],
					$O[NATION]
				);
				fwrite ($csv, $DATA, strlen ($DATA));
			} else {
				if ( preg_match ('/^[^0-9]/', $r) )
					continue;

				$v = explode (',', $r);
				$vlen = count ($v);
				if ( $vlen < 3 )
					continue;

				$C = array ($v[0], $v[1], $v[2]);

				if ( $vlen > 3 )
					for ( $i=3; $i<$vlen; $i++ )
						$C[ISPNAME] .= $v[$i];

				self::removeQuote ($C[ISPNAME]);

				if ( ! is_array ($O) ) {
					$O = $C;
					continue;
				}

				if ( ($O[END_LONG] + 1) == $C[START_LONG] && $O[ISPNAME] == $C[ISPNAME] ) {
					$O[END_LONG] = $C[END_LONG];
					continue;
				}

				$DATA = sprintf (
					"%s\t%s\t%s\n",
					$O[START_LONG],
					$O[END_LONG],
					$O[ISPNAME]
				);
				fwrite ($csv, $DATA, strlen ($DATA));
			}
			$O = $C;
		}
		self::progress ($num, true);

		if ( self::$mode == T_GEOIP) {
			$DATA = sprintf (
				"%s\t%s\t%s|%s\n",
				$O[START_LONG],
				$O[END_LONG],
				$O[NATION_CODE],
				$O[NATION]
			);
			fwrite ($csv, $DATA, strlen ($DATA));
		} else {
			$DATA = sprintf (
				"%s\t%s\t%s\n",
				$O[START_LONG],
				$O[END_LONG],
				$O[ISPNAME]
			);
			fwrite ($csv, $DATA, strlen ($DATA));
		}

		fclose ($csv);
		fclose ($p);

		fprintf ($stderr, "\n");

		return 0;
	}

	function progress ($no, $stop = false) {
		fprintf (self::$stderr, '%d', $no);

		if ( $stop === true ) {
			fprintf (self::$stderr, "\n");
			return;
		}

		$len = strlen ($no);
		for ( $i=0; $i<$len; $i++ ) {
			fprintf (self::$stderr, '%c', '008');
			echo ' ';
			fprintf (self::$stderr, '%c', '008');
		}
	}
	/*
	function progress ($no, $stop = false) {
		$v = $no % 4;
		switch ($v) {
			case '3' :
				$p = '-';
				break;
			case '2' :
				$p = '\\';
				break;
			case '1' :
				$p = '|';
				break;
			default:
				$p = '/';
		}
		fprintf (self::$stderr, '%s', $p);

		if ( $stop === true ) {
			fprintf (self::$stderr, "\n", $no);
			return;
		}

		fprintf (self::$stderr, '%c', '008');
		echo ' ';
		fprintf (self::$stderr, '%c', '008');

		$p = $no % 1000;
		if ( $p < 1 ) {
			fprintf (self::$stderr, '%d', $no);
			usleep (100);
			$len = strlen ($no);
			for ( $i=0; $i<$len; $i++ ) {
				fprintf (self::$stderr, '%c', '008');
				echo ' ';
				fprintf (self::$stderr, '%c', '008');
			}
		}
	}
	 */

	function removeQuote (&$data, $ret = false) {
		if ( $ret !== false )
			return preg_replace ('/^"|"$/', '', $data);

		$data = preg_replace ('/^"|"$/', '', $data);
	}

	function __destruct () {
		if ( is_resource (self::$stderr) )
			fclose (self::$stderr);
	}
} // }}}

/*
 * Main
 */

banner ();

if ( $argc == 1 )
	$f = './GeoIPCountryWhois.csv';
else {
	if ( file_exists ($argv[1]) )
		$f = $argv[1];
	else {
		error_log ('ERROR: Can\'t find GeoIP CSV file', 0);
		exit (1);
	}
}

$krisp = new GeoIP_KRISP;

if ( preg_match ('/geoisp/', $argv[0]) ) {
	$krisp->mode = T_GEOIPISP;
	if ( $argc == 1 )
		$f = './GeoIPISP.csv';
}

if ( ($ret = $krisp->GeoIP ($f)) !== 0 ) {
	switch ($ret) {
		case 1 :
			fprintf ($krisp->stderr, " => ERROR: %s is not exits or not file\n", $f);
			break;
		case 2 :
			fprintf ($krisp->stderr, " => ERROR: file open failed (%s)\n", $f);
			break;
		case 3 :
			fprintf ($krisp->stderr, " => ERROR: You don't have write permission on cwd\n");
			break;
	}
}

exit (0);
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
?>
