
CC=gcc
CFLAG=-O2 -g -Wall -Iinclude
LIBS= -Llib -lhz

.PHONY: all hzlib clean install-home install

all: autob5 autogb hzlib  xchat-plugins

autob5: autogb
	rm -f autob5;ln -s autogb autob5
autogb:autogb.c  hzlib
	$(CC) $(CFLAG) $(LIBS) autogb.c -lhz -o autogb
autogb-static:autogb.c  hzlib
	$(CC) $(CFLAG) autogb.c lib/libhz.a -o autogb
hzlib:
	cd hzconvert;make
xchat-plugins: 
	cd contrib/xchat-plugins;make
clean:
	rm -f autob5 autogb *.o *~
	rm -f include/*~
	cd hzconvert;make clean
	cd contrib/xchat-plugins;make clean
install-home:
	[ ! -f $(HOME)/bin ] || mkdir $(HOME)/bin
	rm -f $(HOME)/bin/autob5
	rm -f $(HOME)/bin/autogb
	cp -f autogb $(HOME)/bin
	ln -s $(HOME)/bin/autogb $(HOME)/bin/autob5
	chmod 755 $(HOME)/bin/autogb 
install:
	install -m 755 autogb $(DESTDIR)/usr/bin
	install -m 755 lib/libhz.so.0.0 $(DESTDIR)/usr/lib
	cd $(DESTDIR)/usr/lib;ln -s  libhz.so.0.0 libhz.so.0
	cd $(DESTDIR)/usr/lib;ln -s  libhz.so.0 libhz.so
	install -m 755 lib/libhz.a $(DESTDIR)/usr/lib
	install -m 755 contrib/xchat-plugins/xchat-autogb.so $(DESTDIR)/usr/share/zh-autoconvert
	install -m 755 contrib/xchat-plugins/xchat-autob5.so $(DESTDIR)/usr/share/zh-autoconvert
	install -m 644 include/*.h  $(DESTDIR)/usr/include
	ln -s autogb $(DESTDIR)/usr/bin/autob5
