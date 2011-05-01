#!/usr/bin/php
<?php
#
# $Id$
#
# merge GEOIP database and GeoISP database
#

require_once ('oGetopt.php');

function banner () { // {{{
	echo <<<EOF
 * Merge GEOIP data and GeoIPISP CSV format
 *

EOF;
} // }}}

function usage ($prog) { // {{{
	echo <<<EOF
Usage: {$prog} [opt]
Options:
            -g|--geoip=GEOIP_CSV    path of GeoIP csv format file
                                    [Default: ./GeoIPCountryWhois.csv]
            -h|--help               print this messages
            -i|--geoisp=GEOISP_CSV  path of GeoIPISP csv format file
                                    [Default: ./GeoIPISP.csv]
            -v|--verbose            verbose mode

EOF;

	exit (1);
} // }}}

$errMark = ePrint::asPrintf ('white', _('ERROR'));

define ('END_LONG', 0);
define ('COUNTRY_CODE', 1);
define ('COUNTRY_NAME', 2);
define ('ISP_NAME', 1);

Class GEOIP_GEOISP_MERGE // {{{
{
	// {{{ prpperties
	static public $today;
	static public $eMark;
	static public $save;
	static public $verbose;
	// }}}

	function __construct () { // {{{
		$this->today   = date ('Ymd');
		$this->eMark   = $_GLOBALS['errMakr'];
		$this->save    = 'geoip-geoisp-merge.csv-' . date ('Ymd');
		$this->verbose = 0;
		self::$today   = &$this->today;
		self::$eMark   = &$this->eMark;
		self::$save    = &$this->save;
		self::$verbose = &$this->verbose;
	} // }}}

	function merge ($opt) { // {{{
		$geoip = self::geoip_data ($opt->geoip);
		$geoisp = self::geoisp_data ($opt->geoisp);

		self::doMerge ($geoip, $geoisp);
	} // }}}

	function doMerge ($geoip, $geoisp) { // {{{
		fprintf (ePrint::$stderr, ' * Merge Database .. ');

		$fp = fopen (self::$save, 'wb');
		if ( ! is_resource ($fp) ) {
			ePrint::ePrintf ("\n%s: save file open failed", self::$eMark);
			exit (1);
		}

		$t1 = microtime ();

		$lno = 1;
		$O = '';

		$fetchisp = 1;
		$fetchcountry = 1;
		$cases = (object) array (
			'c0' => 0,
			'c1' => 0,
			'c2' => 0,
			'c3' => 0,
			'c4' => 0,
			'c5' => 0,
			'c6' => 0,
			'c7' => 0,
			'c8' => 0,
			'c9' => 0,
			'c10' => 0,
			'c11' => 0,
		);

		while ( true ) {
			self::progress ($lno++);

			if ( $fetchcountry > 0 ) {
				if ( self::$verbose )
					echo "* Get country array info\n";

				if ( ($carray = each ($geoip)) == false )
					break;
				list ($ckey, $cvalue) = $carray;
				$c->start = $ckey;
				$c->end   = $cvalue[END_LONG];
				$c->code  = $cvalue[COUNTRY_CODE];
				$c->name  = $cvalue[COUNTRY_NAME];
				$fetchcountry = 0;
			}

			if ( $fetchisp > 0 ) {
				if ( self::$verbose )
					echo "* Get ISP array info\n";

				if ( ($iarray = each ($geoisp)) !== false ) {
					list ($ikey, $ivalue) = $iarray;
					$i->start = $ikey;
					$i->end   = $ivalue[END_LONG];
					$i->isp   = $ivalue[ISP_NAME];
					$fetchisp = 0;
				} else {
					// whether need?
					if ( $i )
						unset ($i);
					$fetchisp = -1;
				}
			}

			if ( $fetchisp == -1 ) {
				if ( self::$verbose ) {
					echo "Case 0: \n";
					echo '$c => ';
					print_r ($c);
				}

				$cases->c0++;
				self::mergeWrite ($fp, $c->start, $c->end, $c->code, $c->name);
				$fetchcountry = 1;
				$fetchisp = 1;
				unset ($c);
				continue;
			}

			// Case 1
			// C ------
			// I ------
			// c->start == i->start && c->end == i->end
			if ( $c->start == $i->start && $c->end == $i->end ) { // {{{
				$cases->c1++;

				if ( self::$verbose ) {
					echo "Case 1: \n";
					echo '$c => ';
					print_r ($c);
					echo '$i => ';
					print_r ($i);
				}

				self::mergeWrite ($fp, $c->start, $c->end, $c->code, $c->name, $i->isp);
				$fetchcountry = 1;
				$fetchisp = 1;
				unset ($c);
				unset ($i);
				continue;
			} // }}}

			// Case 2
			// C ------
			// I       ------
			// c->end < i->start
			else if ( $c->end < $i->start ) { // {{{
				$cases->c2++;

				if ( self::$verbose ) {
					echo "Case 2: \n";
					echo '$c => ';
					print_r ($c);
					echo '$i => ';
					print_r ($i);
				}

				self::mergeWrite ($fp, $c->start, $c->end, $c->code, $c->name, '');
				$fetchcountry = 1;
				unset ($c);
				continue;
			} // }}}

			// Case 3
			// C       ------
			// I ------
			// c->start > i->end
			else if ( $c->start > $i->end ) { // {{{
				$cases->c3++;

				if ( self::$verbose ) {
					echo "Case 3: \n";
					echo '$c => ';
					print_r ($c);
					echo '$i => ';
					print_r ($i);
				}

				/* skip no c->code range
				self::mergeWrite ($fp, $i->start, $i->end, '', '', $i->isp);
				 */
				$fetchisp = 1;
				unset ($i);
				continue;
			} // }}}

			// Case 4
			// C ------
			// I   ------
			// c->start < i->start && c->end >= i->start && c->end < i->end
			else if ( $c->start < $i->start && $c->end > $i->start && $c->end < $i->end ) { // {{{
				$cases->c4++;

				if ( self::$verbose ) {
					echo "Case 4: \n";
					echo '$c => ';
					print_r ($c);
					echo '$i => ';
					print_r ($i);
				}

				self::mergeWrite ($fp, $c->start, $i->start - 1, $c->code, $c->name);
				self::mergeWrite ($fp, $i->start, $c->end, $c->code, $c->name, $i->isp);
				$i->start = $c->end + 1;

				if ( self::$verbose )
					print_r ($i);

				$fetchcountry++;
				unset ($c);
				continue;
			} // }}}

			// Case 5
			// C   ------
			// I ------
			// c->start > i->start && c->start <= i->end && c->end > i->end
			else if ( $c->start > $i->start && $c->start <= $i->end && $c->end > $i->end ) { // {{{
				$cases->c5++;

				if ( self::$verbose ) {
					echo "Case 5: \n";
					echo '$c => ';
					print_r ($c);
					echo '$i => ';
					print_r ($i);
				}

				/* skip no c->code range
				self::mergeWrite ($fp, $i->start, $c->start - 1, '', '', $i->isp);
				 */
				self::mergeWrite ($fp, $c->start, $i->end, $c->code, $c->name, $i->isp);
				$c->start = $i->end + 1;

				if ( self::$verbose )
					print_r ($c);

				$fetchisp = 1;
				unset ($i);
				continue;
			} // }}}

			// Case 6
			// C ------
			// I ----
			// c->start == i->start && c->end > i->end
			else if ( $c->start == $i->start && $c->end > $i->end ) { // {{{
				$cases->c6++;

				if ( self::$verbose ) {
					echo "Case 6: \n";
					echo '$c => ';
					print_r ($c);
					echo '$i => ';
					print_r ($i);
				}

				self::mergeWrite ($fp, $i->start, $i->end, $c->code, $c->name, $i->isp);
				$c->start = $i->end + 1;

				if ( self::$verbose )
					print_r ($c);

				$fetchisp = 1;
				unset ($i);
				continue;
			} // }}}

			// Case 7
			// C   ----
			// I ------
			// c->start > i->start && c->end == i->end
			if ( $c->start > $i->start && $c->end == $i->end ) { // {{{
				$cases->c7++;

				if ( self::$verbose ) {
					echo "Case 7: \n";
					echo '$c => ';
					print_r ($c);
					echo '$i => ';
					print_r ($i);
				}

				/* skip no c->code range
				self::mergeWrite ($fp, $i->start, $c->start - 1, '', '', $i->isp);
				 */
				self::mergeWrite ($fp, $c->start, $c->end, $c->code, $c->name, $i->isp);
				$fetchcountry = 1;
				$fetchisp = 1;
				unset ($c);
				unset ($i);
				continue;
			} // }}}

			// Case 8
			// C ------
			// I   --
			// c->start < i->start && c->end > i->end
			else if ( $c->start < $i->start && $c->end > $i->end ) { // {{{
				$cases->c8++;

				if ( self::$verbose ) {
					echo "Case 8: \n";
					echo '$c => ';
					print_r ($c);
					echo '$i => ';
					print_r ($i);
				}

				self::mergeWrite ($fp, $c->start, $i->start - 1, $c->code, $c->name);
				self::mergeWrite ($fp, $i->start, $i->end, $c->code, $c->name, $i->isp);
				$c->start = $i->end + 1;

				if ( self::$verbose )
					print_r ($c);

				$fetchisp = 1;
				unset ($i);
				continue;
			} // }}}

			// Case 9
			// C   --
			// I ------
			// c->start > i->start && c->end < i->end
			else if ( $c->start > $i->start && $c->end < $i->end ) { // {{{
				$cases->c9++;

				if ( self::$verbose ) {
					echo "Case 9: \n";
					echo '$c => ';
					print_r ($c);
					echo '$i => ';
					print_r ($i);
				}

				/* skip no c->code range
				self::mergeWrite ($fp, $i->start, $c->start - 1, '', '', $i->isp);
				 */
				self::mergeWrite ($fp, $c->start, $c->end, $c->code, $c->name, $i->isp);
				$i->start = $c->end + 1;

				if ( self::$verbose )
					print_r ($i);

				$fetchcountry = 1;
				unset ($c);
				continue;
			} // }}}

			// Case 10
			// C ------
			// I   ----
			// c->start < i->start && c->end == i->end
			else if ( $c->start < $i->start && $c->end == $i->end ) { // {{{
				$cases->c10++;

				if ( self::$verbose ) {
					echo "Case 10: \n";
					echo '$c => ';
					print_r ($c);
					echo '$i => ';
					print_r ($i);
				}

				self::mergeWrite ($fp, $c->start, $i->start - 1, $c->code, $c->name);
				self::mergeWrite ($fp, $i->start, $i->end, $c->code, $c->name, $i->isp);
				$fetchcountry = 1;
				$fetchisp = 1;
				unset ($c);
				unset ($i);
				continue;
			} // }}}

			// Case 11
			// C ----
			// I ------
			// c->start == i->start && c->end < i->end
			else if ( $c->start == $i->start && $c->end < $i->end ) { // {{{
				$cases->c11++;

				if ( self::$verbose ) {
					echo "Case 11: \n";
					echo '$c => ';
					print_r ($c);
					echo '$i => ';
					print_r ($i);
				}

				self::mergeWrite ($fp, $c->start, $c->end, $c->code, $c->name, $i->isp);
				$i->start = $c->end + 1;

				if ( self::$verbose )
					print_r ($i);

				$fetchcountry = 1;
				unset ($c);
				continue;
			} // }}}

			// Any case
			else { // {{{
				eprint::ePrintf ('Undefined case!');
				echo "Any Case:\n";
				echo '$c => ';
				print_r ($c);
				echo '$i => ';
				print_r ($i);
				exit (1);
			} // }}}
		}

		$t2 = microtime ();
		ePrint::ePrintf (
			'%d Completes (%s)',
			array ($lno, self::executeTime ($t1, $t2))
		);

		print_r ($cases);

		fclose ($fp);
	} // }}}

	function mergeWrite (&$fp, $s, $e, $code, $name, $isp= '') { // {{{
		$DATA = sprintf (
			"%s\t%s\t%s|%s||%s\n",
			$s,
			$e,
			$code,
			$name,
			$isp
		);
		$DATA = iconv ('iso-8859-1', 'utf-8', $DATA);
		fwrite ($fp, $DATA, strlen ($DATA));
	} // }}}

	function geoip_data ($f) { // {{{
		fprintf (ePrint::$stderr, ' * Open GeoIP Country Database .. ');
		$fp = fopen ($f, 'r');
		$lno = 1;

		$t1 = microtime ();
		$O = '';
		while ( ($buffer = fgets ($fp, 1024)) !== false ) {
			self::progress ($lno++);
			$buffer = preg_replace ('/"/', '', trim ($buffer));
			$buf = explode (',', $buffer);

			$rsize = count ($buf);
			if ( $rsize < 6 )
				continue;

			if ( $rsize > 6 )
				for ( $i=6; $i<$rsize; $i++ )
					$buf[5] .= ',' . $buf[$i];

			if ( ! is_array ($O) ) {
				$O = $buf;
				continue;
			}

			// if OLD_END_IP + 1 == CUR_START && OLD_NATION == CUR NATION,
			// merge 2 lines.
			if ( ($O[3] + 1) == $buf[2] && $O[4] == $buf[4] ) {
				$O[3] = $buf[3];
				continue;
			}

			// key -> long start ip
			// value -> long end ip, nation code, nation name
			$key = $O[2];
			$r[$key] = array ($O[3], $O[4], $O[5]);
			$O = $buf;
		}
		$key = $O[2];
		$r[$key] = array ($O[3], $O[4], $O[5]);

		$t2 = microtime ();
		fclose ($fp);

		ePrint::ePrintf (
			'%d Completes (%s)',
			array ($lno, self::executeTime ($t1, $t2))
		);

		fprintf (ePrint::$stderr, ' * Sort Database .. ');
		$t1 = microtime ();
		ksort ($r);
		reset ($r);
		$t2 = microtime ();
		ePrint::ePrintf (
			'Done (%s)',
			self::executeTime ($t1, $t2)
		);

		return $r;
	} // }}}

	function geoisp_data ($f = null) { // {{{
		fprintf (ePrint::$stderr, ' * Open GeoIP ISP Database .. ');
		$fp = fopen ($f, 'r');
		$lno = 1;

		$t1 = microtime ();
		while ( ($buffer = fgets ($fp, 1024)) !== false ) {
			self::progress ($lno++);
			$buffer = preg_replace ('/"/', '', trim ($buffer));
			$buf = explode (',', $buffer);
			$rsize = count ($buf);
			if ( $rsize < 3 )
				continue;

			if ( $rsize > 3 )
				for ( $i=3; $i<$rsize; $i++ )
					$buf[2] .= ',' . $buf[$i];

			if ( ! is_array ($O) ) {
				$O = $buf;
				continue;
			}

			// if OLD_END_IP + 1 == CUR_START && OLD_NATION == CUR NATION,
			// merge 2 lines.
			if ( ($O[1] + 1) == $buf[0] && $O[2] == $buf[2] ) {
				$O[1] = $buf[1];
				continue;
			}

			// key -> long start ip
			// value -> long end ip, nation code, nation name
			$key = $O[0];
			$r[$key] = array ($O[1], $O[2]);
			$O = $buf;
		}
		$key = $O[0];
		$r[$key] = array ($O[1], $O[2]);

		$t2 = microtime ();
		fclose ($fp);

		ePrint::ePrintf (
			'%d Completes (%s)',
			array ($lno, self::executeTime ($t1, $t2))
		);

		fprintf (ePrint::$stderr, ' * Sort Database .. ');
		$t1 = microtime ();
		ksort ($r);
		reset ($r);
		$t2 = microtime ();
		ePrint::ePrintf (
			'Done (%s)',
			self::executeTime ($t1, $t2)
		);

		return $r;
	} // }}}

	function backspace ($no, $handle = null) { // {{{
		if ( ! is_numeric ($no) )
			return '';

		for ( $i=0; $i<$no; $i++ ) {
			if ( $handle === null ) {
				printf ('%c %c', '008', '008');
			} else {
				fprintf ($handle, '%c %c', '008', '008');
			}
		}
	} // }}}

	function progress ($no, $stop = false) { // {{{
		$no = number_format ($no);
		fprintf (ePrint::$stderr, '%s', $no);

		if ( $stop === true ) {
			fprintf (ePrint::$stderr, "\n");
			return;
		}

		self::backspace (strlen ($no), ePrint::$stderr);
	} // }}}

	function executeTime ($o, $n) { // {{{
		$start = explode (' ', $o);
		$end   = explode (' ', $n);

		return sprintf ('%.2f secs', ($end[1] + $end[0]) - ($start[1] + $start[0]));
	} // }}}
} // }}}


/*
 * Main
 */

banner ();

ePrint::regist ();
oGetopt::init ();
oGetopt::$longopt = (object) array (
	'geoip' => 'g',
	'help'  => 'h',
	'geoisp' => 'i',
);

$opt->geoip  = './GeoIPCountryWhois.csv';
$opt->help   = 0;
$opt->geoisp = './GeoIPISP.csv';
$opt->verbose = 0;

while ( ($opv = oGetopt::exec ($argc, $argv, 'g:hi:v')) !== false ) {
	switch ($opv) {
		case 'g' :
			$opt->geoip = oGetopt::$optarg;
			break;
		case 'h' :
			usage ($argv[0]);
			break;
		case 'i' :
			$opt->geoisp = oGetopt::$optarg;
			break;
		case 'v' :
			$opt->verbose++;
			break;
		default :
			usage ($argv[0]);
	}
}

if ( oGetopt::$optcno > 0 ) {
	ePrint::ePrintf ('%s: Can\'t use command line argument', $errMark);
	usage ($argv[0]);
}

foreach ( $opt as $key => $v ) {
	if ( $key == 'help' || $key == 'verbose' )
		continue;

	if ( $v === null )
		continue;

	if ( ! file_exists ($v) ) {
		ePrint::ePrintf (
			'%s: %s csv file is not found (%s)',
			array ($errMark, $key, $v)
		);
		exit (1);
	}
}

$geo = new GEOIP_GEOISP_MERGE;
$geo->verbose = $opt->verbose;
$ret = $geo->merge ($opt);

ePrint::unregist ();
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
