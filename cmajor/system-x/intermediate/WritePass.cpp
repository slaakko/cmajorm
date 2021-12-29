// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/WritePass.hpp>
#include <system-x/intermediate/Writer.hpp>
#include <system-x/intermediate/Context.hpp>
#include <soulng/util/Path.hpp>

namespace cmsx::intermediate {

WritePass::WritePass() : ModulePass("write")
{
}

void WritePass::Run(Context* context)
{
    Writer writer(context, Path::ChangeExtension(context->FilePath(), ".si"));
    context->GetCode().VisitFunctions(writer);
}

} // cmsx::intermediate
