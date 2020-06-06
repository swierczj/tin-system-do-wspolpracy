#ifndef TIN_CLIENTID_H
#define TIN_CLIENTID_H

#include "Message.h"

class ClientID : public Message
{
private:
    int id;
public:
    ClientID(int ID) : id(ID) {}
    std::string get_string_rep() const override;
    int get_message_type() const override;
};


#endif //TIN_CLIENTID_H
