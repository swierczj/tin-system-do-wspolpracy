#include "TextFileHandler.h"

void TextFileHandler::add_filename(const std::string &fn)
{
    filenames.push_back(fn);
}

void TextFileHandler::delete_filename(const std::string &fn)
{
    filenames.erase(std::remove(filenames.begin(), filenames.end(), fn), filenames.end());
}

std::string TextFileHandler::get_filenames_in_str(char delimiter)
{
    std::string res;
    for (auto const &fn : filenames)
    {
        std::string tmp = parse_name(fn) + delimiter;
        res += tmp;
    }
    return res;
}

std::string TextFileHandler::parse_name(const std::string &fn)
{
    auto slash_pos = fn.rfind('\\');
    if (slash_pos == std::string::npos)
        return fn;
    return fn.substr(slash_pos + 1, fn.size());
}

int TextFileHandler::get_size()
{
    return filenames.size();
}

std::string TextFileHandler::get_filename_at_pos(int index)
{
    return filenames[index];
}

std::string TextFileHandler::get_recent_fn()
{
    return filenames.back();
}