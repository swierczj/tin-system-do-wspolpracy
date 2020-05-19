#ifndef TIN_HEADER_H
#define TIN_HEADER_H

#include <string>
#include <algorithm>
#include <iostream>
#include "Message.h"

class Header : public Message
{
private:
    int msg_type;
    int msg_len;
    enum header_msg_type {LOGIN = 0, STATEMENT = 1, EDIT = 2, PUBLIC_KEY = 3};
    enum consts {header_size = 6, msg_type_len = 2};
public:
    Header(int type, int len) : msg_type(type), msg_len(len) {};
    std::string get_string_rep();
    friend Header parse_from_string(std::string const &str);
    int get_msg_type();
    int get_msg_len();
};

//Header parse_from_string(std::string const &str)
//{
//    if (str.size() != Header::header_size || std::all_of(str.begin(), str.end(), ::isdigit))
//        throw std::underflow_error("wrong header");
//    std::string msg_type_part = str.substr(0, Header::msg_type_len);
//    std::string msg_len_part = str.substr(Header::msg_type_len, Header::header_size);
//    int msg_type = std::stoi(msg_type_part);
//    int msg_len = std::stoi(msg_len_part);
//    return Header(msg_type, msg_len);
//}

#endif //TIN_HEADER_H
