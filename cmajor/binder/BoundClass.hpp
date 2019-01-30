// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_BOUND_CLASS_INCLUDED
#define CMAJOR_BINDER_BOUND_CLASS_INCLUDED
#include <cmajor/binder/BoundNode.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

class BINDER_API BoundClass : public BoundNode
{
public:
    BoundClass(Module* module_, ClassTypeSymbol* classTypeSymbol_);
    BoundClass(const BoundClass&) = delete;
    BoundClass& operator=(const BoundClass&) = delete;
    void Accept(BoundNodeVisitor& visitor) override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void AddMember(std::unique_ptr<BoundNode>&& member);
    const std::vector<std::unique_ptr<BoundNode>>& Members() const { return members; }
    ClassTypeSymbol* GetClassTypeSymbol() { return classTypeSymbol; }
    bool ContainsSourceFunctions() const;
    bool IsInlineFunctionContainer() const { return inlineFunctionContainer; }
    void SetInlineFunctionContainer() { inlineFunctionContainer = true; }
private:
    ClassTypeSymbol* classTypeSymbol;
    std::vector<std::unique_ptr<BoundNode>> members;
    bool inlineFunctionContainer;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_BOUND_CLASS_INCLUDED
