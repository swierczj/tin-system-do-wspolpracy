#ifndef TIN_LOGINHANDLER_H
#define TIN_LOGINHANDLER_H

#include <string>

class LoginHandler
{
private:
    struct LoginInfo
    {
        std::string login;
        std::string passwd;
        LoginInfo(std::string log, std::string pass) : login(std::move(log)), passwd(std::move(pass)) {};
        bool is_lexically_correct() const;
        bool user_exists();
    };
    //bool is_lexically_correct();
public:
    LoginInfo parse_login_info_from_string(std::string const &data);
    bool is_login_info_correct(LoginInfo const &login_inf);
};


#endif //TIN_LOGINHANDLER_H
