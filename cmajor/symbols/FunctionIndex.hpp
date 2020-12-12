// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_FUNCTION_INDEX_INCLUDED
#define CMAJOR_SYMBOLS_FUNCTION_INDEX_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <soulng/util/BinaryWriter.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <mutex>

namespace cmajor { namespace symbols {

using namespace soulng::util;

class Module;
class FunctionSymbol;

class SYMBOLS_API FunctionIndex
{
public:
    FunctionIndex(Module* module_);
    void AddFunction(const boost::uuids::uuid& functionId, FunctionSymbol* functionSymbol);
    void SetMainFunctionId(const boost::uuids::uuid& functionId);
    const boost::uuids::uuid& GetMainFunctionId() const { return mainFunctionId; }
    void Write(BinaryWriter& writer);
private:
    Module* module;
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    boost::uuids::uuid mainFunctionId;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_FUNCTION_INDEX_INCLUDED
