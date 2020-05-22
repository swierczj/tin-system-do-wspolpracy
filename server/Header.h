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
//    enum header_msg_type {LOGIN = 0, STATEMENT = 1, EDIT = 2, PUBLIC_KEY = 3};
//    enum consts {header_size = 6, msg_type_len = 2};
public:
    Header(int type, int len) : msg_type(type), msg_len(len) {};
    enum header_msg_type {LOGIN = 0, STATEMENT = 1, EDIT = 2, PUBLIC_KEY = 3};
    enum consts {header_size = 6, msg_type_len = 2};
    std::string get_string_rep() override;
    int get_msg_type();
    int get_msg_len();
    friend Header parse_from_string(std::string const &str);
};

#endif //TIN_HEADER_H
