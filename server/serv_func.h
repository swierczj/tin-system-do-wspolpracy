#ifndef TIN_SERV_FUNC_H
#define TIN_SERV_FUNC_H

#include <sys/socket.h>
#include <netdb.h>

void print_host(sockaddr_in &);
int get_byte_width(int num);

#endif //TIN_SERV_FUNC_H
