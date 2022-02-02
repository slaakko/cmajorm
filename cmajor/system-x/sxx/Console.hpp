// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <iostream>
#include <system-x/kernel/File.hpp>
#include <soulng/util/Unicode.hpp>

namespace sxx {

using namespace soulng::unicode;

class Console : public cmsx::kernel::File
{
public:
    Console();
    bool IsReadable() const override { return true; }
    bool IsWritable() const override { return true; }
    bool IsConsole() const override { return true; }
    bool IsHostTextFile() const override { return false; }
    bool HasColors() const override { return false; }
    void Close(cmsx::machine::Process* process) override;
    std::vector<uint8_t> Read(int64_t count, cmsx::machine::Process* process) override;
    int64_t Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process) override;
    void SetToUtf16Mode();
    void SetToTextMode();
private:
    Utf8ToUtf32Engine utf8Engine;
    bool stdInInUtf16Mode;
    bool stdOutInUtf16Mode;
    bool stdErrInUtf16Mode;
};

} // namespace sxx
