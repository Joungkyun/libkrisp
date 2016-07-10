libkrisp - Korea ISP library
====


***libkrisp***는 [APNIC](https://www.apnic.net/)에서 [KRNIC(KISA)](http://kisa.or.kr)에 할당한 IP와 KRNIC에서 관리 대행자(ISP등) 및 독립 사용자에게 할당한 정보를 관리하는 library 입니다.

이 library를 이용하여 한국에 할당된 IP에 대한 다음의 정보를 얻을 수 있습니다.

```bash
[root@host ~]$ krisplookup kisa.or.kr
kisa.or.kr (1.201.101.120): 주식회사 케이아이엔엑스 (KINXINC)
SUBNET    : 255.255.0.0
NETWORK   : 1.201.0.0
BROADCAST : 1.201.255.255
DB RANGE  : 1.201.0.0 - 1.201.255.255
NATION    : Korea, Republic of (KR)
[root@host ~]$
```

***libkrisp***에서 공식적을 제공을 하는 database는 다음 [KISA](http://kisa.or.kr)에서 공개한 데이터 베이스를 기본 database로 사용을 합니다.

***libkrisp***는 2개의 외부 database를 이용 합니다. 기본 database로는 다음 [KISA](http://kisa.or.kr)에서 공개한 데이터 베이스를 기본 database로 사용을 합니다.

 * [KISA 국내 IPv4주소 목록](http://www.krnic.or.kr/jsp/infoboard/stats/interProCurrent.jsp)
 * [KISA 관리대행자별 IPv4주소 목록](http://www.krnic.or.kr/jsp/business/management/isCurrentIpv4.jsp)
 * [KISA 독립사용자 목록](http://www.krnic.or.kr/jsp/business/management/piCurrent.jsp)
  * 2010년경 까지는 홈페이지에서 독립 사용자 할당 내역을 받을 수 있으나 현재는 정보가 없어, 매달 새로 할당된 내역을 확인 하여 whois database에서 수작업으로 관리 하고 있음.

또 하나는, [KISA](http://kisa.or.kr)에서 제공하는 database와 [MaxMind]([http://maxmind.com)에서 제공하는 GeoIP Lite database를 merge한 database를 제공 합니다.

기본 제공 되는 [KISA](http://kisa.or.kr) database는 한국의 IP 및 ISP 정보만을 가지고 있어 빠르게 탐색할 수 있는 장점이 있지만 외국의 IP 대역에 대해서는 처리를 하지 못하기 때문에, 별도로 geoip lite를 merge 하여 외국의 IP 정보를 관리하도록 합니다. (단 외국 IP의 경우 ISP 정보는 가지고 있지 않습니다.)

***libkrisp***의 database는 다음 URL에서 2006년 부터 현재까지의 database가 매월 1회씩 배포되고 있습니다.

  * http://mirror.oops.org/pub/oops/libkrisp/data/

[database 갱신 문서](db/README.md)를 참조 하십시오.

또한, [MaxMind]([http://maxmind.com)의 GeoIP 또는 GeoIPISP 상용 데이터를 구매를 하여 ***libkrisp*** database에 merge를 할 수 있으며, CSV format 을 잘 맞추면 임의의 데이터를 구성하여 사용할 수도 있습니다.

## Requirements

 * sqlite2 or sqlite 3
 * [libipcalc](http://github.com/Joungkyun/libipcalc)

## API Usage

간단한 사용법은 아래와 같습니다. (자세한 사항은 ```man 3 kr_search``` 를 참고 하십시오.)

```c
#include <krisp.h>

int main (void) {
    KR_API * db;
    KRNET_API isp;
    //char * ip = "168.126.63.1";
    char * ip = "kns.kornet.net";
    int r;
    char * database = NULL;
    char err[1024];

    if ( kr_open (&db, database, err)) == false ) {
        fprintf (stderr, "ERROR Connect: %s\n", err);
        return 1;
    }

    // Check database modification. If libkrisp detects database
    // modification, reopen database. Default is 0 seconds (Don't check).
    // support libkrisp >= 3.1
    db->db_time_stamp_interval = 3600; // 1h
    
    isp.verbose = false;
    // defined SAFECPY_256 in krispapi.h
    SAFECPY_256 (isp.ip, addr);
    if ( kr_search (&isp, db) ) {
        printf ("ERROR: %s\n", isp.err);
        kr_close (&db);
        return 1;
    }

    kr_close (&db);
    
    printf ("%-15s %-15s %-20s", ip, isp.ip, isp.icode);
    // long2ip_r is include libipcalc.
    {
        char ip[16] = { 0, };
        printf ("%-15s ", long2ip_r (isp.start, ip));
        printf ("%-15s %s", long2ip_r (isp.end, ip), isp.iname);
    }
    
    return 0;
}
```

## License

Copyright &copy; 2016 [JoungKyun.Kim](http://oops.org) all rights reserved.

This program is under LGPL v2.1
