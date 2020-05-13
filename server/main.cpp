//#include <iostream>
//#include <sys/types.h>
//#include <unistd.h>
//#include <sys/socket.h>
//#include <netdb.h>
//#include <arpa/inet.h>
//#include <string>
//#include <cstring>
//#include <sys/ioctl.h>
//#include <sstream>
//#include <netinet/tcp.h>
//#include "serv_func.h"
#include "Server.h"

//windows
/*
#include <winsock.h>
#include <windows.h>
#include <ws2tcpip.h>
*/

int main()
{
    Server server;
    server.run();
    return 0;
}
