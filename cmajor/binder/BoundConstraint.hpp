// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_BOUND_CONSTRAINT_INCLUDED
#define CMAJOR_BINDER_BOUND_CONSTRAINT_INCLUDED
#include <cmajor/binder/BoundNode.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

class BINDER_API BoundConstraint : public BoundNode
{
public:
    BoundConstraint(const Span& span_, const boost::uuids::uuid& moduleId_, BoundNodeType boundNodeType_);
    virtual bool Subsume(BoundConstraint* that) const = 0;
    virtual BoundConstraint* Clone() const = 0;
    virtual bool IsBinaryConstraint() const { return false; }
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
};

class BINDER_API BoundAtomicConstraint : public BoundConstraint
{
public:
    BoundAtomicConstraint(const Span& span_, const boost::uuids::uuid& moduleId_, bool satisfied_);
    void Accept(BoundNodeVisitor& visitor) override;
    bool Subsume(BoundConstraint* that) const override;
    BoundConstraint* Clone() const override;
    bool Satisfied() const { return satisfied; }
    void SetConcept(ConceptSymbol* conceptSymbol_) { conceptSymbol = conceptSymbol_; }
private:
    bool satisfied;
    ConceptSymbol* conceptSymbol;
};

class BINDER_API BoundBinaryConstraint : public BoundConstraint
{
public:
    BoundBinaryConstraint(const Span& span_, const boost::uuids::uuid& moduleId_, BoundNodeType boundNodeType_, BoundConstraint* left_, BoundConstraint* right_);
    BoundBinaryConstraint(const BoundBinaryConstraint& that);
    bool IsBinaryConstraint() const override { return true; }
    BoundConstraint* Left() const { return left.get(); }
    BoundConstraint* Right() const { return right.get(); }
private:
    std::unique_ptr<BoundConstraint> left;
    std::unique_ptr<BoundConstraint> right;
};

class BINDER_API BoundDisjunctiveConstraint : public BoundBinaryConstraint
{
public:
    BoundDisjunctiveConstraint(const Span& span_, const boost::uuids::uuid& moduleId_, BoundConstraint* left_, BoundConstraint* right_);
    BoundDisjunctiveConstraint(const BoundDisjunctiveConstraint& that);
    bool Subsume(BoundConstraint* that) const override;
    void Accept(BoundNodeVisitor& visitor) override;
    BoundConstraint* Clone() const override;
};

class BINDER_API BoundConjunctiveConstraint : public BoundBinaryConstraint
{
public:
    BoundConjunctiveConstraint(const Span& span_, const boost::uuids::uuid& moduleId_, BoundConstraint* left_, BoundConstraint* right_);
    BoundConjunctiveConstraint(const BoundConjunctiveConstraint& that);
    bool Subsume(BoundConstraint* that) const override;
    void Accept(BoundNodeVisitor& visitor) override;
    BoundConstraint* Clone() const override;
};

class BINDER_API BoundConcept : public BoundNode
{
public:
    BoundConcept(ConceptSymbol* conceptSymbol_, const std::vector<TypeSymbol*>& typeArguments_, const Span& span_, const boost::uuids::uuid& moduleId_);
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

struct BINDER_API BoundConceptKey
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

struct BINDER_API BoundConceptKeyHash
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
