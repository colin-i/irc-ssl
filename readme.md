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
On Fedora, <i>.rpm</i> file from [releases](https://github.com/colin-i/irc-ssl/releases).
```sh
yum install sirc-*.*.rpm
```
\
On linux distributions, <i>.AppImage</i> file from [releases](https://github.com/colin-i/irc-ssl/releases).

## From source
```sh
autoreconf -i
./configure
make install
```
For *install*, *sudo make install* if it is the user.\
*\-\-disable\-cpp* at *./configure* to set c rules.\
GTK3 required.

## Donations
The *donations* section is here
*https://gist.github.com/colin-i/e324e85e0438ed71219673fbcc661da6*
