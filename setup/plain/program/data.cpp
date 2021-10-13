#include "data.hpp"

namespace setup {

std::string PackageResourceName()
{
    return "cmajor.package";
}

int64_t UncompressedPackageSize()
{
    return 1415323708;
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
    return "4.2.0";
}

int Compression()
{
    return 0; // none
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

