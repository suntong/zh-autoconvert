CC=gcc -I../include
CFLAGS=-O2 -Wall -g
SHARED_LIB=libhz.so
SHARED_LIB_MAJOR=$(SHARED_LIB).0
SHARED_LIB_MINOR=$(SHARED_LIB_MAJOR).0
OBJS=b2g.o b2u.o g2u.o hz2gb.o b2g_tables.o b2u_tables.o g2u_tables.o io.o unicode.o judge.o judge3.o gbpercent.o b5percent.o search.o 

%.o: %.c
	$(CC) -fPIC $(CFLAGS) -c $<

all: libhzconvert

libhzconvert: $(OBJS)
	ar rs  ../lib/libhz.a $(OBJS)
	gcc -fPIC -shared -Wl,-soname,$(SHARED_LIB_MAJOR) -o ../lib/$(SHARED_LIB_MINOR)  $(OBJS) -ldl
	rm -f ../lib/$(SHARED_LIB_MAJOR)
	rm -f ../lib/$(SHARED_LIB)
	cd ../lib;ln -s $(SHARED_LIB_MINOR) $(SHARED_LIB_MAJOR)
	cd ../lib;ln -s $(SHARED_LIB_MAJOR) $(SHARED_LIB)

clean:
	rm -f *.o *~ ../lib/lib*
