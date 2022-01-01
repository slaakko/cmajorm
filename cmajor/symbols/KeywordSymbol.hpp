// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_KEYWORD_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_KEYWORD_SYMBOL_INCLUDED
#include <cmajor/symbols/Symbol.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowTokens.hpp>

namespace cmajor { namespace symbols {

using namespace CmajorNothrowTokens;

class SYMBOLS_API KeywordSymbol : public Symbol
{
public:
    KeywordSymbol(int tokenId_, const std::u32string& name_);
    std::string GetSymbolCategoryStr() const override { return "KW"; }
    std::string GetSymbolCategoryDescription() const override { return "keyword"; }
    std::string TypeString() const override { return "keyword_symbol"; }
    std::u32string FullName() const override { return Name(); }
    std::u32string FullNameWithSpecifiers() const override { return Name(); }
    int TokenId() const { return tokenId; }
private:
    int tokenId;
};

SYMBOLS_API KeywordSymbol* GetKeywordSymbol(int tokenId);

SYMBOLS_API void InitKeywordSymbol();
SYMBOLS_API void DoneKeywordSymbol();

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_KEYWORD_SYMBOL_INCLUDED
