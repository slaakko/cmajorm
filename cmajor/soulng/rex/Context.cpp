// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/rex/Context.hpp>
#include <soulng/rex/Symbol.hpp>
#include <soulng/rex/Nfa.hpp>

namespace soulng { namespace rex {

Context::Context() : any(), epsilon(eps)
{
}

Context::~Context()
{
    for (NfaState* state : nfaStates)
    {
        delete state;
    }
    for (Symbol* symbol : symbols)
    {
        delete symbol;
    }
}

NfaState* Context::MakeNfaState()
{
    NfaState* state = new NfaState();
    nfaStates.push_back(state);
    return state;
}

Symbol* Context::MakeChar(char32_t c)
{
    std::map<char32_t, Symbol*>::const_iterator it = charSymbols.find(c);
    if (it != charSymbols.cend())
    {
        return it->second;
    }
    else
    {
        Symbol* symbol = new Char(c);
        symbols.push_back(symbol);
        charSymbols[c] = symbol;
        return symbol;
    }
}

Symbol* Context::MakeRange(char32_t start, char32_t end)
{
    Range range(start, end);
    std::map<Range, Symbol*>::const_iterator it = rangeSymbols.find(range);
    if (it != rangeSymbols.cend())
    {
        return it->second;
    }
    else
    {
        Symbol* symbol = new Range(start, end);
        symbols.push_back(symbol);
        rangeSymbols[range] = symbol;
        return symbol;
    }
}

Class* Context::MakeClass()
{
    Class* cls = new Class();
    symbols.push_back(cls);
    return cls;
}

} } // namespace soulng::rex
