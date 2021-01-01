// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/FunctionIndex.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/cmdebug/DebugInfoIo.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

FunctionIndex::FunctionIndex(Module* module_) : module(module_), mainFunctionId(boost::uuids::nil_uuid())
{
}

void FunctionIndex::AddFunction(const boost::uuids::uuid& functionId, FunctionSymbol* functionSymbol)
{
    std::lock_guard<std::recursive_mutex> lock(module->GetLock());
    if (functionMap.find(functionId) == functionMap.cend())
    {
        functionMap[functionId] = functionSymbol;
    }
}

void FunctionIndex::SetMainFunctionId(const boost::uuids::uuid& functionId)
{
    mainFunctionId = functionId;
}

void FunctionIndex::Write(BinaryWriter& writer)
{
    int32_t numFunctions = functionMap.size();
    cmajor::debug::WriteNumberOfFunctionIndexFunctionRecords(writer, numFunctions);
    for (const auto& p : functionMap)
    {
        FunctionSymbol* functionSymbol = p.second;
        cmajor::debug::WriteFunctionIndexFunctionRecord(writer, p.first, ToUtf8(functionSymbol->FullName()), ToUtf8(functionSymbol->MangledName()));
    }
}

} } // namespace cmajor::symbols
