#ifndef CmajorLexer_HPP
#define CmajorLexer_HPP

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmlexer/CmajorLexer.lexer' using soulng lexer generator slg version 3.10.0

#include <sngcm/cmlexer/TokenValueParsers.hpp>
#include <sngcm/cmlexer/CmajorLexerApi.hpp>
#include <soulng/lexer/Lexer.hpp>

class SNGCM_LEXER_API CmajorLexer : public soulng::lexer::Lexer
{
public:
    CmajorLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    CmajorLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    int NextState(int state, char32_t c) override;
    double floatingLit;
    bool floatingLitFloat;
    uint64_t intLit;
    bool intLitUnsigned;
    char32_t chrLit;
    int chrLitPrefix;
    std::u32string strLit;
    int strLitPrefix;
    int leftAngleCount;
    bool parsingOperatorFunctionGroupId;
private:
    int GetTokenId(int statementIndex);
};
#endif
