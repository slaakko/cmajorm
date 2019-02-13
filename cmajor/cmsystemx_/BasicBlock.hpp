// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSX_BASIC_BLOCK_INCLUDED
#define CMAJOR_CMSX_BASIC_BLOCK_INCLUDED
#include <cmajor/cmsystemx/Instruction.hpp>
#include <string>

namespace cmsystemx {

class Context;
class Function;

class CMSYSTEMX_API BasicBlock
{
public:
    BasicBlock(int id_, const std::string& name_, Function* function_);
    BasicBlock(const BasicBlock&) = delete;
    BasicBlock& operator=(const BasicBlock&) = delete;
    void Write(std::ostream& s);
    Instruction* CreateRetVoid(Context& context);
    Instruction* CreateRet(Value* value, Context& context);
    Instruction* CreateInstruction(int opCode);
    void InsertFront(Instruction* instruction);
    Function* GetFunction() { return function; }
private:
    Function* function;
    int id;
    std::string name;
    std::vector<std::unique_ptr<Instruction>> instructions;
};

} // namesapce cmsystemx

#endif // CMAJOR_CMSX_BASIC_BLOCK_INCLUDED
