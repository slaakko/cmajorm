// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_REX_ALGORITHM_INCLUDED
#define SOULNG_REX_ALGORITHM_INCLUDED
#include <soulng/rex/RexApi.hpp>
#include <string>
#include <vector>

namespace soulng { namespace rex {

class NfaState;
class Nfa;

SOULNG_REX_API std::vector<NfaState*> EpsilonClosure(const std::vector<NfaState*>& states);
SOULNG_REX_API std::vector<NfaState*> EpsilonClosure(NfaState* state);
SOULNG_REX_API std::vector<NfaState*> Move(const std::vector<NfaState*>& states, char32_t c);
SOULNG_REX_API bool Match(Nfa& nfa, const std::u32string& s);

} } // namespace soulng::rex

#endif // SOULNG_REX_ALGORITHM_INCLUDED
