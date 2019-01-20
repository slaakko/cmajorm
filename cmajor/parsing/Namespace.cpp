// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/Namespace.hpp>
#include <cmajor/parsing/Scope.hpp>
#include <cmajor/parsing/Visitor.hpp>

namespace cmajor { namespace parsing {

Namespace::Namespace(const std::u32string& name_, Scope* enclosingScope_) : ParsingObject(name_, enclosingScope_, ObjectKind::ns)
{
    SetScope(new Scope(Name(), EnclosingScope())); 
}

void Namespace::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    GetScope()->Accept(visitor);
    visitor.EndVisit(*this);
}

UsingObject::UsingObject(cmajor::codedom::UsingObject* subject_, Scope* enclosingScope_) : 
    ParsingObject(subject_->Name(), enclosingScope_, ObjectKind::usingObject), subject(subject_)
{
    if (!subject->IsOwned())
    {
        subject->SetOwned();
        ownedSubject.reset(subject);
    }
}

void UsingObject::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace cmajor::parsing
