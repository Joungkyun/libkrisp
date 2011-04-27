<?php
##
## $Id: class.krispnet.php,v 1.1.1.1 2006-06-08 19:31:53 oops Exp $
##
## this file name is 'class.krispnet.php'
##
## KR-IPV4 : NETMASK - ISP
##
## krispnet object
##
## [original author]
##  - Chilbong Kim, <san2(at)linuxchannel.net>
##  - http://linuxchannel.net/
##
## [author]
##  - JoungKyun.Kim
##  - http://www.oops.org
##
## [changes]
##  - 2005.11.07 : update
##  - 2005.09.12 : update
##  - 2005.06.13 : update
##  - 2004.04.20 : new build
##  - 2006.06.08 : change isp information with sqlite2
##
## [references]
##  - http://member.nic.or.kr/link/ISPAllocation.jsp
##
## [usage]
##
## [example]
##   $host = gethostbyname ($argv[1]);
##   $ips = new krispnet;
##   $get = $ips->search($host); // return array
##   print_r ($get);
##
class krispnet
{
	var $_db = "/usr/share/krisp/krisp-sqlite.dat";

	## main core
	##
	function &search($ipaddr)
	{
		list($ipa) = explode('.',$ipaddr);
		//list($_NET,$_ISPS) = _krispnet($ipa);

		$_NET = $this->netmask($ipa);

		if(!$_NET) return;

		$ipb = $this->inet_atob($ipaddr);
		$size = sizeof($_NET);

		for($i=0; $i<$size; $i++)
		{
			$mask = $this->inet_atob($_NET[$i]);
			$network = bindec($mask & $ipb);

			if($_ISP = $this->ispmap ("$network")) // require double quota
			{
				$fnet = $this->inet_atob($_ISP['netmask']) & $ipb;
				$fnet = $this->inet_ntoa(bindec($fnet));

				if($fnet == $_ISP['network']) // real network
				{
					return $_ISP; // array
					break;
				}
			}
		}

		return;
	}

	function &inet_ntoa($base)
	{
		return long2ip($base);
	}

	function &inet_atob($ipaddr)
	{
		return sprintf('%032b',ip2long($ipaddr));
	}

	function &netmask($ipa=0, $db = '')
	{
		$dbo = 0;
		if ( ! $db ) {
			$db = sqlite_open ($this->_db, 0644);
			$dbo = 1;
		}

		if ( $ipa ) {
			$sql = "SELECT subnet FROM netmask WHERE net = '$ipa'";
			$r = sqlite_query ($db, $sql);
			$n = sqlite_num_rows ($r);

			$_NET = array();
			for ( $i=0; $i<$n; $i++ ) {
				$_r = sqlite_fetch_single ($r);
				array_push ($_NET, $_r);
			}
		} else {
			$sql = "SELECT net, subnet FROM netmask";
			$r = sqlite_query ($db, $sql);
			$n = sqlite_num_rows ($r);

			$_NET = array();
			for ( $i=0; $i<$n; $i++ ) {
				$_r = sqlite_fetch_array ($r, SQLITE_ASSOC);
				if ( ! is_array ($_NET[$_r['net']]) ) $_NET[$_r['net']] = array ();
				array_push ($_NET[$_r['net']], $_r['subnet']);
			}
		}

		if ( $dbo ) {
			sqlite_close ($db);
		}

		return $_NET;
	}

	function &ispmap($nnetwork=0, $db = '')
	{
		$dbo = 0;
		if ( ! is_resource ($db) ) {
			$db = sqlite_open ($this->_db, 0644);
			$dbo = 1;
		}

		if ( $nnetwork ) {
			$sql = "SELECT * FROM isp WHERE longip = '$nnetwork'";
			$r = sqlite_query ($db, $sql);
			$_r = sqlite_current ($r, SQLITE_ASSOC);
			$_ISP = $_r;
		} else {
			$sql = "SELECT * FROM isp";
			$r = sqlite_query ($db, $sql);
			$n = sqlite_num_rows ($r);

			for ( $i=0; $i<$n; $i++ ) {
				$_r = sqlite_fetch_array ($r, SQLITE_ASSOC);
				$_ISP[$_r['longip']] = array ($_r['network'], $_r['broadcast'], $_r['netmask'], $_r['organization'], $_r['servicename']);
			}
		}

		if ( $dbo ) {
			sqlite_close ($db);
		}

		return $_ISP;
	}
} // end of class

/*
$host = gethostbyname ($argv[1]);
$ips = new krispnet;
$get = $ips->search($host); // return array
print_r ($get);
*/
?>
