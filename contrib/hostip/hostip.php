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
	'3421424384' => 1, 		# 203.238.203.0
	'3530972672' => 1,		# 210.118.94.0
	'3739753216' => 1, 		# 222.232.27.0
);

# wrong ip & cities
$wrong = array (
	'3420616448' => array ('Changwon', '20'),		# 203.226.119.0
	'3420617472' => array ('Changwon', '20'),		# 203.226.123.0
	'3420617984' => array ('Changwon', '20'),		# 203.226.125.0
	'3420693504' => array ('Goyang', '6'),			# 203.227.164.0
	'3420694272' => array ('Goyang', '6'),			# 203.227.167.0
	'3421615872' => array ('Sasang-gu', 10),		# 203.241.183.0 ºÎ»ê »ç»ó±¸ µ¿¼­´ëÇĞ±³
	'3422095360' => array ('Suwon', '13'),			# 203.249.8.0
	'3422097664' => array ('Suwon', '13'),			# 203.249.20.0
	'3422098432' => array ('Suwon', '13'),			# 203.249.20.0
	'3411119616' => array ('Songnam', '13'),		# 203.81.142.0
	'3529169664' => array ('Changwon', '20'),		# 210.90.219.0
	'3543771648' => array ('Chongsong', '14'),		# 211.57.170.0
	'3543785216' => array ('Namsandong', '13'),		# 211.57.223.0
	'3695600640' => array ('Jinhae', '20')			# 220.70.100.0 fixed it
);

$addip = array (
	'2109365760' => array ('Seongnam', 13),			# 125.186.94.0
	'3421424384' => array ('Seongnam', 13),			# 203.238.203.0
	'3739753216' => array ('Suwon', 13),			# 222.232.27.0
);

$cfix = array (
	'×gy÷ng' => array ('×gy÷ng', 0),
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
	'S÷ngnam' => array ('Seongnam', 13),
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
	'Y÷gyang' => array ('Yeongyang', 6),
	'Yongin' => array ('Yongin', 13),
	'Yuch\'on' => array ('Yeocheon', 16),
	'Chungju' => array ('Cheongju', 5),
	'Cheongju' => array ('Cheongju', 5),
);

#$cfix = array (
#	'×gy÷ng' => array ('×gy÷ng', 0),
#	'Andong' => array ('¾Èµ¿', 14),
#	'Anyang' => array ('¾È¾ç', 13),
#	'Bungi' => array ('Bungi', 0),
#	'Bucheon' => array ('ºÎÃµ', 13),
#	'Pusan' => array ('ºÎ»ê', 10),
#	'Busan' => array ('ºÎ»ê', 10),
#	'Changwon' => array ('Ã¢¿ø', 20),
#	'Cheju' => array ('Á¦ÁÖ', 1),
#	'Cheonju' => array ('ÀüÁÖ', 3),
#	'Chongsong' => array ('Ã»¼Û', 14),
#	'Chuncheon' => array ('ÃáÀü', 6),
#	'Chungbuk' => array ('Ã»ºÏ', 5),
#	'Chunju' => array ('ÀüÁÖ', 3),
#	'Chungju' => array ('Ã»ÁÖ', 5),
#	'Cheongju' => array ('Ã»ÁÖ', 5),
#	'Taejeon' => array ('´ëÀü', 19),
#	'Daejeon' => array ('´ëÀü', 19),
#	'Gangreung' => array ('°­¸ª', 6),
#	'Gunsan' => array ('±º»ê', 16),
#	'Gwangju' => array ('±¤ÁÖ', 18),
#	'Gyeongju' => array ('°æÁÖ', 14),
#	'Hogye' => array ('È£°è', 0),
#	'Iksan' => array ('ÀÍ»ê', 3),
#	'Ilsan' => array ('ÀÏ»ê', 13),
#	'Inch\'÷n' => array ('ÀÎÃµ', 12),
#	'Incheon' => array ('ÀÎÃµ', 12),
#	'Jeonju' => array ('ÀüÁÖ', 3),
#	'Jinhae' => array ('ÁøÇØ', 20),
#	'Jinju' => array ('ÁøÁÖ', 20),
#	'Kimhae' => array ('±èÇØ', 20),
#	'Kongnim' => array ('Kongnim', 0),
#	'Koyang' => array ('°í¾ç', 6),
#	'Kumi' => array ('±¸¹Ì', 14),
#	'Kunsan' => array ('±º»ê', 16),
#	'Kwangju' => array ('±¤ÁÖ', 18),
#	'Kwangyang' => array ('±¤¾ç', 16),
#	'Gwangyang' => array ('±¤¾ç', 16),
#	'Muju' => array ('¹«ÁÖ', 3),
#	'Namsandong' => array ('³²¼ºµ¿', 0),
#	'Osan' => array ('¿À»ê', 13),
#	'P\'ohang' => array ('Æ÷Ç×', 14),
#	'S÷ngnam' => array ('¼º³²', 13),
#	'Sangnim' => array ('Sangnim', 0),
#	'Seul' => array ('¼­¿ï', 11),
#	'Seoul' => array ('¼­¿ï', 11),
#	'Songnam' => array ('¼º³²', 13),
#	'Seongnam' => array ('¼º³²', 13),
#	'Suwon' => array ('¼ö¿ø', 13),
#	'T\'ojin' => array ('µµÁø', 0),
#	'Taegu' => array ('´ë±¸', 15),
#	'Daegu' => array ('´ë±¸', 15),
#	'Tangjin' => array ('´çÁø', 17),
#	'ULSAN' => array ('¿ï»ê', 21),
#	'Ulsan' => array ('¿ï»ê', 21),
#	'Y÷gyang' => array ('¿µ¾ç', 6),
#	'Yongin' => array ('¿ëÀÎ', 13),
#	'Yuch\'on' => array ('¿©Ãµ', 16),
#);


function _callback ($m) {
	return urldecode ($m[1]);
}

function check_addip ($o, $c) {
	global $addip;

	if ( ! is_array ($addip) || ! $o || ! $c ) :
		return FALSE;
	endif;

	foreach ( $addip as $key => $value ) :
		if ( $key > $o && $key < $c ) :
			return array ($key, $value[0], $value[1]);
		endif;
	endforeach;

	return FALSE;
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
			if ( preg_match ('/[\s](ALLOCATED|LEGACY)$/', trim ($v)) ) :
				continue;
			endif;

			$va = split ('[[:space:]]+', $v);
			$_aclass = preg_replace ('!/[0-9]+!', '', $va[0]);
			$_block[] = $_aclass;
		endforeach;
		$_block[] = '255';
	endif;

	$ipa = preg_replace ('/\..*/', '', $ip);
	$ipa = explode ('.', $ip);

	if ( array_search ($ipa[0], $_block) === FALSE )
		return true;

	return false;
}

$_file = $argv[1] ? trim ($argv[1]) : 'hip_ip4_city_lat_lng.csv';
$stderr = fopen ('php://stderr', 'w');

$gi = GeoIP_open (GEOIP_MEMORY_CACHE|GEOIP_CHECK_CACHE);
$fp = fopen ($_file, 'rb');

if ( ! is_resource ($fp) ) :
	fprintf ($stderr, "%s open failed\n", $_file);
	exit (1);
endif;

fprintf ($stderr, "* Parsed %s\n", $_file);

$i = 0;
while ( ! feof ($fp) ) :
	$line = explode (',', fgets ($fp, 1024));
	fprintf ($stderr, "  => %d\r", ++$i);

	if ( count ($line) != 4 ) :
		continue;
	endif;

	$_lip = $line[0];
	$_ip = long2ip ($_lip);

	$uniq["$_lip"]++;

	if ( chk_block ($_ip) === false ) :
		continue;
	endif;

	$_city = convert_char ($_lip, $line[1]);

	$_gir = GeoIP_id_by_name ($gi, $_ip);

	$_co = $_gir['code'];
	$_con = $_gir['name'];

	$flags = $fix[$_lip] ? 1 : 0;

	$add_ip = check_addip ($o_lip, $_lip);
	if ( $add_ip !== FALSE ) :
		if ( ! $uniq[$add_ip[1]] ) :
			$uniq[$add_ip[1]]++;
			printf ("%s|KR|Korea, Republic of|||%s|%s|1\n", $add_ip[0], $add_ip[1], $add_ip[2]);
		endif;
	endif;

	#printf ("%s|%s|%s|%s|%s|0\n", $_lip, $_ip, $_co, $_con, $_city[0], $_city[1]);
	printf ("%s|%s|%s|||%s|%s|%s\n", $_lip, $_co, $_con, $_city[0], $_city[1], $flags);

	$o_lip = $_lip;
endwhile;

fprintf ($stderr, "* Check uniq entry\n");
$i = 0;
foreach ( $uniq as $k => $v ) :
	fprintf ($stderr, "  => %d", ++$i);
	if ( $v > 1 ) :
		error_log ("\n$k is not uniq ($v)");
	endif;
	fprintf ($stderr, "\r");
endforeach;

fclose ($stderr);
GeoIP_close ($gi);
?>
