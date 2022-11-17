#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

void InitApplication()
{
    soulng::util::Init();
}

void DoneApplication()
{
    soulng::util::Done();
}

using namespace soulng::util;

void ExpandTabs(const std::string& directory, bool verbose)
{
    boost::filesystem::directory_iterator it(directory);
    while (it != boost::filesystem::directory_iterator())
    {
        if (it->path().filename_is_dot() || it->path().filename_is_dot_dot())
        {
            ++it;
            continue;
        }
        if (boost::filesystem::is_regular_file(it->path()))
        {
            int tabs = 0;
            std::string filePath = it->path().generic_string();
            if (Path::GetExtension(filePath) == ".cm")
            {
                std::string s = ReadFile(filePath);
                std::string t;
                for (char c : s)
                {
                    if (c == '\t')
                    {
                        ++tabs;
                        t.append(4, ' ');
                    }
                    else if (c != '\r')
                    {
                        t.append(1, c);
                    }
                }
                if (tabs > 0 && verbose)
                {
                    std::cout << "file '" << it->path().generic_string() << "' contained " << tabs << " tabs" << std::endl;
                }
                std::time_t lastWriteTime = boost::filesystem::last_write_time(filePath);
                std::string oldFilePath = filePath + ".old";
                boost::filesystem::rename(filePath, oldFilePath);
                {
                    std::ofstream outFile(filePath);
                    outFile << t;
                }
                boost::filesystem::last_write_time(filePath, lastWriteTime);
                boost::filesystem::remove(oldFilePath);
            }
        }
        ++it;
    }
}

int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        bool verbose = false;
        std::vector<std::string> directories;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg.starts_with("--"))
            {
                if (arg == "--verbose")
                {
                    verbose = true;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (arg.starts_with("-"))
            {
                std::string options = arg.substr(1);
                for (char c : options)
                {
                    switch (c)
                    {
                        case 'v':
                        {
                            verbose = true;
                            break;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, c) + "'");
                        }
                    }
                }
            }
            else
            {
                directories.push_back(GetFullPath(arg));
            }
        }
        for (const auto& dir : directories)
        {
            if (verbose)
            {
                std::cout << "> " << dir << std::endl;
            }
            ExpandTabs(dir, verbose);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    DoneApplication();
    return 0;
}