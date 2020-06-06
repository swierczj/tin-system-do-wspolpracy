#ifndef TIN_MESSAGE_H
#define TIN_MESSAGE_H

#include <string>

class Message
{
public:
    virtual std::string get_string_rep() const = 0;
    virtual int get_message_type() const = 0;
};


#endif //TIN_MESSAGE_H
