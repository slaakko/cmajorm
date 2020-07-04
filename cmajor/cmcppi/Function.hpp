// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPI_FUNCTION_INCLUDED
#define CMAJOR_CMCPPI_FUNCTION_INCLUDED
#include <cmajor/cmcppi/Type.hpp>
#include <cmajor/cmcppi/BasicBlock.hpp>
#include <cmajor/cmcppi/Scope.hpp>
#include <soulng/util/BinaryWriter.hpp>

namespace cmcppi {

class Context;

class CMCPPI_API Function : public Value
{
public:
    Function(const std::string& name_, FunctionType* type_, Context& context);
    Function(const Function&) = delete;
    Function& operator=(const Function&) = delete;
    BasicBlock* CreateBasicBlock(const std::string& name, Context& context);
    BasicBlock* CreateCleanupBasicBlock(Context& context);
    const std::vector<std::unique_ptr<BasicBlock>>& BasicBlocks() const { return basicBlocks; }
    void Finalize(bool hasCleanup);
    uint64_t GetNextResultNumber() { return nextResultNumber++; }
    uint64_t GetNextLocalNumber() { return nextLocalNumber++; }
    uint64_t GetNextArgumentNumber() { return nextArgumentNumber++; }
    Type* GetType(Context& context) override { return type; }
    Value* GetParam(int index) const;
    std::string Name(Context& context) override { return name; }
    void SetFullName(const std::string& functionName);
    void WriteDeclaration(CodeFormatter& formatter, Context& context);
    void Write(CodeFormatter& formatter, Context& context, BinaryWriter& writer, int32_t& numFunctions);
    void WriteValueDeclarations(CodeFormatter& formatter, Context& context);
    void SetLinkOnce() { linkOnce = true; }
    void RemoveUnreferencedBasicBlocks();
    void SetNothrow() { nothrow = true; }
    void RemoveNothrow() { nothrow = false; }
    void AddResultInstruction(Instruction* instruction);
    void SetFileIndex(int32_t fileIndex_);
    int32_t FileIndex() const { return fileIndex; }
    void SetFunctionId(const boost::uuids::uuid& functionId_);
    void AddScope(Scope* scope);
    const std::vector<std::unique_ptr<Scope>>& Scopes() const { return scopes; }
    Scope* GetScope(int16_t scopeId);
    bool NopResultDeclarationWritten() const { return nopResultDeclarationWritten; }
    void SetNopResultDeclarationWritten() { nopResultDeclarationWritten = true; }
private:
    std::string name;
    std::string fullName;
    FunctionType* type;
    std::vector<std::unique_ptr<ParamInstruction>> params;
    std::unique_ptr<BasicBlock> entryBlock;
    std::vector<std::unique_ptr<BasicBlock>> basicBlocks;
    std::vector<std::unique_ptr<BasicBlock>> cleanupBasicBlocks;
    std::vector<Instruction*> resultInstructions;
    uint64_t nextResultNumber;
    uint64_t nextLocalNumber;
    uint64_t nextArgumentNumber;
    bool linkOnce;
    int nextBBNumber;
    bool nothrow;
    int32_t fileIndex;
    boost::uuids::uuid functionId;
    std::vector<std::unique_ptr<Scope>> scopes;
    bool nopResultDeclarationWritten;
};

} // namespace cmcppi

#endif // CMAJOR_CMCPPI_FUNCTION_INCLUDED

