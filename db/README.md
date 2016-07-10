libkrisp Database management
===

KRISP database used two external databases. One is ***[KRNIC(KISA)](http://kisa.or.kr) isp database and another is GeoLiteCountry database in [Maxmind](http://maxmind.com), available from http://maxmind.com.

## Database update cycle

update database every month, between 7th and 14th.


## Database download

Get ***krisp.csv.gz*** or ***krisp-geoip.csv.gz*** at follow url:

     http://mirror.oops.org/pub/oops/libkrisp/data/v2/

Each database has features as follows:

 * ***krisp.csv.gz***
   * only Korea allocated information
   * enable ISP inforamtion
   * small size (< 1MB)
 * ***krisp-geoip.csv.gz***
   * world wide IP information
   * only Korean ISP information eanbled
   * big size (>20MB)

## Database building

Firs, download ***krisp*** database

```bash
    [root@host ~]$ # blocking wget user agent, be carefull.
    [root@host ~]$ wget -U mozilla 'http://mirror.oops.org/pub/oops/libkrisp/data/v2/krisp-geoip.csv.gz
```

And, uncompress download database.

```base
     gzip -d krisp-geoip.csv.gz
```

And, build database with libkrisp scheme in libkrisp source

The scheme file is located in current directory by named 'table'

```bash
    sqlite3 krisp-geoip.dat < ./table
```

Last, move data file to libkrisp data directory. ($prefix/share/krisp)
