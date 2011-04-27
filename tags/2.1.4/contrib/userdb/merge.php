#!/usr/bin/php
<?
function usage () {
	global $stderr;

	fprintf ($stderr, "Usage: merge.php original.data attached.data\n");
	fclose ($stderr);
	exit (1);
}

function arrange ($f) {
	global $stderr;
	$p = fopen ($f, 'rb');

	$i=0;
	while ( ! feof ( $p) ) :
		fprintf ($stderr, "  ==> %d\r", ++$i);
		$buf = trim (fgets ($p));
		$v = explode ('|', $buf);
		$_r[$v[0]] = $buf;
	endwhile;
	fprintf ($stderr, "\n");

	fclose ($p);
	
	return $_r;
}

ini_set ('memory_limit', '256M');

$stderr = fopen ('php://stderr', 'w');

if ( $argc != 3 ) :
	usage ();
endif;

if ( ! file_exists ($argv[1]) ) :
	fprintf ($stderr, "original data (%s) 파일이 존재하지 않습니다.\n", $argv[1]);
	fclose ($stderr);
	exit (1);
endif;

$hostip = $argv[1];

if ( ! file_exists ($argv[2]) ) :
	fprintf ($stderr, "추가할 파일(%s)이 존재하지 않습니다.\n", $argv[2]);
	fclose ($stderr);
	exit (1);
endif;

$addip = $argv[2];

# hostip 를 배열로 정렬

# hostip array 정리
fprintf ($stderr, "* Get Original data (%s)\n", $hostip);
$r_hostip = arrange ($hostip);
$l['r_hostip'] = count ($r_hostip);
fprintf ($stderr, "* Get Attched data (%s)\n", $addip);
$r_addip = arrange ($addip);
$l['r_addip'] = count ($r_addip);

fprintf ($stderr, "* Merge Attached data (%d)\n", $l['r_addip']);
$i = 0;
foreach ($r_addip as $k => $v ) :
	fprintf ($stderr, "  ==> %d\r", ++$i);
	$r_hostip[$k] = $v;
endforeach;
fprintf ($stderr, "\n");
$l['merged'] = count ($r_hostip);

fprintf ($stderr, "* Sort Merged data (%d)\n", $l['merged']);
natsort ($r_hostip);

fprintf ($stderr, "* Record Data (%d)\n", $l['merged']);
$i = 0;
foreach ( $r_hostip as $v ) :
	fprintf ($stderr, "  ==> %d\r", ++$i);
	echo $v."\n";
endforeach;
fprintf ($stderr, "\n");

fclose ($stderr);
?>
