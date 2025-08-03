# IRC with SSL

## Install
On Ubuntu(amd64 arm64)(jammy noble) from PPA.
```sh
sudo add-apt-repository ppa:colin-i/ppa
```
Or the *manual installation step* from this link *https://gist.github.com/colin-i/e324e85e0438ed71219673fbcc661da6* \
Install:
```sh
sudo apt-get install sirc
```
\
On openSUSE, run the following as __root__:\
For openSUSE Tumbleweed(x86_64/i586 aarch64):
```sh
zypper addrepo https://download.opensuse.org/repositories/home:costin/openSUSE_Tumbleweed/home:costin.repo
```
For openSUSE Leap:
```sh
zypper addrepo https://download.opensuse.org/repositories/home:costin/openSUSE_Leap_16.0/home:costin.repo
```
Replace *16.0*(x86_64 aarch64) with *15.6*(x86_64/i586 aarch64) if needed.\
And:
```sh
zypper refresh
zypper install sirc
```
\
On Fedora 42(x86_64 aarch64), run the following as __root__:
```sh
dnf copr enable colin/project
dnf install sirc
```
\
On Arch Linux:
```sh
yay -Sy sirc
```
Or <i>.zst</i>(x86_64) file from [releases](https://github.com/colin-i/irc-ssl/releases).
\
\
On Windows, there is this **mingw-\*-sirc-\*.zst** pkg at [releases](https://github.com/colin-i/irc-ssl/releases) which is using [mingw-w64](https://www.mingw-w64.org).
\
\
On linux distributions(x86_64) with gtk3, <i>.AppImage</i> file from [releases](https://github.com/colin-i/irc-ssl/releases).

## From source
Using libssl-dev and libgtk-3-dev. There is a "headless" build file as well.
```sh
autoreconf -i
./configure
make install
```
For *install*, *sudo make install* if it is the user.\
*\-\-disable\-cpp* at *./configure* to set c rules.

## Donations
The *donations* section is here
*https://gist.github.com/colin-i/e324e85e0438ed71219673fbcc661da6*
