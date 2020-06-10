#ifndef TIN_EDIT_H
#define TIN_EDIT_H

#include "Message.h"

class Edit : public Message
{
private:
    std::string data;
public:
    Edit(std::string edit_str) : data(std::move(edit_str)) {}
    int get_message_type() const override;
    std::string get_string_rep() const override;
};


#endif //TIN_EDIT_H
