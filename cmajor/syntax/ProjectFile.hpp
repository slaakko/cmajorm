#ifndef ProjectFile_hpp_5581
#define ProjectFile_hpp_5581

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/syntax/Project.hpp>

namespace cmajor { namespace syntax {

class ProjectFile : public cmajor::parsing::Grammar
{
public:
    static ProjectFile* Create();
    static ProjectFile* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    Project* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    ProjectFile(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ProjectFileRule;
    class ProjectFileContentRule;
    class SourceRule;
    class ReferenceRule;
    class FilePathRule;
};

} } // namespace cmajor.syntax

#endif // ProjectFile_hpp_5581
