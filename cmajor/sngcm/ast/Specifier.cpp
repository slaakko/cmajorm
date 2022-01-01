// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Specifier.hpp>

namespace sngcm { namespace ast {

std::string SpecifierStr(Specifiers specifiers)
{
    std::string s;
    if ((specifiers & Specifiers::public_) != Specifiers::none)
    {
        s.append("public");
    }
    if ((specifiers & Specifiers::protected_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("protected");
    }
    if ((specifiers & Specifiers::private_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("private");
    }
    if ((specifiers & Specifiers::internal_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("internal");
    }
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("static");
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("virtual");
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("override");
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("abstract");
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("inline");
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("explicit");
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("extern");
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("suppress");
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("default");
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("constexpr");
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("cdecl");
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("nothrow");
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("throw");
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("new");
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("unit_test");
    }
    return s;
}

bool StaticConstructorSpecifiers(Specifiers specifiers)
{
    if ((specifiers & Specifiers::static_) == Specifiers::none) return false;
    return true;
}

} } // namespace sngcm::ast
