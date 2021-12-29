// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/CompileUnit.hpp>

namespace cmsx::intermediate {

CompileUnit::CompileUnit() : context(nullptr), id(), metadataRef()
{
}

void CompileUnit::SetFilePath(const std::string& filePath_)
{
    filePath = filePath_;
}

void CompileUnit::SetInfo(const std::string& id_, MetadataRef* metadataRef_) 
{
    id = id_;
    metadataRef = metadataRef_;
}

} // cmsx::intermediate
