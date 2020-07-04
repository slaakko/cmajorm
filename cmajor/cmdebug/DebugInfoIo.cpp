// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DebugInfoIo.hpp>
#include <stdexcept>

namespace cmajor { namespace debug {

void WriteCmdbFileTag(BinaryWriter& writer)
{
    writer.Write("CMDB", false);
    writer.Write(currentDebugInfoFormat);
}

void ReadCmdbFileTag(BinaryReader& reader)
{
    char c;
    char m;
    char d;
    char b;
    uint8_t format;
    try
    {
        c = reader.ReadChar();
        m = reader.ReadChar();
        d = reader.ReadChar();
        b = reader.ReadChar();
        format = reader.ReadByte();
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("invalid CMDB file tag read from '" + reader.FileName() + "': not a CMDB debug information file: " + ex.what());
    }
    if (c != 'C' || m != 'M' || d != 'D' || b != 'B')
    {
        throw std::runtime_error("invalid CMDB file tag read from '" + reader.FileName() + "': not a CMDB debug information file");
    }
    if (format != currentDebugInfoFormat)
    {
        throw std::runtime_error("wrong CMDB file format version number: format version " + std::string(1, format) + " read, format version " +
            std::string(1, currentDebugInfoFormat) + " expected. Please rebuild project(s) from sources.");
    }
}

void WriteMainProjectName(BinaryWriter& writer, const std::string& mainProjectName)
{
    writer.Write(mainProjectName);
}

void ReadMainProjectName(BinaryReader& reader, std::string& mainProjectName)
{
    mainProjectName = reader.ReadUtf8String();
}

void WriteNumberOfProjects(BinaryWriter& writer, int32_t numProjects)
{
    writer.Write(numProjects);
}

void ReadNumberOfProjects(BinaryReader& reader, int32_t& numProjects)
{
    numProjects = reader.ReadInt();
}

void WriteProjectTableHeader(BinaryWriter& writer, const std::string& projectName, const std::string& projectDirectoryPath, int32_t numCompileUnits,
    const boost::uuids::uuid& mainFunctionId)
{
    writer.Write(projectName);
    writer.Write(projectDirectoryPath);
    writer.Write(numCompileUnits);
    writer.Write(mainFunctionId);
}

void ReadProjectTableHeader(BinaryReader& reader, std::string& projectName, std::string& projectDirectoryPath, int32_t& numCompileUnits, boost::uuids::uuid& mainFunctionId)
{
    projectName = reader.ReadUtf8String();
    projectDirectoryPath = reader.ReadUtf8String();
    numCompileUnits = reader.ReadInt();
    reader.ReadUuid(mainFunctionId);
}

void WriteProjectTableRecord(BinaryWriter& writer, const std::string& compileUnitBaseName)
{
    writer.Write(compileUnitBaseName);
}

void ReadProjectTableRecord(BinaryReader& reader, std::string& compileUnitBaseName)
{
    compileUnitBaseName = reader.ReadUtf8String();
}

void WriteNumberOfFileIndexRecords(BinaryWriter& writer, int32_t numFileIndexRecords)
{
    writer.Write(numFileIndexRecords);
}

void ReadNumberOfFileIndexRecords(BinaryReader& reader, int32_t& numFileIndexRecords)
{
    numFileIndexRecords = reader.ReadInt();
}

void WriteFileIndexRecord(BinaryWriter& writer, int32_t fileIndex, const std::string& sourceFilePath)
{
    writer.Write(fileIndex);
    writer.Write(sourceFilePath);
}

void ReadFileIndexRecord(BinaryReader& reader, int32_t& fileIndex, std::string& sourceFilePath)
{
    fileIndex = reader.ReadInt();
    sourceFilePath = reader.ReadUtf8String();
}

void WriteNumberOfCompileUnitFunctionRecords(BinaryWriter& writer, int32_t numFunctionRecords)
{
    writer.Write(numFunctionRecords);
}

void ReadNumberOfCompileUnitFunctionRecords(BinaryReader& reader, int32_t& numFunctionRecords)
{
    numFunctionRecords = reader.ReadInt();
}

void WriteCompileUnitFunctionRecord(BinaryWriter& writer, int32_t fileIndex, const boost::uuids::uuid& functionId)
{
    writer.Write(fileIndex);
    writer.Write(functionId);
}

void ReadCompileUnitFunctionRecord(BinaryReader& reader, int32_t& fileIndex, boost::uuids::uuid& functionId)
{
    fileIndex = reader.ReadInt();
    reader.ReadUuid(functionId);
}

void WriteNumberOfInstructionRecords(BinaryWriter& writer, int32_t numInstructionRecords)
{
    writer.Write(numInstructionRecords);
}

void ReadNumberOfInstructionRecords(BinaryReader& reader, int32_t& numInstructionRecords)
{
    numInstructionRecords = reader.ReadInt();
}

void WriteInstructionRecord(BinaryWriter& writer, int32_t cppLineNumber, int32_t sourceLineNumber, int32_t cppLineIndex, int16_t scopeId, int16_t flags)
{
    writer.Write(cppLineNumber);
    writer.Write(sourceLineNumber);
    writer.Write(cppLineIndex);
    writer.Write(scopeId);
    writer.Write(flags);
}

void ReadInstructionRecord(BinaryReader& reader, int32_t& cppLineNumber, int32_t& sourceLineNumber, int32_t& cppLineIndex, int16_t& scopeId, int16_t& flags)
{
    cppLineNumber = reader.ReadInt();
    sourceLineNumber = reader.ReadInt();
    cppLineIndex = reader.ReadInt();
    scopeId = reader.ReadShort();
    flags = reader.ReadShort();
}

void WriteNumberOfScopes(BinaryWriter& writer, int32_t numScopes)
{
    writer.Write(numScopes);
}

void ReadNumberOfScopes(BinaryReader& reader, int32_t& numScopes)
{
    numScopes = reader.ReadInt();
}

void WriteScopeRecord(BinaryWriter& writer, int16_t scopeId, int16_t parentScopeId, int32_t numLocalVariables)
{
    writer.Write(scopeId);
    writer.Write(parentScopeId);
    writer.Write(numLocalVariables);
}

void ReadScopeRecord(BinaryReader& reader, int16_t& scopeId, int16_t& parentScopeId, int32_t& numLocalVariables)
{
    scopeId = reader.ReadShort();
    parentScopeId = reader.ReadShort();
    numLocalVariables = reader.ReadInt();
}

void WriteNumberOfFunctionIndexFunctionRecords(BinaryWriter& writer, int32_t numFunctionIndexFunctionRecords)
{
    writer.Write(numFunctionIndexFunctionRecords);
}

void ReadNumberOfFunctionIndexFunctionRecords(BinaryReader& reader, int32_t& numFunctionIndexFunctionRecords)
{
    numFunctionIndexFunctionRecords = reader.ReadInt();
}

void WriteFunctionIndexFunctionRecord(BinaryWriter& writer, const boost::uuids::uuid& functionId, const std::string& fullFunctionName,
    const std::string& mangledFunctionName)
{
    writer.Write(functionId);
    writer.Write(fullFunctionName);
    writer.Write(mangledFunctionName);
}

void ReadFunctionIndexFunctionRecord(BinaryReader& reader, boost::uuids::uuid& functionId, std::string& fullFunctionName,
    std::string& mangledFunctionName)
{
    reader.ReadUuid(functionId);
    fullFunctionName = reader.ReadUtf8String();
    mangledFunctionName = reader.ReadUtf8String();
}

} } // namespace cmajor::debug
