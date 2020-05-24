#ifndef TIN_RECVBUFFERS_H
#define TIN_RECVBUFFERS_H
#include <vector>
#include <map>

class RecvBuffers
{
private:
    typedef int socket;
    typedef std::vector<char> bytes;
    std::map<socket, bytes> buffers;
public:
    bool add_new(int sockfd);
    int get_size();
    int get_buffer_current_size(int sockfd);
    void clear_buff(int sockfd);
    void delete_buff(int sockfd);
    std::string get_string_from_bytes(int sockfd);
    void cpy_to_recv_buff(char* from, int len, int sockfd);
};


#endif //TIN_RECVBUFFERS_H
