PROGS = myapp
CXX = g++
CXXFLAGS = -std=c++17 -pedantic -Wall -g
APPOFILES = main.o 
all: main 

-include dependencies

build: $(APPOFILES) 
	$(CXX) $(CXXFLAGS) -o ../$(PROGS) $^

main: $(APPOFILES) 
	$(CXX) $(CXXFLAGS) -o $(PROGS) $^

dependencies:
	$(CXX) -MM *cpp >dependencies

clean:
	rm -f $(PROGS) *.o dependencies ../$(PROGS)	