// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_WRITER_INCLUDED
#define CMSX_INTERMEDIATE_WRITER_INCLUDED
#include <system-x/intermediate/Visitor.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <fstream>

namespace cmsx::intermediate {

using namespace soulng::util;

class CMSX_INTERMEDIATE_API Writer : public Visitor
{
public:
    Writer(Context* context_, const std::string& fileName);
    Writer(Context* context_, CodeFormatter& formatter_);
    void SetNewLines(bool newLines_) { newLines = newLines_; }
    CodeFormatter& Formatter() { return formatter; }
    void Visit(Function& function) override;
    void Visit(BasicBlock& basicBlock) override;
    void Visit(StoreInstruction& inst) override;
    void Visit(ArgInstruction& inst) override;
    void Visit(JmpInstruction& inst) override;
    void Visit(BranchInstruction& inst) override;
    void Visit(ProcedureCallInstruction& inst) override;
    void Visit(RetInstruction& inst) override;
    void Visit(SwitchInstruction& inst) override;
    void Visit(NotInstruction& inst) override;
    void Visit(NegInstruction& inst) override;
    void Visit(SignExtendInstruction& inst) override;
    void Visit(ZeroExtendInstruction& inst) override;
    void Visit(TruncateInstruction& inst) override;
    void Visit(BitcastInstruction& inst) override;
    void Visit(IntToFloatInstruction& inst) override;
    void Visit(FloatToIntInstruction& inst) override;
    void Visit(IntToPtrInstruction& inst) override;
    void Visit(PtrToIntInstruction& inst) override;
    void Visit(AddInstruction& inst) override;
    void Visit(SubInstruction& inst) override;
    void Visit(MulInstruction& inst) override;
    void Visit(DivInstruction& inst) override;
    void Visit(ModInstruction& inst) override;
    void Visit(AndInstruction& inst) override;
    void Visit(OrInstruction& inst) override;
    void Visit(XorInstruction& inst) override;
    void Visit(ShlInstruction& inst) override;
    void Visit(ShrInstruction& inst) override;
    void Visit(EqualInstruction& inst) override;
    void Visit(LessInstruction& inst) override;
    void Visit(ParamInstruction& inst) override;
    void Visit(LocalInstruction& inst) override;
    void Visit(LoadInstruction& inst) override;
    void Visit(ElemAddrInstruction& inst) override;
    void Visit(PtrOffsetInstruction& inst) override;
    void Visit(PtrDiffInstruction& inst) override;
    void Visit(FunctionCallInstruction& inst) override;
    void Visit(TrapInstruction& inst) override;
    void Visit(PhiInstruction& inst) override;
    void Visit(NoOperationInstruction& inst) override;
private:
    std::ofstream file;
    std::unique_ptr<CodeFormatter> fileFormatter;
    CodeFormatter& formatter;
    bool newLines;
};

CMSX_INTERMEDIATE_API void Write(Context& context, const std::string& fileName);

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_WRITER_INCLUDED
