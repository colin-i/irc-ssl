# IRC with SSL

## Install
On Ubuntu from PPA.\
Architecture: amd64 arm64
```sh
sudo add-apt-repository ppa:colin-i/ppa
```
Or the *manual installation step* from this link *https://gist.github.com/colin-i/e324e85e0438ed71219673fbcc661da6* \
Install:
```sh
sudo apt-get install sirc
```
\
On openSUSE (x86_64,aarch64,i586), run the following as __root__:\
For openSUSE Tumbleweed:
```sh
zypper addrepo https://download.opensuse.org/repositories/home:costin/openSUSE_Tumbleweed/home:costin.repo
```
For openSUSE Leap:
```sh
zypper addrepo https://download.opensuse.org/repositories/home:costin/openSUSE_Leap_15.6/home:costin.repo
```
And:
```sh
zypper refresh
zypper install sirc
```
\
On Fedora, run the following as __root__:
```sh
dnf copr enable colin/project
dnf install sirc
```
\
On Arch Linux, <i>.zst</i> file from [releases](https://github.com/colin-i/irc-ssl/releases). Or:
```sh
yay -Sy sirc
```
\
On Windows, there is this **mingw-\*-sirc-\*.zst** pkg at [releases](https://github.com/colin-i/irc-ssl/releases) which is using [mingw-w64](https://www.mingw-w64.org).
\
\
On other linux distributions with gtk3, <i>.AppImage</i> file from [releases](https://github.com/colin-i/irc-ssl/releases).

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
