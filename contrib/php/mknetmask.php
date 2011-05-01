#!/usr/bin/php
<?php
##
## $Id$
##
## -- make network / netmask
## -- 2005.11.07: bug fixed <san2(at)linuxchannel.net>
##    2004.10.??: new build <san2(at)linuxchannel.net>
##    2006.06.09: changed sqlite <JoungKyum.Kim http://www.oops.org>
##
## file format: http://member.nic.or.kr/link/ISPAllocation.jsp
## korean-ispname  6KANet  203.254.0.0  203.254.63.255  19940401

$dbpath = "./krisp-sqlite.dat";

$dbs['net'] = "CREATE TABLE netmask\n" .
			"(\n" .
			"	no integer NOT NULL DEFAULT '0',\n" .
			"	net integer NOT NULL DEFAULT '0',\n" .
			"	subnet char(15) NOT NULL DEFAULT '0.0.0.0',\n" .
			"	PRIMARY KEY (no)\n" .
			")";

$dbs['isp'] = "CREATE TABLE isp\n" .
			"(\n" .
			"	longip char(10) NOT NULL DEFAULT '0',\n" .
			"	network char(15) NOT NULL DEFAULT '0.0.0.0',\n" .
			"	broadcast char(15) NOT NULL DEFAULT '0.0.0.0',\n" .
			"	netmask char(15) NOT NULL DEFAULT '0.0.0.0',\n" .
			"	organization varchar,\n" .
			"	servicename varchar,\n" .
			"	PRIMARY KEY (longip)\n" .
			")";

function search_net ($ln, $start, $end) {
	for ( $l=$ln; $l<=32; $l++ ) {
		$hbit = 32 - $l;
		$host = pow (2, $hbit);
		$tbit = str_repeat (1, $l) . str_repeat (0, $hbit);
		$tnet = bindec ($tbit);
		$tend = $start + $host - 1;

		if ( $tend > $end ) continue;

		$tand = bindec ($tbit & sprintf ('%032b', $tend));

		if ( $start == $tand ) {
			$nstart = $tend + 1; // change
			break;
		}
	}

	return array ($tend, $tnet);
}

function is_subnet ($some) {
	$some = trim ($some);

	if (preg_match ('/^[\d]+\.[\d]+\.[\d]+\.[\d]+$/', $some)) {
		$some = sprintf ('%u', ip2long ($some));
	}

	$dec = abs ($some);
	$bits = decbin ($dec);

	return (boolean) preg_match ('/^[1]+[0]*$/', $bits);
}

function is_network ($some) {
	return FALSE; // all false

	$some = trim ($some);

	if ( ! preg_match ('/^[\d]+\.[\d]+\.[\d]+\.[\d]+$/', $some) ) {
		$some = long2ip ($some);
	}

	$tmp = explode ('.', $some);
	$r = TRUE;

	if ($tmp[3] % 2) $r = FALSE;
	else if ($tmp[2] % 2) $r = FALSE;
	else if ($tmp[1] % 2) $r = FALSE;
	else if ($tmp[0] % 2) $r = FALSE;

	return (boolean) $r;
}

function inet_aton ($ipaddr) {
	return sprintf ('%u', ip2long ($ipaddr));
}

function inet_ntoa ($base) {
	return long2ip ($base);
}

function inet_ntob ($base) {
	return sprintf ('%032b', $base);
}

function inet_atob ($ipaddr) {
	return sprintf ('%032b', ip2long ($ipaddr));
}

function inet_btoa ($bits) {
	return long2ip (sprintf ('%u', $bits));
}

## file format:
## korean-ispname			6KANet	203.254.0.0		 203.254.63.255	19940401
##
if ( ! file_exists ($file=$_SERVER['argv'][1]) ) exit;

$lists = file ($file); // dot.ipv4
$size = sizeof ($lists);

## get array and get block group
##
for ( $i=0; $i<$size; $i++ ) {
	list ($kblock, $eblock, $s, $e) = preg_split ('/\t+/', trim ($lists[$i]));

	$kblock = trim ($kblock);
	$eblock = trim ($eblock);
	$e = trim ($e);
	$s = trim ($s);

	if ( preg_match ('/^#/', $kblock) || ! $kblock ) continue;

	$s = inet_aton ($s);
	$e = inet_aton ($e);

	$krblock[$eblock][$s] = array ($kblock, $eblock, $s, $e);
}

$merge = array ();

## merge
##
foreach ( $krblock AS $arr ) {
	unset ($start); // reset
	ksort ($arr);

	$new = array_values ($arr);
	$size = sizeof ($new);

	for ( $i=0; $i<$size; $i++ ) {
		$total++;
		$kname = $new[$i][0];
		$ename = $new[$i][1];
		$start = $start ? $start : $new[$i][2];
		$end   = $new[$i][3];

		if ( ($new[$i][3] + 1) == $new[$i+1][2] ) {
			continue; // not next skip
		}

		$m++; // count of merge

		## patch 2005.11.07, is if (TRUE)
		##
		$nn = $start + inet_aton ('255.255.255.255') - $end;
		$nu = $end - $start + 1; // number of host

		$merge[$ename][] = array ($kname, $ename, $start, $end, $nu, $nn);

		unset ($nu);
		unset ($start);
	}
}

$class_a = inet_aton ('127.0.0.0');
$class_b = inet_aton ('192.0.0.0');
$me = 0;

foreach ( $merge AS $arr ) {
	foreach ( $arr AS $tmp ) {
		$kname = $tmp[0];
		$ename = $tmp[1];
		$start = $tmp[2];
		$end   = $tmp[3];
		$nu    = $tmp[4];
		$nn    = $tmp[5];

		if ($nu) {
			$ln = strlen (preg_replace ('/^([1]+).*$/', '\1', decbin ($nn)));
			$sbit = decbin ($start);

			if ( $start < $class_a ) $ln = 1; // A class
			else if ( $start < $class_b ) $ln = 8; // B class

			while ( $start < $end ) {
				list ($tend, $mask) = search_net ($ln, $start, $end);

				$ps = inet_ntoa ($start);
				$pe = inet_ntoa ($tend);
				$netmask = inet_ntoa ($mask);
				$idx = explode ('.', $ps);

				$_IPNETWORK[$idx[0]]['netmask'][] = $netmask;
				$_IPNETWORK[$idx[0]][inet_aton ($ps)] = array ($ps, $pe, $netmask, $kname, $ename);

				$start = $tend + 1; // change
				$me++;
			}
		} else {
			$ps = inet_ntoa ($start);
			$pe = inet_ntoa ($end);
			$mask = $start + inet_aton ('255.255.255.255') - $end;
			$netmask = inet_ntoa ($mask);
			$idx = explode ('.', $ps);

			$_IPNETWORK[$idx[0]]['netmask'][] = $netmask;
			$_IPNETWORK[$idx[0]][inet_aton ($ps)] = array ($ps, $pe, $netmask, $kname, $ename);
			$me++;
		}
	}
}

$ipas = array_keys ($_IPNETWORK);
sort ($ipas);

$c = sqlite_open ($dbpath, 0644);
if ( ! is_resource ($c) ) {
	echo "Cant' open $dbpath\n";
	exit (1);
}

# crete database
foreach ( $dbs as $v ) {
	sqlite_query ($c, $v);
}

foreach ( $ipas AS $ipa ) {
	$_NETMASK = array_unique ($_IPNETWORK[$ipa]['netmask']);
	sort ($_NETMASK);

	$v = array_values ($_NETMASK);
	$s = sizeof ($v) - 1;

	for ( $i=0; $i<$s; $i++ ) {
		$sql = "INSERT INTO netmask (net, subnet) VALUES ('$ipa', '$v[$i]')";
		sqlite_query ($c, $sql);
	}
	$sql = "INSERT INTO netmask (net, subnet) VALUES ('$ipa', '$v[$s]')";
	sqlite_query ($c, $sql);
}

foreach ( $ipas AS $ipa ) {
	unset ($_IPNETWORK[$ipa]['netmask']);
	ksort ($_IPNETWORK[$ipa]);

	foreach ( $_IPNETWORK[$ipa] AS $k=>$arr ) {
		$sql = "INSERT INTO isp (longip, network, broadcast, netmask, organization, servicename) " .
			 "			 VALUES ('$k', '$arr[0]', '$arr[1]', '$arr[2]', '$arr[3]', '$arr[4]')";
		if ( sqlite_query ($c, $sql) === FALSE ) {
			echo $sql."\n";
		}
	}
}

sqlite_close ($c);

//print_r ($ipas);
//print_r ($_IPNETWORK);

echo $total.' '.$me."\n";

exit(0);
?>
