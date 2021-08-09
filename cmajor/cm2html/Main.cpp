#include <sngcm/ast/InitDone.hpp>
#include <sngcm/ast/SourceToken.hpp>
#include <sngcm/cmparser/SourceTokenParser.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

using namespace sngcm::ast;
using namespace soulng::util;
using namespace soulng::unicode;

struct InitDone
{
    InitDone()
    {
        sngcm::ast::Init();
        soulng::util::Init();
    }
    ~InitDone()
    {
        soulng::util::Done();
        sngcm::ast::Done();
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

const char* version = "4.1.0";

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

std::vector<std::u32string> GetLines(const std::u32string& text)
{
    std::vector<std::u32string> lines;
    std::u32string line;
    int state = 0;
    for (char32_t c : text)
    {
        switch (state)
        {
        case 0:
        {
            if (c == '\n')
            {
                lines.push_back(std::move(line));
                line.clear();
                state = 1;
            }
            else if (c != '\r')
            {
                line.append(1, c);
            }
            break;
        }
        case 1:
        {
            if (c == '\n')
            {
                lines.push_back(std::move(line));
                line.clear();
            }
            else if (c != '\r')
            {
                line.append(1, c);
                state = 0;
            }
            break;
        }
        }
    }
    if (state == 0)
    {
        lines.push_back(std::move(line));
    }
    return lines;
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
            bool inBlockComment = false;
            MappedInputFile file(sourceFile);
            std::u32string s(ToUtf32(std::string(file.Begin(), file.End())));
            HtmlSourceTokenFormatter formatter(sourceFile, styleSheetPath);
            std::vector<std::u32string> lines = GetLines(s);
            for (const auto& line : lines)
            {
                sngcm::parser::ParseSourceLine(line, &formatter, inBlockComment);
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
