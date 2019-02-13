// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_FUNCTION_INCLUDED
#define CMAJOR_CMSXI_FUNCTION_INCLUDED
#include <cmajor/cmsxi/Type.hpp>
#include <cmajor/cmsxi/BasicBlock.hpp>

namespace cmsxi {

class CMSXI_API Function : public Value
{
public:
    Function(const std::string& name_, FunctionType* type_);
    Function(const Function&) = delete;
    Function& operator=(const Function&) = delete;
    BasicBlock* CreateBasicBlock();
    uint64_t GetNextResultNumber() { return nextResultNumber++; }
    Type* GetType(Context& context) override { return type; }
    Value* GetParam(int index) const;
    std::string Name() override { return "@" + name; }
    void Write(CodeFormatter& formatter, Context& context);
private:
    std::string name;
    FunctionType* type;
    std::vector<Instruction*> params;
    std::unique_ptr<BasicBlock> entryBlock;
    std::vector<std::unique_ptr<BasicBlock>> basicBlocks;
    uint64_t nextResultNumber;
};

} // namespace cmsxi

#endif // CMAJOR_CMSXI_FUNCTION_INCLUDED
