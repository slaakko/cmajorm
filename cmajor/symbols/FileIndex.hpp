// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_FILE_INDEX_INCLUDED
#define CMAJOR_SYMBOLS_FILE_INDEX_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <soulng/util/BinaryWriter.hpp>
#include <unordered_map>
#include <mutex>

namespace cmajor { namespace symbols {

using namespace soulng::util;

class Module;

/*
class SYMBOLS_API FileIndex
{
public:
    void AddFile(int32_t fileIndex, const std::string& filePath);
    void Write(BinaryWriter& writer);
private:
    std::unordered_map<int32_t, std::string> fileMap;
};
*/

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_FILE_INDEX_INCLUDED
