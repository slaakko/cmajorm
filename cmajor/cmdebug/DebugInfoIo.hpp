// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_INFO_IO_INCLUDED
#define CMAJOR_DEBUG_INFO_IO_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#include <soulng/util/BinaryReader.hpp>
#include <soulng/util/BinaryWriter.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>

namespace cmajor { namespace debug {

using namespace soulng::util;

const uint8_t debugInfoFormat_1 = uint8_t('1');
const uint8_t debugInfoFormat_2 = uint8_t('2');
const uint8_t currentDebugInfoFormat = debugInfoFormat_2;

DEBUG_API void WriteCmdbFileTag(BinaryWriter& writer);
DEBUG_API void ReadCmdbFileTag(BinaryReader& reader);
DEBUG_API void WriteMainProjectName(BinaryWriter& writer, const std::string& mainProjectName);
DEBUG_API void ReadMainProjectName(BinaryReader& reader, std::string& mainProjectName);
DEBUG_API void WriteNumberOfProjects(BinaryWriter& writer, int32_t numProjects);
DEBUG_API void ReadNumberOfProjects(BinaryReader& reader, int32_t& numProjects);
DEBUG_API void WriteProjectTableHeader(BinaryWriter& writer, const std::string& projectName, const std::string& projectDirectoryPath, const boost::uuids::uuid& moduleId, int32_t numCompileUnits,
    const boost::uuids::uuid& mainFunctionId);
DEBUG_API void ReadProjectTableHeader(BinaryReader& reader, std::string& projectName, std::string& projectDirectoryPath, boost::uuids::uuid& moduleId, int32_t& numCompileUnits,
    boost::uuids::uuid& mainFunctionId);
DEBUG_API void WriteProjectTableRecord(BinaryWriter& writer, const std::string& compileUnitBaseName);
DEBUG_API void ReadProjectTableRecord(BinaryReader& reader, std::string& compileUnitBaseName);
DEBUG_API void WriteNumberOfCompileUnitFunctionRecords(BinaryWriter& writer, int32_t numFunctionRecords);
DEBUG_API void ReadNumberOfCompileUnitFunctionRecords(BinaryReader& reader, int32_t& numFunctionRecords);
DEBUG_API void WriteCompileUnitFunctionRecord(BinaryWriter& writer, int32_t fileIndex, const boost::uuids::uuid& sourceModuleId, const boost::uuids::uuid& functionId);
DEBUG_API void ReadCompileUnitFunctionRecord(BinaryReader& reader, int32_t& fileIndex, boost::uuids::uuid& sourceModuleId, boost::uuids::uuid& functionId);
DEBUG_API void WriteNumberOfInstructionRecords(BinaryWriter& writer, int32_t numInstructionRecords);
DEBUG_API void ReadNumberOfInstructionRecords(BinaryReader& reader, int32_t& numInstructionRecods);
DEBUG_API void WriteInstructionRecord(BinaryWriter& writer, int32_t cppLineNumber, const SourceSpan& span, int32_t cppLineIndex, int16_t scopeId, int16_t flags);
DEBUG_API void ReadInstructionRecord(BinaryReader& reader, int32_t& cppLineNumber, SourceSpan& span, int32_t& cppLineIndex, int16_t& scopeId, int16_t& flags);
DEBUG_API void WriteControlFlowGraphNodeCount(BinaryWriter& writer, int32_t nodeCount);
DEBUG_API void ReadControlFlowGraphNodeCount(BinaryReader& reader, int32_t& nodeCount);
DEBUG_API void WriteControlFlowGraphNode(BinaryWriter& writer, int32_t nodeId, const SourceSpan& span, int32_t cppLineIndex, int32_t cppLineNumber);
DEBUG_API void ReadControlFlowGraphNode(BinaryReader& reader, int32_t& nodeId, SourceSpan& span, int32_t& cppLineIndex, int32_t& cppLineNumber);
DEBUG_API void WriteControlFlowGraphNodeEdgeCount(BinaryWriter& writer, int32_t edgeCount);
DEBUG_API void ReadControlFlowGraphNodeEdgeCount(BinaryReader& reader, int32_t& edgeCount);
DEBUG_API void WriteControlFlowGraphNodeEdge(BinaryWriter& writer, int32_t endNodeId);
DEBUG_API void ReadControlFlowGraphNodeEdge(BinaryReader& reader, int32_t& endNodeId);
DEBUG_API void WriteNumberOfScopes(BinaryWriter& writer, int32_t numScopes);
DEBUG_API void ReadNumberOfScopes(BinaryReader& reader, int32_t& numScopes);
DEBUG_API void WriteScopeRecord(BinaryWriter& writer, int16_t scopeId, int16_t parentScopeId, int32_t numLocalVariables);
DEBUG_API void ReadScopeRecord(BinaryReader& reader, int16_t& scopeId, int16_t& parentScopeId, int32_t& numLocalVariables);
DEBUG_API void WriteNumberOfFileIndexRecords(BinaryWriter& writer, int32_t numFileIndexRecords);
DEBUG_API void ReadNumberOfFileIndexRecords(BinaryReader& reader, int32_t& numFileIndexRecords);
DEBUG_API void WriteFileIndexRecord(BinaryWriter& writer, int32_t fileIndex, const std::string& sourceFilePath);
DEBUG_API void ReadFileIndexRecord(BinaryReader& reader, int32_t& fileIndex, std::string& sourceFilePath);
DEBUG_API void WriteNumberOfFunctionIndexFunctionRecords(BinaryWriter& writer, int32_t numFunctionIndexFunctionRecords);
DEBUG_API void ReadNumberOfFunctionIndexFunctionRecords(BinaryReader& reader, int32_t& numFunctionIndexFunctionRecords);
DEBUG_API void WriteFunctionIndexFunctionRecord(BinaryWriter& writer, const boost::uuids::uuid& functionId, const std::string& fullFunctionName,
    const std::string& mangledFunctionName);
DEBUG_API void ReadFunctionIndexFunctionRecord(BinaryReader& reader, boost::uuids::uuid& functionId, std::string& fullFunctionName,
    std::string& mangledFunctionName);

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_INFO_IO_INCLUDED
