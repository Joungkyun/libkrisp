<?php
require_once '../lib/ValidIP.php';

define ('NETWORK',   0);
define ('BROAD',     1);
define ('CITY',      2);
define ('BLOCK',     2);
define ('REGION',    3);
define ('LATITUDE',  4);
define ('LONGITUDE', 5);

$loc = './GeoLiteCity-Location.csv';
$f = './GeoLiteCity-Blocks.csv';
$SAVE = 'geocity.csv-' . date ('Ymd');

function block_array ($f) {
	global $stderr;

	$fp = fopen ($f, 'rb');
	if ( ! is_resource ($fp) )
		return null;

	$i = 0;
	error_log ("* Make Block Array", 0);
	while ( ($line = fgets ($fp, 1024)) != false ) {
		fprintf ($stderr, "  ==> %d\r", $i++);

		if ( ! is_numeric ($line[0]) )
			continue;
		$line = preg_replace ('/"/', '', trim ($line));
		$l = explode (',', $line);
		$block[$l[0]] = array ($l[2], $l[3], $l[5], $l[6]);
	}
	fclose ($fp);
	fprintf ($stderr, "\n");

	return $block;
}

$stderr = fopen ('php://stderr', 'w');
$fp = fopen ($f, 'rb');
if ( ! is_resource ($fp) ) {
	fprintf ($stderr, "ERROR: %s open error\n", $f);
	fclose ($stderr);
	exit (1);
}

$save = fopen ($SAVE, 'wb');
if ( ! is_resource ($save) ) {
	fprintf ($stderr, "ERROR: %s open error\n", $SAVE);
	fclose ($stderr);
	fclose ($fp);
	exit (1);
}

$block = block_array ($loc);


$i = 0;
$o = '';
error_log ("* Read line", 0);
while ( ($line_r = fgets ($fp, 1024)) !== false ) {
	fprintf ($stderr, "  ==> %d\r", $i++);
	$line_r = preg_replace ('/"/', '', $line_r);

	$line = explode (',', trim ($line_r));
	if ( ValidIP::valid ($line[NETWORK]) === false )
		continue;

	if ( ! $block[$line[BLOCK]][1] )
		continue;

	$l = array (
		$line[NETWORK],
		$line[BROAD],
		$block[$line[BLOCK]][1],
		$block[$line[BLOCK]][0],
		$block[$line[BLOCK]][2],
		$block[$line[BLOCK]][3],
	);

	if ( ! is_array ($o) ) {
		$o = $l;
		continue;
	}

	if ( $o[BROAD] + 1 == $l[NETWORK] && $o[CITY] == $l[CITY] ) {
		$o[BROAD] = $l[BROAD];
		continue;
	}

	if ( $o[NETWORK] == $l[NETWORK] ) {
		if ( ! $conflict[$o[NETWORK]] )
			$conflict[$o[NETWORK]] = 2;
		else
			$conflict[$o[NETWORK]] += 1;
		$o = $l;
		continue;
	}

	$DATA = sprintf (
		"%s\t%s\t%s|%s|%s|%s\n",
		$o[NETWORK],
		$o[BROAD],
		$o[CITY],
		$o[REGION],
		$o[LATITUDE],
		$o[LONGITUDE]
	);
	$DATA = iconv ('iso-8859-1', 'utf-8', $DATA);
	fwrite ($save, $DATA, strlen ($DATA));
	$o = $l;
}

$DATA = sprintf (
	"%s\t%s\t%s|%s|%s|%s\n",
	$o[NETWORK],
	$o[BROAD],
	$o[CITY],
	$o[REGION],
	$o[LATITUDE],
	$o[LONGITUDE]
);
$DATA = iconv ('iso-8859-1', 'utf-8', $DATA);
fwrite ($save, $DATA, strlen ($DATA));

fclose ($save);
fclose ($fp);
fclose ($stderr);

echo "\n";
print_r ($conflict);
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
?>
