// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_DIR_FILE_INCLUDED
#define CMSX_KERNEL_DIR_FILE_INCLUDED
#include <system-x/kernel/File.hpp>
#include <system-x/kernel/Fs.hpp>

namespace cmsx::kernel {

class CMSX_KERNEL_API DirFile : public File
{
public:
    DirFile(const std::string& name_);
    bool IsDirFile() const override { return true; }
    bool IsReadable() const override { return false; }
    bool IsWritable() const override { return false; }
    bool IsConsole() const override { return false; }
    bool IsHostTextFile() const override { return false; }
    bool HasColors() const override { return false; }
    virtual int32_t Read(DirectoryEntry& dirEntry, cmsx::machine::Process* process) = 0;
};

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_DIR_FILE_INCLUDED
