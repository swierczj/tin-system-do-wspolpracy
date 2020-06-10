CXX=g++-9
CXXFLAGS=-c -g -std=gnu++17 -std=c++17

all: server

server: main.o Server.o Networking.o RecvBuffers.o ClientsMonitor.o LoginHandler.o NotepadHandler.o TextFileHandler.o Edit.o ClientID.o Statement.o Header.o Message.o FileIOHandler.o Note.o serv_func.o
	$(CXX) main.o Server.o Networking.o RecvBuffers.o ClientsMonitor.o LoginHandler.o NotepadHandler.o TextFileHandler.o Edit.o ClientID.o Statement.o Header.o Message.o FileIOHandler.o Note.o serv_func.o -o server

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp
	
Server.o: Server.h Server.cpp
	$(CXX) $(CXXFLAGS) Server.h Server.cpp 	

Networking.o: Networking.h Networking.cpp
	$(CXX) $(CXXFLAGS) Networking.h Networking.cpp

RecvBuffers.o: RecvBuffers.h RecvBuffers.cpp
	$(CXX) $(CXXFLAGS) RecvBuffers.h RecvBuffers.cpp

ClientsMonitor.o: ClientsMonitor.h ClientsMonitor.cpp
	$(CXX) $(CXXFLAGS) ClientsMonitor.h ClientsMonitor.cpp

LoginHandler.o: LoginHandler.h LoginHandler.cpp
	$(CXX) $(CXXFLAGS) LoginHandler.h LoginHandler.cpp

NotepadHandler.o: NotepadHandler.h NotepadHandler.cpp
	$(CXX) $(CXXFLAGS) NotepadHandler.h NotepadHandler.cpp

TextFileHandler.o: TextFileHandler.h TextFileHandler.cpp
	$(CXX) $(CXXFLAGS) TextFileHandler.h TextFileHandler.cpp

Edit.o: Edit.h Edit.cpp
	$(CXX) $(CXXFLAGS) Edit.h Edit.cpp

ClientID.o: ClientID.h ClientID.cpp
	$(CXX) $(CXXFLAGS) ClientID.h ClientID.cpp

Statement.o: Statement.h Statement.cpp
	$(CXX) $(CXXFLAGS) Statement.h Statement.cpp

Header.o: Header.h Header.cpp
	$(CXX) $(CXXFLAGS) Header.h Header.cpp

Message.o: Message.h Message.cpp
	$(CXX) $(CXXFLAGS) Message.h Message.cpp

FileIOHandler.o: FileIOHandler.h FileIOHandler.cpp
	$(CXX) $(CXXFLAGS) FileIOHandler.h FileIOHandler.cpp -lstdc++fs

Note.o: Note.cpp
	$(CXX) $(CXXFLAGS) Note.cpp

serv_func.o: serv_func.h serv_func.cpp
	$(CXX) $(CXXFLAGS) serv_func.h serv_func.cpp
	
clean:
	rm -rf *h.gch *o server


