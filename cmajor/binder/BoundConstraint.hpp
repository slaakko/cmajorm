// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_BOUND_CONSTRAINT_INCLUDED
#define CMAJOR_BINDER_BOUND_CONSTRAINT_INCLUDED
#include <cmajor/binder/BoundNode.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

class BoundConstraint : public BoundNode
{
public:
    BoundConstraint(Module* module_, const Span& span_, BoundNodeType boundNodeType_);
    virtual bool Subsume(BoundConstraint* that) const = 0;
    virtual BoundConstraint* Clone() const = 0;
    virtual bool IsBinaryConstraint() const { return false; }
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
};

class BoundAtomicConstraint : public BoundConstraint
{
public:
    BoundAtomicConstraint(Module* module_, const Span& span_, bool satisfied_);
    void Accept(BoundNodeVisitor& visitor) override;
    bool Subsume(BoundConstraint* that) const override;
    BoundConstraint* Clone() const override;
    bool Satisfied() const { return satisfied; }
    void SetConcept(ConceptSymbol* concept_) { concept = concept_; }
private:
    bool satisfied;
    ConceptSymbol* concept;
};

class BoundBinaryConstraint : public BoundConstraint
{
public:
    BoundBinaryConstraint(Module* module_, const Span& span_, BoundNodeType boundNodeType_, BoundConstraint* left_, BoundConstraint* right_);
    BoundBinaryConstraint(const BoundBinaryConstraint& that);
    bool IsBinaryConstraint() const override { return true; }
    BoundConstraint* Left() const { return left.get(); }
    BoundConstraint* Right() const { return right.get(); }
private:
    std::unique_ptr<BoundConstraint> left;
    std::unique_ptr<BoundConstraint> right;
};

class BoundDisjunctiveConstraint : public BoundBinaryConstraint
{
public:
    BoundDisjunctiveConstraint(Module* module_, const Span& span_, BoundConstraint* left_, BoundConstraint* right_);
    BoundDisjunctiveConstraint(const BoundDisjunctiveConstraint& that);
    bool Subsume(BoundConstraint* that) const override;
    void Accept(BoundNodeVisitor& visitor) override;
    BoundConstraint* Clone() const override;
};

class BoundConjunctiveConstraint : public BoundBinaryConstraint
{
public:
    BoundConjunctiveConstraint(Module* module_, const Span& span_, BoundConstraint* left_, BoundConstraint* right_);
    BoundConjunctiveConstraint(const BoundConjunctiveConstraint& that);
    bool Subsume(BoundConstraint* that) const override;
    void Accept(BoundNodeVisitor& visitor) override;
    BoundConstraint* Clone() const override;
};

class BoundConcept : public BoundNode
{
public:
    BoundConcept(Module* module_, ConceptSymbol* conceptSymbol_, const std::vector<TypeSymbol*>& typeArguments_, const Span& span_);
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    ConceptSymbol* GetConceptSymbol() const { return conceptSymbol; }
    const std::vector<TypeSymbol*>& TypeArguments() const { return typeArguments; }
    int Arity() const { return typeArguments.size(); }
    TypeSymbol* CommonType() const { return commonType; }
    void SetCommonType(TypeSymbol* commonType_) { commonType = commonType_; }
    void SetBoundConstraint(std::unique_ptr<BoundConstraint>&& boundConstraint_);
    BoundConstraint* GetBoundConstraint() { return boundConstraint.get(); }
    void AddBoundTemplateParameter(std::unique_ptr<BoundTemplateParameterSymbol>&& boundTemplateParameter);
private:
    std::u32string name;
    ConceptSymbol* conceptSymbol;
    std::vector<TypeSymbol*> typeArguments;
    TypeSymbol* commonType;
    std::unique_ptr<BoundConstraint> boundConstraint;
    std::vector<std::unique_ptr<BoundTemplateParameterSymbol>> boundTemplateParameters;
};

struct BoundConceptKey
{
    BoundConceptKey(ConceptSymbol* conceptSymbol_, std::vector<TypeSymbol*> typeArguments_) : conceptSymbol(conceptSymbol_), typeArguments(typeArguments_) {}
    int Arity() const { return typeArguments.size(); }
    ConceptSymbol* conceptSymbol;
    std::vector<TypeSymbol*> typeArguments;
};

bool operator==(const BoundConceptKey& left, const BoundConceptKey& right);
inline bool operator!=(const BoundConceptKey& left, const BoundConceptKey& right)
{
    return !(left == right);
}

struct BoundConceptKeyHash
{
    bool operator()(const BoundConceptKey& boundConceptKey) const
    {
        size_t hashCode = boost::hash<boost::uuids::uuid>()(boundConceptKey.conceptSymbol->TypeId());
        int n = boundConceptKey.Arity();
        for (int i = 0; i < n; ++i)
        {
            hashCode ^= boost::hash<boost::uuids::uuid>()(boundConceptKey.typeArguments[i]->TypeId());
        }
        return hashCode;
    }
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_BOUND_CONSTRAINT_INCLUDED
