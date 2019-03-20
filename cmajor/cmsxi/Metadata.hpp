// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_METADATA_INCLUDED
#define CMAJOR_CMSXI_METADATA_INCLUDED
#include <stdint.h>
#include <string>
#include <vector>
#include <cmajor/util/CodeFormatter.hpp>

namespace cmsxi {

using namespace cmajor::util;

enum class MDItemKind : uint8_t
{
    bool_ = 0u, long_ = 1u, string = 2u, structRef = 3u, struct_ = 4u
};

const int64_t fileInfoNodeType = 0;
const int64_t funcInfoNodeType = 1;
const int64_t lineInfoNodeType = 2;

class MDItem
{
public:
    MDItem(MDItemKind kind_);
    virtual ~MDItem();
    MDItemKind Kind() const { return kind; }
    virtual void Write(CodeFormatter& formatter) = 0;
private:
    MDItemKind kind;
};

class MDBool : public MDItem
{
public:
    MDBool(bool value_);
    void Write(CodeFormatter& formatter) override;
private:
    bool value;
};

class MDLong : public MDItem
{
public:
    MDLong(int64_t value_);
    void Write(CodeFormatter& formatter) override;
private:
    int64_t value;
};

class MDString : public MDItem
{
public:
    MDString(const std::string& value_);
    void Write(CodeFormatter& formatter) override;
private:
    std::string value;
};

class MDStructRef : public MDItem
{
public:
    MDStructRef(int id_);
    void Write(CodeFormatter& formatter) override;
private:
    int id;
};

class MDStruct : public MDItem
{
public:
    MDStruct(int id_);
    void Write(CodeFormatter& formatter) override;
    void WriteDefinition(CodeFormatter& formatter);
    void AddItem(const std::string& fieldName, MDItem* item);
    int Id() const { return id; }
private:
    int id;
    std::vector<std::pair<std::string, MDItem*>> items;
};

class Metadata
{
public:
    Metadata();
    Metadata(const Metadata&) = delete;
    Metadata& operator=(const Metadata&) = delete;
    MDBool* CreateMDBool(bool value);
    MDLong* CreateMDLong(int64_t value);
    MDString* CreateMDString(const std::string& value);
    MDStructRef* CreateMDStructRef(int id);
    MDStruct* CreateMDStruct();
    void Write(CodeFormatter& formatter);
private:
    void AddItem(MDItem* item);
    std::vector<std::unique_ptr<MDItem>> items;
    std::vector<std::unique_ptr<MDStruct>> structs;
    MDBool mdTrue;
    MDBool mdFalse;
};

} // namespace cmsxi

#endif // CMAJOR_CMSXI_METADATA_INCLUDED
