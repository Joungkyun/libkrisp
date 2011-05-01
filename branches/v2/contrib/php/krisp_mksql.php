#!/usr/bin/php
<?
#
# $Id$
#
# get Korea ISP information to text format and make krisp database sql
#

class domesticIP {
	private $url = "http://online.nida.or.kr/link/ISPAllocation.jsp";
	#private $url = 'ISPAllocation.html';
	public $r;

	function __construct ($f = '', $type = 'txt') {
		$r = $this->getpage ($f);

		if ( $type == 'txt' ) {
			$this->r = $r;
			return;
		}

		$this->r = $this->makeArray ($r);
	}

	# get url context and parse krisp needed text form
	#
	# form is follows:
	# "korean-ispname  6KANet  203.254.0.0  203.254.63.255  19940401"
	#
	function getpage ($f = '') {
		if ( $f && file_exists ($f) )
			return file_get_contents ($f);

		$dom = new nDom ($this->url);

		if ( $dom->err ) {
			error_log ("ERROR: invalid data format", 0);
			exit (1);
		}

		return $dom->row;
	}

	function makeArray ($v) {
		return explode ("\n", trim ($v));
	}
}

class nDom {
	public $row;
	public $err = 0;

	function __construct ($f) {
		$context = file_get_contents ($f);
		if ( $context === FALSE ) {
			error_log ("ERROR: URL open failed ($f)", 0);
			exit (1);
		}
		$context = $this->decode_entities ($context);

		$dom = new DOMDocument ();
		#
		# 메모리에 HTML dom tree 를 load.
		# 주의: loadHTMLFile 은 utf8 로 처리가 된다. 단, 입력 데이터가 iso-8859-1
		#       로 가정을 하기 때문에 출력시에 utf8 을 iso-8859-1 로 변환 시켜야
		#       한다.
		#
		$dom->loadHTML ($context);

		# get table entity
		# ISP 정보는 4번째 테이블에 위치
		$tbls = $this->entity ($dom, 'table');

		$tno = 0;
		foreach ( $tbls as $tbl ) {
			# ISP 는 3번째 테이블이므로, 이전 테이블 까지는 skip
			if ( $tno++ != 2 )
				continue;

			$trs = $this->entity ($tbl, 'tr');

			$trno = 0;
			foreach ( $trs as $tr ) {
				# 첫번째 tr 은 테이블 헤더, skip
				if ( $trno++ == 0 )
					continue;

				$tds = $this->entity ($tr, 'td');

				unset ($row);
				$no = 0;
				foreach ( $tds as $td ) {
					$row .= $this->str ($td) . "\t";
					$no++;
				}

				if ( $no != 5 ) {
					$this->err = 1;
					return;
				}

				$this->row .= trim ($row) . "\n";
			}
		}

		$this->row = trim ($this->row);
	}

	function entity ($stream, $tag) {
		return $stream->getElementsByTagName ($tag);
	}

	function str ($stream) {
		return trim (iconv ('utf8', 'iso-8859-1', $stream->textContent));
	}

	function decode_entities ($str) {
		$r = array ('/&nbsp;/');
		$d = array (' ');

		return preg_replace ($r, $d, $str);
	}
}

class mkSQL {
	private $types;

	function __construct ($lists, $_t = 'sql') {
		$size = count ($lists);

		## get array and get block group
		##
		for ( $i=0; $i<$size; $i++ ) {
			list ($kblock, $eblock, $s, $e) = preg_split ('/\t+/', trim ($lists[$i]));

			$kblock = trim ($kblock);
			$eblock = trim ($eblock);
			$e = trim ($e);
			$s = trim ($s);

			if ( preg_match ('/^#/', $kblock) || ! $kblock )
				continue;

			if ( ! $s || preg_match ('/[^0-9.]|^[0-9]+$/', $s) ) {
				error_log ("ERROR: invalide data format\n", 0);
				exit (1);
			}

			if ( ! $e || preg_match ('/[^0-9.]|^[0-9]+$/', $e) ) {
				error_log ("ERROR: invalide data format\n", 0);
				exit (1);
			}

			$s = $this->inet_aton ($s);
			$e = $this->inet_aton ($e);

			$this->types = $_t;

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

				if ( ($new[$i][3] + 1) == $new[$i+1][2] )
					continue; // not next skip

				$m++; // count of merge

				## patch 2005.11.07, is if (TRUE)
				##
				$nn = $start + $this->inet_aton ('255.255.255.255') - $end;
				$nu = $end - $start + 1; // number of host

				$merge[$ename][] = array ($kname, $ename, $start, $end, $nu, $nn);

				unset ($nu);
				unset ($start);
			}
		}

		$class_a = $this->inet_aton ('127.0.0.0');
		$class_b = $this->inet_aton ('192.0.0.0');
		$me = 0;

		foreach ( $merge AS $arr ) {
			foreach ( $arr AS $tmp ) {
				$kname = $tmp[0];
				$ename = $tmp[1];
				$start = $tmp[2];
				$end   = $tmp[3];
				$nu	= $tmp[4];
				$nn	= $tmp[5];

				if ($nu) {
					$ln = strlen (preg_replace ('/^([1]+).*$/', '\1', decbin ($nn)));
					$sbit = decbin ($start);

					if ( $start < $class_a )
						$ln = 1; // A class
					else if ( $start < $class_b )
						$ln = 8; // B class

					while ( $start < $end ) {
						list ($tend, $mask) = $this->search_net ($ln, $start, $end);

						$ps = $this->inet_ntoa ($start);
						$pe = $this->inet_ntoa ($tend);
						$netmask = $this->inet_ntoa ($mask);
						$idx = explode ('.', $ps);

						$_IPNETWORK[$idx[0]]['netmask'][] = $netmask;
						$_IPNETWORK[$idx[0]][$this->inet_aton ($ps)] = array ($ps, $pe, $netmask, $kname, $ename);

						$start = $tend + 1; // change
						$me++;
					}
				} else {
					$ps = $this->inet_ntoa ($start);
					$pe = $this->inet_ntoa ($end);
					$mask = $start + $this->inet_aton ('255.255.255.255') - $end;
					$netmask = $this->inet_ntoa ($mask);
					$idx = explode ('.', $ps);

					$_IPNETWORK[$idx[0]]['netmask'][] = $netmask;
					$_IPNETWORK[$idx[0]][$this->inet_aton ($ps)] = array ($ps, $pe, $netmask, $kname, $ename);
					$me++;
				}
			}
		}

		$ipas = array_keys ($_IPNETWORK);
		sort ($ipas);

		# crete table sql
		$this->makeTable ();

		foreach ( $ipas AS $ipa ) {
			$_NETMASK = array_unique ($_IPNETWORK[$ipa]['netmask']);
			sort ($_NETMASK);

			$v = array_values ($_NETMASK);
			$s = sizeof ($v) - 1;

			for ( $i=0; $i<$s; $i++ ) {
				if ( $this->types == 'sql' )
					$sql = "INSERT INTO netmask (net, subnet) VALUES ('$ipa', '$v[$i]');";
				else
					$sql = "$ipa|$v[$i]";

				echo $sql."\n";
			}

			if ( $this->types == 'sql' )
				$sql = "INSERT INTO netmask (net, subnet) VALUES ('$ipa', '$v[$s]');";
			else
				$sql = "$ipa|$v[$s]";

			echo $sql."\n";
		}

		echo "\n";

		$this->makeTable ('isp');
		foreach ( $ipas AS $ipa ) {
			unset ($_IPNETWORK[$ipa]['netmask']);
			ksort ($_IPNETWORK[$ipa]);

			foreach ( $_IPNETWORK[$ipa] AS $k=>$arr ) {
				if ( $this->types == 'sql' ) {
					#$sql = "INSERT INTO isp (longip, network, broadcast, netmask, organization, servicename) " .
					#		"VALUES ('$k', '$arr[0]', '$arr[1]', '$arr[2]', '$arr[3]', '$arr[4]');";
					$sql = "INSERT INTO isp " .
							"VALUES ('$k', '$arr[0]', '$arr[1]', '$arr[2]', '$arr[3]', '$arr[4]');";
				} else
					$sql = "$k|{$arr[0]}|{$arr[1]}|{$arr[2]}|{$arr[3]}|{$arr[4]}";

				echo $sql."\n";
			}
		}

		//print_r ($ipas);
		//print_r ($_IPNETWORK);

		#echo $total.' '.$me."\n";
	}

	function makeTable ($t = 'netmask') {
		switch ( $t ) {
			case 'netmask' :
				echo "CREATE TABLE netmask " .
					"(" .
					"   no integer NOT NULL DEFAULT '0'," .
					"   net integer NOT NULL DEFAULT '0'," .
					"   subnet char(15) NOT NULL DEFAULT '0.0.0.0'," .
					"   PRIMARY KEY (no)" .
					");\n\n";
				break;
			default :
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
	}

	function search_net ($ln, $start, $end) {
		for ( $l=$ln; $l<=32; $l++ ) {
			$hbit = 32 - $l;
			$host = pow (2, $hbit);
			$tbit = str_repeat (1, $l) . str_repeat (0, $hbit);
			$tnet = bindec ($tbit);
			$tend = $start + $host - 1;

			if ( $tend > $end )
				continue;

			$tand = bindec ($tbit & sprintf ('%032b', $tend));

			if ( $start == $tand ) {
				$nstart = $tend + 1; // change
				break;
			}
		}

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


if ( $argc != 2 && $argc != 3 )
	usage ($argv[0]);

if ( $argc == 3 ) {
	if ( $argv[1] != 'sql' && $argv[1] != 'csv' )
		usage ($argv[0]);

	if ( $argv[2] ) {
		if ( ! file_exists ($argv[2]) ) {
			error_log ("ERROR: {$argv[2]} is not found", 0);
			exit (1);
		} else
			$txtformat = $argv[2];
	} else
		$txtformat = '';
}


$_type = $argv[1];

if ( $_type != 'txt' && $_type != 'sql' && $_type != 'csv' )
	usage ($argv[0]);

$ip = new domesticIP ($txtformat, $_type);

if ( $_type == 'txt' ) {
	echo $ip->r . "\n";
	exit (0);
}

if ( ! count ($ip->r) ) {
	error_log ("ERROR: ip list array is empty\n", 0);
	exit (1);
}

new mkSQL ($ip->r, $_type);
?>
