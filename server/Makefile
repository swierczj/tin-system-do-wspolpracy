CXX=g++
CXXFLAGS=-c -g -std=gnu++11

all: server

server: main.o Server.o Header.o Message.o serv_func.o 
	$(CXX) main.o Server.o Header.o Message.o serv_func.o -o server

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp
	
Server.o: Server.h Server.cpp
	$(CXX) $(CXXFLAGS) Server.h Server.cpp 	

Header.o: Header.h Header.cpp
	$(CXX) $(CXXFLAGS) Header.h Header.cpp

Message.o: Message.h Message.cpp
	$(CXX) $(CXXFLAGS) Message.h Message.cpp

serv_func.o: serv_func.h serv_func.cpp
	$(CXX) $(CXXFLAGS) serv_func.h serv_func.cpp
	
clean:
	rm -rf *h.gch *o server


