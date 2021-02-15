// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_MODULE_INCLUDED
#define CMAJOR_SYMBOLS_MODULE_INCLUDED
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/Warning.hpp>
#include <cmajor/symbols/FileIndex.hpp>
#include <cmajor/symbols/FunctionIndex.hpp>
#include <cmajor/symbols/TypeIndex.hpp>
#include <cmajor/symbols/Sources.hpp>
#include <cmajor/cmdebug/Container.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#ifdef _WIN32
#include <cmajor/symbols/ResourceTable.hpp>
#endif
#include <sngcm/ast/Project.hpp>
#include <sngcm/cmlexer/CmajorLexer.hpp>
#include <cmajor/cmtoolchain/ToolChains.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <mutex>
#include <map>
#include <set>
#include <time.h>

namespace cmajor { namespace symbols {

SYMBOLS_API bool IsSystemModule(const std::u32string& moduleName);
SYMBOLS_API cmajor::debug::ContainerClassTemplateKind GetContainerClassTemplateKind(const std::u32string& fullClassName);

extern const char* cmajorModuleTag;

struct ModuleTag
{
    ModuleTag();
    void Write(SymbolWriter& writer);
    void Read(SymbolReader& reader);
    uint8_t bytes[4];
};

const uint8_t moduleFormat_1 = uint8_t('1');
const uint8_t moduleFormat_2 = uint8_t('2');
const uint8_t moduleFormat_3 = uint8_t('3');
const uint8_t moduleFormat_4 = uint8_t('4');
const uint8_t moduleFormat_5 = uint8_t('5');
const uint8_t moduleFormat_6 = uint8_t('6');
const uint8_t moduleFormat_7 = uint8_t('7');
const uint8_t moduleFormat_8 = uint8_t('8');
const uint8_t moduleFormat_9 = uint8_t('9');
const uint8_t moduleFormat_10 = uint8_t('A');
const uint8_t moduleFormat_11 = uint8_t('B');
const uint8_t moduleFormat_12 = uint8_t('C');
const uint8_t moduleFormat_13 = uint8_t('D');
const uint8_t moduleFormat_14 = uint8_t('E');
const uint8_t moduleFormat_15 = uint8_t('F');
const uint8_t moduleFormat_16 = uint8_t('G');
const uint8_t currentModuleFormat = moduleFormat_16;

enum class ModuleFlags : uint8_t
{
    none = 0, system = 1 << 0, core = 1 << 1, root = 1 << 2, immutable = 1 << 3, compiling = 1 << 4, fileIndexFilePathMapBuilt = 1 << 5, readFromModuleFile = 1 << 6
};

inline ModuleFlags operator|(ModuleFlags left, ModuleFlags right)
{
    return ModuleFlags(uint8_t(left) | uint8_t(right));
}

inline ModuleFlags operator&(ModuleFlags left, ModuleFlags right)
{
    return ModuleFlags(uint8_t(left) & uint8_t(right));
}

inline ModuleFlags operator~(ModuleFlags flags)
{
    return ModuleFlags(~uint8_t(flags));
}

std::string ModuleFlagStr(ModuleFlags flags);

class Module;
class Sources;
struct ParseResult;

class SYMBOLS_API ModuleDependency
{
public:
    ModuleDependency(Module* module_);
    Module* GetModule() const { return module; }
    void AddReferencedModule(Module* referencedModule);
    const std::vector<Module*>& ReferencedModules() const { return referencedModules; }
    void Dump(CodeFormatter& formatter);
private:
    Module* module;
    std::vector<Module*> referencedModules;
};

class SYMBOLS_API FileTable
{
public:
    int32_t RegisterFilePath(const std::string& filePath);
    std::string GetFilePath(int32_t fileIndex) const;
    int32_t NumFilePaths() const { return filePaths.size(); }
    bool IsEmpty() const { return filePaths.empty(); }
    void Write(BinaryWriter& writer, bool systemModule);
    void Read(BinaryReader& reader, bool systemModule);
    void Dump(CodeFormatter& formatter);
private:
    std::vector<std::string> filePaths;
};

class SYMBOLS_API SourceFileCache
{
public:
    SourceFileCache();
    SourceFileCache(const SourceFileCache&) = delete;
    SourceFileCache(SourceFileCache&&) = delete;
    SourceFileCache& operator=(const SourceFileCache&) = delete;
    SourceFileCache& operator=(SourceFileCache&&) = delete;
    const std::u32string& GetFileContent(const std::string& filePath);
private:
    std::unordered_map<std::string, std::unique_ptr<std::u32string>> fileContentMap;
};

SYMBOLS_API cmajor::debug::SourceSpan MakeSourceSpan(const Span& span, const boost::uuids::uuid& sourceModuleId);

class SYMBOLS_API Module
{
public:
    Module();
    Module(const std::string& filePath);
    Module(const std::string& filePath, bool readRoot);
    Module(const std::u32string& name_, const std::string& filePath_, sngcm::ast::Target target);
    ~Module();
    uint8_t Format() const { return format; }
    ModuleFlags Flags() const { return flags; }
    const std::u32string& Name() const { return name; }
    const boost::uuids::uuid& Id() const { return id; }
    const std::string& OriginalFilePath() const { return originalFilePath; }
    const std::string& FilePathReadFrom() const { return filePathReadFrom; }
    const std::string& LibraryFilePath() const { return libraryFilePath; }
    const std::string& ResourceFilePath() const { return resourceFilePath; }
    void SetResourceFilePath(const std::string& resourceFilePath_);
    const std::vector<Module*> AllReferencedModules() const { return allRefModules; }
    void PrepareForCompilation(const std::vector<std::string>& references, sngcm::ast::Target target);
    SymbolTable& GetSymbolTable() { return *symbolTable; }
    bool HasSymbolTable() const { return symbolTable != nullptr; }
    void CreateSymbolTable();
    uint32_t SymbolTablePos() const { return symbolTablePos; }
    FileTable& GetFileTable() { return fileTable; }
    void RegisterFileTable(FileTable* fileTable, Module* module);
    void MakeFilePathFileIndexMap();
#ifdef _WIN32
    ResourceTable& GetResourceTable() { return resourceTable; }
    ResourceTable& GetGlobalResourceTable() { return globalResourceTable; }
#endif
    void SetLexers(std::vector<std::unique_ptr<CmajorLexer>>&& lexers_);
    std::vector<soulng::lexer::Lexer*>* GetLexers();
    std::string GetFilePath(int32_t fileIndex) const;
    std::u32string GetErrorLines(const Span& span) const;
    void GetColumns(const Span& span, int32_t& startCol, int32_t& endCol) const;
    void Write(SymbolWriter& writer);
    void SetDirectoryPath(const std::string& directoryPath_);
    void SetObjectFileDirectoryPath(const std::string& objectFileDirectoryPath_);
    const std::string& DirectoryPath() const { return directoryPath; }
    const std::string& ObjectFileDirectoryPath() const { return objectFileDirectoryPath; }
    const std::vector<std::string>& LibraryFilePaths() const { return libraryFilePaths; }
    bool IsSystemModule () const { return GetFlag(ModuleFlags::system); }
    void SetSystemModule() { SetFlag(ModuleFlags::system); }
    bool IsRootModule() const { return GetFlag(ModuleFlags::root); }
    void SetRootModule() { SetFlag(ModuleFlags::root); }
    bool IsImmutable() const { return GetFlag(ModuleFlags::immutable); }
    void SetImmutable() { SetFlag(ModuleFlags::immutable); }
    bool IsCore() const { return GetFlag(ModuleFlags::core); }
    void SetCore() { SetFlag(ModuleFlags::core); }
    bool GetFlag(ModuleFlags flag) const { return (flags & flag) != ModuleFlags::none; }
    void SetFlag(ModuleFlags flag) { flags = flags | flag; }
    void ResetFlag(ModuleFlags flag) { flags = flags & ~flag; }
    void AddExportedFunction(const std::string& exportedFunction);
    void AddExportedData(const std::string& data);
    const std::vector<std::string>& ExportedFunctions() { return exportedFunctions; }
    const std::vector<std::string>& ExportedData() { return exportedData; }
    const std::vector<std::string>& AllExportedFunctions() const { return allExportedFunctions; }
    const std::vector<std::string>& AllExportedData() const { return allExportedData; }
    void Dump();
    ModuleDependency& GetModuleDependency() { return moduleDependency; }
    void SetCurrentProjectName(const std::u32string& currentProjectName_);
    std::u32string GetCurrentProjectName();
    void SetCurrentToolName(const std::u32string& currentToolName_);
    std::u32string GetCurrentToolName();
    CompileWarningCollection& WarningCollection();
    void ClearDefines();
    void DefineSymbol(const std::u32string& symbol);
    bool IsSymbolDefined(const std::u32string& symbol);
    void SetLogStreamId(int logStreamId_) { logStreamId = logStreamId_; }
    int LogStreamId() const { return logStreamId; }
    Module* GetSystemCoreModule();
    void Check();
    std::vector<Module*>& AllRefModules() { return allRefModules; }
    std::vector<Module*>& ReferencedModules() { return referencedModules; }
    void AddReferencedModule(Module* referencedModule);
    const std::vector<std::string>& ReferenceFilePaths() const { return referenceFilePaths; }
    void ReadHeader(sngcm::ast::Target target, SymbolReader& reader, Module* rootModule, std::unordered_set<std::string>& importSet, std::vector<Module*>& modules,
        std::unordered_map<std::string, ModuleDependency*>& moduleDependencyMap, std::unordered_map<std::string, Module*>& readMap, bool& first);
    int DebugLogIndent() const { return debugLogIndent; }
    void IncDebugLogIndent() { ++debugLogIndent; }
    void DecDebugLogIndent() { --debugLogIndent; }
    int Index() const { return index; }
    void SetIndex(int index_) { index = index_; }
    std::recursive_mutex& GetLock() { return lock; }
    void StartBuild();
    void StopBuild();
    int64_t GetBuildStartTimeMs() const { return buildStartMs; }
    int GetBuildTimeMs();
    bool Preparing() const { return preparing; }
    void SetPreparing(bool preparing_) { preparing = preparing_; }
    void AddCompileUnitId(const std::string& compileUnitId);
    const std::set<std::string>& AllCompileUnitIds() const { return allCompileUnitIds; }
    FunctionIndex& GetFunctionIndex() { return functionIndex; }
    TypeIndex& GetTypeIndex() { return typeIndex; }
    void WriteProjectDebugInfoFile(const std::string& projectDebufInfoFilePath);
    void WriteCmdbFile(const std::string& cmdbFilePath);
    void WriteDebugInfo(BinaryWriter& cmdbWriter, int32_t& numProjects, Module* rootModule);
    std::unordered_map<int16_t, std::string>* GetModuleNameTable() { return &moduleNameTable; }
    std::unordered_map<std::string, int16_t>* GetModuleIdMap() { return &moduleIdMap; }
    cmajor::debug::SourceSpan SpanToSourceSpan(const Span& span);
    int32_t GetFileIndexForFilePath(const std::string& filePath) const;
    void UpdateSourceFileModuleMap();
    std::recursive_mutex& Lock() { return lock; }
    void SetSources(Sources* sources_) { sources.reset(sources_); }
    Sources* GetSources() const { return sources.get(); }
    ParseResult ParseSources();
    ParseResult ParseSource(const std::string& sourceFilePath, const std::u32string& sourceCode);
    std::string GetCCList(const std::string& sourceFilePath, const std::string& ccText);
    std::string GetOverloadList(const std::string& functionGroup);
private:
    uint8_t format;
    ModuleFlags flags;
    std::u32string name;
    boost::uuids::uuid id;
    sngcm::ast::BackEnd backend;
    sngcm::ast::Config config;
    std::string originalFilePath;
    std::string filePathReadFrom;
    std::string libraryFilePath;
    std::string resourceFilePath;
    std::vector<std::string> referenceFilePaths;
    FileTable fileTable;
    std::set<std::string> compileUnitIds;
    std::set<std::string> allCompileUnitIds;
#ifdef _WIN32
    ResourceTable resourceTable;
    ResourceTable globalResourceTable;
#endif
    std::vector<FileTable*> fileTables;
    std::unordered_map<std::string, int32_t> filePathFileIndexMap;
    std::vector<std::unique_ptr<CmajorLexer>> lexers;
    std::vector<soulng::lexer::Lexer*> lexerVec;
    std::unordered_map<int16_t, std::string> moduleNameTable;
    std::unordered_map<std::string, int16_t> moduleIdMap;
    std::vector<std::string> exportedFunctions;
    std::vector<std::string> exportedData;
    std::vector<std::string> allExportedFunctions;
    std::vector<std::string> allExportedData;
    ModuleDependency moduleDependency;
    std::vector<Module*> referencedModules;
    std::vector<Module*> allRefModules;
    uint32_t symbolTablePos;
    std::unique_ptr<SymbolTable> symbolTable;
    std::string directoryPath;
    std::string objectFileDirectoryPath;
    std::vector<std::string> libraryFilePaths;
    std::u32string currentProjectName;
    std::u32string currentToolName;
    CompileWarningCollection warnings;
    std::set<std::u32string> defines;
    int logStreamId;
    bool headerRead;
    int debugLogIndent;
    Module* systemCoreModule;
    int index;
    bool preparing;
    std::recursive_mutex lock;
    int64_t buildStartMs;
    int64_t buildStopMs;
    FunctionIndex functionIndex;
    TypeIndex typeIndex;
    SourceFileCache sourceFileCache;
    std::unique_ptr<Sources> sources;
    void CheckUpToDate();
};

SYMBOLS_API std::string GetSourceFilePath(int32_t fileIndex, const boost::uuids::uuid& moduleId);
SYMBOLS_API bool HasRootModuleForCurrentThread();
SYMBOLS_API Module* GetRootModuleForCurrentThread();
SYMBOLS_API void SetRootModuleForCurrentThread(Module* rootModule_);

SYMBOLS_API void InitModule();
SYMBOLS_API void DoneModule();

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_MODULE_INCLUDED
