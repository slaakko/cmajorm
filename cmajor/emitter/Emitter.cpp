// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/emitter/Emitter.hpp>
#include <cmajor/symbols/Module.hpp>
#ifdef _WIN32
    #include <cmajor/emitter/WindowsEmitter.hpp>
#else
    #include <cmajor/emitter/LinuxEmitter.hpp>
#endif

namespace cmajor { namespace emitter {

void GenerateCode(EmittingContext& emittingContext, BoundCompileUnit& boundCompileUnit)
{
#ifdef _WIN32
    WindowsEmitter emitter(emittingContext, boundCompileUnit.GetCompileUnitNode()->FilePath(), boundCompileUnit.GetModule());
#else
    LinuxEmitter emitter(emittingContext, boundCompileUnit.GetCompileUnitNode()->FilePath(), boundCompileUnit.GetModule());
#endif
    boundCompileUnit.Accept(emitter);
}

} } // namespace cmajor::emitter
