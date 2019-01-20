#ifndef ParserFile_hpp_5581
#define ParserFile_hpp_5581

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/syntax/ParserFileContent.hpp>

namespace cmajor { namespace syntax {

class ParserFile : public cmajor::parsing::Grammar
{
public:
    static ParserFile* Create();
    static ParserFile* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    ParserFileContent* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, int id_, cmajor::parsing::ParsingDomain* parsingDomain_);
private:
    ParserFile(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ParserFileRule;
    class IncludeDirectivesRule;
    class IncludeDirectiveRule;
    class FileAttributeRule;
    class IncludeFileNameRule;
    class NamespaceContentRule;
    class NamespaceRule;
};

} } // namespace cmajor.syntax

#endif // ParserFile_hpp_5581
