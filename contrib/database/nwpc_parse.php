#!/usr/bin/php
<?php
/*
 * Copyright (C) 2016 JoungKyun.Kim <http://oops.org>
 * 
 * This file is part of libkrisp
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

define (IPCHACKER, true);

define (START, 0);
define (END, 1);
define (REGION, 2);
define (CITY, 3);
define (NAME, 4);

Class NWPCB {
	private $SAVE = './nwpc.csv';
	private $fips;
	private $src = './PCBangIPlist.txt';
	private $nwzformat = false;
	private $nwzdebug = false;
	private $stderr = null;
	private $fp = null;

	private $geoip = false;
	private $gi = null;

	function __construct ($argc, $argv) { // {{{
		if ( file_exists ('../lib/fix-eng-gu.php') ) {
			require_once '../lib/fix-eng-gu.php';
		}

		$this->SAVE .= '-' . date ('Ymd');

		$this->stderr = fopen ('php://stderr', 'w');

		// GeoIP function init
		if ( ! extension_loaded ('geoip') ) {
			if ( version_compare (PHP_VERSION, '5.3.0') < 0 )
				dl ('geoip.so');

			if ( function_exists ('GeoIP_open') ) {
				$geoip_t = GEOIP_INDEX_CACHE | GEOIP_CHECK_CACHE;
				$this->gi    = GeoIP_open ($_geoip_t);
				$this->geoip = true; // extension
			} else {
				@include_once 'Net/GeoIP.php';

				if ( ! class_exists ('Net_GeoIP') ) {
					error_log ('ERROR: This system don\'t support GeoIP extension or Pear', 0);
					error_log ('       Get php extension on http://cvs.oops.org/index.cgi/mod_geoip/?cvsroot=OOPS-PHP or', 0);
					error_log ('       Get php pear on http://pear.php.net/package/Net_GeoIP/', 0);
					exit (1);
				}

				$this->gi = Net_GeoIP::getInstance ('/usr/share/GeoIP/GeoIP.dat', Net_GeoIP::MEMORY_CACHE);
			}
		} else {
			$this->geoip = 1;
			$geoip_t = GEOIP_INDEX_CACHE | GEOIP_CHECK_CACHE;
			$this->gi = GeoIP_open ($_geoip_t);
		}

		if ( file_exists ('/usr/local/src/cvs/php/pear_krisp/v1/KRISP/georegion.php') ) {
			require_once '/usr/local/src/cvs/php/pear_krisp/v1/KRISP/georegion.php';
			$this->fips = $FIPS_K;
		}

		if ( trim ($argv[1]) && file_exists ($argv[1]) ) {
			$this->src = $argv[1];
		}

		if ( ! file_exists ($this->src) ) {
			error_log ("ERROR: {$this->src} not found", 0);
			exit (1);
		}

		error_log ("* Source file results: {$this->src}", 0);
		error_log ("* Save data results: {$this->SAVE}", 0);

		if ( ($argc == 3 || $argc == 4) && $argv[2] == 'nwz' )
			$this->nwzformat = true;

		$this->nwzdebug = ($argc == 4) ? 1 : 0;

		$this->fp = fopen ($this->SAVE, 'wb');
		if ( ! is_resource ($this->fp) ) {
			fprintf ($this->stderr, "ERROR: %s open failed\n", $this->SAVE);
			exit (1);
		}
	} // }}}

	function ip2long ($ip) { // {{{
		return sprintf ('%u', ip2long ($ip));
	} // }}}

	function addr_parser ($str) { // {{{
		unset ($_s);
		unset ($_t);
		$_s[] = '/^"/';
		$_t[] = '';
		$_s[] = '/^(서울|부산|대구|광주|인천|대전|울산)(.*)/';
		$_t[] = '\\1 \\2';
		$_s[] = '/^(경기|강원|충남|충북|경남|경북|전남|전북|제주)(...+(시|군)).*/';
		$_t[] = '\\1 \\2';

		$p = preg_replace ($_s, $_t, $str);
		return $p;
	} // }}}

	function fix_ip (&$ip) { // {{{
		$p = explode ('.', $ip);
		foreach ( $p as $v )
			$r[] = sprintf ('%d', trim ($v));

		$ip = join ('.', $r);
	} // }}}

	function fix_city ($c) { // {{{
		$c = preg_replace ('/(시|군)$/', '', $c);
		$_c = $this->fips['cityMap'][$c];
		$_c = fixgu ($_c ? $_c : $c); 

		return $_c;
	} // }}}

	function fix_region ($_r) { // {{{
		$key = array_search ($_r, $this->fips['short']);
		return $key ? $key : $_r;
	} // }}}

	function fixgu ($city) { // {{{
		global $fix_eng_gu;

		if ( ! is_array ($fix_eng_gu) || ! count ($fix_eng_gu) )
			return $city;

		return $fix_eng_gu[$city] ? $fix_eng_gu[$city] : $city;
	} // }}}

	function init_src () { // {{{
		$fp = fopen ($this->src, 'rb');
		if ( ! is_resource ($fp) ) {
			fprintf ($this->stderr, "Source file %s is not found\n", $this->src);
			exit (1);
		}

		error_log ("* Read line", 0);
		$i = 1;
		while ( ($buf = fgets ($fp, '1024')) ) {
			// change utf8 format
			$buf = iconv ('cp949', 'utf-8', trim ($buf));

			if ( preg_match ('/테스트|test/', $buf) ) {
				fprintf ($this->stderr, "  ==> %d\r", $i++);
				continue;
			}

			$component = explode ('|', $buf);
			/*
			 * component array
			 * 0 - PCBang address
			 * 1 - PCBang name
			 * 2 - class of ip range ( a.b.c )
			 * 3 - start ip of ip range ( d )
			 * 4 - end ip of ip range ( e )
			 */

			$this->fix_ip ($component[2]);
			$b[0] = $this->ip2long ($component[2] . '.' . $component[3]);
			$b[1] = $this->ip2long ($component[2] . '.' . $component[4]);
			$component[0] = $this->addr_parser ($component[0]);
			$region = preg_split ('/[ ]+/', $component[0]);
			$b[2] = $region[1];
			$b[3] = $region[0];
			$b[4] = trim ($component[1]);

			$r[] = join ('|', $b);
			fprintf ($this->stderr, "  ==> %d\r", $i++);
		}
		fprintf ($this->stderr, "%20s\r* Sort data start\n", ' ');
		natsort ($r);

		return $r;
	} // }}}

	function confirm_ip ($ip) { // {{{
		if ( ! is_numeric ($ip) )
			$ip = $this->ip2long ($ip);

		// 1.0.0.0 <-> 255.255.255.255
		if ( $ip >= 16777216 && $ip <= 4294967295 )
			return true;

		return false;
	} // }}}

	function check_ip ($ip) { // {{{
		if ( $this->confirm_ip ($ip) == false )
			return false;

		if ( is_numeric ($ip) )
			$ip = long2ip ($ip);

		if ( $this->geoip ) {
			$_gir = GeoIP_id_by_name ($this->gi, $ip);
			$gir = $_gir['code'];
		} else
			$gir = $this->gi->lookupCountryCode ($ip);

		if ( $gir != 'KR' )
			return false;

		return true;
	} // }}}

	function parse_write ($d) { // {{{
		if ( $this->nwzdebug )
			$nup = sprintf ("%-3d\t", $d[END] - $d[START]);

		if ( $this->nwzformat ) {
			$DATA = sprintf (
				"%s%s\t%s\t%s|%s|%s\n",
				$nup,
				$d[START],
				$d[END],
				$d[NAME],
				$d[CITY],
				$d[REGION]
			);
		} else {
			$DATA = sprintf (
				"%s%s\t%s\t%s|%s\n",
				$nup,
				$d[START],
				$d[END],
				$d[CITY],
				$d[REGION]
			);
		}

		fwrite ($this->fp, $DATA, strlen ($DATA));
	} // }}}

	function duplicated_msg ($old, $cur) { // {{{
		fprintf ($this->stderr, "ERROR: duplicated ..\n");
		fprintf ($this->stderr, "       $old\n");
		fprintf ($this->stderr, "       $cur\n");
	} // }}}

	function parse () { // {{{
		// 시작/마지막 IP를 '|'로 구분해서 소트함
		$lines = $this->init_src();

		$c = array (0, 0, 0, 0, 0);

		error_log ("* Start line parse", 0);
		$oldline = '';
		$i = 0;
		foreach ( $lines as $line ) {
			fprintf ($this->stderr, "  ==> %d\r", ++$i);

			$cur = explode ('|', $line);
			// 한국 IP가 아니면 무시
			if ( ! $this->check_ip ($cur[START]) || ! $this->check_ip ($cur[END]) )
				continue;

			if ( ! $oldline ) {
				$oldline = $line;
				continue;
			}

			//echo "$oldline\n";
			//echo "$line";
			//sleep (1);
			if ( $oldline == $line ) {
				//echo " .. skip\n\n";
				continue;
			} else {
				//echo "\n\n";
			}

			$old = explode ('|', $oldline);

			$oname = $old[REGION].$old[CITY].$old[NAME];
			$cname = $cur[REGION].$cur[CITY].$cur[NAME];

			if ( $oname == $cname && ($old[END] + 1) == $cur[START] ) {
				$old[END] = $cur[END];
				continue;
			}


			// 아래 경우 외에 더 있을까? 소트가 되었으니 없을 것 같은데.. T.T
			// 중복이 될 경우, 작은 range에 우선권을 준다.

			// old ------
			// cur ------
			if ( $old[START] == $cur[START] && $old[END] == $cur[END] ) {
				$c[0]++;
				// 이건 위에서 처리 했는데.. 명시적으로 나둔다.
				if ( $oname == $cname )
					continue;

				$this->duplicated_msg ($oldline, $line);
				exit (1);
			}
			// old ---
			// cur ------
			else if ( $old[START] == $cur[START] && $old[END] < $cur[END] ) {
				$c[1]++;
				if ( $oname == $cname )
					continue;

				$this->duplicated_msg ($oldline, $line);
				$write = $cur;
				$old[START] = $cur[END] + 1;
			}
			// old ------
			// cur   ---
			else if ( $old[START] < $cur[START] && $old[END] > $cur[END] ) {
				$c[2]++;
				if ( $oname == $cname )
					continue;

				$this->duplicated_msg ($oldline, $line);
				$write = $old;
				$write[END] = $cur[START] - 1;
				$this->parse_write ($write);
				$write = $cur;
				$old[START] = $cur[END] + 1;
			}
			// old ------ or -------
			// cur   ----      -------
			else if ( $old[START] < $cur[START] && $old[END] >= $cur[START] ) {
				$c[3]++;
				if ( $oname == $cname ) {
					$old[END] = $cur[END];
					$oldline = join ('|', $old);
					continue;
				}

				$this->duplicated_msg ($oldline, $line);
				$write = $old;
				$write[END] = $cur[START] - 1;
				$old = $cur;
			}
			else {
				$c[4]++;
				// 마지막이 겹칠 경우가 누락되어 있으니 처리 하자!
				$write = $old;
				$old = $cur;
			}
			$oldline = join ('|', $old);
			$this->parse_write ($write);
		}

		if ( $oldline ) {
			$old = explode ('|', $oldline);
			$this->parse_write ($old);
		}

		print_r ($c);
	} // }}}

	function __destruct () { // {{{
		if ( is_resource ($this->fp) )
			fclose ($this->fp);
		if ( is_resource ($this->stderr) )
			fclose ($this->stderr);
	} // }}}
}

$pcb = new NWPCB ($argc, $argv);
$pcb->parse ();
?>
