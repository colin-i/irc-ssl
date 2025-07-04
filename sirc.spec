
Name: sirc
Version: 1.19
Release: 0
License: GPLv3
Summary: IRC with SSL
Url: https://github.com/colin-i/irc-ssl
Source0: %{name}-%{version}.tar.gz
BuildRequires: autoconf automake make bc gcc-c++ openssl-devel gtk3-devel
Requires: openssl-libs gtk3

%description
IRC client with ssl

%prep
%autosetup

%build
autoreconf -i
%configure
make

%install
%make_install

%files
%attr(0755, root, root) "%{_bindir}/%{name}"

%changelog
* Fri Jul 04 2025 Costin Botescu <costin.botescu@gmail.com> 1.19-0
- retain CFLAGS (costin.botescu@gmail.com)
- appimage (costin.botescu@gmail.com)

* Thu Jul 03 2025 Costin Botescu <costin.botescu@gmail.com> 1.18-0
- hidden switch (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- required action field (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- pub with msys and arch (costin.botescu@gmail.com)
- windows at readme (costin.botescu@gmail.com)
- windows build/test (costin.botescu@gmail.com)
- wsastart (costin.botescu@gmail.com)
- closesocket (costin.botescu@gmail.com)
- various fixes at win (costin.botescu@gmail.com)
- win mkdir (costin.botescu@gmail.com)
- msys wflow and getdelim at win (costin.botescu@gmail.com)
- archlinux (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- pkg (costin.botescu@gmail.com)
- readme (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "sync" (costin.b.84@gmail.com)

* Thu Jul 03 2025 Unknown name 1.18-0
- hidden switch (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- required action field (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- pub with msys and arch (costin.botescu@gmail.com)
- windows at readme (costin.botescu@gmail.com)
- windows build/test (costin.botescu@gmail.com)
- wsastart (costin.botescu@gmail.com)
- closesocket (costin.botescu@gmail.com)
- various fixes at win (costin.botescu@gmail.com)
- win mkdir (costin.botescu@gmail.com)
- msys wflow and getdelim at win (costin.botescu@gmail.com)
- archlinux (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- pkg (costin.botescu@gmail.com)
- readme (costin.botescu@gmail.com)
- "up" (costin.botescu@gmail.com)
- "sync" (costin.b.84@gmail.com)

* Mon Jun 16 2025 Costin Botescu <costin.botescu@gmail.com> 1.17-0
- new package built with tito

