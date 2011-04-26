%define _unpackaged_files_terminate_build 0
%define sqlitepkgname sqlite
#define compat 30

Summary: Korean ISP library
Summary(ko): 한국 ISP 라이브러리
Name: libkrisp%{?compat}
Version: @PACKAGE_VERSION@
Release: 1
Epoch: 1
License: LGPL
Group: System Environment/Libraries
Requires: %{sqlitepkgname} libipcalc
BuildRequires: %{sqlitepkgname}-devel libipcalc-devel
Source0: ftp://mirror.oops.org/pub/oops/${name}/%{name}-%{version}.tar.bz2
URL: http://devel.oops.org
Buildroot: /var/tmp/%{name}-%{version}-root

%description
The libkrisp%{?compat} is library that get Korean IPS information from IP address.

%description -l ko
libkrisp%{?compat} 는 IP 주소로 부터 한국 ISP 정보를 얻기 위한 라이브러리이다.

%package devel
Summary: Header and object files for development using libkrisp%{?compat}
Summary(ko): libkrisp%{?compat} 를 이용하여 개발하기 위한 header 파일과 목적 파일들
Requires: libkrisp%{?compat}
Group: System Environment/Libraries

%description devel
The libkrisp%{?compat}-devel package contains the header and object files necessary
for developing programs which use the libkrisp%{?compat} libraries.

%description devel -l ko
libkrisp%{?compat}-devel 패키지는 libkrisp%{?compat} 라이브러리를 이용하여 개발을 하기 위한 목적
코드와 헤더 파일들을 포함하고 있다.

%prep
rm -rf %{buildroot}
%setup -q

%build
%configure %{?compat:--with-compat=%{compat}}

%{__make} %{?_smp_mflags}

#
# create database
#
#{__make} data
pushd db
[ -f krisp.dat ] && %{__rm} -f krisp.dat
sqlite3 krisp.dat < table
popd

%install
%{__make} DESTDIR=%{buildroot} install

%clean
%{__rm} -rf %{buildroot}

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(0755,root,root)
%{_bindir}/krisplookup%{?compat}
%{_bindir}/krispmkdb%{?compat}
%{_libdir}/libkrisp%{?compat}.so.*
%attr (0644, root, root) %{_datadir}/krisp%{?compat}/*.dat
%{?compat:%attr (0644, root, root) %{_mandir}/ko/man1/*.1*}

%files devel
%defattr(0644,root,root,0755)
%attr(0755,root,root) %{_bindir}/krisp%{?compat}-config
%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/*.la
%{_includedir}/*.h
%{?compat:%{_mandir}/ko/man3/*.3*}

%changelog
* @PACKAGE_DATE@ JoungKyun.Kim <http://oops.org> - @PACKAGE_VERSION@-1
- release @PACKAGE_VERSION@

