User define database tool
===

이 곳의 script는 사용자 database 파일 생성에 사용이 되는 script 입니다.
현재 current version인 3.x 에서는 GeoIP 관련 script만 사용이 가능 합니다.

여기의 script들은 ***libkrisp*** 설치시에 설치가 되지 않습니다. 그러므로
다음과 같이 스크립트 초기화 과정을 준비 하십시오. 이곳의 설명은 다음의 
초기화 과정을 거쳤다고 가정을 하고 기술 합니다.

```bash
[root@host ~]$ # /opt 에 libkrisp source를 다운로드
[root@host ~]$ cd /opt
[root@host ~]$ git clone 'https://github.com/Joungkyun/libkrisp.git'
[root@host ~]$ # /usr/local/bin/krisp 로 /opt/libkrisp/contrib/database를 soft link
[root@host ~]$ ln -sf /opt/libkrisp/contrib/database /usr/local/bin/krisp
[root@host ~]$ # librisp/contrib/database 에 libkrisp/db/table 을 scheme 라는 이름으로 soft link
[root@host ~]$ cd libkrisp/contrib/database
[root@host ~]$ ln -sf ../../db/table ./scheme
```

## Requirements

이 곳의 스크립트 실행을 위해서는 다음의 pear package가 필요합니다.

```bash
[root@host ~]$ pear channel-discover pear.oops.org
[root@host ~]$ pear install oops/oGetopt
[root@host ~]$ pear install oops/ePrint
[root@host ~]$ pear install Net_GeoIP
```

모든 데이터의 산출물은 utf-8로 저장이 됩니다!

## GeoIP 상용 database merge

### GeoIP Country 상용 database merge

```bash
[root@host work]$ wget -U mozilla 'http://mirror.oops.org/pub/oops/libkrisp/data/v2/krisp.csv.gz'
[root@host work]$ gzip -d krisp.csv.gz
[root@host work]$ # 아래 명령은 krisp-geoip-merge.csv-YYYYMMDD 형식으로 저장 됨.
[root@host work]$ php /usr/local/bin/krisp/krisp-geoip.php -k krisp.csv -g ./GeoIPCountryWhois.csv
[root@host work]$ # 아래 명령은 krisp-geoip-merge.csv-YYYYMMDD-vaccum 형식으로 저장 됨.
[root@host work]$ php /usr/local/bin/krisp/vaccum.php krisp-geoip-merge.csv-YYYYMMDD
 * Vaccum the krisp csv data
 *
 * Vaccuming .. 243051 Completes (13.90 secs)
[root@host ~]$ mv krisp-geoip-merge.csv-YYYYMMDD-vaccum krisp.csv
[root@host ~]$ sqlite3 krisp.dat < /usr/local/bin/krisp/scheme
```

### GeoIP ISP 상용 database merge

먼저 GeoIP Country와 GeoIP ISP data를 merge 합니다. 작업 디렉토리에 GeoIP Country와 GeoIP ISP의 csv 파일을 위치 합니다.

```bash
[root@host work]$ php /usr/local/bin/krisp/geoip-geoisp.php -g ./GeoIPCountryWhois.csv -i ./GeoIPISP.csv
[root@host work]$ php /usr/local/bin/krisp/vaccum.php geoip-geoisp-merge.csv-YYYYMMDD
[root@host work]$ mv geoip-geoisp-merge.csv-YYYYMMDD-vaccum geoip-geoisp-merge.csv-YYYYMMDD
```

다음 krisp.csv (krisp-geoip.csv가 아닙니다.)과 geoip-geoisp-merge.csv-YYYYMMDD 를 merge 합니다.

```bash
[root@host work]$ wget -U mozilla 'http://mirror.oops.org/pub/oops/libkrisp/data/v2/krisp.csv.gz'
[root@host work]$ gzip -d krisp.csv.gz
[root@host work]$ php /usr/local/bin/krisp/krisp-geoisp.php -k krisp.csv -g geoip-geoisp-merge.csv-YYYYMMDD
[root@host work]$ php /usr/local/bin/krisp/vaccum.php krisp-geoisp-merge.csv-YYYYMMDD
[root@host work]$ mv krisp-geoisp-merge.csv-YYYYMMDD-vaccum krisp.csv
[root@host work]$ sqlite3 krisp.dat < /usr/local/bin/krisp/scheme
[root@host work]$ mv krisp.dat /usr/share/krisp/krisp.dat
```

## 각 스크립트 설명

> ***geoip-geoisp.php***:

geoip country data(GeoIPCountryWhois.csv)와 geoip isp data(GeoIPISP.csv)를
merge 하여 krisp database format의 cvs를 생성 합니다.

> ***krisp-geoip.php***:

geoip(GeoIPCountryWhois.csv)와 krisp(krisp.csv)를 merge하여 krisp database
format의 csv를 생성 합니다. libkrisp source tree의 data/krisp.csv 파일이 이
스크립트의 산출물 입니다.

krisp data의 업데이트는 http://mirror.oops.org/pub/oops/libkrisp/data 에 있는
***krisp-geoip.csv***를 이용하면 되며, 만약 GeoIP Country 상용 데이터 베이스를
구매했을 경우, 이 script를 이용하여 database를 갱신 시킬 수 있습니다.

> ***krisp-geoisp.php***:

krisp database(krisp.csv)와 geoip-geoisp.php의 산출물로 생성된 csv를 merge
하여 krisp data format의 cvs를 생성 합니다.

> ***vaccum.php***:

위의 script를 이용하여 얻은 data에서 연속된 block을 compress해 주는 script

> ***geo-krisp.php***:

Maxmind의 GeoIP cvs format으로 제공하는 GeoIPCountryWhois.csv을 읽어서 krisp
database 형식의 csv, sql file을 생성

> ***geoisp-krisp.php***:

geo-krisp.php를 geoisp-krisp.php로 softlink를 하면 GeoIPISP의 csv format 파
일 GeoIPISP.csv을 읽어서 krisp database format 파일을 생성

> ***geocity.php***:

GeoIPCity의 cvs format을 읽어서 krisp database format 파일을 생성. libkrisp 2.x
에서 사용하던 포맷입니다. 3.x에서는 사용하지 못합니다.

> ***hostip.php***:

Hostip.info의 csv format을 읽어서 krisp database format 파일을 생성. libkrisp 2.x
에서 제공하던 hostip 기능을 위한 스크립트 입니다. 3.x 에서는 사용하지 못합니다.

