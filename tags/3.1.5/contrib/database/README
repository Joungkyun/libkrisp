$Id$

스크립트 실행을 위해서는 다음의 pear package가 필요하다.

pear channel-discover pear.oops.org
pear install oops/oGetopt
pear install oops/ePrint
pear install Net_GeoIP

데이터의 산출물은 utf-8로 저장이 됨!

* geoip-geoisp.php:

  geoip country data(GeoIPCountryWhois.csv)와 geoip isp data(GeoIPISP.csv)를
  merge 하여 krisp database format의 cvs를 생성

* krisp-geoip.php:

  geoip(GeoIPCountryWhois.csv)와 krisp(krisp.csv)를 merge하여 krisp database
  format의 csv를 생성. libkrisp source tree의 data/krisp.csv파일이 이 스크립
  트의 산출물 임.

  krisp data의 업데이트는 ftp://mirror.oops.org/pub/oops/libkrisp/data에 있는
  krisp.csv를 이용하면 됨.

* krisp-geoisp.php:

  krisp database(krisp.csv)와 geoip-geoisp.php의 산출물로 생성된 csv를 merge
  하여 krisp data format의 cvs를 생성. krisp/geoip/geoisp database를 merge한
  결과임.

* vaccum.php:

  위의 script를 이용하여 얻은 data에서 연속된 block을 compress해 주는 script

* geo-krisp.php:

  Maxmind의 GeoIP cvs format으로 제공하는 GeoIPCountryWhois.csv을 읽어서 krisp
  database 형식의 csv, sql file을 생성

* geoisp-krisp.php:

  geo-krisp.php를 geoisp-krisp.php로 softlink를 하면 GeoIPISP의 csv format 파
  일 GeoIPISP.csv을 읽어서 krisp database format 파일을 생성

* geocity.php

  GeoIPCity의 cvs format을 읽어서 krisp database format 파일을 생성

* hostip.php

  Hostip.info의 csv format을 읽어서 krisp database format 파일을 생성

