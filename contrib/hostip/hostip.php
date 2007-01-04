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

# fix it flag
$fix = array (
	'3530972672' => 1,
);

# wrong ip & cities
$wrong = array (
	'3420616448' => array ('Changwon', '20'),
	'3420617472' => array ('Changwon', '20'),
	'3420617984' => array ('Changwon', '20'),
	'3420693504' => array ('Goyang', '6'),
	'3420694272' => array ('Goyang', '6'),
	'3422095360' => array ('Suwon', '13'),
	'3422097664' => array ('Suwon', '13'),
	'3422098432' => array ('Suwon', '13'),
	'3411119616' => array ('Songnam', '13'),
	'3529169664' => array ('Changwon', '20'),
	'3543771648' => array ('Chongsong', '14'),
	'3543785216' => array ('Namsandong', '13'),
	'3695600640' => array ('Jinhae', '20')
);

$cfix = array (
	'�gy�ng' => array ('�gy�ng', 0),
	'Andong' => array ('Andong', 14),
	'Anyang' => array ('Anyang', 13),
	'Bungi' => array ('Bungi', 0),
	'Pusan' => array ('Busan', 10),
	'Busan' => array ('Busan', 10),
	'Changwon' => array ('Changwon', 20),
	'Cheju' => array ('Jeju', 1),
	'Cheonju' => array ('Jeonju', 3),
	'Chongsong' => array ('CheongSong', 14),
	'Chuncheon' => array ('Chuncheon', 6),
	'Chungbuk' => array ('Chungbuk', 5),
	'Chunju' => array ('Jeonju', 3),
	'Taejeon' => array ('Daejeon', 19),
	'Gunsan' => array ('Gunsan', 16),
	'Gwangju' => array ('Gwangju', 18),
	'Gyeongju' => array ('Gyeongju', 14),
	'Hogye' => array ('Hogye', 0),
	'Iksan' => array ('Iksan', 3),
	'Ilsan' => array ('Ilsan', 13),
	'Incheon' => array ('Incheon', 12),
	'Jeonju' => array ('Jeonju', 3),
	'Jinhae' => array ('Jinhae', 20),
	'Jinju' => array ('Jinju', 20),
	'Kimhae' => array ('Gimhae', 20),
	'Kongnim' => array ('Kongnim', 0),
	'Koyang' => array ('Goyang', 6),
	'Kumi' => array ('Gumi', 14),
	'Kunsan' => array ('Gunsan', 16),
	'Kwangju' => array ('Gwangju', 18),
	'Kwangyang' => array ('Gwangyang', 16),
	'Gwangyang' => array ('Gwangyang', 16),
	'Muju' => array ('Muju', 3),
	'Namsandong' => array ('Namsungdong', 0),
	'Osan' => array ('Osan', 13),
	'P\'ohang' => array ('Pohang', 14),
	'S�ngnam' => array ('Seongnam', 13),
	'Sangnim' => array ('Sangnim', 0),
	'Seoul' => array ('Seoul', 11),
	'Songnam' => array ('Seongnam', 13),
	'Seongnam' => array ('Seongnam', 13),
	'Suwon' => array ('Suwon', 13),
	'T\'ojin' => array ('Dojin', 0),
	'Taegu' => array ('Daegu', 15),
	'Daegu' => array ('Daegu', 15),
	'Tangjin' => array ('Dangjin', 17),
	'ULSAN' => array ('Ulsan', 21),
	'Ulsan' => array ('Ulsan', 21),
	'Y�gyang' => array ('Yeongyang', 6),
	'Yongin' => array ('Yongin', 13),
	'Yuch\'on' => array ('Yeocheon', 16),
	'Chungju' => array ('Cheongju', 5),
	'Cheongju' => array ('Cheongju', 5),
);

#$cfix = array (
#	'�gy�ng' => array ('�gy�ng', 0),
#	'Andong' => array ('�ȵ�', 14),
#	'Anyang' => array ('�Ⱦ�', 13),
#	'Bungi' => array ('Bungi', 0),
#	'Bucheon' => array ('��õ', 13),
#	'Pusan' => array ('�λ�', 10),
#	'Busan' => array ('�λ�', 10),
#	'Changwon' => array ('â��', 20),
#	'Cheju' => array ('����', 1),
#	'Cheonju' => array ('����', 3),
#	'Chongsong' => array ('û��', 14),
#	'Chuncheon' => array ('����', 6),
#	'Chungbuk' => array ('û��', 5),
#	'Chunju' => array ('����', 3),
#	'Chungju' => array ('û��', 5),
#	'Cheongju' => array ('û��', 5),
#	'Taejeon' => array ('����', 19),
#	'Daejeon' => array ('����', 19),
#	'Gangreung' => array ('����', 6),
#	'Gunsan' => array ('����', 16),
#	'Gwangju' => array ('����', 18),
#	'Gyeongju' => array ('����', 14),
#	'Hogye' => array ('ȣ��', 0),
#	'Iksan' => array ('�ͻ�', 3),
#	'Ilsan' => array ('�ϻ�', 13),
#	'Inch\'�n' => array ('��õ', 12),
#	'Incheon' => array ('��õ', 12),
#	'Jeonju' => array ('����', 3),
#	'Jinhae' => array ('����', 20),
#	'Jinju' => array ('����', 20),
#	'Kimhae' => array ('����', 20),
#	'Kongnim' => array ('Kongnim', 0),
#	'Koyang' => array ('���', 6),
#	'Kumi' => array ('����', 14),
#	'Kunsan' => array ('����', 16),
#	'Kwangju' => array ('����', 18),
#	'Kwangyang' => array ('����', 16),
#	'Gwangyang' => array ('����', 16),
#	'Muju' => array ('����', 3),
#	'Namsandong' => array ('������', 0),
#	'Osan' => array ('����', 13),
#	'P\'ohang' => array ('����', 14),
#	'S�ngnam' => array ('����', 13),
#	'Sangnim' => array ('Sangnim', 0),
#	'Seul' => array ('����', 11),
#	'Seoul' => array ('����', 11),
#	'Songnam' => array ('����', 13),
#	'Seongnam' => array ('����', 13),
#	'Suwon' => array ('����', 13),
#	'T\'ojin' => array ('����', 0),
#	'Taegu' => array ('�뱸', 15),
#	'Daegu' => array ('�뱸', 15),
#	'Tangjin' => array ('����', 17),
#	'ULSAN' => array ('���', 21),
#	'Ulsan' => array ('���', 21),
#	'Y�gyang' => array ('����', 6),
#	'Yongin' => array ('����', 13),
#	'Yuch\'on' => array ('��õ', 16),
#);


function _callback ($m) {
	return urldecode ($m[1]);
}

function convert_char ($lip, $m) {
	global $wrong, $cfix;
	$_reg = '';

	$_s = '/(%[0-9a-z]{2})/i';
	$_r = preg_replace_callback ($_s, '_callback', $m);

	$_r = preg_replace ('/,[ ]+/', ',', $_r);
	if ( preg_match ('!([^,]+),(.+)!', $_r, $_match) ) :
		$_r = $_match[1];
		$_reg = $_match[2];
	endif;

	if ( is_array ($wrong[$lip]) ) :
		$_r = $wrong[$lip][0];
		$_reg = $wrong[$lip][1];
	endif;

	if ( is_array ($cfix[$_r]) ) :
		$_r = $cfix[$_r][0];
		$_reg = $cfix[$_r][1];
	endif;

	$_reg = ! $_reg ? "" : $_reg;

	$r = array ($_r, $_reg);

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

	$uniq["$_lip"]++;

	if ( chk_block ($_ip) ) :
		continue;
	endif;

	$_city = convert_char ($_lip, $line[1]);

	$_gir = GeoIP_id_by_name ($gi, $_ip);

	$_co = $_gir['code'];
	$_con = $_gir['name'];

	$flags = is_array ($fix[$_lip]) ? 1 : 0;

	#printf ("%s|%s|%s|%s|%s|0\n", $_lip, $_ip, $_co, $_con, $_city[0], $_city[1]);
	printf ("%s|%s|%s|||%s|%s|%s\n", $_lip, $_co, $_con, $_city[0], $_city[1], $flags);
endforeach;

foreach ( $uniq as $k => $v ) :
	if ( $v > 1 ) :
		error_log ("\n\n$k is not uniq ($v)");
	endif;
endforeach;

GeoIP_close ($gi);
?>
