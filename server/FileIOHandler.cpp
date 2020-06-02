#include "FileIOHandler.h"

void FileIOHandler::create_file(const std::string &path, const std::string &name)
{
    if (path.size() + name.size() == 0)
        throw std::runtime_error("creating file error");
    std::string full = path + name;
    std::ofstream file {full};
}

void FileIOHandler::find(const std::string &str)
{

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