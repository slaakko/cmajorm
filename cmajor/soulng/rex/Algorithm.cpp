// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/rex/Algorithm.hpp>
#include <soulng/rex/Nfa.hpp>
#include <soulng/rex/Symbol.hpp>
#include <algorithm>

namespace soulng { namespace rex {

std::vector<NfaState*> EpsilonClosure(const std::vector<NfaState*>& states)
{
    std::vector<NfaState*> stack;
    for (NfaState* state : states)
    {
        stack.push_back(state);
    }
    std::vector<NfaState*> epsilonClosure = states;
    while (!stack.empty())
    {
        NfaState* s = stack.back();
        stack.pop_back();
        std::vector<NfaState*> u = s->Next(eps);
        for (NfaState* v : u)
        {
            if (std::find(epsilonClosure.cbegin(), epsilonClosure.cend(), v) == epsilonClosure.cend())
            {
                epsilonClosure.push_back(v);
                stack.push_back(v);
            }
        }
    }
    return epsilonClosure;
}

std::vector<NfaState*> EpsilonClosure(NfaState* state)
{
    std::vector<NfaState*> states(1, state);
    return EpsilonClosure(states);
}

std::vector<NfaState*> Move(const std::vector<NfaState*>& states, char32_t c)
{
    std::vector<NfaState*> next;
    for (NfaState* state : states)
    {
        std::vector<NfaState*> n = state->Next(c);
        for (NfaState* s : n)
        {
            if (std::find(next.cbegin(), next.cend(), s) == next.cend())
            {
                next.push_back(s);
            }
        }
    }
    return next;
}

bool Match(Nfa& nfa, const std::u32string& s)
{
    std::vector<NfaState*> states = EpsilonClosure(nfa.Start());
    for (char32_t c : s)
    {
        states = EpsilonClosure(Move(states, c));
    }
    for (NfaState* state : states)
    {
        if (state->Accept()) return true;
    }
    return false;
}

} } // namespace soulng::rex
