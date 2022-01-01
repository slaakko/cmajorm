// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Trace.hpp>
#include <soulng/util/Error.hpp>
#include <soulng/util/Trace.hpp>
#include <sngcm/ast/Project.hpp>
#include <soulng/util/Path.hpp>
#include <memory>
#include <vector>
#include <mutex>
#include <map>
#include <fstream>

namespace cmajor { namespace symbols {

using namespace soulng::util;

int32_t GetTraceFunctionId(const std::string& functionFullName)
{
    return soulng::util::GetTraceFunctionId(functionFullName);
}

void ReadTraceTable()
{
    soulng::util::ReadTraceTable();
}
void WriteTraceTable()
{
    soulng::util::WriteTraceTable();
}

} } // namespace cmajor::symbols
