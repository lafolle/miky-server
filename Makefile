
CC=g++
LIBS=-lbluetooth  -lm -lX11 -lXrandr -lxdo -lnotify -lglib-2.0
CPPFLAGS=-fpermissive -std=c++0x
LDFLAGS=
LIBDIR=
INCLUDEPATH=-Iinclude -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/gdk-pixbuf-2.0
RM=rm -f
SOURCE=rfcomm-server.cpp
EXEC=rfcomm-server
OBJECTS=registersdp.o util.o

all: registersdp.o util.o
	$(CC) $(SOURCE) $(OBJECTS) -o $(EXEC) $(LIBS) $(CPPFLAGS) $(INCLUDEPATH)

registersdp.o: registersdp.h registersdp.cpp
	$(CC) $(CPPFLAGS) -c -lbluetooth -lm registersdp.cpp

util.o: util.h util.cpp
	$(CC) -c -lX11 -lXrandr $(CPPFLAGS) util.cpp

clean:
	$(RM) *.o *~ rfcomm-server