// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/Style.hpp>
#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <boost/filesystem.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/CodeFormatter.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

namespace cmajor { namespace cmdoclib {

using namespace cmajor::symbols;
using namespace cmajor::util;

void GenerateStyleFile()
{
    Input* input = GetInputPtr();
    std::string targetDir = input->targetDirPath;
    boost::filesystem::create_directories(targetDir);
    std::string styleDir = GetFullPath(Path::Combine(targetDir, "style"));
    boost::filesystem::create_directories(styleDir);
    std::string styleFileName = "style.css";
    std::string styleFilePath = GetFullPath(Path::Combine(styleDir, styleFileName));
    if (GetGlobalFlag(GlobalFlags::optimizeCmDoc) && boost::filesystem::exists(styleFilePath))
    {
        return;
    }
    std::ofstream styleFile(styleFilePath);
    CodeFormatter formatter(styleFile);
    formatter.WriteLine(".diagram");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("text-align: center;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".tree");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("margin: 20px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".indent");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("margin: 20px;");
    formatter.WriteLine("max-width: 800px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".description");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("margin: 20px;");
    formatter.WriteLine("max-width: 800px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("h1, h2, h3, h4, h5, h6");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #005ab4;");
    formatter.WriteLine("font-family: sans-serif;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("table");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: sans-serif;");
    formatter.WriteLine("border-collapse: collapse;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("table, th, td");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("text-align: left;");
    formatter.WriteLine("border: 1px solid #dddddd;");
    formatter.WriteLine("margin: 20px;");
    formatter.WriteLine("padding: 8px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("td.rightAlignedCol");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("text-align: right;");
    formatter.WriteLine("border: 1px solid #dddddd;");
    formatter.WriteLine("padding: 8px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    //  grammars:
    formatter.WriteLine("table.grammar");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("background-color: FloralWhite;");
    formatter.WriteLine("margin: 20px;");
    formatter.WriteLine("padding: 20px;");
    formatter.WriteLine("max-width: 800px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("table.grammar, td.grammarThirdCol");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("vertical-align: top;");
    formatter.WriteLine("text-align: left;");
    formatter.WriteLine("padding: 8px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("td.grammarFirstCol");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("vertical-align: top;");
    formatter.WriteLine("text-align: right;");
    formatter.WriteLine("padding: 8px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine("td.grammarSecondCol");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("vertical-align: top;");
    formatter.WriteLine("text-align: left;");
    formatter.WriteLine("font-family: serif;");
    formatter.WriteLine("padding: 8px;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".rule");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: serif;");
    formatter.WriteLine("font-style: italic;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleChar");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: Consolas;");
    formatter.WriteLine("color: #a31515;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleString");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: Consolas;");
    formatter.WriteLine("color: #a31515;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleCharSet");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: Consolas;");
    formatter.WriteLine("color: #a31515;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleKeyword");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #0000cc;");
    formatter.WriteLine("font-weight: bold;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleParams");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: black;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleOp");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("padding-left: 2pt;");
    formatter.WriteLine("padding-right: 2pt;");
    formatter.WriteLine("font-family: serif;");
    formatter.WriteLine("color: black;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".ruleLink");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: serif;");
    formatter.WriteLine("font-style: italic;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    //  code:
    formatter.WriteLine(".code");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("font-family: Consolas;");
    formatter.WriteLine("font-size: small;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".lineNumber");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #2b91af;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".type");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #2b91af;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".pp");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #808080;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".kw");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #0000cc;");
    formatter.WriteLine("font-weight: bold;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".string");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #a31515;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".char");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: #a31515;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".comment");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: green;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".identifier");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: black;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".number");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: black;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine(".other");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("color: black;");
    formatter.DecIndent();
    formatter.WriteLine("}");
}

} } // namespace cmajor::cmdoclib
