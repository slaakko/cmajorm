// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_SPECIFIER_INCLUDED
#define SNGCM_AST_SPECIFIER_INCLUDED
#include <sngcm/ast/AstApi.hpp>
#include <string>
#include <stdint.h>

namespace sngcm { namespace ast {

enum class Specifiers : int32_t
{
    none = 0,
    public_ = 1 << 0,
    protected_ = 1 << 1,
    private_ = 1 << 2,
    internal_ = 1 << 3,
    static_ = 1 << 4,
    virtual_ = 1 << 5,
    override_ = 1 << 6,
    abstract_ = 1 << 7,
    inline_ = 1 << 8,
    explicit_ = 1 << 9,
    external_ = 1 << 10,
    suppress_ = 1 << 11,
    default_ = 1 << 12,
    constexpr_ = 1 << 13,
    cdecl_ = 1 << 14,
    nothrow_ = 1 << 15,
    throw_ = 1 << 16,
    new_ = 1 << 17,
    const_ = 1 << 18,
    unit_test_ = 1 << 19,
    access_ = public_ | protected_ | private_ | internal_
};

inline Specifiers operator|(Specifiers left, Specifiers right)
{
    return static_cast<Specifiers>(static_cast<int32_t>(left) | static_cast<int32_t>(right));
}

inline Specifiers operator&(Specifiers left, Specifiers right)
{
    return static_cast<Specifiers>(static_cast<int32_t>(left) & static_cast<int32_t>(right));
}

inline Specifiers operator~(Specifiers operand)
{
    return static_cast<Specifiers>(~static_cast<int32_t>(operand));
}

SNGCM_AST_API bool StaticConstructorSpecifiers(Specifiers specifiers);

SNGCM_AST_API std::string SpecifierStr(Specifiers specifiers);

} } // namespace sngcm::ast

#endif // SNGCM_AST_SPECIFIER_INCLUDED
