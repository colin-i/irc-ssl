# IRC with SSL

## Install
On Ubuntu, but other distros may have the same tree and dependencies.\
Architecture: amd64 arm64
```sh
sudo add-apt-repository ppa:colin-i/ppa
sudo apt-get update
sudo apt-get install sirc
```

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
| Name      | Address                                    |
|-----------|--------------------------------------------|
| Bitcoin   | 1DcXWYXpmopfgg3oZYWVBTLbDTmQ6nWG7s         |
| Ethereum  | 0xd8ea69f877b93fa663652bc2d944e71a338cd5f9 |
| Dogecoin  | DP28QjzNcWCF4XqdUoDcZ7DeWKhjTmZqY9         |
| Decred    | DsSdAMyVkKbX18fXK5pYJbNgXhfisc4onT9        |
| Digibyte  | DPK6t296EMSHNMzuoMyP2zbxRjtisaaCRu         |
| Ravencoin | RECqJbqzqNiGQeodcRSBqkAZjh2fbroUHL         |