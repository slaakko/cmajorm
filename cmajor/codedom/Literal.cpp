// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codedom/Literal.hpp>
#include <cmajor/codedom/Visitor.hpp>

namespace cmajor { namespace codedom {

Literal::Literal(const std::u32string& name_): CppObject(name_)
{
}

void Literal::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace cmajor::codedom
