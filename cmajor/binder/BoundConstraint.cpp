// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/BoundConstraint.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/symbols/Exception.hpp>

namespace cmajor { namespace binder {

BoundConstraint::BoundConstraint(const Span& span_, const boost::uuids::uuid& moduleId_, BoundNodeType boundNodeType_) : BoundNode(span_, moduleId_, boundNodeType_)
{
}

void BoundConstraint::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot load constraint", GetSpan(), ModuleId());
}

void BoundConstraint::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store constraint", GetSpan(), ModuleId());
}

BoundAtomicConstraint::BoundAtomicConstraint(const Span& span_, const boost::uuids::uuid& moduleId_, bool satisfied_) : 
    BoundConstraint(span_, moduleId_, BoundNodeType::boundAtomicConstraint), satisfied(satisfied_), conceptSymbol(nullptr)
{
}

void BoundAtomicConstraint::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundAtomicConstraint::Subsume(BoundConstraint* that) const
{
    if (that->IsBinaryConstraint())
    {
        BoundBinaryConstraint* thatBinaryConstraint = static_cast<BoundBinaryConstraint*>(that);
        BoundConstraint* thatLeft = thatBinaryConstraint->Left();
        BoundConstraint* thatRight = thatBinaryConstraint->Right();
        bool subsumeLeft = Subsume(thatLeft);
        bool subsumeRight = Subsume(thatRight);
        if (that->GetBoundNodeType() == BoundNodeType::boundConjunctiveConstraint)
        {
            return subsumeLeft && subsumeRight;
        }
        else if (that->GetBoundNodeType() == BoundNodeType::boundDisjunctiveConstraint)
        {
            return subsumeLeft || subsumeRight;
        }
        else // assert(false)
        {
            Assert(false, "unknown binary constraint type");
            return false;
        }
    }
    else if (that->GetBoundNodeType() == BoundNodeType::boundAtomicConstraint)
    {
        BoundAtomicConstraint* thatAtomic = static_cast<BoundAtomicConstraint*>(that);
        if (satisfied && !thatAtomic->Satisfied())
        {
            return true;
        }
        else if (!satisfied && thatAtomic->Satisfied())
        {
            return false;
        }
        else
        {
            if (conceptSymbol && !thatAtomic->conceptSymbol)
            {
                return true;
            }
            else if (!conceptSymbol && thatAtomic->conceptSymbol)
            {
                return false;
            }
            else if (!conceptSymbol && !thatAtomic->conceptSymbol)
            {
                return true;
            }
            else
            {
                if (conceptSymbol == thatAtomic->conceptSymbol)
                {
                    return true;
                }
                ConceptSymbol* refinedConcept = conceptSymbol->RefinedConcept();
                while (refinedConcept)
                {
                    if (refinedConcept == thatAtomic->conceptSymbol)
                    {
                        return true;
                    }
                    else
                    {
                        refinedConcept = refinedConcept->RefinedConcept();
                    }
                }
                return false;
            }
        }
    }
    else
    {
        return false;
    }
}

BoundConstraint* BoundAtomicConstraint::Clone() const
{
    return new BoundAtomicConstraint(*this);
}

BoundBinaryConstraint::BoundBinaryConstraint(const Span& span_, const boost::uuids::uuid& moduleId_, BoundNodeType boundNodeType_, BoundConstraint* left_, BoundConstraint* right_) :
    BoundConstraint(span_, moduleId_, boundNodeType_), left(left_), right(right_)
{
}

BoundBinaryConstraint::BoundBinaryConstraint(const BoundBinaryConstraint& that) : BoundConstraint(that), left(that.left->Clone()), right(that.right->Clone())
{
}

BoundDisjunctiveConstraint::BoundDisjunctiveConstraint(const Span& span_, const boost::uuids::uuid& moduleId_, BoundConstraint* left_, BoundConstraint* right_) :
    BoundBinaryConstraint(span_, moduleId_, BoundNodeType::boundDisjunctiveConstraint, left_, right_)
{
}

BoundDisjunctiveConstraint::BoundDisjunctiveConstraint(const BoundDisjunctiveConstraint& that) : BoundBinaryConstraint(that)
{
}

bool BoundDisjunctiveConstraint::Subsume(BoundConstraint* that) const
{
    BoundConstraint* left = Left();
    BoundConstraint* right = Right();
    if (that->IsBinaryConstraint())
    {
        BoundBinaryConstraint* thatBinaryConstraint = static_cast<BoundBinaryConstraint*>(that);
        BoundConstraint* thatLeft = thatBinaryConstraint->Left();
        BoundConstraint* thatRight = thatBinaryConstraint->Right();
        bool leftSubsumeThatLeft = left->Subsume(thatLeft);
        bool leftSubsumeThatRight = left->Subsume(thatRight);
        bool rightSubsumeThatLeft = right->Subsume(thatLeft);
        bool rightSubsumeThatRight = right->Subsume(thatRight);
        bool leftSubsumeThatLeftOrThatRight = leftSubsumeThatLeft || leftSubsumeThatRight;
        bool rightSubsumeThatLeftOrThatRight = rightSubsumeThatLeft || rightSubsumeThatRight;
        if (that->GetBoundNodeType() == BoundNodeType::boundConjunctiveConstraint)
        {
            return leftSubsumeThatLeftOrThatRight && rightSubsumeThatLeftOrThatRight;
        }
        else if (that->GetBoundNodeType() == BoundNodeType::boundDisjunctiveConstraint)
        {
            return leftSubsumeThatLeftOrThatRight || rightSubsumeThatLeftOrThatRight;
        }
        else // assert(false)
        {
            Assert(false, "unknown binary constraint type");
            return false;
        }
    }
    else
    {
        bool leftSubsumeThat = left->Subsume(that);
        bool rightSubsumeThat = right->Subsume(that);
        return leftSubsumeThat && rightSubsumeThat;
    }
}

void BoundDisjunctiveConstraint::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundConstraint* BoundDisjunctiveConstraint::Clone() const
{
    return new BoundDisjunctiveConstraint(*this);
}

BoundConjunctiveConstraint::BoundConjunctiveConstraint(const Span& span_, const boost::uuids::uuid& moduleId_, BoundConstraint* left_, BoundConstraint* right_) :
    BoundBinaryConstraint(span_, moduleId_, BoundNodeType::boundConjunctiveConstraint, left_, right_)
{
}

BoundConjunctiveConstraint::BoundConjunctiveConstraint(const BoundConjunctiveConstraint& that) : BoundBinaryConstraint(that)
{
}

bool BoundConjunctiveConstraint::Subsume(BoundConstraint* that) const
{
    BoundConstraint* left = Left();
    BoundConstraint* right = Right();
    if (that->IsBinaryConstraint())
    {
        BoundBinaryConstraint* thatBinaryConstraint = static_cast<BoundBinaryConstraint*>(that);
        BoundConstraint* thatLeft = thatBinaryConstraint->Left();
        BoundConstraint* thatRight = thatBinaryConstraint->Right();
        bool leftSubsumeThatLeft = left->Subsume(thatLeft);
        bool rightSubsumeThatLeft = right->Subsume(thatLeft);
        bool leftSubsumeThatRight = left->Subsume(thatRight);
        bool rightSubsumeThatRight = right->Subsume(thatRight);
        bool leftOrRightSubsumeThatLeft = leftSubsumeThatLeft || rightSubsumeThatLeft;
        bool leftOrRightSubsumeThatRight = leftSubsumeThatRight || rightSubsumeThatRight;
        if (that->GetBoundNodeType() == BoundNodeType::boundConjunctiveConstraint)
        {
            return leftOrRightSubsumeThatLeft && leftOrRightSubsumeThatRight;
        }
        else if (that->GetBoundNodeType() == BoundNodeType::boundDisjunctiveConstraint)
        {
            return leftOrRightSubsumeThatLeft || leftOrRightSubsumeThatRight;
        }
        else // assert(false)
        {
            Assert(false, "unknown binary constraint type");
            return false;
        }
    }
    else
    {
        bool leftSubsumeThat = left->Subsume(that);
        bool righSubsumeThat = right->Subsume(that);
        return leftSubsumeThat || righSubsumeThat;
    }
}

void BoundConjunctiveConstraint::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundConstraint* BoundConjunctiveConstraint::Clone() const
{
    return new BoundConjunctiveConstraint(*this);
}

std::u32string MakeBoundConceptName(ConceptSymbol* conceptSymbol, const std::vector<TypeSymbol*>& typeArguments)
{
    std::u32string s = conceptSymbol->GroupName();
    s.append(1, '<');
    int n = typeArguments.size();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            s.append(U", ");
        }
        TypeSymbol* typeSymbol = typeArguments[i];
        s.append(typeSymbol->FullName());
    }
    s.append(1, '>');
    return s;
}

BoundConcept::BoundConcept(ConceptSymbol* conceptSymbol_, const std::vector<TypeSymbol*>& typeArguments_, const Span& span_, const boost::uuids::uuid& moduleId_) :
    BoundNode(span_, moduleId_, BoundNodeType::boundConcept), name(MakeBoundConceptName(conceptSymbol_, typeArguments_)), conceptSymbol(conceptSymbol_), typeArguments(typeArguments_), 
    commonType(nullptr)
{
}

void BoundConcept::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot load bound concept", GetSpan(), ModuleId());
}

void BoundConcept::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store bound concept", GetSpan(), ModuleId());
}

void BoundConcept::Accept(BoundNodeVisitor& visitor)
{
    throw Exception("cannot visit bound concept", GetSpan(), ModuleId());
}

void BoundConcept::SetBoundConstraint(std::unique_ptr<BoundConstraint>&& boundConstraint_)
{
    boundConstraint = std::move(boundConstraint_);
}

void BoundConcept::AddBoundTemplateParameter(std::unique_ptr<BoundTemplateParameterSymbol>&& boundTemplateParameter)
{
    boundTemplateParameters.push_back(std::move(boundTemplateParameter));
}

bool operator==(const BoundConceptKey& left, const BoundConceptKey& right)
{
    if (left.conceptSymbol != right.conceptSymbol) return false;
    if (left.Arity() != right.Arity()) return false;
    int n = left.Arity();
    for (int i = 0; i < n; ++i)
    {
        if (!TypesEqual(left.typeArguments[i], right.typeArguments[i])) return false;
    }
    return true;
}

} } // namespace cmajor::binder
