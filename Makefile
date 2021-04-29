PROGS = myapp
export PROGS
PACKNAME = 1-xkeprt03-xfabom01
CXX = g++
CXXFLAGS = -std=c++17 -pedantic -Wall -g

all: doxygen
	$(MAKE) -C src build	

doxygen:
	doxygen

pack: clean
	zip -r $(PACKNAME).zip * 
	
clean:
	$(MAKE) -C src clean
	rm -fr doc/*
	