// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_INFO_INCLUDED
#define CMAJOR_DEBUG_INFO_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <cmajor/cmdebug/DIVariable.hpp>
#include <cmajor/cmdebug/DIType.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Json.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <map>
#include <set>

namespace cmajor { namespace debug {

using soulng::util::CodeFormatter;
using soulng::util::JsonValue;

class CompileUnitFunction;
class Scope;
class SourceFileReference;
class DebuggerOutputWriter;

struct DEBUG_API SourceSpan
{
    SourceSpan() : line(0), scol(0), ecol(0) {}
    SourceSpan(int32_t line_, int16_t scol_, int16_t ecol_) : line(line_), scol(scol_), ecol(ecol_) {}
    std::string ToString() const;
    int32_t line;
    int16_t scol;
    int16_t ecol;
};

DEBUG_API inline bool operator==(const SourceSpan& left, const SourceSpan& right)
{
    return left.line == right.line && left.scol == right.scol && left.ecol == right.ecol;
}

struct DEBUG_API Frame
{
    Frame();
    std::string ToString(bool printLevel) const;
    std::unique_ptr<JsonValue> ToJson(bool includeLevel) const;
    bool IsEmpty() const { return func.empty() && file.empty() && line == 0; }
    int level;
    std::string func;
    std::string file;
    int line;
    int scol;
    int ecol;
};

struct DEBUG_API InstructionLocation
{
    InstructionLocation();
    SourceSpan span;
    int32_t projectIndex;
    int32_t compileUnitIndex;
    int32_t cppLineNumber;
};

enum class InstructionFlags : int16_t
{
    none = 0,
    entryCode = 1 << 0,
    exitCode = 1 << 1,
    beginBrace = 1 << 2,
    endBrace = 1 << 3,
    startFunction = 1 << 4,
    throwInst = 1 << 5,
    catchInst = 1 << 6
};

DEBUG_API inline constexpr InstructionFlags operator|(InstructionFlags left, InstructionFlags right)
{
    return InstructionFlags(int16_t(left) | int16_t(right));
}

DEBUG_API inline constexpr InstructionFlags operator&(InstructionFlags left, InstructionFlags right)
{
    return InstructionFlags(int16_t(left) & int16_t(right));
}

DEBUG_API inline constexpr InstructionFlags operator~(InstructionFlags operand)
{
    return InstructionFlags(~int16_t(operand));
}

DEBUG_API std::string InstructionFlagsStr(InstructionFlags flags);

class DEBUG_API Instruction
{
public:
    Instruction(CompileUnitFunction* compileUnitFunction_, int32_t cppLineNumber_, const SourceSpan& span_, int32_t cppLineIndex_, int16_t scopeId_, InstructionFlags flags_);
    CompileUnitFunction* GetCompileUnitFunction() const { return compileUnitFunction; }
    SourceFileReference* GetSourceFileReference() const;
    Scope* GetScope() const;
    int32_t CppLineNumber() const { return cppLineNumber; }
    const SourceSpan& GetSourceSpan() const { return span; }
    int32_t CppLineIndex() const { return cppLineIndex; }
    int16_t ScopeId() const { return scopeId; }
    InstructionFlags GetFlags() const { return flags; }
    std::string GetExplicitCppLocationArgs() const;
    std::string GetFileLineCppLocationArgs() const;
    Frame GetCmajorFrame() const;
    void PrintSource(CodeFormatter& formatter);
    Instruction* Next() const { return next; }
    void SetNext(Instruction* next_) { next = next_; }
private:
    CompileUnitFunction* compileUnitFunction;
    int32_t cppLineNumber;
    SourceSpan span;
    int32_t cppLineIndex;
    int16_t scopeId;
    InstructionFlags flags;
    Instruction* next;
};

class DEBUG_API ControlFlowGraphNode
{
public:
    ControlFlowGraphNode(int32_t nodeId_, const SourceSpan& span_, int32_t cppLineIndex_, int32_t cppLineNumber_);
    int32_t NodeId() const { return nodeId; }
    const SourceSpan& GetSourceSpan() const { return span; }
    int32_t CppLineIndex() const { return cppLineIndex; }
    int32_t CppLineNumber() const { return cppLineNumber; }
    const std::set<int32_t>& Next() const { return nextSet; }
    void AddNext(int32_t next);
    void SetInst(Instruction* inst_) { inst = inst_; }
    Instruction* Inst() const { return inst; }
private:
    int32_t nodeId;
    SourceSpan span;
    int32_t cppLineIndex;
    int32_t cppLineNumber;
    std::set<int32_t> nextSet;
    Instruction* inst;
};

class DEBUG_API ControlFlowGraph
{
public:
    ControlFlowGraph();
    ControlFlowGraph(const ControlFlowGraph&) = delete;
    ControlFlowGraph(ControlFlowGraph&&) = delete;
    ControlFlowGraph& operator=(const ControlFlowGraph&) = delete;
    ControlFlowGraph& operator=(ControlFlowGraph&&) = delete;
    void AddNode(ControlFlowGraphNode* node);
    ControlFlowGraphNode* GetNodeById(int32_t nodeId) const;
    ControlFlowGraphNode* GetNodeByCppLineNumber(int32_t cppLineNumber) const;
private:
    std::unordered_map<int32_t, ControlFlowGraphNode*> nodeIdMap;
    std::unordered_map<int32_t, ControlFlowGraphNode*> cppLineNodeMap;
    std::vector<std::unique_ptr<ControlFlowGraphNode>> nodes;
};

class DEBUG_API FunctionScope : public Scope
{
public:
    FunctionScope(CompileUnitFunction* compileUnitFunction_, int16_t id_, int16_t parentScopeId_);
    FunctionScope(const FunctionScope&) = delete;
    FunctionScope(FunctionScope&&) = delete;
    FunctionScope& operator=(const FunctionScope&) = delete;
    FunctionScope& operator=(FunctionScope&&) = delete;
    std::string Name() const override;
    int16_t Id() const { return id; }
    int16_t ParentScopeId() const { return parentScopeId; }
    CompileUnitFunction* GetCompileUnitFunction() const { return compileUnitFunction; }
    Scope* GetParentScope() const;
    void AddLocalVariable(DIVariable* localVariable);
    DIVariable* GetVariable(const std::string& name) const override;
    const std::vector<std::unique_ptr<DIVariable>>& LocalVariables() const { return localVariables; }
private:
    CompileUnitFunction* compileUnitFunction;
    int16_t id;
    int16_t parentScopeId;
    std::vector<std::unique_ptr<DIVariable>> localVariables;
    std::unordered_map<std::string, DIVariable*> localVariableMap;
};

class CompileUnit;
class SourceFile;
class Function;
class SourceFileReference;

struct DEBUG_API SourceFileKey
{
    SourceFileKey(int32_t fileIndex_, const boost::uuids::uuid& sourceModuleId_) : fileIndex(fileIndex_), sourceModuleId(sourceModuleId_) {}
    int32_t fileIndex;
    boost::uuids::uuid sourceModuleId;
};

bool operator==(const SourceFileKey& left, const SourceFileKey& right);
bool operator<(const SourceFileKey& left, const SourceFileKey& right);

class DEBUG_API CompileUnitFunction
{
public:
    CompileUnitFunction(CompileUnit* compileUnit_, int32_t fileIndex_, const boost::uuids::uuid& sourceModuleId_, const boost::uuids::uuid& functionId_);
    CompileUnitFunction(const CompileUnitFunction&) = delete;
    CompileUnitFunction(CompileUnitFunction&&) = delete;
    CompileUnitFunction& operator=(const CompileUnitFunction&) = delete;
    CompileUnitFunction& operator=(CompileUnitFunction&&) = delete;
    CompileUnit* GetCompileUnit() const { return compileUnit; }
    SourceFileReference* GetSourceFileReference() const;
    SourceFile& GetSourceFile() const;
    Function* GetFunction() const;
    int32_t FileIndex() const { return fileIndex; }
    const boost::uuids::uuid& FunctionId() const { return functionId; }
    void AddInstruction(Instruction* instruction);
    Instruction* GetInstruction(int index) const;
    void AddScope(FunctionScope* scope);
    const std::vector<std::unique_ptr<Instruction>>& Instructions() const { return instructions; }
    const std::vector<std::unique_ptr<FunctionScope>>& Scopes() const { return scopes; }
    ControlFlowGraph& GetControlFlowGraph() { return controlFlowGraph; }
    const std::vector<DIVariable*>& LocalVariables() const { return localVariables; }
    std::vector<DIVariable*>& LocalVariables() { return localVariables; }
    void AddLocalVariable(DIVariable* localVariable);
private:
    CompileUnit* compileUnit;
    int32_t fileIndex;
    boost::uuids::uuid sourceModuleId;
    boost::uuids::uuid functionId;
    std::vector<std::unique_ptr<Instruction>> instructions;
    std::vector<std::unique_ptr<FunctionScope>> scopes;
    ControlFlowGraph controlFlowGraph;
    std::vector<DIVariable*> localVariables;
};

class Project;

class DEBUG_API CompileUnit
{
public:
    CompileUnit(Project* project_, const std::string& baseName_);
    CompileUnit(const CompileUnit&) = delete;
    CompileUnit(CompileUnit&&) = delete;
    CompileUnit& operator=(const CompileUnit&) = delete;
    CompileUnit& operator=(CompileUnit&&) = delete;
    Project* GetProject() const { return project; }
    const std::string& BaseName() const { return baseName; }
    void AddCompileUnitFunction(CompileUnitFunction* compileUnitFunction);
    const std::vector<std::unique_ptr<CompileUnitFunction>>& CompileUnitFunctions() const { return compileUnitFunctions; }
    void AddInstruction(Instruction* instruction);
    Instruction* GetInstruction(int32_t cppLineNumber) const;
private:
    Project* project;
    std::string baseName;
    std::vector<std::unique_ptr<CompileUnitFunction>> compileUnitFunctions;
    std::unordered_map<int32_t, Instruction*> instructionMap;
};

class DEBUG_API SourceFileReference
{
public:
    SourceFileReference(int32_t fileIndex_, const std::string& filePath_);
    int32_t FileIndex() const { return fileIndex; }
    const std::string& FilePath() const { return filePath; }
private:
    int32_t fileIndex;
    std::string filePath;
};

class DEBUG_API Function
{
public:
    Function(const boost::uuids::uuid& id_, const std::string& fullName_, const std::string& mangledName_);
    const boost::uuids::uuid& Id() const { return id; }
    const std::string& FullName() const { return fullName; }
    const std::string& MangledName() const { return mangledName; }
private:
    boost::uuids::uuid id;
    std::string fullName;
    std::string mangledName;
};

class DebugInfo;

class DEBUG_API Project
{
public:
    Project(DebugInfo* debugInfo_, const std::string& name_, const std::string& directoryPath_, const boost::uuids::uuid& moduleId_);
    Project(const Project&) = delete;
    Project(Project&&) = delete;
    Project& operator=(const Project&) = delete;
    Project& operator=(Project&&) = delete;
    const std::string& Name() const { return name; }
    const std::string& DirectoryPath() const { return directoryPath; }
    void AddCompileUnit(CompileUnit* compileUnit);
    CompileUnit* GetCompileUnit(const std::string& baseName) const;
    CompileUnit* GetCompileUnit(int32_t compileUnitIndex) const;
    void AddSourceFileReference(SourceFileReference* sourceFileReference);
    SourceFileReference* GetSourceFileReference(int32_t fileIndex) const;
    SourceFile& GetSourceFile(int32_t fileIndex) const;
    void AddFunction(Function* function);
    Function* GetFunction(const boost::uuids::uuid& functionId) const;
    void SetMainFunction(CompileUnitFunction* mainFunction_) { mainFunction = mainFunction_; }
    CompileUnitFunction* GetMainFunction() const;
    void AddType(DIType* type);
    DIType* GetType(const boost::uuids::uuid& typeId) const;
    DIPointerType* GetPointerType(DIType* pointedToType);
    DIReferenceType* GetReferenceType(DIType* referredToType);
    DIConstType* GetConstType(DIType* baseType);
    DIType* GetLongType() const { return longType; }
    DIType* GetBoolType() const { return boolType; }
    DebugInfo* GetDebugInfo() const { return debugInfo; }
    const boost::uuids::uuid& ModuleId() const { return moduleId; }
private:
    DebugInfo* debugInfo;
    std::string name;
    std::string directoryPath;
    boost::uuids::uuid moduleId;
    std::vector<std::unique_ptr<CompileUnit>> compileUnits;
    std::unordered_map<std::string, CompileUnit*> compileUnitMap;
    std::vector<std::unique_ptr<SourceFileReference>> sourceFileReferences;
    std::unordered_map<int32_t, SourceFileReference*> sourceFileReferenceMap;
    std::vector<std::unique_ptr<Function>> functions;
    std::unordered_map<boost::uuids::uuid, Function*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<DIType>> types;
    std::unordered_map<boost::uuids::uuid, DIType*, boost::hash<boost::uuids::uuid>> typeMap;
    std::unordered_map<boost::uuids::uuid, DIPointerType*, boost::hash<boost::uuids::uuid>> pointerTypeMap;
    std::unordered_map<boost::uuids::uuid, DIReferenceType*, boost::hash<boost::uuids::uuid>> referenceTypeMap;
    std::unordered_map<boost::uuids::uuid, DIConstType*, boost::hash<boost::uuids::uuid>> constTypeMap;
    CompileUnitFunction* mainFunction;
    DIType* longType;
    DIType* boolType;
};

class DEBUG_API SourceFile
{
public:
    SourceFile(DebugInfo* debugInfo_, const std::string& filePath_);
    const std::string& FilePath() const { return filePath; }
    const std::vector<std::string>& Lines() const { return lines; }
    void Print(CodeFormatter& formatter, int lineNumber, Instruction* currentInstruction, bool window);
private:
    DebugInfo* debugInfo;
    std::string filePath;
    std::vector<std::string> lines;
};

class DEBUG_API SourceFileCache
{
public:
    SourceFileCache(DebugInfo* debugInfo_);
    SourceFileCache(const SourceFileCache&) = delete;
    SourceFileCache(SourceFileCache&&) = delete;
    SourceFileCache& operator=(const SourceFileCache&) = delete;
    SourceFileCache& operator=(SourceFileCache&&) = delete;
    SourceFile& GetSourceFile(const std::string& filePath);
private:
    DebugInfo* debugInfo;
    std::vector<std::unique_ptr<SourceFile>> sourceFiles;
    std::unordered_map<std::string, SourceFile*> sourceFileMap;
};

class DEBUG_API SourceLineMap
{
public:
    SourceLineMap(DebugInfo* debugInfo_);
    std::vector<Instruction*> GetInstructions(int32_t sourceLineNumber) const;
    void AddInstructionLocation(const InstructionLocation& location);
private:
    DebugInfo* debugInfo;
    std::map<int32_t, std::vector<InstructionLocation>> sourceLineLocationsMap;
};

struct DEBUG_API SourceLocation
{
    SourceLocation();
    SourceLocation(const std::string& path_, int line_);
    std::string ToString() const;
    bool IsEmpty() const { return path.empty() && line == 0; }
    std::string path;
    int line;
};

class DEBUG_API SourceFileMap
{
public:
    SourceFileMap(DebugInfo* debugInfo_);
    SourceFileMap(const SourceFileMap&) = delete;
    SourceFileMap(SourceFileMap&&) = delete;
    SourceFileMap& operator=(const SourceFileMap&) = delete;
    SourceFileMap& operator=(SourceFileMap&&) = delete;
    std::vector<Instruction*> GetInstructions(const SourceLocation& location) const;
    std::string GetSourceFilePath(const SourceLocation& location) const;
    void AddInstructionLocation(const SourceFileKey& sourceFileKey, const InstructionLocation& location);
    void AddSourceFile(const SourceFileKey& sourceFileKey, const std::string& sourceFilePath);
    void ProcessSourceFileKeyLocationsMap();
private:
    DebugInfo* debugInfo;
    std::map<std::string, SourceLineMap*> sourceFileMap;
    std::vector<std::unique_ptr<SourceLineMap>> sourceLineMaps;
    std::map<SourceFileKey, std::string> sourceFileKeyMap;
    std::map<SourceFileKey, std::vector<InstructionLocation>> sourceFileKeyLocationsMap;
    void AddInstructionLocations(const SourceFileKey& sourceFileKey, const std::string& sourceFilePath);
};

class DEBUG_API DebugInfo
{
public:
    DebugInfo(const std::string& filePath_);
    DebugInfo(const DebugInfo&) = delete;
    DebugInfo(DebugInfo&&) = delete;
    DebugInfo& operator=(const DebugInfo&) = delete;
    DebugInfo& operator=(DebugInfo&&) = delete;
    void SetMainProject(Project* mainProject_);
    Project* GetMainProject() const;
    void AddProject(Project* project);
    Project* GetProjectByPath(const std::string& directoryPath) const;
    Project* GetProjectByName(const std::string& projectName) const;
    Project* GetProjectById(const boost::uuids::uuid& projectId) const;
    SourceFileCache& GetSourceFileCache() { return sourceFileCache; }
    SourceFileMap& GetSourceFileMap() { return sourceFileMap; }
    int GetSourceFileWindowSize() const { return sourceFileWindowSize; }
    void SetSourceFileWindowSize(int sourceFileWindowSize_) { sourceFileWindowSize = sourceFileWindowSize_; }
    Instruction* GetEntryInstruction(CompileUnitFunction* compileUnitFunction) const;
    Instruction* GetMainFunctionEntryInstruction() const;
    Instruction* GetInstruction(const Frame& frame, DebuggerOutputWriter& outputWriter) const;
    Instruction* GetInstruction(const InstructionLocation& location) const;
    DIType* GetPolymorphicType(const std::string& vmtVarName) const;
    void AddPolymorphicType(DIClassType* polymorphicType);
    const std::string& FilePath() const { return filePath; }
    DIType* GetType(const std::string& typeId) const;
    void AddType(DIType* type);
    void AddThrowInstruction(Instruction* throwInstruction);
    const std::vector<Instruction*>& GetThrowInstructions() const { return throwInstructions; }
    void AddCatchInstruction(Instruction* catchInstruction);
    const std::vector<Instruction*>& GetCatchInstructions() const { return catchInstructions; }
    void ProcessSourceFileKeyLocationsMap();
private:
    std::string filePath;
    std::vector<std::unique_ptr<Project>> projects;
    std::unordered_map<std::string, Project*> projectPathMap;
    std::unordered_map<std::string, Project*> projectNameMap;
    std::unordered_map<boost::uuids::uuid, Project*, boost::hash<boost::uuids::uuid>> projectIdMap;
    std::unordered_map<std::string, DIType*> polymorphicTypeMap;
    std::unordered_map<std::string, DIType*> typeMap;
    Project* mainProject;
    SourceFileCache sourceFileCache;
    SourceFileMap sourceFileMap;
    int sourceFileWindowSize;
    std::vector<Instruction*> throwInstructions;
    std::vector<Instruction*> catchInstructions;
};

std::unique_ptr<DebugInfo> ReadDebugInfo(const std::string& cmdbFilePath);

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_INFO_INCLUDED
