// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OBJECT_BINARY_FILE_FORMATTER_INCLUDED
#define CMSX_OBJECT_BINARY_FILE_FORMATTER_INCLUDED
#include <system-x/object/InstructionFormatter.hpp>

namespace cmsx::object {

using namespace cmsx::machine;

class BinaryFile;
class Section;
class HeaderSection;
class CodeSection;
class DataSection;
class SymbolSection;
class LinkSection;
class DebugSection;
class SymbolTable;
class Symbol;

class CMSX_OBJECT_API BinaryFileFormatter : public InstructionFormatter
{
public:
    BinaryFileFormatter(cmsx::machine::Machine& machine_, BinaryFile* file_);
    BinaryFile* File() const { return file; }
    Section* CurrentSection() const { return currentSection; }
    virtual void Format();
    virtual void FormatOcta(uint64_t octa);
    virtual void FormatSymbol(Symbol* symbol);
    virtual void FormatSection(HeaderSection* section);
    virtual void FormatSection(CodeSection* section);
    virtual void FormatSection(DataSection* section);
    virtual void FormatSection(SymbolSection* section);
    virtual void FormatSection(LinkSection* section);
    virtual void FormatSection(DebugSection* section);
    virtual void FormatSymbolTable(SymbolTable& symbolTable);
private:
    BinaryFile* file;
    Section* currentSection;
    std::map<int64_t, int64_t> binaryFileFormatterSetAddressMap;
};

} // namespace cmsx::object

#endif // CMSX_OBJECT_BINARY_FILE_FORMATTER_INCLUDED
