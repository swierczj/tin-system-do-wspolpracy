#include "LoginHandler.h"

LoginHandler::LoginInfo LoginHandler::parse_login_info_from_string(const std::string &data)
{
    int split_pos = data.find('\n');
    std::string login = data.substr(0, split_pos);
    std::string passwd = data.substr(split_pos + 1, data.size());
    return LoginInfo(login, passwd);
}

bool LoginHandler::is_login_info_correct(const LoginInfo &login_inf)
{
    if (!login_inf.is_lexically_correct())
        return false;
    if (user_exists(login))
    {
        // pseudocode below!!!
        auto salt = get_salt(login);
        auto hash = get_hash(login);
        auto to_comp = calculate_hash(passwd + salt);
        return compare_hashes(hash, to_comp);
    }
    else
    {
        add_new_user(login);
        return true;
    }
}

bool LoginHandler::LoginInfo::is_lexically_correct() const
{
    return (login.find_first_of("\n\t ") == std::string::npos && passwd.find_first_of("\n\t ") == std::string::npos);
//    return (login.find(' ') == std::string::npos && login.find('\n') == std::string::npos && login.find('\t') == std::string::npos
//         && passwd.find(' ') == std::string::npos && passwd.find('\n') == std::string::npos && passwd.find('\t') == std::string::npos)
}

bool LoginHandler::user_exists(std::string const &login)
{
    if (!users_info.get_size())
        return false;

}