#!/usr/bin/php
<?php
/*
 * get KRNIC's Korea IP and ISP information
 *
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


define ('KRIP_DATA', 'http://ip.kisa.or.kr/ip_cate_stat/stat_05_04_toexcel.act');
define ('KRISP_DATA', 'http://ip.kisa.or.kr/ip_cate_stat/stat_06_02_toexcel.act');
#define ('KRIP_DATA', 'krip.txt');
#define ('KRISP_DATA', 'krisp.txt');

function banner () {
	global $argv;

	echo <<<EOF
 * Gathering KRNIC IP database
 *

EOF;
}

class nDom {
	const KRIP_MODE  = 0;
	const KRISP_MODE = 1;

	static public $row;
	static public $err = 0;

	static private $MODE;
	static private $encoding;

	/*
	 * mode 0 => Korea IP data
	 *      1 => Korea ISP data
	 */
	function __construct ($mode, $charset = 'utf-8') {
		self::init ($mode, $charset);

		$this->row = &self::$row;
		$this->err = &self::$err;
	}

	function init ($mode, $charset = 'utf-8') {
		self::$MODE = $mode;
		self::$encoding = $charset;

		$f = self::$MODE ? KRISP_DATA : KRIP_DATA;

		#$context = file_get_contents ($f);
		#$context = KRNIC_data::get ($f);
		$context = KRNIC_data::getPage ($f);
		if ( $context === FALSE ) {
			error_log ("ERROR: URL open failed ($f)", 0);
			exit (1);
		}
		$context = self::decode_entities ($context);

		$dom = new DOMDocument ();
		$dom->encoding = 'utf-8';
		$dom->formatOutput = true;
		#
		# 메모리에 HTML dom tree 를 load.
		# 주의: loadHTMLFile 은 utf8 로 처리가 된다. 단, 입력 데이터가 iso-8859-1
		#       로 가정을 하기 때문에 출력시에 utf8 을 iso-8859-1 로 변환 시켜야
		#       한다.
		#
		$dom->loadHTML ($context);

		# get table entity
		# Excel format 은 1번째 테이블에 위치
		$tbls = self::entity ($dom, 'table');

		self::$row = '';
		$tno = 0;
		foreach ( $tbls as $tbl ) {
			# DATA 는 1번째 테이블이므로, 그 이후는 skip
			if ( $tno++ != 0 )
				break;

			$trs = self::entity ($tbl, 'tr');

			$trno = 0;
			foreach ( $trs as $tr ) {
				# 첫번째 tr 은 테이블 헤더, skip
				switch (self::$MODE) {
					case self::KRIP_MODE :
						$start_tr = 3;
						$td_skip  = array (3, 4);
						$td_max   = 5;
						break;
					case self::KRISP_MODE :
						$start_tr = 2;
						$td_skip  = array (5);
						$td_max   = 6;
						break;
				}
					
				if ( $trno++ < $start_tr )
					continue;

				$tds = self::entity ($tr, 'td');

				unset ($row);
				$no = 0;
				foreach ( $tds as $td ) {
					$no++;
					if ( array_search ($no, $td_skip) !== false )
						continue;

					if ( $no === ($td_max) )
						$row = self::str ($td) . "\t" . $row; 
					else
						$row .= self::str ($td) . "\t";
				}

				if ( $no != $td_max ) {
					self::$err = 1;
					return;
				}

				self::$row .= trim ($row) . "\n";
			}
		}

		self::$row = trim (self::$row);
	}

	function entity ($stream, $tag) {
		return $stream->getElementsByTagName ($tag);
	}

	function str ($stream) {
		if ( self::$encoding == 'utf8')
			return $stream->textContent;

		return trim (iconv ('utf8', self::$encoding, $stream->textContent));
	}

	function decode_entities ($str) {
		$r = array ('/&nbsp;/');
		$d = array (' ');

		return preg_replace ($r, $d, $str);
	}
}

class KRNIC {
	const KRIP       = 0;
	const KRISP      = 1;

	static public $r;

	function __construct () {
		self::init();
		$this->r   = &self::$r;
	}

	function init ($encoding = 'euc-kr') {
		/*
		 * KRNIC IP Allocation Data
		 */
		printf ('%4s. %-45s .. ', '', 'KRNIC IP Allocation Data');
		$r = self::getpage (self::KRIP, $encoding);
		if ( $r === false )
			echo "Failed\n";

		$save = './krnic-allocation-ip.csv' . date ('-Ymd');
		$status = file_put_contents ($save, trim ($r));
		printf ("%s\n", ($status !== false) ? $save : 'Failed');

		/*
		 * ISP IP Allocation Data
		 */
		printf ('%4s. %-45s .. ', '', 'KRNIC ISP Allocation Data');
		$r = self::getpage (self::KRISP, $encoding);
		if ( $r === false )
			echo "Failed\n";

		$data = explode ("\n", trim ($r));
		/* sort by date */
		sort ($data);
		$r = implode ("\n", $data);
		unset ($data);

		$save = './isp-allocation-ip.csv' . date ('-Ymd');
		$status = file_put_contents ($save, $r);
		printf ("%s\n", ($status !== false) ? $save : 'Failed');
	}

	# get url context and parse krisp needed text form
	#
	# form is follows:
	# "korean-ispname  6KANet  203.254.0.0  203.254.63.255  19940401"
	#
	function getpage ($type = KRIP, $encoding = 'euc-kr') {
		nDom::init ($type, $encoding);

		// ERROR: invalid data format
		if ( nDom::$err )
			return false;

		return nDom::$row;
	}
}

Class KRNIC_data {
	static public $useragent = 'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)';

	function get ($url) {
		if ( false === ($cookie = self::getCookie ($url)) )
			return false;

		if ( false === ($data = self::getPage ($url, $cookie)) )
			return false;

		return $data;
	}

	function getPage ($url, $cookie = '') {
		$c = curl_init ($url);

		curl_setopt ($c, CURLOPT_URL, $url);
		curl_setopt ($c, CURLOPT_TIMEOUT, 60);
		curl_setopt ($c, CURLOPT_NOPROGRESS, 1);
		curl_setopt ($c, CURLOPT_RETURNTRANSFER, 1);
		curl_setopt ($c, CURLOPT_USERAGENT, self::$useragent);

		$src = array ('!http[s]?://!', '!/.*!');
		$dst = array ('', '');
		$host = preg_replace ($src, $dst, $url);

		$header[] = 'Host: ' . $host;
		#$header[] = 'Excpet:';

		curl_setopt ($c, CURLOPT_HEADER, 0);
		curl_setopt ($c, CURLOPT_NOBODY, 0); 
		curl_setopt ($c, CURLOPT_HTTPHEADER, $header);
		curl_setopt ($c, CURLOPT_FAILONERROR, 1);

		curl_setopt ($c, CURLOPT_SSL_VERIFYPEER, false);

		if ( $cookie )
			curl_setopt ($c, CURLOPT_COOKIE, $cookie);

		$data = curl_exec($c);

		if ( empty ($data) ) {
			error_log ('Error: ' . curl_error ($c), 0);
			return false;
		}
		curl_close ($c);

		return $data;
	}

	function getCookie ($url) {
		$data = self::getPage ($url);
		preg_match ('/(_accessKey2=[^\']+)\'/', $data, $m);

		if ( ! trim ($m[1]) ) {
			error_log ('Error: Can\'t get krnic cookies => ' . $m[1], 0);
			return false;
		}

		return $m[1];
	}
}

/*
 * Main
 */

banner ();
KRNIC::init ('utf-8');

exit (0);

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
?>
