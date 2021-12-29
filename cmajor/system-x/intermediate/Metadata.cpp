// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Metadata.hpp>
#include <system-x/intermediate/Context.hpp>
#include <system-x/intermediate/Error.hpp>

namespace cmsx::intermediate {

MetadataRef::MetadataRef(const SourcePos& sourcePos_, int32_t nodeId_) : sourcePos(sourcePos_), nodeId(nodeId_), metadataStruct(nullptr)
{
}

MetadataItem::MetadataItem()
{
}

MetadataItem::~MetadataItem()
{
}

MetadataBool::MetadataBool(bool value_) : value(value_)
{
}

MetadataLong::MetadataLong(int64_t value_) : value(value_)
{
}

MetadataString::MetadataString(const std::string& value_) : value(value_)
{
}

MetadataStruct::MetadataStruct(const SourcePos& sourcePos_, int32_t id_) : sourcePos(sourcePos_), id(id_)
{
}

void MetadataStruct::AddItem(const std::string& fieldName, MetadataItem* item)
{
    itemMap[fieldName] = item;
}

MetadataItem* MetadataStruct::GetItem(const std::string& fieldName) const
{
    auto it = itemMap.find(fieldName);
    if (it != itemMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

Metadata::Metadata() : context(nullptr), trueItem(), falseItem()
{
}

MetadataStruct* Metadata::GetMetadataStruct(int32_t id) const
{
    auto it = metadataMap.find(id);
    if (it != metadataMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

MetadataStruct* Metadata::AddMetadataStruct(const SourcePos& sourcePos, int32_t id, Context* context)
{
    MetadataStruct* prev = GetMetadataStruct(id);
    if (prev)
    {
        Error("error adding metadata node: node id " + std::to_string(id) + " not unique", sourcePos, context, prev->GetSourcePos());
    }
    MetadataStruct* metadataStruct = new MetadataStruct(sourcePos, id);
    metadataNodes.push_back(std::unique_ptr<MetadataStruct>(metadataStruct));
    metadataMap[metadataStruct->Id()] = metadataStruct;
    return metadataStruct;
}

MetadataBool* Metadata::CreateMetadataBool(bool value)
{
    if (value)
    {
        if (!trueItem)
        {
            trueItem = new MetadataBool(true);
            metadataItems.push_back(std::unique_ptr<MetadataItem>(trueItem));
        }
        return trueItem;
    }
    else
    {
        if (!falseItem)
        {
            falseItem = new MetadataBool(false);
            metadataItems.push_back(std::unique_ptr<MetadataItem>(falseItem));
        }
        return falseItem;
    }
}

MetadataLong* Metadata::CreateMetadataLong(int64_t value)
{
    auto it = longItemMap.find(value);
    if (it != longItemMap.cend())
    {
        return it->second;
    }
    else
    {
        MetadataLong* metadataLong = new MetadataLong(value);
        metadataItems.push_back(std::unique_ptr<MetadataItem>(metadataLong));
        longItemMap[value] = metadataLong;
        return metadataLong;
    }
}

MetadataString* Metadata::CreateMetadataString(const std::string& value)
{
    std::string val = value.substr(1, value.length() - 2);
    auto it = stringItemMap.find(val);
    if (it != stringItemMap.cend())
    {
        return it->second;
    }
    else
    {
        MetadataString* metadataString = new MetadataString(val);
        metadataItems.push_back(std::unique_ptr<MetadataItem>(metadataString));
        stringItemMap[val] = metadataString;
        return metadataString;
    }
}

MetadataRef* Metadata::CreateMetadataRef(const SourcePos& sourcePos, int32_t nodeId)
{
    auto it = referenceMap.find(nodeId);
    if (it != referenceMap.cend())
    {
        return it->second;
    }
    else
    {
        MetadataRef* metadataRef = new MetadataRef(sourcePos, nodeId);
        referenceMap[metadataRef->NodeId()] = metadataRef;
        metadataItems.push_back(std::unique_ptr<MetadataItem>(metadataRef));
        metadataReferences.push_back(metadataRef);
        return metadataRef;
    }
}

void Metadata::ResolveMetadataReferences(Context* context)
{
    for (MetadataRef* metadataRef : metadataReferences)
    {
        int32_t nodeId = metadataRef->NodeId();
        MetadataStruct* metadataNode = GetMetadataStruct(nodeId);
        if (metadataNode)
        {
            metadataRef->SetMetadataStruct(metadataNode);
        }
        else
        {
            Error("error resolving metadata reference: node id " + std::to_string(nodeId) + " not found", metadataRef->GetSourcePos(), context);
        }
    }
}

} // cmsx::intermediate
