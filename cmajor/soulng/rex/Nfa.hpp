// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_REX_NFA_INCLUDED
#define SOULNG_REX_NFA_INCLUDED
#include <soulng/rex/Symbol.hpp>

namespace soulng { namespace rex {

class NfaState;
class Context;

class SOULNG_REX_API NfaEdge
{
public:
    NfaEdge(Symbol* symbol_, NfaState* next_);
    Symbol* GetSymbol() const { return symbol; }
    NfaState* Next() const { return next; }
private:
    Symbol* symbol;
    NfaState* next;
};

class SOULNG_REX_API NfaState
{
public:
    NfaState();
    bool Accept() const { return accept; }
    void SetAccept(bool accept_) { accept = accept_; }
    const std::vector<NfaEdge>& Edges() const { return edges; }
    void SetEdges(const std::vector<NfaEdge>& edges_);
    void AddEdge(const NfaEdge& edge);
    std::vector<NfaState*> Next(char32_t c) const;
private:
    bool accept;
    std::vector<NfaEdge> edges;
};

class SOULNG_REX_API Nfa
{
public:
    Nfa();
    Nfa(NfaState* start_, NfaState* end_);
    NfaState* Start() const { return start; }
    void SetStart(NfaState* start_) { start = start_; }
    NfaState* End() const { return end; }
    void SetEnd(NfaState* end_) { end = end_; }
private:
    NfaState* start;
    NfaState* end;
};

SOULNG_REX_API Nfa MakeNfa(Context& context, Symbol* symbol);
SOULNG_REX_API Nfa Cat(const Nfa& left, const Nfa& right);
SOULNG_REX_API Nfa Alt(Context& context, const Nfa& left, const Nfa& right);
SOULNG_REX_API Nfa Kleene(Context& context, const Nfa& nfa);
SOULNG_REX_API Nfa Pos(Context& context, const Nfa& nfa);
SOULNG_REX_API Nfa Opt(Context& context, const Nfa& nfa);

} } // namespace soulng::rex

#endif // SOULNG_SLG_NFA_INCLUDED
