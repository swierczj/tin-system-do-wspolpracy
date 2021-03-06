#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include "serv_func.h"

void print_host(sockaddr_in &addres)
{
    char host[NI_MAXHOST]; // client's remote name
    char service[NI_MAXSERV]; // port the client is connected on
    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);
    int address_size = sizeof(addres);

    if (getnameinfo((sockaddr*)&addres, address_size, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) // successful
        std::cout << host << " connected on port " << service << std::endl;
    else
    {
        inet_ntop(AF_INET, &addres.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on port " << ntohs(addres.sin_port) << " getnameinfo unsuccessful" << std::endl;
    }
}

int get_byte_width(int num)
{
        if (num < 0)
            return -1;
        int res = 0;
        //std::cout << "in get_b_width, num: " << num << std::endl;
        do
        {
            num /= 10;
            res += 1;
        }
        while (num > 0);
        //std::cout << "result from width: " << res << std::endl;
        return res;
}
