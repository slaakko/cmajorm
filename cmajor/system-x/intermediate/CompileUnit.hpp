// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_COMPILE_UNIT_INCLUDED
#define CMSX_INTERMEDIATE_COMPILE_UNIT_INCLUDED
#include <system-x/intermediate/Metadata.hpp>
#include <string>

namespace cmsx::intermediate {

class CompileUnit
{
public:
    CompileUnit();
    Context* GetContext() const { return context; }
    void SetContext(Context* context_) { context = context_; }
    const std::string& FilePath() const { return filePath; }
    void SetFilePath(const std::string& filePath_);
    const std::string& Id() const { return id; }
    void SetInfo(const std::string& id_, MetadataRef* metadataRef_);
private:
    Context* context;
    std::string filePath;
    std::string id;
    MetadataRef* metadataRef;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_COMPILE_UNIT_INCLUDED
