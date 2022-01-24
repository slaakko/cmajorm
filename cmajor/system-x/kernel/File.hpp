// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_FILE_INCLUDED
#define CMSX_KERNEL_FILE_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <string>
#include <vector>
#include <stdint.h>

namespace cmsx::kernel {

class Process;

class CMSX_KERNEL_API File
{
public:
    File(const std::string& name_);
    virtual ~File();
    virtual std::vector<uint8_t> Read(int64_t count);
    virtual int64_t Write(const std::vector<uint8_t>& buffer);
private:
    std::string name;
};

class CMSX_KERNEL_API ProcessFileTable
{
public:
    ProcessFileTable();
    File* GetFile(int32_t fd) const;
    void CopyFrom(const ProcessFileTable& that);
private:
    std::vector<File*> files;
};

CMSX_KERNEL_API void SetConsoleFiles(File* consoleOutputFile, File* consoleInputFile);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_FILE_INCLUDED
