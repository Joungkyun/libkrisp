#!/usr/bin/php
<?
dl ('geoip.so');

if ( file_exists ('/usr/local/src/mycvs/php/pear_krisp/krisp/georegion.php') ) :
	require_once '/usr/local/src/mycvs/php/pear_krisp/krisp/georegion.php';
endif;

function addr_parser ($str) {
	$_s[] = '/^"/';
	$_t[] = '';
	$_s[] = '/^(서울|부산|대구|광주|인천|대전|울산).*/';
	$_t[] = '\\1 \\1';
	$_s[] = '/(시|군).*/';
	$_t[] = '\\1';
	$_s[] = '/^경기시/';
	$_t[] = '경기시흥시';
	$_s[] = '/^경기군/';
	$_t[] = '경기군포시';
	$_s[] = '/^경북군/';
	$_t[] = '경북군위군';
	$_s[] = '/^전북군/';
	$_t[] = '전북군산시';
	$_s[] = '/^(강원|경기|경남|경북|전남|전북|제주|충남|충북)/';
	$_t[] = '\\1 ';

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
	foreach ($p as $v) :
		if ( $n ) :
			switch ( strlen (trim ($v)) ) :
				case 2:
					$v = '0' . trim ($v);
					break;
				case 1:
					$v = '00' . trim ($v);
					break;
			endswitch;
		else :
			$v = preg_replace ('/^[0]{1,2}/', '', trim ($v));
		endif;
		$_p .= $v . '.';
	endforeach;

	$_p = preg_replace ('/\.$/', '', $_p);

	return $_p;
}

function fix_strint ($d, $n = 0) {
	if ( $n ) :
		switch ( strlen (trim ($d)) ) :
			case 2:
				$d = '0' . trim ($d);
				break;
			case 1:
				$d = '00' . trim ($d);
				break;
		endswitch;
	else :
		$d = preg_replace ('/^0+/', '', $d);
	endif;

	return $d;
}

function fix_city ($c) {
	global $FIPS_K;
	$c = preg_replace ('/(시|군)$/', '', $c);
	$c = $FIPS_K['cityMap'][$c];

	return $c;
}

function fix_region ($r) {
	global $FIPS_K;
	$key = array_search ($r, $FIPS_K['short']);
	return $key;
}

function location_callback ($m) {
	$m[3] = rvk_class ($m[3], 1);
	$m[4] = fix_strint ($m[4], 1);
	$m[5] = fix_strint ($m[5], 1);

	return "{$m[3]}\t{$m[4]}\t{$m[5]}\t{$m[1]}\t{$m[2]}";
}

function _file_init ($f) {
	$_l = file ($f);

	$_l = preg_replace ('/[\r\n]/', '', $_l);
	$_l = preg_replace_callback ('/([^\t]+)\t([^\t]+)\t([^\t]+)\t([^\t]+)\t([^\t]+)/', 'location_callback', $_l);
	sort ($_l);

	return $_l;
}

$geoip_t = GEOIP_INDEX_CACHE | GEOIP_CHECK_CACHE;
$gi = GeoIP_open ($_geoip_t);

if ( trim ($argv[1]) && file_exists ($argv[1]) ) :
	$_file = $argv[1];
else :
	$_file = './PC방IP리스트.txt';
endif;

$_line = _file_init ($_file);

define (F_REGION, 0);
define (F_CITY, 1);

define (F_CLASS, 0);
define (F_START, 1);
define (F_END, 2);
define (F_ADDR, 3);
define (F_NAME, 4);

define (O_START, 0);
define (O_END, 1);
define (O_NAME, 2);
define (O_CITY, 3);
define (O_REGION, 4);
define (O_CLASS, 5);


foreach ( $_line as $_l ) :
	$_la = split ("[\t]+", trim ($_l));

	for ( $j=0; $j<5; $j++ ) :
		$_la[$j] = trim ($_la[$j]);
	endfor;
	$_la[F_START] = fix_strint ($_la[F_START]);
	$_la[F_END]   = fix_strint ($_la[F_END]);

	$_la[F_CLASS] = rvk_class ($_la[F_CLASS]);
	$_start = $_la[F_CLASS] . $_la[F_START];
	$_end   = $_la[F_CLASS] . $_la[F_END];

	$gir = GeoIP_id_by_name ($gi, $_la[F_CLASS] . '.0');

	if ( $gir['code'] != 'KR' ) :
		continue;
	endif;

	$_la[F_ADDR] = addr_parser ($_la[F_ADDR]);
	$_lp = explode (" ", $_la[F_ADDR]);

	if ( ! is_array ($old) ) :
		$old = array ($_la[F_START], $_la[F_END], $_la[F_NAME], $_lp[F_CITY], $_lp[F_REGION], $_la[F_CLASS]);
	else :
		if ( $old[O_CLASS] == $_la[F_CLASS] ) :
			#if ( $_la[F_START] < $old[O_END] ) :
				if ( $_la[F_START] < $old[O_START] ) :
					$old[O_START] = $_la[F_START];
				endif;
				if ( $_la[F_END] > $old[O_END] ) :
					$old[O_END] = $_la[F_END];
				endif;
			#else :
			#	$o_start = $old[O_CLASS] . '.' . $old[O_START];
			#	$o_end   = $old[O_CLASS] . '.' . $old[O_END];
			#	printf ("%s|%s|%s|%s|%s\n", $o_start, $o_end, $old[O_NAME], $old[O_CITY], $old[O_REGION]);
			#	$old = array ($_la[F_START], $_la[F_END], $_la[F_NAME], $_lp[F_CITY], $_lp[F_REGION], $_la[F_CLASS]);
			#endif;
		else :
			$o_start = $old[O_CLASS] . '.' . $old[O_START];
			$o_end   = $old[O_CLASS] . '.' . $old[O_END];
			#printf ("%s|%s|%s|%s|%s\n", $o_start, $o_end, $old[O_NAME], $old[O_CITY], $old[O_REGION]);
			printf ("%s|KR|Korea, Republic of|||%s|%s|1\n",
					_ip2long ($old[O_CLASS].".0"), fix_city ($old[O_CITY]), fix_region ($old[O_REGION]));
			$old = array ($_la[F_START], $_la[F_END], $_la[F_NAME], $_lp[F_CITY], $_lp[F_REGION], $_la[F_CLASS]);
		endif;
	endif;
endforeach;

GeoIP_close ($gi);
?>
