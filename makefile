##   makefile for spy process accounting programs 
##       ( LSST software stack checking  )

all: spypracc stopspypracc

spypracc : Objs/spypracc
	echo ' spypracc made'

stopspypracc : Objs/stopspypracc
	echo ' stopspypracc made'

clean:
	rm Objs/*

CXX = g++ 
# CPPFLAGS = -DDarwin 
CPPFLAGS = -DLinux 

## --- spypracc
Objs/spypracc : Objs/spypracc.o Objs/procacct.o Objs/spypasighand.o Objs/timing.o
	$(CXX) -o Objs/spypracc Objs/spypracc.o Objs/procacct.o Objs/spypasighand.o Objs/timing.o

Objs/spypracc.o : spypracc.cc procacct.h spypasighand.h timing.h 
	$(CXX) $(CPPFLAGS) -c -o Objs/spypracc.o spypracc.cc

Objs/procacct.o : procacct.cc procacct.h 
	$(CXX) $(CPPFLAGS) -c -o Objs/procacct.o procacct.cc

Objs/spypasighand.o : spypasighand.cc spypasighand.h 
	$(CXX) $(CPPFLAGS) -c -o Objs/spypasighand.o spypasighand.cc

Objs/timing.o : timing.c timing.h 
	$(CXX) $(CPPFLAGS) -c -o Objs/timing.o timing.c

## --- stopspypracc
Objs/stopspypracc : Objs/stopspypracc.o 
	$(CXX) -o Objs/stopspypracc Objs/stopspypracc.o 

Objs/stopspypracc.o : stopspypracc.cc 
	$(CXX) $(CPPFLAGS) -c -o Objs/stopspypracc.o stopspypracc.cc
