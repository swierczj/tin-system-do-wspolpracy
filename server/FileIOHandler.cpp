#include "FileIOHandler.h"

void FileIOHandler::create_file(std::string const &fn)
{
    if (fn.empty())
        throw std::runtime_error("creating file error");
    std::ofstream file(fn);
    file.close();
}

bool FileIOHandler::file_exists(const std::string &fn)
{
    std::fstream f;
    f.open(fn);
    auto res = !f.fail();
    return res;
}

std::string FileIOHandler::read_to_buffer(const std::string &fn)
{
    std::string buffer;
    std::ifstream from(fn);
    from.seekg(0, std::ios::end);
    auto f_len = from.tellg();
    buffer.resize(f_len);
    from.seekg(0);
    from.read(buffer.data(), buffer.size());
    return buffer;
}

void FileIOHandler::write_from_buffer(const std::string &fn, const std::string &buff)
{
    std::ofstream f(fn);
    f.write(buff.data(), buff.size());
}

bool FileIOHandler::is_empty(const std::string &fn)
{
    std::ifstream f(fn);
    return f.peek() == std::ifstream::traits_type::eof();
}

int FileIOHandler::get_files_number(const std::string &path)
{
    int res = 0;
    for (auto const &entry : std::filesystem::directory_iterator(path))
        res += 1;
    return res;
}