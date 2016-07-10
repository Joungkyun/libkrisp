<?php
/*
 * Validate IP range
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

Class ValidIP {
	function valid ($v) { // {{{
		if ( preg_match ('/[0-9]{1,3}\.([0-9]{1,3}){3}$/', $v) )
			$ip = self::ip2long ($v);
		else if ( strstr ($v, '.') !== false ) {
			$ip = gethostbyname ($v);
			if ( $ip === false )
				return false;
			$ip = self::ip2long ($ip);
		} else
			$ip = $v;

		if ( $ip === false )
			return false;

		return self::valid_longip ($ip);
	} // }}}

	function valid_longip ($v) { // {{{
		//if ( $v < 16777216 || $v > 4294967295 )
		if ( $v < 16777216 || $v >= 4026531840 )
			return false;

		if ( self::is_loopback ($v) === true )
			return false;

		if ( self::is_private_addr ($v) === true )
			return false;

		if ( self::is_multicast ($v) === true )
			return false;

		if ( self::is_eclass ($v) === true )
			return false;

		if ( self::is_reserved ($v) === true )
			return false;

		return true;
	} // }}}

	function is_reserved ($v) { // {{{
		// 169.254.0.0/16 reserved for Link Local [RFC5735]
		if ( $v >= 2851995648 && $v <= 2852061183 )
			return true;

		// 192.0.0.0/24 reserved for IANA IPv4 Special Purpose Address Registry [RFC5736]
		if ( $v >= 3221225472 && $v <= 3221225727 )
			return true;

		// 192.0.2.0/24  reserved for TEST-NET-1 [RFC5737]
		if ( $v >= 3221225984 && $v <= 3221226239 )
			return true;

		// 198.18.0.0/15 reserved for Network Interconnect Device Benchmark Testing [RFC5735]
		if ( $v >= 3323068416 && $v <= 3323199487 )
			return true;

		// 198.51.100.0/24 reserved for TEST-NET-2 [RFC5737]
		if ( $v >= 3325256704 && $v <= 3325256959 )
			return true;

		// 192.88.99.0/24 reserved for 6to4 Relay Anycast [RFC3068]
		if ( $v >= 3227017984 && $v <= 3227018239 )
			return true;

		// 203.0.113.0/24 reserved for TEST-NET-3 [RFC5737]
		if ( $v >= 3405803776 && $v <= 3405804031 )
			return true;

		return false;
	} // }}}

	function is_loopback ($v) { // {{{
		if ( $v >= 2130706432 && $v <= 2147483647 )
			return true;
		return false;
	} // }}}

	function is_private_addr ($v) { // {{{
		// A class 10.0.0.0 - 10.255.255.255
		if ( $v >= 167772160 && $v <= 184549375 )
			return true;

		// B class 172.16.0.0 - 172.31.0.0
		if ( $v >= 2886729728 && $v <= 2887712768 )
			return true;

		// C class 192.168.0.0 - 192.168.255.255
		if ( $v >= 3232235520 && $v <= 3232301055 )
			return true;

		return false;
	} // }}}

	function is_multicast ($v) { // {{{
		// 224.0.0.0 - 239.255.255.0
		if ( $v >= 3758096384 && $v <= 4026531839 )
			return true;

		return false;
	} // }}}

	function is_eclass ($v) { // {{{
		if ( $v >= 4026531840 && $v <= 4294967295 )
			return true;

		return false;
	} // }}}

	function ip2long ($v) { // {{{
		$longip = ip2long ($v);
		if ( $longip === false )
			return '';
		return sprintf ('%u', $longip);
	} // }}}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
?>
