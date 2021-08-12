#ifndef DATA_H
#define DATA_H
#include <string>

namespace setup {

std::string PackageResourceName();

int64_t UncompressedPackageSize();

std::string UnicodeDBResourceName();

int64_t UncompressedUnicodeDBSize();

std::string SetupIconResourceName();

std::string AppName();

std::string AppVersion();

int Compression();

std::string DefaultContainingDirPath();

std::string InstallDirName();

} // setup

#endif // DATA_H
