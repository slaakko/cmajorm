// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEDOM_LITERAL_INCLUDED
#define CMAJOR_CODEDOM_LITERAL_INCLUDED
#include <cmajor/codedom/Object.hpp>

namespace cmajor { namespace codedom {

class Literal: public CppObject
{
public:
    Literal(const std::u32string& name_);
    int Rank() const override { return 24; }
    void Accept(Visitor& visitor) override;
};

} } // namespace cmajor::codedom

#endif // CMAJOR_CODEDOM_LITERAL_INCLUDED

