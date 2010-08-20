#!/usr/bin/php
<?php
/*
 * $Id: nwpc_parse.php,v 1.3 2010-08-20 09:30:26 oops Exp $
 * Requirement
 *
 * over PHP5
 * GeoIP
 * GeoIP Pear (http://pear.php.net/package/Net_GeoIP/) or
 * GeoIP php extension (http://cvs.oops.org/index.cgi/mod_geoip/?cvsroot=OOPS-PHP)
 */

define (_ADDR, 0);
define (_NAME, 1);
define (_CLASS, 2);
define (_START, 3);
define (_END, 4);

define (F_CLASS, 0);
define (F_START, 1);
define (F_END, 2);
define (F_CITY, 3);
define (F_REGION, 4);
define (F_NAME, 5);

define (O_START, 0);
define (O_END, 1);
define (O_NAME, 2);
define (O_CITY, 3);
define (O_REGION, 4);
define (O_CLASS, 5);

$SAVE = './nwpc.csv-' . date ('Ymd');

function addr_parser ($str) {
	unset ($_s);
	unset ($_t);
	$_s[] = '/^"/';
	$_t[] = '';
	$_s[] = '/^(서울|부산|대구|광주|인천|대전|울산)(.*)/';
	$_t[] = '\\1 \\2';
	$_s[] = '/^(경기|강원|충남|충북|경남|경북|전남|전북|제주)(...+(시|군)).*/';
	$_t[] = '\\1 \\2';

	$p = preg_replace ($_s, $_t, $str);
	return $p;
}

function _ip2long ($ip) {
	return sprintf ('%u', ip2long ($ip));
}

function rvk_class ($class, $n=0) {
	unset ($_s);
	unset ($_t);

	$p = explode ('.', $class);
	foreach ($p as $v) {
		if ( $n ) {
			switch ( strlen (trim ($v)) ) {
				case 2:
					$v = '0' . trim ($v);
					break;
				case 1:
					$v = '00' . trim ($v);
					break;
			}
		} else {
			$v = preg_replace ('/^[0]+/', '', trim ($v));
			if ( ! trim ($v) )
				$v = 0;
		}
		$_p .= $v . '.';
	}

	$_p = preg_replace ('/\.$/', '', $_p);

	return $_p;
}

function fix_strint ($d, $n = 0) {
	if ( $n ) {
		switch ( strlen (trim ($d)) ) {
			case 2:
				$d = '0' . trim ($d);
				break;
			case 1:
				$d = '00' . trim ($d);
				break;
		}
	} else {
		$d = preg_replace ('/^0+/', '', trim ($d));
		if ( ! trim ($d) )
			$d = 0;
	}

	return $d;
}

function fix_city ($c) {
	global $FIPS_K;
	$c = preg_replace ('/(시|군)$/', '', $c);
	$_c = $FIPS_K['cityMap'][$c];
	$_c = fixgu ($_c ? $_c : $c);

	return $_c;
}

function fix_region ($_r) {
	global $FIPS_K;
	$key = array_search ($_r, $FIPS_K['short']);
	return $key ? $key : $_r;
}

function _file_init ($f) {
	global $stderr;

	$_l = file ($f);
	$_s = count ($_l);

	if ( $_s < 1 ) {
		error_log ("ERROR: no data", 0);
		exit (1);
	}

	$_l = preg_replace ('/[\r\n]/', '', $_l);

	error_log ("* Read line", 0);
	for ( $i=0; $i<$_s; $i++ ) {
		// change utf8 format
		$_l[$i] = iconv ('cp949', 'utf8', $_l[$i]);
		$_component = explode ('|', trim ($_l[$i]));

		$_r[$i]  = rvk_class ($_component[_CLASS]) . '|';
		$_r[$i] .= fix_strint ($_component[_START]) . '|';
		$_r[$i] .= fix_strint ($_component[_END]) . '|';

		$_component[_ADDR]  = addr_parser (trim ($_component[_ADDR]));
		$_region = split ('[ ]+', $_component[_ADDR]);
		$_r[$i]  .= $_region[1] . '|';
		$_r[$i]  .= $_region[0] . '|';
		$_r[$i]  .= trim ($_component[_NAME]);

		fprintf ($stderr, "  ==> %d\r", $i);
	}
	fprintf ($stderr, "%20s\r* Sort data start\n", ' ');

	natsort ($_r);
	fprintf ($stderr, "%20\r", ' ');

	return $_r;
}

function test_checker ($target) {
	if ( ! is_array ($target) )
		return TRUE;

	foreach ( $target as $v ) {
		if ( preg_match ('/테스트/', $v) )
			return FALSE;
	}

	return TRUE;
}

function fixgu ($city) {
	global $fix_eng_gu;

	if ( ! is_array ($fix_eng_gu) || ! count ($fix_eng_gu) )
		return $city;

	return $fix_eng_gu[$city] ? $fix_eng_gu[$city] : $city;
}

if ( file_exists ('../lib/fix-eng-gu.php') ) {
	require_once '../lib/fix-eng-gu.php';
}

$_geoip = 0;
if ( ! extension_loaded ('geoip') ) {
	dl ('geoip.so');
	$_geoip = 1; // extension

	if ( function_exists ('GeoIP_open') ) {
		$geoip_t = GEOIP_INDEX_CACHE | GEOIP_CHECK_CACHE;
		$gi      = GeoIP_open ($_geoip_t);
	} else {
		$_geoip = 0; // pear
		@include_once 'Net/GeoIP.php';

		if ( ! class_exists ('Net_GeoIP') ) {
			error_log ('ERROR: This system don\'t support GeoIP extension or Pear', 0);
			error_log ('       Get php extension on http://cvs.oops.org/index.cgi/mod_geoip/?cvsroot=OOPS-PHP or', 0);
			error_log ('       Get php pear on http://pear.php.net/package/Net_GeoIP/', 0);
			exit (1);
		}

		$gi = Net_GeoIP::getInstance ('/usr/share/GeoIP/GeoIP.dat', Net_GeoIP::MEMORY_CACHE);
	}
} else {
	$_geoip = 1;
	$geoip_t = GEOIP_INDEX_CACHE | GEOIP_CHECK_CACHE;
	$gi      = GeoIP_open ($_geoip_t);
}

if ( file_exists ('/usr/local/src/cvs/php/pear_krisp/v1/KRISP/georegion.php') ) {
	require_once '/usr/local/src/cvs/php/pear_krisp/v1/KRISP/georegion.php';
}

if ( trim ($argv[1]) && file_exists ($argv[1]) ) {
    $_file = $argv[1];
} else {
	$_file = './PCBangIPlist.txt';

	if ( ! file_exists ($_file) ) {
		error_log ("ERROR: $_file not found", 0);
		exit (1);
	}
}

error_log ("* Save file results: {$SAVE}", 0);

$nwzformat = 0;
if ( ($argc == 3 || $argc == 4) && $argv[2] == 'nwz' )
	$nwzformat = 1;

$nwzdebug = ($argc == 4) ? 1 : 0;

$stderr = fopen ('php://stderr', 'w');
$line = _file_init ($_file);

$fp = fopen ($SAVE, 'wb');
if ( ! is_resource ($fp) ) {
	fprintf ("ERROR: %s open failed\n", $SAVE);
	fclose ($stderr);
	exit (1);
}

error_log ("* Start line parse", 0);
$i = 0;
foreach ( $line as $v ) {
	fprintf ($stderr, "  ==> %d\r", ++$i);

	$l = explode ('|', trim ($v));

	if ( test_checker (array ($l[F_NAME], $l[F_REGION])) === FALSE )
		continue;

	for ( $j=0; $j<5; $j++ )
		$l[$j] = trim ($l[$j]);

	$_start = $l[F_CLASS] . '.' . $l[F_START];
	$_end   = $l[F_CLASS] . '.' . $l[F_END];

	if ( $_geoip ) {
		$_gir = GeoIP_id_by_name ($gi, $l[F_CLASS] . '.0');
		$gir = $_gir['code'];
	} else
		$gir = $gi->lookupCountryCode ($l[F_CLASS] . '.0');

	if ( $gir != 'KR' )
		continue;

	if ( ! is_array ($old) ) {
		$old = array ($l[F_START], $l[F_END], $l[F_NAME], $l[F_CITY], $l[F_REGION], $l[F_CLASS]);
		continue;
	}

	if ( $old[O_CLASS] == $l[F_CLASS] && $old[O_NAME] == $l[F_NAME] ) {
		if ( $l[F_START] < $old[O_START] )
			$old[O_START] = $l[F_START];

		if ( $l[F_END] > $old[O_END] )
			$old[O_END] = $l[F_END];

		continue;
	} 

	$o_start = $old[O_CLASS] . '.' . $old[O_START];
	$o_end   = $old[O_CLASS] . '.' . $old[O_END];
	#printf ("%s|%s|%s|%s|%s\n", $o_start, $o_end, $old[O_NAME], $old[O_CITY], $old[O_REGION]);
	$nu = _ip2long ($o_end) - _ip2long ($o_start);
	if ( $nwzdebug )
		$nup = sprintf ("%-3d\t", $nu);

	if ( $nu > 1 ) {
		if ( $nwzformat ) {
			$DATA = sprintf (
				"%s%s\t%s\t%s|%s|%s\n",
				$nup,
				_ip2long ($o_start),
				_ip2long ($o_end),
				$old[O_NAME],
				$old[O_CITY],
				$old[O_REGION]
			);
		} else {
			$DATA = sprintf (
				"%s%s\t%s\t%s|%s\n",
				$nup,
				_ip2long ($o_start),
				_ip2long ($o_end),
				$old[O_CITY],
				$old[O_REGION]
			);
		}
		fwrite ($fp, $DATA, strlen ($DATA));
	}
	/*
	printf ("%s|KR|Korea, Republic of|||%s|%s|1\n",
			_ip2long ($old[O_CLASS].".0"), fix_city ($old[O_CITY]), fix_region ($old[O_REGION]));
	 */
	$old = array ($l[F_START], $l[F_END], $l[F_NAME], $l[F_CITY], $l[F_REGION], $l[F_CLASS]);
}

$o_start = $old[O_CLASS] . '.' . $old[O_START];
$o_end   = $old[O_CLASS] . '.' . $old[O_END];
$nu = _ip2long ($o_end) - _ip2long ($o_start);
if ( $nwzdebug )
	$nup = sprintf ("%-3d\t", $nu);

if ( $nwzformat ) {
	$DATA = sprintf (
		"%s%s\t%s\t%s|%s|%s\n",
		$nup,
		_ip2long ($o_start),
		_ip2long ($o_end),
		$old[O_NAME],
		$old[O_CITY],
		$old[O_REGION]
	);
} else {
	$DATA = sprintf (
		"%s%s\t%s\t%s|%s\n",
		$nup,
		_ip2long ($o_start),
		_ip2long ($o_end),
		$old[O_CITY],
		$old[O_REGION]
	);
}
fwrite ($fp, $DATA, strlen ($DATA));

fclose ($fp);
fclose ($stderr);

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
