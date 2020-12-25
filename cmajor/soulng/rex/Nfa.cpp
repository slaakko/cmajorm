// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/rex/Nfa.hpp>
#include <soulng/rex/Context.hpp>

namespace soulng { namespace rex {

NfaEdge::NfaEdge(Symbol* symbol_, NfaState* next_) : symbol(symbol_), next(next_)
{
}

NfaState::NfaState() : accept(false), edges()
{
}

void NfaState::SetEdges(const std::vector<NfaEdge>& edges_)
{
    edges = edges_;
}

void NfaState::AddEdge(const NfaEdge& edge)
{
    edges.push_back(edge);
}

std::vector<NfaState*> NfaState::Next(char32_t c) const
{
    std::vector<NfaState*> next;
    for (const NfaEdge& edge : edges)
    {
        Symbol* symbol = edge.GetSymbol();
        if (symbol->Match(c))
        {
            next.push_back(edge.Next());
        }
    }
    return next;
}

Nfa::Nfa() : start(nullptr), end(nullptr)
{
}

Nfa::Nfa(NfaState* start_, NfaState* end_) : start(start_), end(end_)
{
}

Nfa MakeNfa(Context& context, Symbol* symbol)
{
    NfaState* start = context.MakeNfaState();
    NfaState* end = context.MakeNfaState();
    end->SetAccept(true);
    start->AddEdge(NfaEdge(symbol, end));
    return Nfa(start, end);
}

Nfa Cat(const Nfa& left, const Nfa& right)
{
    Nfa cat(left);
    NfaState* leftEnd = cat.End();
    leftEnd->SetAccept(false);
    NfaState* rightStart = right.Start();
    leftEnd->SetEdges(rightStart->Edges());
    cat.SetEnd(right.End());
    return cat;
}

Nfa Alt(Context& context, const Nfa& left, const Nfa& right)
{
    NfaState* leftStart = left.Start();
    NfaState* leftEnd = left.End();
    NfaState* rightStart = right.Start();
    NfaState* rightEnd = right.End();
    NfaState* start = context.MakeNfaState();
    NfaState* end = context.MakeNfaState();
    end->SetAccept(true);
    start->AddEdge(NfaEdge(context.MakeEpsilon(), leftStart));
    start->AddEdge(NfaEdge(context.MakeEpsilon(), rightStart));
    Nfa alt;
    alt.SetStart(start);
    leftEnd->SetAccept(false);
    leftEnd->AddEdge(NfaEdge(context.MakeEpsilon(), end));
    rightEnd->SetAccept(false);
    rightEnd->AddEdge(NfaEdge(context.MakeEpsilon(), end));
    alt.SetEnd(end);
    return alt;
}

Nfa Kleene(Context& context, const Nfa& nfa)
{
    Nfa kleene;
    NfaState* start = context.MakeNfaState();
    NfaState* end = context.MakeNfaState();
    end->SetAccept(true);
    start->AddEdge(NfaEdge(context.MakeEpsilon(), end));
    kleene.SetStart(start);
    NfaState* nfaStart = nfa.Start();
    start->AddEdge(NfaEdge(context.MakeEpsilon(), nfaStart));
    NfaState* nfaEnd = nfa.End();
    nfaEnd->SetAccept(false);
    nfaEnd->AddEdge(NfaEdge(context.MakeEpsilon(), end));
    nfaEnd->AddEdge(NfaEdge(context.MakeEpsilon(), nfaStart));
    kleene.SetEnd(end);
    return kleene;
}

Nfa Pos(Context& context, const Nfa& nfa)
{
    Nfa pos;
    NfaState* start = context.MakeNfaState();
    NfaState* end = context.MakeNfaState();
    end->SetAccept(true);
    pos.SetStart(start);
    NfaState* nfaStart = nfa.Start();
    start->AddEdge(NfaEdge(context.MakeEpsilon(), nfaStart));
    NfaState* nfaEnd = nfa.End();
    nfaEnd->SetAccept(false);
    nfaEnd->AddEdge(NfaEdge(context.MakeEpsilon(), end));
    nfaEnd->AddEdge(NfaEdge(context.MakeEpsilon(), nfaStart));
    pos.SetEnd(end);
    return pos;
}

Nfa Opt(Context& context, const Nfa& nfa)
{
    Nfa opt(nfa);
    opt.Start()->AddEdge(NfaEdge(context.MakeEpsilon(), opt.End()));
    return opt;
}

} } // namespace soulng::rex
