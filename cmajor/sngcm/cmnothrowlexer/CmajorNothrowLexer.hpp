#ifndef CmajorNothrowLexer_HPP
#define CmajorNothrowLexer_HPP

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowlexer/CmajorNothrowLexer.lexer' using soulng lexer generator slg version 4.0.0

#include <sngcm/cmnothrowlexer/NothrowTokenValueParsers.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexerApi.hpp>
#include <soulng/lexer/Lexer.hpp>

class SNGCM_NOTHROW_LEXER_API CmajorNothrowLexer : public soulng::lexer::Lexer
{
public:
    CmajorNothrowLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    CmajorNothrowLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
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
