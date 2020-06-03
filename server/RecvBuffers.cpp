#include "RecvBuffers.h"

bool RecvBuffers::add_new(int sockfd)
{
    return buffers.insert(std::make_pair(sockfd, std::vector<char>())).second;
}

int RecvBuffers::get_size()
{
    return buffers.size();
}

int RecvBuffers::get_buffer_current_size(int sockfd)
{
    if (!buffers.count(sockfd))
        return -1;
    return buffers[sockfd].size();
}

void RecvBuffers::clear_buff(int sockfd)
{
    buffers[sockfd].clear();
}

void RecvBuffers::delete_buff(int sockfd)
{
    buffers.erase(sockfd);
}

std::string RecvBuffers::get_string_from_bytes(int sockfd)
{
    return std::string(buffers[sockfd].begin(), buffers[sockfd].end());
}

void RecvBuffers::cpy_to_recv_buff(char *from, int len, int sockfd)
{
    auto &v = buffers[sockfd];
    v.insert(v.end(), from, from + len);
}