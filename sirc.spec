
Name: sirc
Version: 1.17
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
* Mon Jun 16 2025 Costin Botescu <costin.botescu@gmail.com> 1.17-0
- new package built with tito

