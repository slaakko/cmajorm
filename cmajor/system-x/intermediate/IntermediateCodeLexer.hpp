#ifndef IntermediateCodeLexer_HPP
#define IntermediateCodeLexer_HPP

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/system-x/intermediate/IntermediateCodeLexer.lexer' using soulng lexer generator slg version 3.1.0

#include <system-x/intermediate/Api.hpp>
#include <soulng/lexer/Lexer.hpp>

class CMSX_INTERMEDIATE_API IntermediateCodeLexer : public soulng::lexer::Lexer
{
public:
    IntermediateCodeLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    IntermediateCodeLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    int NextState(int state, char32_t c) override;
private:
    int GetTokenId(int statementIndex);
};
#endif
