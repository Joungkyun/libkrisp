#!/usr/bin/php
<?php
/*
 * Vaccum the data
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

require_once ('ePrint.php');

function banner () { // {{{
	echo <<<EOF
 * Vaccum the krisp csv data
 *

EOF;
} // }}}

function usage ($prog) { // {{{
	echo <<<EOF
Usage: {$prog} vaccumed_file

EOF;
	exit (1);
} // }}}

Class VACCUM {
	static private $eMark;
	static private $save;

	function __construct () { // {{{
		$this->eMark = $_GLOBAL['eMark'];
		$this->save  = '';
		self::$eMark = $this->eMark;
		self::$save  = $this->save;
	} // }}}

	function execute ($f) { // {{{
		fprintf (ePrint::$stderr, ' * Vaccuming .. ');

		self::$save = $f . '-vaccum';

		$fp = fopen ($f, 'rb');
		if ( ! is_resource ($fp) ) {
			fprintf (ePrint::$stderr, "%s: open error (%s)\n", self::$eMark, $argv[1]);
			return 1;
		}

		$sh = fopen (self::$save, 'wb');
		if ( ! is_resource ($sh) ) {
			ePrint::ePrintf ("\n%s: save file open failed", self::$eMark);
			return 1;
		}

		$t1 = microtime ();

		$lno = 1;
		$O = '';

		while ( ($buffer = fgets ($fp, 1024)) !== false ) {
			self::progress ($lno++);
			$buf = explode ("\t", trim ($buffer));

			if ( ! is_array ($O) ) {
				$O = $buf;
				continue;
			}

			if ( ($O[1] + 1) == $buf[0] && $O[2] == $buf[2] ) {
				$O[1] = $buf[1];
				continue;
			}

			$DATA = sprintf ("%s\t%s\t%s\n", $O[0], $O[1], $O[2]);
			fwrite ($sh, $DATA, strlen ($DATA));
			$O = $buf;
		}
		$DATA = sprintf ("%s\t%s\t%s\n", $O[0], $O[1], $O[2]);
		fwrite ($sh, $DATA, strlen ($DATA));

		$t2 = microtime ();

		ePrint::ePrintf (
			'%d Completes (%s)',
			array ($lno, self::executeTime ($t1, $t2))
		);
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
}


/*
 * Main
 */
banner ();
ePrint::regist ();

$eMark = ePrint::asPrintf ('white', _('ERROR'));

if ( $argc != 2 )
	usage ($argv[0]);

if ( ! file_exists ($argv[1]) ) {
	ePrint::ePrintf ('%s: \'%s\' is not exists.', array ($eMark, $argv[1]));
	usage ($argv[0]);
}

$vac = new VACCUM;
$vac->execute ($argv[1]);

ePrint::unregist ();
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
?>
