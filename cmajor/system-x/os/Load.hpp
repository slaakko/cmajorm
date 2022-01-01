// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OS_LOAD_INCLUDED
#define CMSX_OS_LOAD_INCLUDED
#include <system-x/os/Api.hpp>
#include <system-x/machine/Machine.hpp>

namespace cmsx::os {

class Process;

CMSX_OS_API void Load(Process* process, const std::vector<std::string>& args, const std::vector<std::string>& env, cmsx::machine::Machine& machine);

} // namespace cmsx::os

#endif // CMSX_OS_LOAD_INCLUDED
