CXX=g++
CXXFLAGS=-c

all: server

server: main.o serv_func.o
	$(CXX) main.o serv_func.o -o server

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp

serv_func.o: serv_func.h serv_func.cpp
	$(CXX) $(CXXFLAGS) serv_func.h serv_func.cpp
	
clean:
	rm -rf *o server


