// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSX_FUNCTION_INCLUDED
#define CMAJOR_CMSX_FUNCTION_INCLUDED
#include <cmajor/cmsystemx/BasicBlock.hpp>
#include <cmajor/cmsystemx/Type.hpp>

namespace cmsystemx {

const int maxLocalVariableReg = 128;

class CMSYSTEMX_API Function : public Value
{
public:
    Function(Context& context, const std::string& mangledName_, FunctionType* type_);
    Function(const Function&) = delete;
    Function& operator=(const Function&) = delete;
    void Write(std::ostream& s) override;
    Type* GetType(Context& context) override;
    void WriteDeclaration(std::ostream& s);
    void WriteDefinition(Context& context, std::ostream& s);
    const std::string& MangledName() const { return mangledName; }
    FunctionType* GetType() const { return type; }
    BasicBlock* CreateBasicBlock(const std::string& name);
    LocalVariable* CreateLocalVariable(Type* type, Context& context);
    LocalRegister* GetReg(Type* type);
    bool Defined() const;
    void CreateProlog(Context& context);
    void SetLinkOnce() { linkOnce = true; }
    Value* GetArgument(int argumentIndex);
    Value* CreateLoad(Value* ptr, Context& context);
    void CreateStore(Value* value, Value* ptr, Context& context);
    Value* CreateCall(Value* callee, const std::vector<Value*>& args, Context& context);
    Value* CreateCallInst(int numLocalRegs, Context& context);
    void IncFrameOffset(Type* type);
private:
    std::string mangledName;
    FunctionType* type;
    std::vector<std::unique_ptr<BasicBlock>> basicBlocks;
    int nextBasicBlockId;
    std::vector<std::unique_ptr<Argument>> arguments;
    std::vector<std::unique_ptr<FrameLocation>> frameArguments;
    std::vector<std::unique_ptr<FrameLocation>> frameLocalVars;
    std::vector<std::unique_ptr<LocalVariable>> localVariables;
    std::vector<std::unique_ptr<LocalRegister>> localRegisters;
    int nextLocalRegNumber;
    uint64_t frameOffset;
    bool linkOnce;
};

} // namespace cmsystemx

#endif // CMAJOR_CMSX_FUNCTION_INCLUDED
