// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/Debug.hpp>
#include <system-x/object/BinaryFile.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmsx::object {

using namespace soulng::util;

std::string DebugRecordKindStr(DebugRecordKind kind)
{
    switch (kind)
    {
        case DebugRecordKind::fileInfo: return "FILEINFO";
        case DebugRecordKind::funcInfo: return "FUNCINFO";
        case DebugRecordKind::startFunc: return "STARTFUNC";
        case DebugRecordKind::endFunc: return "ENDFUNC";
        case DebugRecordKind::lineInfo: return "LINEINFO";
        case DebugRecordKind::beginTry: return "BEGINTRY";
        case DebugRecordKind::endTry: return "ENDTRY";
        case DebugRecordKind::catch_: return "CATCH";
        case DebugRecordKind::beginCleanup: return "BEGINCLEANUP";
        case DebugRecordKind::endCleanup: return "ENDCLEANUP";
    }
    return std::string();
}

DebugRecord::DebugRecord(DebugRecordKind kind_) : kind(kind_)
{
}

void DebugRecord::Emit(DebugSection* debugSection)
{
    debugSection->EmitByte(static_cast<uint8_t>(kind));
}

void DebugRecord::Read(DebugSection* debugSection)
{
}

FileInfoRecord::FileInfoRecord() : DebugRecord(DebugRecordKind::fileInfo), sourceFileName(), sourceFileNameId()
{
}

FileInfoRecord::FileInfoRecord(const std::string& sourceFileName_, uint32_t sourceFileNameId_) : 
    DebugRecord(DebugRecordKind::fileInfo), sourceFileName(sourceFileName_), sourceFileNameId(sourceFileNameId_)
{
}

void FileInfoRecord::Emit(DebugSection* debugSection)
{
    DebugRecord::Emit(debugSection);
    debugSection->EmitString(sourceFileName); 
    debugSection->EmitTetra(sourceFileNameId);
}

void FileInfoRecord::Read(DebugSection* debugSection)
{
    DebugRecord::Read(debugSection);
    sourceFileName = debugSection->ReadString();
    sourceFileNameId = debugSection->ReadTetra();
}

std::string FileInfoRecord::ToString() const
{
    std::string str = DebugRecordKindStr(Kind());
    str.append("(").append("sourceFileName=").append(sourceFileName).append(", sourceFileNameId=").append(std::to_string(sourceFileNameId)).append(")");
    return str;
}

FuncInfoRecord::FuncInfoRecord() : DebugRecord(DebugRecordKind::funcInfo), functionSymbolIndex(), fullName(), sourceFileNameId(), frameSize()
{
}

FuncInfoRecord::FuncInfoRecord(uint32_t functionSymbolIndex_, const std::string& fullName_, uint32_t sourceFileNameId_, int64_t frameSize_) : 
    DebugRecord(DebugRecordKind::funcInfo), functionSymbolIndex(functionSymbolIndex_), fullName(fullName_), sourceFileNameId(sourceFileNameId_), frameSize(frameSize_)
{
}

void FuncInfoRecord::Emit(DebugSection* debugSection)
{
    DebugRecord::Emit(debugSection);
    debugSection->EmitTetra(functionSymbolIndex);
    debugSection->EmitString(fullName);
    debugSection->EmitTetra(sourceFileNameId);
    debugSection->EmitOcta(frameSize);
}

void FuncInfoRecord::Read(DebugSection* debugSection) 
{
    DebugRecord::Read(debugSection);
    functionSymbolIndex = debugSection->ReadTetra();
    fullName = debugSection->ReadString();
    sourceFileNameId = debugSection->ReadTetra();
    frameSize = debugSection->ReadOcta();
}

std::string FuncInfoRecord::ToString() const
{
    std::string str = DebugRecordKindStr(Kind());
    str.append("(").append("functionSymbolIndex=").append(std::to_string(functionSymbolIndex)).append(", fullName=").append(fullName).append(", sourceFileNameId=").append(
        std::to_string(sourceFileNameId)).append(", frameSize=#").append(ToHexString(std::uint64_t(frameSize))).append(")");
    return str;
}

StartFuncRecord::StartFuncRecord() : DebugRecord(DebugRecordKind::startFunc), functionSymbolIndex()
{
}

StartFuncRecord::StartFuncRecord(uint32_t functionSymbolIndex_) : DebugRecord(DebugRecordKind::startFunc), functionSymbolIndex(functionSymbolIndex_)
{
}

void StartFuncRecord::Emit(DebugSection* debugSection)
{
    DebugRecord::Emit(debugSection);
    debugSection->EmitTetra(functionSymbolIndex);
}

void StartFuncRecord::Read(DebugSection* debugSection)
{
    DebugRecord::Read(debugSection);
    functionSymbolIndex = debugSection->ReadTetra();
}

std::string StartFuncRecord::ToString() const
{
    std::string str = DebugRecordKindStr(Kind());
    str.append("(").append("functionSymbolIndex=").append(std::to_string(functionSymbolIndex)).append(")");
    return str;
}

EndFuncRecord::EndFuncRecord() : DebugRecord(DebugRecordKind::endFunc), functionSymbolIndex()
{
}

EndFuncRecord::EndFuncRecord(uint32_t functionSymbolIndex_) : DebugRecord(DebugRecordKind::endFunc), functionSymbolIndex(functionSymbolIndex_)
{
}

void EndFuncRecord::Emit(DebugSection* debugSection)
{
    DebugRecord::Emit(debugSection);
    debugSection->EmitTetra(functionSymbolIndex);
}

void EndFuncRecord::Read(DebugSection* debugSection)
{
    DebugRecord::Read(debugSection);
    functionSymbolIndex = debugSection->ReadTetra();
}

std::string EndFuncRecord::ToString() const
{
    std::string str = DebugRecordKindStr(Kind());
    str.append("(").append("functionSymbolIndex=").append(std::to_string(functionSymbolIndex)).append(")");
    return str;
}

LineInfoRecord::LineInfoRecord() : DebugRecord(DebugRecordKind::lineInfo), offset(), lineNumber()
{
}

LineInfoRecord::LineInfoRecord(uint32_t offset_, uint32_t lineNumber_) : 
    DebugRecord(DebugRecordKind::lineInfo), offset(offset_), lineNumber(lineNumber_)
{
}

void LineInfoRecord::Emit(DebugSection* debugSection)
{
    DebugRecord::Emit(debugSection);
    debugSection->EmitTetra(offset);
    debugSection->EmitTetra(lineNumber);
}

void LineInfoRecord::Read(DebugSection* debugSection)
{
    DebugRecord::Read(debugSection);
    offset = debugSection->ReadTetra();
    lineNumber = debugSection->ReadTetra();
}

std::string LineInfoRecord::ToString() const
{
    std::string str = DebugRecordKindStr(Kind());
    str.append("(offset=#").append(ToHexString(offset)).append(", lineNumber=").append(std::to_string(lineNumber)).append(")");
    return str;
}

BeginTryRecord::BeginTryRecord() : DebugRecord(DebugRecordKind::beginTry), tryBlockId(), parentTryBlockId(), offset()
{
}

BeginTryRecord::BeginTryRecord(uint32_t tryBlockId_, uint32_t parentTryBlockId_, uint32_t offset_) : 
    DebugRecord(DebugRecordKind::beginTry), tryBlockId(tryBlockId_), parentTryBlockId(parentTryBlockId_), offset(offset_)
{
}

void BeginTryRecord::Emit(DebugSection* debugSection)
{
    DebugRecord::Emit(debugSection);
    debugSection->EmitTetra(tryBlockId);
    debugSection->EmitTetra(parentTryBlockId);
    debugSection->EmitTetra(offset);
}

void BeginTryRecord::Read(DebugSection* debugSection)
{
    DebugRecord::Read(debugSection);
    tryBlockId = debugSection->ReadTetra();
    parentTryBlockId = debugSection->ReadTetra();
    offset = debugSection->ReadTetra();
}

std::string BeginTryRecord::ToString() const
{
    std::string str = DebugRecordKindStr(Kind());
    str.append("(").append("tryBlockId=#").append(ToHexString(tryBlockId)).append(", parentTryBlockId=#").append(ToHexString(parentTryBlockId)).append(
        ", offset=#").append(ToHexString(offset)).append(")");
    return str;
}

EndTryRecord::EndTryRecord() : DebugRecord(DebugRecordKind::endTry), tryBlockId(), offset()
{
}

EndTryRecord::EndTryRecord(uint32_t tryBlockId_, uint32_t offset_) : 
    DebugRecord(DebugRecordKind::endTry), tryBlockId(tryBlockId_), offset(offset_)
{
}

void EndTryRecord::Emit(DebugSection* debugSection)
{
    DebugRecord::Emit(debugSection);
    debugSection->EmitTetra(tryBlockId);
    debugSection->EmitTetra(offset);
}

void EndTryRecord::Read(DebugSection* debugSection)
{
    DebugRecord::Read(debugSection);
    tryBlockId = debugSection->ReadTetra();
    offset = debugSection->ReadTetra();
}

std::string EndTryRecord::ToString() const
{
    std::string str = DebugRecordKindStr(Kind());
    str.append("(").append("tryBlockId=#").append(ToHexString(tryBlockId)).append(", offset=#").append(ToHexString(offset)).append(")");
    return str;
}

CatchRecord::CatchRecord() : DebugRecord(DebugRecordKind::catch_), catchBlockId(), tryBlockId(), caughtTypeId1(), caughtTypeId2()
{
}

CatchRecord::CatchRecord(uint32_t catchBlockId_, uint32_t tryBlockId_, uint64_t caughtTypeId1_, uint64_t caughtTypeId2_) :
    DebugRecord(DebugRecordKind::catch_), catchBlockId(catchBlockId_), tryBlockId(tryBlockId_), caughtTypeId1(caughtTypeId1_), caughtTypeId2(caughtTypeId2_)
{
}

void CatchRecord::Emit(DebugSection* debugSection)
{
    DebugRecord::Emit(debugSection);
    debugSection->EmitTetra(catchBlockId);
    debugSection->EmitTetra(tryBlockId);
    debugSection->EmitOcta(caughtTypeId1);
    debugSection->EmitOcta(caughtTypeId2);
}

void CatchRecord::Read(DebugSection* debugSection)
{
    DebugRecord::Read(debugSection);
    catchBlockId = debugSection->ReadTetra();
    tryBlockId = debugSection->ReadTetra();
    caughtTypeId1 = debugSection->ReadOcta();
    caughtTypeId2 = debugSection->ReadOcta();
}

std::string CatchRecord::ToString() const
{
    boost::uuids::uuid typeId;
    IntsToUuid(caughtTypeId1, caughtTypeId2, typeId);
    std::string str = DebugRecordKindStr(Kind());
    str.append("(").append("catchBlockId=#").append(ToHexString(catchBlockId)).append(", tryBlockId=#").append(ToHexString(tryBlockId)).append(
        ", typeId=").append(boost::uuids::to_string(typeId)).append(")");
    return str;
}

BeginCleanupRecord::BeginCleanupRecord() : DebugRecord(DebugRecordKind::beginCleanup), cleanupBlockId(), tryBlockId(), offset()
{
}

BeginCleanupRecord::BeginCleanupRecord(uint32_t cleanupBlockId_, uint32_t tryBlockId_, uint32_t offset_) :  
    DebugRecord(DebugRecordKind::beginCleanup), cleanupBlockId(cleanupBlockId_), tryBlockId(tryBlockId_), offset(offset_)
{

}
void BeginCleanupRecord::Emit(DebugSection* debugSection)
{
    DebugRecord::Emit(debugSection);
    debugSection->EmitTetra(cleanupBlockId);
    debugSection->EmitTetra(tryBlockId);
    debugSection->EmitTetra(offset);
}

void BeginCleanupRecord::Read(DebugSection* debugSection)
{
    DebugRecord::Read(debugSection);
    cleanupBlockId = debugSection->ReadTetra();
    tryBlockId = debugSection->ReadTetra();
    offset = debugSection->ReadTetra();
}

std::string BeginCleanupRecord::ToString() const
{
    std::string str = DebugRecordKindStr(Kind());
    str.append("(").append("cleanupBlockId=#").append(ToHexString(cleanupBlockId)).append(", tryBlockId=#").append(ToHexString(tryBlockId)).append(
        ", offset=#").append(ToHexString(offset)).append(")");
    return str;
}

EndCleanupRecord::EndCleanupRecord() : DebugRecord(DebugRecordKind::endCleanup), cleanupBlockId(), offset()
{
}

EndCleanupRecord::EndCleanupRecord(uint32_t cleanupBlockId_, uint32_t offset_) : 
    DebugRecord(DebugRecordKind::endCleanup), cleanupBlockId(cleanupBlockId_), offset(offset_)
{
}

void EndCleanupRecord::Emit(DebugSection* debugSection)
{
    DebugRecord::Emit(debugSection);
    debugSection->EmitTetra(cleanupBlockId);
    debugSection->EmitTetra(offset);
}

void EndCleanupRecord::Read(DebugSection* debugSection)
{
    DebugRecord::Read(debugSection);
    cleanupBlockId = debugSection->ReadTetra();
    offset = debugSection->ReadTetra();
}

std::string EndCleanupRecord::ToString() const
{
    std::string str = DebugRecordKindStr(Kind());
    str.append("(").append("cleanupBlockId=#").append(ToHexString(cleanupBlockId)).append(
        ", offset=#").append(ToHexString(offset)).append(")");
    return str;
}

DebugRecord* MakeDebugRecord(DebugRecordKind kind)
{
    switch (kind)
    {
        case DebugRecordKind::fileInfo: return new FileInfoRecord();
        case DebugRecordKind::funcInfo: return new FuncInfoRecord();
        case DebugRecordKind::startFunc:return new StartFuncRecord();
        case DebugRecordKind::endFunc: return new EndFuncRecord();
        case DebugRecordKind::lineInfo: return new LineInfoRecord();
        case DebugRecordKind::beginTry: return new BeginTryRecord();
        case DebugRecordKind::endTry: return new EndTryRecord();
        case DebugRecordKind::catch_: return new CatchRecord();
        case DebugRecordKind::beginCleanup: return new BeginCleanupRecord();
        case DebugRecordKind::endCleanup: return new EndCleanupRecord();
    }
    return nullptr;
}

DebugRecord* ReadDebugRecord(DebugSection* debugSection)
{
    DebugRecordKind kind = static_cast<DebugRecordKind>(debugSection->ReadByte());
    DebugRecord* debugRecord = MakeDebugRecord(kind);
    debugRecord->Read(debugSection);
    return debugRecord;
}

} // namespace cmsx::object
