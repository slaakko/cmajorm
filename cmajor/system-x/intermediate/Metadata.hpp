// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_METADATA_INCLUDED
#define CMSX_INTERMEDIATE_METADATA_INCLUDED
#include <system-x/intermediate/Data.hpp>

namespace cmsx::intermediate {

class MetadataStruct;

class CMSX_INTERMEDIATE_API MetadataItem
{
public:
    MetadataItem();
    virtual ~MetadataItem();
};

class CMSX_INTERMEDIATE_API MetadataRef : public MetadataItem
{
public:
    MetadataRef(const SourcePos& sourcePos_, int32_t nodeId_);
    const SourcePos& GetSourcePos() const { return sourcePos; }
    int32_t NodeId() const { return nodeId; }
    MetadataStruct* GetMetadataStruct() const { return metadataStruct; }
    void SetMetadataStruct(MetadataStruct* metadataStruct_) { metadataStruct = metadataStruct_; }
private:
    SourcePos sourcePos;
    int32_t nodeId;
    MetadataStruct* metadataStruct;
};

class CMSX_INTERMEDIATE_API MetadataBool : public MetadataItem
{
public:
    MetadataBool(bool value_);
    bool Value() const { return value; }
private:
    bool value;
};

class CMSX_INTERMEDIATE_API MetadataLong : public MetadataItem
{
public:
    MetadataLong(int64_t value_);
    int64_t Value() const { return value; }
private:
    int64_t value;
};

class CMSX_INTERMEDIATE_API MetadataString : public MetadataItem
{
public:
    MetadataString(const std::string& value_);
    const std::string& Value() const { return value; }
private:
    std::string value;
};

class CMSX_INTERMEDIATE_API MetadataStruct
{
public:
    MetadataStruct(const SourcePos& sourcePos_, int32_t id_);
    MetadataStruct(const MetadataStruct&) = delete;
    MetadataStruct& operator=(const MetadataStruct&) = delete;
    const SourcePos& GetSourcePos() const { return sourcePos; }
    int32_t Id() const { return id; }
    void AddItem(const std::string& fieldName, MetadataItem* item);
    MetadataItem* GetItem(const std::string& fieldName) const;
private:
    SourcePos sourcePos;
    int32_t id;
    std::map<std::string, MetadataItem*> itemMap;
};

class CMSX_INTERMEDIATE_API Metadata
{
public:
    Metadata();
    Metadata(const Metadata&) = delete;
    Metadata& operator=(const Metadata&) = delete;
    Context* GetContext() const { return context; }
    void SetContext(Context* context_) { context = context_; }
    MetadataStruct* GetMetadataStruct(int32_t id) const;
    MetadataStruct* AddMetadataStruct(const SourcePos& sourcePos, int32_t id, Context* context);
    MetadataBool* CreateMetadataBool(bool value);
    MetadataLong* CreateMetadataLong(int64_t value);
    MetadataString* CreateMetadataString(const std::string& value);
    MetadataRef* CreateMetadataRef(const SourcePos& sourcePos, int32_t nodeId);
    void ResolveMetadataReferences(Context* context);
private:
    Context* context;
    std::vector<std::unique_ptr<MetadataStruct>> metadataNodes;
    std::vector<std::unique_ptr<MetadataItem>> metadataItems;
    std::map<int32_t, MetadataStruct*> metadataMap;
    MetadataBool* trueItem;
    MetadataBool* falseItem;
    std::map<int64_t, MetadataLong*> longItemMap;
    std::map<std::string, MetadataString*> stringItemMap;
    std::map<int32_t, MetadataRef*> referenceMap;
    std::vector<MetadataRef*> metadataReferences;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_METADATA_INCLUDED
