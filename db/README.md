$Id$

KRISP database used two external databases. One is KRNIC isp database
and another is GeoLiteCountry database.

GeoLiteCountry database is created by MaxMind, available from
http://maxmind.com.

* Database building

  1. Get krisp.csv.gz or krisp-geoip.csv.gz at follow url.

     http://mirror.oops.org/pub/oops/libkrisp/data/v2/

     krisp.csv.gz has only Korean ip information and, krisp-geoip.csv.gz
     has world ip information.

  2. uncompress

     gzip -d krisp.csv.gz

  3. building

     The scheme is located current directory by named 'table'
     sqlite3 krisp.dat < ./table

     And, move data file to libkrisp data directory. ($prefix/share/krisp)