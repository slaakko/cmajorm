// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OBJECT_TEXT_FORMAT_INCLUDED
#define CMSX_OBJECT_TEXT_FORMAT_INCLUDED
#include <system-x/object/Api.hpp>
#include <system-x/machine/Machine.hpp>
#include <string>

namespace cmsx::object {

class SymbolTable;

using TrapAdderFunc = void(*)(SymbolTable& symbolTable);

CMSX_OBJECT_API void SetTrapAdderFunc(TrapAdderFunc trapAdderFunc);

CMSX_OBJECT_API void WriteBinaryFileAsText(const std::string& binaryFileName, cmsx::machine::Machine& machine, std::string& textFileName);

} // namespace cmsx::object

#endif // CMSX_OBJECT_TEXT_FORMAT_INCLUDED
