#!/usr/bin/php
<?php
#
# $Id: krisp-data.php,v 1.2 2010-07-16 09:38:42 oops Exp $
#
# get Korea ISP information to text format and make krisp database sql
#

function banner () { // {{{
	global $argv;

	echo <<<EOF
 * Generater KRISP data format (TXT, CSV)
 *

EOF;
} // }}}

define ('START_LONG', 0);
define ('END_LONG', 1);
define ('ISP_CODE', 2);
define ('ISP_NAME', 3);
define ('REGDATE', 4);

Class KRNIC_KRISP // {{{
{
	// {{{ properties
	static public $stderr;
	static public $today;
	// }}}

	function __construct () { // {{{
		$this->stderr = fopen ('php://stderr', 'w');
		$this->today = date ('Ymd');

		self::$stderr = &$this->stderr;
		self::$today  = &$this->today;
	} // function __construct }}}

	function krnic ($f) { // {{{
		if ( ! file_exists ($f) || ! is_file ($f) )
			return 1;

		fprintf (self::$stderr, ' * Make KRISP database .. ');

		// sort data {{{
		if ( ($raw_r = file ($f)) === false )
			return 2;

		foreach ( $raw_r as $raw_e ) {
			$raw_e = iconv ('cp949', 'utf8', $raw_e);
			$e = explode ("\t", trim ($raw_e));
			$key = self::inet_aton ($e[3]);
			$raw[$key] = array (self::inet_aton ($e[4]), $e[2], $e[1], $e[0]);
		}

		unset ($raw_r);
		ksort ($raw);
		// sort end }}}

		$csvH = './krisp.csv-' . self::$today;
		$txtH= './iplist.txt-' . self::$today;
		
		$csv = fopen ($csvH, 'wb');
		if ( ! is_resource ($csv) ) {
			if ( is_resource ($p) )
				fclose ($p);
			return 3;
		}

		$txt = fopen ($txtH, 'wb');
		if ( ! is_resource ($txt) ) {
			if ( is_resource ($p) )
				fclose ($p);
			return 3;
		}

		$num = 1;
		$O = '';
		foreach ( $raw as $start => $val ) {
			self::progress ($num++);

			$C = array ($start, $val[0], $val[1], $val[2], $val[3]);

			if ( ! is_array ($O) ) {
				$O = $C;
				continue;
			}

			if ( ($O[END_LONG] + 1) == $C[START_LONG] && $O[ISP_CODE] == $C[ISP_CODE] ) {
				$O[END_LONG] = $C[END_LONG];
				continue;
			}

			$DATA = sprintf (
				"%s\t%s\tKR|Korea, Republic of|%s|%s\n",
				$O[START_LONG],
				$O[END_LONG],
				$O[ISP_CODE],
				$O[ISP_NAME]
			);
			fwrite ($csv, $DATA, strlen ($DATA));

			//$O[ISP_NAME] = preg_replace ('/\'/', '\'\'', $O[ISP_NAME]);
			$DATA = sprintf (
				"%s\t%s\t%s\t%s\t%s\n",
				$O[ISP_CODE],
				$O[ISP_NAME],
				self::inet_ntoa ($O[START_LONG]),
				self::inet_ntoa ($O[END_LONG]),
				$O[REGDATE]
			);
			fwrite ($txt, $DATA, strlen ($DATA));

			$O = $C;
		}
		self::progress ($num, true);

		$DATA = sprintf (
			"%s\t%s\tKR|Korea, Republic of|%s|%s\n",
			$O[START_LONG],
			$O[END_LONG],
			$O[ISP_CODE],
			$O[ISP_NAME]
		);
		fwrite ($csv, $DATA, strlen ($DATA));

		$O[ISP_NAME] = preg_replace ('/\'/', '\'\'', $O[ISP_NAME]);
		$DATA = sprintf (
			"%s\t%s\t%s\t%s\t%s\n",
			$O[ISP_CODE],
			$O[ISP_NAME],
			self::inet_ntoa ($O[START_LONG]),
			self::inet_ntoa ($O[END_LONG]),
			$O[REGDATE]
		);
		fwrite ($txt, $DATA, strlen ($DATA));

		fclose ($csv);
		fclose ($txt);

		fprintf (self::$stderr, "\n");

		return 0;
	} // function krnic }}}

	function progress ($no, $stop = false) { // {{{
		fprintf (self::$stderr, '%d', $no);

		if ( $stop === true ) {
			fprintf (self::$stderr, "\n");
			return;
		}

		$len = strlen ($no);
		for ( $i=0; $i<$len; $i++ ) {
			fprintf (self::$stderr, '%c', '008');
			echo ' ';
			fprintf (self::$stderr, '%c', '008');
		}
	} // function progress }}}
	/*
	function progress ($no, $stop = false) { // {{{
		$v = $no % 4;
		switch ($v) {
			case '3' :
				$p = '-';
				break;
			case '2' :
				$p = '\\';
				break;
			case '1' :
				$p = '|';
				break;
			default:
				$p = '/';
		}
		fprintf (self::$stderr, '%s', $p);

		if ( $stop === true ) {
			fprintf (self::$stderr, "\n", $no);
			return;
		}

		fprintf (self::$stderr, '%c', '008');
		echo ' ';
		fprintf (self::$stderr, '%c', '008');

		$p = $no % 1000;
		if ( $p < 1 ) {
			fprintf (self::$stderr, '%d', $no);
			usleep (100);
			$len = strlen ($no);
			for ( $i=0; $i<$len; $i++ ) {
				fprintf (self::$stderr, '%c', '008');
				echo ' ';
				fprintf (self::$stderr, '%c', '008');
			}
		}
	} // function progress }}}
	 */

	function inet_aton ($ipaddr) { // {{{
		return sprintf ('%u', ip2long ($ipaddr));
	} // function inet_aton }}}

	function inet_ntoa ($base) { // {{{
		return long2ip ($base);
	} // function inet_ntoa }}}

	function __destruct () { // {{{
		if ( is_resource (self::$stderr) )
			fclose (self::$stderr);
	} // function __destruct }}}
} // Class KRNIC }}}

/*
 * Main
 */

banner ();

if ( $argc == 1 )
	$f = './krisp-rawdata.csv';
else {
	if ( file_exists ($argv[1]) )
		$f = $argv[1];
	else {
		error_log (' => ERROR: Can\'t find KRNIC raw data file', 0);
		exit (1);
	}
}

$krisp = new KRNIC_KRISP;

if ( ($ret = $krisp->krnic ($f)) !== 0 ) {
	switch ($ret) {
		case 1 :
			fprintf ($krisp->stderr, " => ERROR: %s is not exits or not file\n", $f);
			break;
		case 2 :
			fprintf ($krisp->stderr, " => ERROR: file open failed (%s)\n", $f);
			break;
		case 3 :
			fprintf ($krisp->stderr, " => ERROR: You don't have write permission on cwd\n");
			break;
	}
}

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
