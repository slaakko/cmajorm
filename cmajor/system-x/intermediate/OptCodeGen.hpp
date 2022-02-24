// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_OPT_CODE_GEN_INCLUDED
#define CMSX_INTERMEDIATE_OPT_CODE_GEN_INCLUDED
#include <system-x/intermediate/CodeGen.hpp>

namespace cmsx::intermediate {

CMSX_INTERMEDIATE_API void EmitOptSwitch(SwitchInstruction& inst, CodeGenerator& codeGen);

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_OPT_CODE_GEN_INCLUDED
