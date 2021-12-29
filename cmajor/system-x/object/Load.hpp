// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OBJECT_LOAD_INCLUDED
#define CMSX_OBJECT_LOAD_INCLUDED
#include <system-x/object/Api.hpp>
#include <system-x/machine/Machine.hpp>
#include <stdint.h>
#include <string>
#include <vector>

namespace cmsx::object {

CMSX_OBJECT_API void Load(const std::string& executableFilePath, uint64_t rv, const std::vector<std::string>& args, const std::vector<std::string>& env, 
    cmsx::machine::Machine& machine);

} // namespace cmsx::object

#endif // CMSX_OBJECT_LOAD_INCLUDED
