// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_REX_SYMBOL_INCLUDED
#define SOULNG_REX_SYMBOL_INCLUDED
#include <soulng/rex/RexApi.hpp>
#include <vector>

namespace soulng { namespace rex {

const char32_t eps = '\0';

class SOULNG_REX_API Symbol
{
public:
    virtual ~Symbol();
    virtual bool Match(char32_t c) = 0;
};

class SOULNG_REX_API Char : public Symbol
{
public:
    Char(char32_t chr_);
    bool Match(char32_t c) override;
    char32_t Chr() const { return chr; }
private:
    char32_t chr;
};

class SOULNG_REX_API Any : public Symbol
{
public:
    bool Match(char32_t c) override;
};

class SOULNG_REX_API Range : public Symbol
{
public:
    Range(char32_t start_, char32_t end_);
    bool Match(char32_t c) override;
    char32_t Start() const { return start; }
    char32_t End() const { return end; }
private:
    char32_t start;
    char32_t end;
};

SOULNG_REX_API inline bool operator==(const Range& left, const Range& right)
{
    return left.Start() == right.Start() && left.End() == right.End();
}

SOULNG_REX_API inline bool operator<(const Range& left, const Range& right)
{
    if (left.Start() < right.Start()) return true;
    if (left.Start() > right.Start()) return false;
    return left.End() < right.End();
}

class SOULNG_REX_API Class : public Symbol
{
public:
    Class();
    bool Match(char32_t c) override;
    void SetInverse() { inverse = true; }
    void AddSymbol(Symbol* symbol);
private:
    bool inverse;
    std::vector<Symbol*> symbols;
};

} } // namespace soulng::rex

#endif // SOULNG_REX_SYMBOL_INCLUDED
