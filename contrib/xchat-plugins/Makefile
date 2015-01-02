LDFLAGS = -g
CFLAGS = -Wall
CC = gcc
LIBS= -L../../lib

all:xchat-autogb.so xchat-autob5.so
xchat-autogb.so:	xchat-autogb.c
	$(CC) $(LDFLAGS) $(CFLAGS) $(LIBS) -I. -DSKIPCONFIG -fPIC -shared -Wl,-soname,xchat-autogb.so -o $@ $< -lhz -lc `gtk-config --cflags`
xchat-autob5.so:	xchat-autob5.c
	$(CC) $(LDFLAGS) $(CFLAGS) $(LIBS) -I. -DSKIPCONFIG -fPIC -shared -Wl,-soname,xchat-autob5.so -o $@ $< -lhz  -lc `gtk-config --cflags`
clean:
	rm -rf *~ xchat-autogb.so xchat-autob5.so
