#!/usr/bin/php
<?
# pear eSystem
require_once 'eSystem.php';

function usage() {
	echo "krispGeoIP: make krisp database sql from GeoIP CSV\n";
	echo "Usage: krispGeoIP -[ic] CSV_file\n";
	echo "Option:\n";
	echo "         -i                GeoIP ISP mode\n";
	echo "         -c                GeoIP City mode\n";
	echo "         -d                GeoIP mode [default]\n";
	echo "         -t [csv|sql]      output result [defalut: sql]\n";
	echo "  if make city database, argv1 is block and argv2 is location cvs foramt\n";
	echo "  krispGeoIP -c GeoCity-Blocks.csv GeoCity-Location.csv\n\n";

	exit (1);
}

class GeoIPs {
	var $mode;
	var $tp;
	var $csv;
	var $nmask;

	function GeoIPs ($m, $t, $c) {
		$this->mode = $m;
		$this->tp   = $t;
		$this->csv  = $c[0];
		$this->csv1 = $c[1];

		switch ($this->mode) :
			case 'isp' :
				$this->isp ();
				break;
			case 'city' :
				$this->city ();
				break;
			default:
				$this->ip ();
		endswitch;
	}

	function netmask ($s, $e) {
		$s = preg_match ('/\./', $s) ? ip2long ($s) : $s;
		$e = preg_match ('/\./', $e) ? ip2long ($e) : $e;
		$self = ip2long ('255.255.255.255');
		return long2ip ($s + $_self - $e - 1);
	}

	function ip () {
		$line = $this->getArray ($this->csv);
		if ( $line == 1 ) :
			return NULL;
		endif;

		if ( $this->tp == 'sql' ) :
			echo "CREATE TABLE ip (" .
				"lsip int(4) NOT NULL DEFAULT '0', ".
				"leip int(4) NOT NULL DEFAULT '0', " .
				"code char (3) NOT NULL DEFAULT '--', " .
				"org varchar, " .
				"PRIMARY KEY (lsip, leip));\n\n";
		endif;

		foreach ( $line as $v ) :
			$r = explode (',', trim (str_replace ('"', '', $v)));
			#$mask = $this->netmask ($r[0], $r[1]);
			#$aclass = $this->getAclass ($r[0]);

			#$this->nmask[$aclass][$mask]++;

			if ( $this->tp == 'sql' ) :
				#echo "INSERT INTO ip (longs, longe, scode, lcode)" .
				echo "INSERT INTO ip" .
					" VALUES (\"{$r[2]}\", \"{$r[3]}\", \"{$r[4]}\", \"{$r[5]}\");\n";
			else:
	            echo "{$r[2]}|{$r[3]}|{$r[4]}|{$r[5]}\n";
			endif;
			$i++;
		endforeach;
		#echo $i."\n";

		#print_r ($this->nmask);
	}

	function isp () {
		$line = $this->getArray ($this->csv);
		if ( $line == 1 ) :
			return NULL;
		endif;

		if ( $this->tp == 'sql' ) :
			echo "CREATE TABLE isp (" .
				"lsip int(4) NOT NULL DEFAULT '0', " .
				"leip int(4) NOT NULL DEFAULT '0', " .
				"isp varchar, " .
				"PRIMARY KEY (lsip, leip));\n\n";
		endif;

		foreach ( $line as $v ) :
			$r = explode (',', trim (str_replace ('"', '', $v)));
			if ( $this->tp == 'sql' ) :
				echo "INSERT INTO isp" .
					" VALUES ('{$r[0]}', '{$r[1]}', '{$r[2]}');\n";
			else:
				echo "{$r[0]}|{$r[1]}|{$r[2]}\n";
			endif;
			$i++;
		endforeach;
		#echo $i."\n";
	}

	function city () {
		$loc = $this->city_loc ();
		if ( $loc == 1 ) :
			return NULL;
		endif;

		$line = $this->getArray ($this->csv);
		if ( $line == 1 ) :
			return NULL;
		endif;

		if ( $this->tp == 'sql' ) :
			echo "CREATE TABLE city (" .
				"lsip int(4) NOT NULL DEFAULT '0', " .
				"leip int(4) NOT NULL DEFAULT '0', " .
				"city varchar, " .
				"PRIMARY KEY (lsip, leip));\n\n";
		endif;

		foreach ( $line as $v ) :
			if ( preg_match ('/^[^0-9"]/', $v) ) :
				continue;
			endif;

			$r = explode (',', trim (str_replace ('"', '', $v)));
			if ( $this->tp == 'sql' ) :
				echo "INSERT INTO city" .
					" VALUES (\"{$r[0]}\", \"{$r[1]}\", \"{$loc[$r[2]]}\");\n";
			else:
				echo "{$r[0]}|{$r[1]}|{$loc[$r[2]]}\n";
			endif;
			$i++;
		endforeach;

	}

	function city_loc () {
		$line = $this->getArray ($this->csv1);
		if ( $line == 1 ) :
			return 1;
		endif;

		foreach ( $line as $v ) :
			if ( preg_match ('/^[^0-9"]/', $v) ) :
				continue;
			endif;

			$_r = explode (',', trim (str_replace ('"', '', $v)));
			$city = ( $_r[2] && ! is_numeric ($_r[2]) ) ? "{$_r[2]} {$_r[3]}" : $_r[3];
			$r[$_r[0]] = $city;
		endforeach;

		return $r;
	}

	function getArray ($f, $m = '') {
		$line = file ($f);

		if ( ! is_array ($line) ) :
			printf ("%s is not contents\n", $this->csv);
			return 1;
		endif;

		return $line;
	}

	function getAclass ($p) {
		if ( preg_match ('/^[0-9]+$/', $p) )
			$p = long2ip ($p);

		$r = explode ('.', $p);

		return $r[0];
	}
}

# getopt init
$gno    = -1;
$optcno = -1;

$mode = 'ip';
$tp   = 'sql';

$e = new eSystem;


while ( ($opt = $e->getopt ($argc, $argv, 'icdht:')) != -1 ) :
	switch ($opt) :
		case 'i' :
			$mode = 'isp';
			break;
		case 'c' :
			$mode = 'city';
			break;
		case 'd' :
			$mode = 'ip';
			break;
		case 't' :
			$tp = $optarg;
			break;
		default:
			usage ();
	endswitch;
endwhile;

if ( $mode == 'city' ) :
	if ( $optcno != 2 ) :
		usage ();
	endif;
else:
	if ( $optcno != 1 ) :
		usage ();
	endif;
endif;

$_csv[0] = $optcmd[0];

if ( $optcno == 2 )
	$_csv[1] = $optcmd[1];

$g = new GeoIPs ($mode, $tp, $_csv);

?>
