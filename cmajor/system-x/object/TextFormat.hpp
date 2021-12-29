// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OBJECT_TEXT_FORMAT_INCLUDED
#define CMSX_OBJECT_TEXT_FORMAT_INCLUDED
#include <system-x/object/Api.hpp>
#include <string>

namespace cmsx::object {

CMSX_OBJECT_API void WriteBinaryFileAsText(const std::string& binaryFileName);

} // namespace cmsx::object

#endif // CMSX_OBJECT_TEXT_FORMAT_INCLUDED
