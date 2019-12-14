// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxi/Metadata.hpp>
#include <cmajor/cmsxi/BasicBlock.hpp>
#include <soulng/util/TextUtils.hpp>

namespace cmsxi {

MDItem::MDItem(MDItemKind kind_) : kind(kind_)
{
}

MDItem::~MDItem()
{
}

MDBool::MDBool(bool value_) : MDItem(MDItemKind::bool_), value(value_)
{
}

void MDBool::Write(CodeFormatter& formatter)
{
    if (value)
    {
        formatter.Write("true");
    }
    else
    {
        formatter.Write("false");
    }
}

MDLong::MDLong(int64_t value_) : MDItem(MDItemKind::long_), value(value_)
{
}

void MDLong::Write(CodeFormatter& formatter)
{
    formatter.Write(std::to_string(value));
}

MDString::MDString(const std::string& value_) : MDItem(MDItemKind::string), value(value_)
{
}

void MDString::Write(CodeFormatter& formatter)
{
    formatter.Write("\"");
    formatter.Write(StringStr(value));
    formatter.Write("\"");
}

MDStructRef::MDStructRef(int id_) : MDItem(MDItemKind::structRef), id(id_)
{
}

void MDStructRef::Write(CodeFormatter& formatter)
{
    formatter.Write("!" + std::to_string(id));
}

MDStruct::MDStruct(int id_) : MDItem(MDItemKind::struct_), id(id_)
{
}

void MDStruct::Write(CodeFormatter& formatter)
{
    formatter.Write("!" + std::to_string(id));
}

void MDStruct::WriteDefinition(CodeFormatter& formatter)
{
    formatter.Write("!" + std::to_string(id) + " = {");
    bool first = true;
    for (const auto& item : items)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(", ");
        }
        formatter.Write(item.first);
        formatter.Write(": "),
        item.second->Write(formatter);
    }
    formatter.WriteLine("}");
}

void MDStruct::AddItem(const std::string& fieldName, MDItem* item)
{
    items.push_back(std::make_pair(fieldName, item));
}

MDBasicBlockRef::MDBasicBlockRef(void* bb_) : MDItem(MDItemKind::basicBlockRef), bb(bb_)
{
}

void MDBasicBlockRef::Write(CodeFormatter& formatter)
{
    BasicBlock* basicBlock = static_cast<BasicBlock*>(bb);
    formatter.Write(std::to_string(basicBlock->Id()));
}

Metadata::Metadata() : mdTrue(true), mdFalse(false)
{
}

MDBool* Metadata::CreateMDBool(bool value)
{
    if (value)
    {
        return &mdTrue;
    }
    else
    {
        return &mdFalse;
    }
}

MDLong* Metadata::CreateMDLong(int64_t value)
{
    MDLong* item = new MDLong(value);
    AddItem(item);
    return item;
}

MDString* Metadata::CreateMDString(const std::string& value)
{
    MDString* item = new MDString(value);
    AddItem(item);
    return item;
}

void Metadata::AddItem(MDItem* item)
{
    items.push_back(std::unique_ptr<MDItem>(item));;
}

MDStructRef* Metadata::CreateMDStructRef(int id)
{
    MDStructRef* item = new MDStructRef(id);
    AddItem(item);
    return item;
}

MDStruct* Metadata::CreateMDStruct()
{
    MDStruct* item = new MDStruct(structs.size());
    structs.push_back(std::unique_ptr<MDStruct>(item));
    return item;
}

MDBasicBlockRef* Metadata::CreateMDBasicBlockRef(void* bb)
{
    MDBasicBlockRef* item = new MDBasicBlockRef(bb);
    AddItem(item);
    return item;
}

void Metadata::Write(CodeFormatter& formatter)
{
    if (structs.empty()) return;
    formatter.WriteLine();
    formatter.WriteLine("metadata");
    formatter.WriteLine("{");
    formatter.IncIndent();
    for (const auto& s : structs)
    {
        s->WriteDefinition(formatter);
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
}

} // namespace cmsxi
