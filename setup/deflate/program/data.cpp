#include "data.hpp"

namespace setup {

std::string PackageResourceName()
{
    return "cmajor_package";
}

int64_t UncompressedPackageSize()
{
    return 1369711998;
}

std::string UnicodeDBResourceName()
{
    return "wng_unicode_db";
}

int64_t UncompressedUnicodeDBSize()
{
    return 53233116;
}

std::string SetupIconResourceName()
{
    return "setup_icon";
}

std::string AppName()
{
    return "Cmajor";
}

std::string AppVersion()
{
    return "4.1.0";
}

int Compression()
{
    return 1; // deflate
}

std::string DefaultContainingDirPath()
{
    return "C:/";
}

std::string InstallDirName()
{
    return "cmajor";
}


} // setup

