// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_REX_CONTEXT_INCLUDED
#define SOULNG_REX_CONTEXT_INCLUDED
#include <soulng/rex/Symbol.hpp>
#include <map>

namespace soulng { namespace rex {

class NfaState;

class SOULNG_REX_API Context
{
public:
    Context();
    ~Context();
    NfaState* MakeNfaState();
    Symbol* MakeChar(char32_t c);
    Symbol* MakeRange(char32_t start, char32_t end);
    Symbol* MakeAny() { return &any; }
    Symbol* MakeEpsilon() { return &epsilon; }
    Class* MakeClass();
private:
    std::vector<NfaState*> nfaStates;
    std::vector<Symbol*> symbols;
    Any any;
    Char epsilon;
    std::map<char32_t, Symbol*> charSymbols;
    std::map<Range, Symbol*> rangeSymbols;
};

} } // namespace soulng::rex

#endif // SOULNG_REX_CONTEXT_INCLUDED
