#ifndef TIN_MESSAGE_H
#define TIN_MESSAGE_H

#include <string>

class Message
{
public:
    virtual std::string get_string_rep() const = 0;
    virtual int get_message_type() const = 0;
    enum msg_types {HEADER = -1, LOGIN = 0, STATEMENT = 1, EDIT = 2, PUBLIC_KEY = 3, CLIENT_ID = 4};
};


#endif //TIN_MESSAGE_H
