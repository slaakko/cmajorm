// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/WriteCfgPass.hpp>
#include <system-x/intermediate/Context.hpp>
#include <soulng/util/Path.hpp>

namespace cmsx::intermediate {

using namespace soulng::util;

WriteCfgPass::WriteCfgPass() : FunctionPass("write-cfg")
{
}

void WriteCfgPass::Run(Function& function)
{
    std::string libDir = Path::GetDirectoryName(function.Parent()->GetContext()->FilePath());
    std::string functionXmlFilePath = Path::Combine(libDir, function.Name() + ".xml");
    function.WriteXmlDocument(functionXmlFilePath);
}

} // cmsx::intermediate
