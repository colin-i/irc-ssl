SUBDIRS = s

#here will add
.PHONY: test
test:
	cp -r s test
	cd test && make clean && ./headless
#test if is going with: autoreconf -i && dh_auto_configure && dh_auto_build && dh_auto_test

#install-exec-local:
#	if [ ! -e "`cat ./bash_home`/.@PROJ@info" ]; then cp "./.@PROJ@info" "`cat ./bash_home`/" --preserve=ownership; fi

#uninstall-local:
#	if [ -e "`cat ./bash_home`/.@PROJ@info" ]; then rm "`cat ./bash_home`/.@PROJ@info"; fi
