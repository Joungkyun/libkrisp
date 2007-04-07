#!/usr/bin/php
<?
function usage () {
	echo "Usage: merge.php hostip.csv add.csv\n";
	exit (1);
}

function arrange ($f) {
	$_f = file ($f);

	foreach ($_f as $v ) :
		$v = trim ($v);
		$_v = explode ('|', $v);
		$_r[$_v[0]] = $v;
	endforeach;

	return $_r;
}

if ( $argc != 3 ) :
	usage ();
endif;

if ( ! file_exists ($argv[1]) ) :
	echo "hostip.csv ������ �������� �ʽ��ϴ�.\n";
	exit (1);
endif;

$hostip = $argv[1];

if ( ! file_exists ($argv[2]) ) :
	echo "�߰��� ������ �������� �ʽ��ϴ�.\n";
	exit (1);
endif;

$addip = $argv[2];

# hostip �� �迭�� ����
$r_hostip = arrange ($hostip);
$r_addip = arrange ($addip);

foreach ($r_addip as $k => $v ) :
	$r_hostip[$k] = $v;
endforeach;

natsort ($r_hostip);

foreach ( $r_hostip as $v ) :
	echo $v."\n";
endforeach;

?>
