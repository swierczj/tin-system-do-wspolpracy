#ifndef TIN_SERVER_H
#define TIN_SERVER_H

//#include <cstdint>
//#include <vector>

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <sys/ioctl.h>
#include <sstream>
#include <netinet/tcp.h>
#include "serv_func.h"
#include "protocol.pb.h"

class Server
{
private:
    uint16_t port_number;
    enum server_consts { default_port = 54000 };
    int max_sd;
    int listening;
    fd_set master;
    
    int set_listening();
    int set_socket(int sock_domain = AF_INET, int sock_type = SOCK_STREAM, int proto = 0);
    int set_socket_opt(int sockfd, int opt_level, int opt_name, bool opt_on);
    int make_nonblocking(int sockfd);
    int bind_address(int sockfd);
    int set_to_listen(int sockfd, int queue_len = SOMAXCONN);
    fd_set initialize_fd_set(int sockfd);
    int select_fds(int nfds, fd_set* read_set = nullptr, fd_set* write_set = nullptr, fd_set* exc_set = nullptr, int timeout_sec = -1);
    //bool is_in_set(int sockfd, fd_set* sock_set);
    bool handle_new_connection(int accept_sd);
    void handle_existing_connection(int sockfd);
    bool forward_message(int sockfd, char* bytes);
    void remove_socket(int sockfd, int max_desc, fd_set* set);
    void cleanup(int max_desc, fd_set* set);

    void client_login(int sockfd);
    std::string make_header(int msg_type, int msg_len);
public:
    Server(int port = default_port) : port_number(htons(port)), max_sd(-1), listening(-1) {}
    void run();
};

#endif //TIN_SERVER_H
