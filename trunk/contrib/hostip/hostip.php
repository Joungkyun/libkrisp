#!/usr/bin/php
<?
if ( ! extension_loaded ('geoip') ) :
	if ( ! @dl ('geoip.so') ) :
		$_msg = "This system not support GeoIP extension.\n" .
				"Download http://cvs.oops.org/index.cgi/mod_geoip/?cvsroot=OOPS-PHP";
		error_log ($_msg, 0);
		exit (0);
	endif;
endif;

function _callback ($m) {
	return urldecode ($m[1]);
}
function convert_char ($m) {
	$_s = '/(%[0-9a-z]{2})/i';
	$r = preg_replace_callback ($_s, '_callback', $m);

	unset ($_s);
	$_s[] = '/,[ ]+/';
	$_d[] = ',';
	$_s[] = '/Pusan/';
	$_d[] = 'Busan';
	$_s[] = '/ULSAN/';
	$_d[] = 'Ulsan';
	$_s[] = '/Daegu/';
	$_d[] = 'Taegu';
	$_s[] = '/Daegeon/';
	$_d[] = 'Taegeon';

	$r = preg_replace ($_s, $_d, $r);

	return $r;
}

function chk_block ($ip) {
	global $_block;

	if ( ! $_block ) :
		$site = file_get_contents ('http://www.iana.org/assignments/ipv4-address-space');
		#$site = file_get_contents ('./ipv4-address-space');

		$_s[] = '/.*(000\/8)/s';
		$_d[] = '\\1';
		$_s[] = '/(255\/8).*/s';
		$_d[] = '\\1';
		$site = preg_replace ($_s, $_d, $site);

		$s = split ("[\r\n]+", $site);
		foreach ( $s as $v ) :
			if ( ! preg_match ('/IANA - [RPM]/', $v) ) :
				continue;
			endif;

			$va = split ('[[:space:]]+', $v);
			$_aclass = preg_replace ('/\/[0-9]+/', '', $va[0]);
			$_block[] = $_aclass;
		endforeach;
		$_block[] = '255';
	endif;

	$ipa = preg_replace ('/\..*/', '', $ip);

	if ( array_search ($ipa, $_block) === FALSE )
		return 0;

	return 1;
}


$_f = file ('hip_ip4_city_lat_lng.csv');

$gi = GeoIP_open (GEOIP_MEMORY_CACHE|GEOIP_CHECK_CACHE);

$i = 0;
foreach ( $_f as $v ) :
	$line = explode (',', $v);

	if ( count ($line) != 4 ) :
		continue;
	endif;

	$_lip = $line[0];
	$_ip = long2ip ($_lip);

	if ( chk_block ($_ip) ) :
		continue;
	endif;

	$_city = convert_char ($line[1]);

	$_gir = GeoIP_id_by_name ($gi, $_ip);

	$_co = $_gir['code'];
	$_con = $_gir['name'];

	#printf ("%s|%s|%s|%s|%s\n", $_lip, $_ip, $_co, $_con, $_city);
	printf ("%s|%s|%s|||%s|0\n", $_lip, $_co, $_con, $_city);
endforeach;

GeoIP_close ($gi);
?>
