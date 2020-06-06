#include "LoginHandler.h"

int LoginHandler::check_login(const std::string &bytes)
{
    std::cout << "in login check" << std::endl;
    std::string users_info_fn = get_users_info_filename();
    if (!FileIOHandler().file_exists(users_info_fn))
    {
        FileIOHandler().create_file(users_info_fn);
        std::cout << "file created" << std::endl;
    }
    auto login_info = parse_login_info_from_string(bytes);
    auto res = is_login_info_correct(login_info);
    if (res == NON_EXISTENT)
    {
        add_new_client(login_info);
        res = CORRECT;
    }
    FileIOHandler file_handler;
    file_handler.write_from_buffer(users_info_fn, users_info_buff);
    close_buff();
    return res;
}

void LoginHandler::close_buff()
{
    users_info_buff.clear();
}

void LoginHandler::add_new_client(const LoginInfo &login_inf)
{
    std::string user_data = login_inf.login + ' ' + login_inf.passwd + '\n';
    users_info_buff.append(user_data);
}

LoginHandler::LoginInfo LoginHandler::parse_login_info_from_string(const std::string &data)
{
    std::cout << "data: " << data << std::endl;
    int split_pos = data.find('\n');
    std::cout << "pos: " << split_pos << std::endl;
    std::string login = data.substr(0, split_pos);
    std::string passwd = data.substr(split_pos + 1, data.size());
    return LoginInfo(login, passwd);
}

int LoginHandler::is_login_info_correct(const LoginInfo &login_inf)
{
    std::cout << "in login info check" << std::endl;
    if (!login_inf.is_lexically_correct())
        return false;
    std::string login = login_inf.login;
    auto login_pos = user_exists(login);
    if (login_pos != -1)
    {
        // pseudocode below!!!
        // version with encryption below
        // auto salt = get_salt(login);
        // auto hash = get_hash(login);
        // auto to_comp = calculate_hash(passwd + salt);
        // return compare_hashes(hash, to_comp);
        return cmp_login_info(login_inf, login_pos);
    }
    return NON_EXISTENT;
}

bool LoginHandler::LoginInfo::is_lexically_correct() const
{
    return (login.find_first_of("\n\t ") == std::string::npos && passwd.find_first_of("\n\t ") == std::string::npos);
//    return (login.find(' ') == std::string::npos && login.find('\n') == std::string::npos && login.find('\t') == std::string::npos
//         && passwd.find(' ') == std::string::npos && passwd.find('\n') == std::string::npos && passwd.find('\t') == std::string::npos)
}

int LoginHandler::user_exists(std::string const &login)
{
    // if users info file does not exist
    std::cout << "in user_exists" << std::endl;
    if (!users_info.get_size())
        return false;
        //return std::string::npos;
    // else
    //FileIOHandler file_handler;
    if (is_buff_empty())
    {
        FileIOHandler file_handler;
        users_info_buff = file_handler.read_to_buffer(get_users_info_filename());
        std::cout << "after reading to buffer" << std::endl;
    }
    return (find_user(login));
}

std::string LoginHandler::get_users_info_filename()
{
    int users_info_pos = 0;
    return users_info.get_filename_at_pos(users_info_pos);
}

int LoginHandler::find_user(const std::string &login)
{
    if (is_buff_empty() && !FileIOHandler().is_empty(get_users_info_filename()))
        throw std::runtime_error("nothing to read");
    // in order to find login, blank space must be appended at the end, it's a delimiter between login and passwd and also prevents from returning logins where parameter is a substring
    std::string to_find = login + ' ';
    auto pos = users_info_buff.find(to_find);
    if (pos == std::string::npos)
        pos = -1;
    return pos;
}

bool LoginHandler::is_buff_empty()
{
    return users_info_buff.empty();
}

bool LoginHandler::cmp_login_info(const LoginInfo &login_inf, int login_pos)
{
    int login_len = login_inf.login.size();
    std::string login_to_cmp = users_info_buff.substr(login_pos, login_len);
    int passwd_pos = login_pos + login_len + 1;
    int end_pos = users_info_buff.find('\n', login_pos);
    int passwd_len = end_pos - passwd_pos;
    std::string passwd_to_cmp = users_info_buff.substr(passwd_pos, passwd_len);
    return login_inf.login == login_to_cmp && login_inf.passwd == passwd_to_cmp;
}
