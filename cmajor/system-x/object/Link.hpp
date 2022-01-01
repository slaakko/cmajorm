// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OBJECT_LINK_INCLUDED
#define CMSX_OBJECT_LINK_INCLUDED
#include <system-x/object/Api.hpp>
#include <stdint.h>
#include <string>
#include <vector>

namespace cmsx::object {

class ObjectFile;
class LinkSection;

enum class LinkCode : uint8_t
{
    once = 0, forwardLongJump = 1, forwardShortJump = 2, absoluteAddrValue = 3, farOcta = 4, clsid = 5, end = 6
};

class ObjectFile;

class CMSX_OBJECT_API LinkCommand
{
public:
    LinkCommand(LinkCode linkCode_);
    LinkCommand(LinkCode linkCode_, uint64_t address_);
    virtual ~LinkCommand();
    virtual LinkCommand* Clone() const = 0;
    virtual void Emit(LinkSection* linkSection);
    virtual void Read(LinkSection* linkSection);
    virtual void Apply(ObjectFile* objectFile, uint64_t value);
    LinkCode Code() const { return linkCode; }
    uint64_t Address() const { return address; }
    void SetId(int32_t id_) { id = id_; }
    int32_t Id() const { return id; }
private:
    LinkCode linkCode;
    uint64_t address;
    int32_t id;
};

class CMSX_OBJECT_API LinkOnceCommand : public LinkCommand
{
public:
    LinkOnceCommand();
    LinkCommand* Clone() const override;
};

class CMSX_OBJECT_API LinkForwardLongJumpCommand : public LinkCommand
{
public:
    LinkForwardLongJumpCommand();
    LinkForwardLongJumpCommand(uint64_t address_, int32_t symbolIndex_);
    void Emit(LinkSection* linkSection) override;
    void Read(LinkSection* linkSection) override;
    void Apply(ObjectFile* objectFile, uint64_t value) override;
    int32_t SymbolIndex() const { return symbolIndex; }
    LinkCommand* Clone() const override;
private:
    int32_t symbolIndex;
};

class CMSX_OBJECT_API LinkForwardShortJumpCommand : public LinkCommand
{
public:
    LinkForwardShortJumpCommand();
    LinkForwardShortJumpCommand(uint64_t address_, int32_t symbolIndex_);
    void Emit(LinkSection* linkSection) override;
    void Read(LinkSection* linkSection) override;
    void Apply(ObjectFile* objectFile, uint64_t value) override;
    int32_t SymbolIndex() const { return symbolIndex; }
    LinkCommand* Clone() const override;
private:
    int32_t symbolIndex;
};

class CMSX_OBJECT_API LinkAbsoluteAddressCommand : public LinkCommand
{
public:
    LinkAbsoluteAddressCommand();
    LinkAbsoluteAddressCommand(uint64_t address_, int32_t symbolIndex_);
    void Emit(LinkSection* linkSection) override;
    void Read(LinkSection* linkSection) override;
    void Apply(ObjectFile* objectFile, uint64_t value) override;
    int32_t SymbolIndex() const { return symbolIndex; }
    LinkCommand* Clone() const override;
private:
    int32_t symbolIndex;
};

class CMSX_OBJECT_API LinkFarOctaCommand : public LinkCommand
{
public:
    LinkFarOctaCommand();
    LinkFarOctaCommand(uint64_t address_, int32_t symbolIndex_);
    void Emit(LinkSection* linkSection) override;
    void Read(LinkSection* linkSection) override;
    void Apply(ObjectFile* objectFile, uint64_t value) override;
    int32_t SymbolIndex() const { return symbolIndex; }
    LinkCommand* Clone() const override;
private:
    int32_t symbolIndex;
};

class CMSX_OBJECT_API LinkClsIdCommand : public LinkCommand
{
public:
    LinkClsIdCommand();
    LinkClsIdCommand(uint64_t address_, uint64_t typeId1_, uint64_t typeId2_);
    void Emit(LinkSection* linkSection) override;
    void Read(LinkSection* linkSection) override;
    void Apply(ObjectFile* objectFile, uint64_t value) override;
    uint64_t TypeId1() const { return typeId1; }
    uint64_t TypeId2() const { return typeId2; }
    LinkCommand* Clone() const override;
private:
    uint64_t typeId1;
    uint64_t typeId2;
};

class CMSX_OBJECT_API LinkEndCommamnd : public LinkCommand
{
public:
    LinkEndCommamnd();
    LinkCommand* Clone() const override;
};

CMSX_OBJECT_API LinkCommand* MakeLinkCommand(LinkCode linkCode);

CMSX_OBJECT_API LinkCommand* ReadLinkCommand(LinkSection* linkSection);

CMSX_OBJECT_API void LinkInternal(ObjectFile* objectFile);
CMSX_OBJECT_API void Link(int logStreamId, const std::string& executableFilePath, const std::vector<std::string>& binaryFileNames, const std::string& clsIdFileName, bool verbose);

} // namespace cmsx::object

#endif // CMSX_OBJECT_LINK_INCLUDED
