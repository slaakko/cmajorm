#include <cmajor/ast/InitDone.hpp>
#include <cmajor/ast/SourceToken.hpp>
#include <cmajor/parser/SourceToken.hpp>
#include <cmajor/parsing/InitDone.hpp>
#include <cmajor/util/InitDone.hpp>
#include <cmajor/util/CodeFormatter.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

using namespace cmajor::ast;
using namespace cmajor::util;
using namespace cmajor::unicode;
using namespace cmajor::parser;

struct InitDone
{
    InitDone()
    {
        cmajor::ast::Init();
        cmajor::parsing::Init();
        cmajor::util::Init();
    }
    ~InitDone()
    {
        cmajor::util::Done();
        cmajor::parsing::Done();
        cmajor::ast::Done();
    }
};

std::string HtmlEscape(const std::string& s)
{
    std::string result;
    for (char c : s)
    {
        switch (c)
        {
        case '<': result.append("&lt;"); break;
        case '>': result.append("&gt;"); break;
        case '\'': result.append("&apos;"); break;
        case '"': result.append("&quot;"); break;
        case '&': result.append("&amp;"); break;
        default:
        {
            result.append(1, c);
        }
        }
    }
    return result;
}

class HtmlSourceTokenFormatter : public SourceTokenFormatter
{
public:
    HtmlSourceTokenFormatter(const std::string& sourceFilePath_, const std::string& styleSheetFilePath_);
    void BeginFormat() override;
    void EndFormat() override;
    void Keyword(const std::u32string& token) override;
    void Identifier(const std::u32string& token) override;
    void Number(const std::u32string& token) override;
    void Char(const std::u32string& token) override;
    void String(const std::u32string& token) override;
    void Spaces(const std::u32string& token) override;
    void Comment(const std::u32string& token) override;
    void NewLine(const std::u32string& token) override;
    void Other(const std::u32string& token) override;
private:
    std::string filePath;
    std::string styleSheetFilePath;
    std::ofstream htmlFile;
    std::unique_ptr<CodeFormatter> formatter;
};

HtmlSourceTokenFormatter::HtmlSourceTokenFormatter(const std::string& sourceFilePath_, const std::string& styleSheetFilePath_) : filePath(sourceFilePath_), styleSheetFilePath(styleSheetFilePath_)
{
    std::string htmlFilePath(sourceFilePath_);
    htmlFilePath.append(".html");
    htmlFile.open(htmlFilePath.c_str(), std::ios_base::out);
    formatter.reset(new CodeFormatter(htmlFile));
}

void HtmlSourceTokenFormatter::BeginFormat()
{
    formatter->WriteLine("<!DOCTYPE html>");
    formatter->WriteLine("<html lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\">");
    formatter->WriteLine("<head>");
    formatter->IncIndent();
    formatter->WriteLine("<meta charset=\"utf-8\"/>");
    boost::filesystem::path fp = filePath;
    std::string fileName = fp.filename().generic_string();
    formatter->WriteLine("<title>" + fileName + "</title>");
    formatter->WriteLine("<link rel=\"stylesheet\" type=\"text/css\" href=\"" + styleSheetFilePath + "\"/>");
    formatter->DecIndent();
    formatter->WriteLine("</head>");
    formatter->WriteLine("<body>");
    formatter->WriteLine("<pre>");
    formatter->WriteLine("<div class=\"code\">");
}

void HtmlSourceTokenFormatter::EndFormat()
{
    formatter->WriteLine("</div");
    formatter->WriteLine("</pre>");
    formatter->WriteLine("</body>");
    formatter->WriteLine("</html>");
}

void HtmlSourceTokenFormatter::Keyword(const std::u32string& token)
{
    formatter->Write("<span class=\"keyword\">" + ToUtf8(token) + "</span>");
}

void HtmlSourceTokenFormatter::Identifier(const std::u32string& token)
{
    formatter->Write("<span class=\"identifier\">" + ToUtf8(token) + "</span>");
}

void HtmlSourceTokenFormatter::Number(const std::u32string& token)
{
    formatter->Write("<span class=\"number\">" + ToUtf8(token) + "</span>");
}

void HtmlSourceTokenFormatter::Char(const std::u32string& token)
{
    formatter->Write("<span class=\"char\">" + HtmlEscape(ToUtf8(token)) + "</span>");
}

void HtmlSourceTokenFormatter::String(const std::u32string& token)
{
    formatter->Write("<span class=\"string\">" + HtmlEscape(ToUtf8(token)) + "</span>");
}

void HtmlSourceTokenFormatter::Spaces(const std::u32string& token)
{
    formatter->Write(ToUtf8(token));
}

void HtmlSourceTokenFormatter::Comment(const std::u32string& token)
{
    std::string trimmedToken = Trim(ToUtf8(token));
    formatter->Write("<span class=\"comment\">" + HtmlEscape(trimmedToken) + "</span>");
    formatter->WriteLine();
}

void HtmlSourceTokenFormatter::NewLine(const std::u32string& token)
{
    formatter->WriteLine();
}

void HtmlSourceTokenFormatter::Other(const std::u32string& token)
{
    formatter->Write("<span class=\"other\">" + HtmlEscape(ToUtf8(token)) + "</span>");
}

const char* version = "3.2.0";

void PrintHelp()
{
    std::cout << "cm2html version " << version << std::endl;
    std::cout << "usage: cm2html [options] { sourcefile.cm }" << std::endl;
    std::cout << "Convert cmajor sourcefiles to html." << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "-s | --style <filepath> : set stylesheet filepath to <filepath> " << std::endl;
    std::cout << "                          default is \"code.css\"" << std::endl;
    std::cout << "-h | --help             : print this help message" << std::endl;
}

int main(int argc, const char** argv)
{
    try
    {
        InitDone initDone;
        if (argc < 2)
        {
            PrintHelp();
            return 0;
        }
        SourceToken* grammar = SourceToken::Create();
        std::vector<std::string> sourceFiles;
        std::string styleSheetPath = "code.css";
        bool prevWasStyle = false;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (!arg.empty() && arg[0] == '-')
            {
                if (arg == "-s" || arg == "--style")
                {
                    prevWasStyle = true;
                }
                else if (arg == "-h" || arg == "--help")
                {
                    PrintHelp();
                    return 0;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (prevWasStyle)
            {
                styleSheetPath = arg;
                prevWasStyle = false;
            }
            else
            {
                sourceFiles.push_back(arg);
            }
        }
        for (const std::string& sourceFile : sourceFiles)
        {
            MappedInputFile file(sourceFile);
            std::u32string s(ToUtf32(std::string(file.Begin(), file.End())));
            HtmlSourceTokenFormatter formatter(sourceFile, styleSheetPath);
            grammar->Parse(&s[0], &s[0] + s.length(), 0, sourceFile, &formatter);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
