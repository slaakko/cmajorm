// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_TYPE_INDEX_INCLUDED
#define CMAJOR_SYMBOLS_TYPE_INDEX_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <cmajor/cmdebug/DIType.hpp>
#include <cmajor/cmdebug/DIEnumConstant.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <soulng/util/BinaryWriter.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <mutex>

namespace cmajor { namespace symbols {

class TypeSymbol;

class SYMBOLS_API TypeIndex
{
public:
    void AddType(const boost::uuids::uuid& typeId, TypeSymbol* typeSymbol, cmajor::ir::Emitter& emitter);
    void Write(soulng::util::BinaryWriter& writer);
private:
    std::recursive_mutex mtx;
    std::unordered_map<boost::uuids::uuid, cmajor::debug::DIType*, boost::hash<boost::uuids::uuid>> typeMap;
    std::vector<std::unique_ptr<cmajor::debug::DIType>> diTypes;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_TYPE_INDEX_INCLUDED
