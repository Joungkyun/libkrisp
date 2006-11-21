#!/usr/bin/php
<?
#
# $Id: krisp_mksql.php,v 1.6 2006-11-21 09:19:05 oops Exp $
#
# get Korea ISP information to text format and make krisp database sql
#

class domesticIP {
	var $url = "http://online.nida.or.kr/link/ISPAllocation.jsp";

	function domesticIP ($f = '') {
		$r = $this->getpage ($f);
		$this->r = $this->makeArray ($r);
	}

	# get url context and parse krisp needed text form
	#
	# form is follows:
	# "korean-ispname  6KANet  203.254.0.0  203.254.63.255  19940401"
	#
	function getpage ($f = '') {
		$v = @file_get_contents ($f ? $f : $this->url);

		if ( $v === FALSE ) :
			error_log ("ERROR: URL open failed ($this->url)", 0);
			exit (1);
		endif;

		if ( ! preg_match ('/6KANet|한국전산원/', $v) ) :
			error_log ("ERROR: invalid data format", 0);
			exit (1);
		endif;

		if ( ! $f ) :
			# remove carage return
			$_src[] = "/\r/";
			$_des[] = '';

			# removed html code
			$_src[] = '/<[^>]+>/';
			$_des[] = '';

			# remove header
			$_src[] = '/.*배정일자/s';
			$_des[] = '';

			$_src[] = "/^\s*&nbsp;\s*/m";
			$_des[] = '';
			$_src[] = '/^[\s]+([0-9])/m';
			$_des[] = "\\1";
		else :
			$_src[] = '/[\t]+/';
			$_des[] = "\n";
		endif;

		$r = trim (preg_replace ($_src, $_des, $v));

		return $r;
	}

	function makeArray ($v) {
		$p = explode ("\n", $v);
		$r = array ();
		$i = 0;

		foreach ($p as $_v) :
			$_v = trim ($_v);

			if ( preg_match ('/^[0-9]+$/', $_v) ) :
				$r[$i] .= $_v;

				# array check
				$chk = split ("[\t]+", $r[$i]);
				if ( count ($chk) != 5 ) :
					error_log ("ERROR: invalid data format", 0);
					exit (1);
				endif;

				$i++;
			else :
				$r[$i] .= "$_v\t";
			endif;
		endforeach;

		return $r;
	}
}

class mkSQL {
	var $types;

	function mkSQL ($lists, $_t = 'sql') {
		$size = count ($lists);

		## get array and get block group
		##
		for ( $i=0; $i<$size; $i++ ) {
			list ($kblock, $eblock, $s, $e) = preg_split ('/\t+/', trim ($lists[$i]));

			$kblock = trim ($kblock);
			$eblock = trim ($eblock);
			$e = trim ($e);
			$s = trim ($s);

			if ( preg_match ('/^#/', $kblock) || ! $kblock ) :
				continue;
			endif;

			if ( ! $s || preg_match ('/[^0-9.]|^[0-9]+$/', $s) ) :
				error_log ("ERROR: invalide data format\n", 0);
				exit (1);
			endif;

			if ( ! $e || preg_match ('/[^0-9.]|^[0-9]+$/', $e) ) :
				error_log ("ERROR: invalide data format\n", 0);
				exit (1);
			endif;

			$s = $this->inet_aton ($s);
			$e = $this->inet_aton ($e);

			$this->types = $_t;

			$krblock[$eblock][$s] = array ($kblock, $eblock, $s, $e);
		}

		$merge = array ();

		## merge
		##
		foreach ( $krblock AS $arr ) :
			unset ($start); // reset
			ksort ($arr);

			$new = array_values ($arr);
			$size = sizeof ($new);

			for ( $i=0; $i<$size; $i++ ) :
				$total++;
				$kname = $new[$i][0];
				$ename = $new[$i][1];
				$start = $start ? $start : $new[$i][2];
				$end   = $new[$i][3];

				if ( ($new[$i][3] + 1) == $new[$i+1][2] ) :
					continue; // not next skip
				endif;

				$m++; // count of merge

				## patch 2005.11.07, is if (TRUE)
				##
				$nn = $start + $this->inet_aton ('255.255.255.255') - $end;
				$nu = $end - $start + 1; // number of host

				$merge[$ename][] = array ($kname, $ename, $start, $end, $nu, $nn);

				unset ($nu);
				unset ($start);
			endfor;
		endforeach;

		$class_a = $this->inet_aton ('127.0.0.0');
		$class_b = $this->inet_aton ('192.0.0.0');
		$me = 0;

		foreach ( $merge AS $arr ) :
			foreach ( $arr AS $tmp ) :
				$kname = $tmp[0];
				$ename = $tmp[1];
				$start = $tmp[2];
				$end   = $tmp[3];
				$nu	= $tmp[4];
				$nn	= $tmp[5];

				if ($nu) :
					$ln = strlen (preg_replace ('/^([1]+).*$/', '\1', decbin ($nn)));
					$sbit = decbin ($start);

					if ( $start < $class_a ) :
						$ln = 1; // A class
					elseif ( $start < $class_b ) :
						$ln = 8; // B class
					endif;

					while ( $start < $end ) :
						list ($tend, $mask) = $this->search_net ($ln, $start, $end);

						$ps = $this->inet_ntoa ($start);
						$pe = $this->inet_ntoa ($tend);
						$netmask = $this->inet_ntoa ($mask);
						$idx = explode ('.', $ps);

						$_IPNETWORK[$idx[0]]['netmask'][] = $netmask;
						$_IPNETWORK[$idx[0]][$this->inet_aton ($ps)] = array ($ps, $pe, $netmask, $kname, $ename);

						$start = $tend + 1; // change
						$me++;
					endwhile;
				else :
					$ps = $this->inet_ntoa ($start);
					$pe = $this->inet_ntoa ($end);
					$mask = $start + $this->inet_aton ('255.255.255.255') - $end;
					$netmask = $this->inet_ntoa ($mask);
					$idx = explode ('.', $ps);

					$_IPNETWORK[$idx[0]]['netmask'][] = $netmask;
					$_IPNETWORK[$idx[0]][$this->inet_aton ($ps)] = array ($ps, $pe, $netmask, $kname, $ename);
					$me++;
				endif;
			endforeach;
		endforeach;

		$ipas = array_keys ($_IPNETWORK);
		sort ($ipas);

		# crete table sql
		$this->makeTable ();

		foreach ( $ipas AS $ipa ) :
			$_NETMASK = array_unique ($_IPNETWORK[$ipa]['netmask']);
			sort ($_NETMASK);

			$v = array_values ($_NETMASK);
			$s = sizeof ($v) - 1;

			for ( $i=0; $i<$s; $i++ ) :
				if ( $this->types == 'sql' ) :
					$sql = "INSERT INTO netmask (net, subnet) VALUES ('$ipa', '$v[$i]');";
				else :
					$sql = "$ipa\t$v[$i]";
				endif;
				echo $sql."\n";
			endfor;

			if ( $this->types == 'sql' ) :
				$sql = "INSERT INTO netmask (net, subnet) VALUES ('$ipa', '$v[$s]');";
			else:
				$sql = "$ipa\t$v[$s]";
			endif;
			echo $sql."\n";
		endforeach;

		echo "\n";

		foreach ( $ipas AS $ipa ) :
			unset ($_IPNETWORK[$ipa]['netmask']);
			ksort ($_IPNETWORK[$ipa]);

			foreach ( $_IPNETWORK[$ipa] AS $k=>$arr ) :
				if ( $this->types == 'sql' ) :
					#$sql = "INSERT INTO isp (longip, network, broadcast, netmask, organization, servicename) " .
					#		"VALUES ('$k', '$arr[0]', '$arr[1]', '$arr[2]', '$arr[3]', '$arr[4]');";
					$sql = "INSERT INTO isp " .
							"VALUES ('$k', '$arr[0]', '$arr[1]', '$arr[2]', '$arr[3]', '$arr[4]');";
				else :
					$sql = "$k\t{$arr[0]}\t{$arr[1]}\t{$arr[2]}\t{$arr[3]}\t{$arr[4]}";
				endif;
				echo $sql."\n";
			endforeach;
		endforeach;

		//print_r ($ipas);
		//print_r ($_IPNETWORK);

		#echo $total.' '.$me."\n";
	}

	function makeTable () {
		echo "CREATE TABLE netmask " .
			"(" .
			"   no integer NOT NULL DEFAULT '0'," .
			"   net integer NOT NULL DEFAULT '0'," .
			"   subnet char(15) NOT NULL DEFAULT '0.0.0.0'," .
			"   PRIMARY KEY (no)" .
			");\n\n";

		echo "CREATE TABLE isp " .
			"(" .
			"   longip char(10) NOT NULL DEFAULT '0'," .
			"   network char(15) NOT NULL DEFAULT '0.0.0.0'," .
			"   broadcast char(15) NOT NULL DEFAULT '0.0.0.0'," .
			"   netmask char(15) NOT NULL DEFAULT '0.0.0.0'," .
			"   organization varchar," .
			"   servicename varchar," .
			"   PRIMARY KEY (longip)" .
			");\n\n";
	}

	function search_net ($ln, $start, $end) {
		for ( $l=$ln; $l<=32; $l++ ) :
			$hbit = 32 - $l;
			$host = pow (2, $hbit);
			$tbit = str_repeat (1, $l) . str_repeat (0, $hbit);
			$tnet = bindec ($tbit);
			$tend = $start + $host - 1;

			if ( $tend > $end ) :
				continue;
			endif;

			$tand = bindec ($tbit & sprintf ('%032b', $tend));

			if ( $start == $tand ) :
				$nstart = $tend + 1; // change
				break;
			endif;
		endfor;

		return array ($tend, $tnet);
	}

	function inet_aton ($ipaddr) {
		return sprintf ('%u', ip2long ($ipaddr));
	}

	function inet_ntoa ($base) {
		return long2ip ($base);
	}
}

function usage ($prog) {
	error_log ("USAGE: ${prog} [type]", 0);
	error_log ("       ${prog} txt", 0);
	error_log ("       ${prog} sql", 0);
	error_log ("       ${prog} csv", 0);
	error_log ("       ${prog} sql /path/txtformat", 0);
	error_log ("       ${prog} csv /path/txtformat", 0);
	error_log ("Option: type", 0);
	error_log ("        txt => original text format", 0);
	error_log ("        sql => parse sql qeury format", 0);
	error_log ("        sql => parse csv format", 0);

	exit (1);
}


if ( $argc != 2 && $argc != 3 ) :
	usage ($argv[0]);
endif;

if ( $argc == 3 ) :
	if ( $argv[1] != 'sql' && $arvg[1] != 'csv' ) :
		usage ($argv[0]);
	endif;

	if ( $argv[2] ) :
		if ( ! file_exists ($argv[2]) ) :
			error_log ("ERROR: {$argv[2]} is not found", 0);
			exit (1);
		else :
			$txtformat = $argv[2];
		endif;
	else :
		$txtformat = '';
	endif;
endif;


$_type = $argv[1];

if ( $_type != 'txt' && $_type != 'sql' && $_type != 'csv' ) :
	usage ($argv[0]);
endif;

$ip = new domesticIP ($txtformat);

if ( $_type == 'txt' ) :
	foreach ( $ip->r as $v ) :
		echo $v . "\n";
	endforeach;
	exit (0);
endif;

if ( ! count ($ip->r) ) :
	error_log ("ERROR: ip list array is empty\n", 0);
	exit (1);
endif;

new mkSQL ($ip->r, $_type);
?>
