// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/Action.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/rex/Context.hpp>
#include <soulng/rex/Match.hpp>
#include <soulng/rex/Nfa.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/FileUtil.hpp>
#include <soulng/util/Log.hpp>
#include <boost/filesystem.hpp>

namespace cmajor { namespace build {

using namespace soulng::unicode;
using namespace soulng::util;

Variable::Variable(const std::string& name_, const std::string& value_) : name(name_), value(value_)
{
}

Variables::Variables()
{
}

void Variables::AddVariable(Variable* variable)
{
    Variable* prev = GetVariable(variable->Name());
    if (prev)
    {
        throw std::runtime_error("build variable name '" + prev->Name() + "' not unique");
    }
    variableMap[variable->Name()] = variable;
    variables.push_back(std::unique_ptr<Variable>(variable));
}

Variable* Variables::GetVariable(const std::string& name) const
{
    auto it = variableMap.find(name);
    if (it != variableMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

std::string Expand(const std::string& str, const Variables& variables)
{
    std::string result;
    std::string variableName;
    int state = 0;
    for (char c : str)
    {
        switch (state)
        {
            case 0:
            {
                if (c == '$')
                {
                    variableName.clear();
                    state = 1;
                }
                else
                {
                    result.append(1, c);
                }
                break;
            }
            case 1:
            {
                if (c == '$')
                {
                    Variable* variable = variables.GetVariable(variableName);
                    if (variable)
                    {
                        result.append(variable->Value());
                    }
                    else
                    {
                        throw std::runtime_error("build variable '" + variableName + "' not found");
                    }
                    state = 0;
                }
                else
                {
                    variableName.append(1, c);
                }
                break;
            }
        }
    }
    if (state == 1)
    {
        throw std::runtime_error("build variable '" + variableName + "' has no ending '$' character");
    }
    return result;
}

void Copy(const std::vector<std::string>& sourceFilePaths, const std::string& targetDir)
{
    for (const auto& sourceFilePath : sourceFilePaths)
    {
        std::string dest = GetFullPath(Path::Combine(targetDir, Path::GetFileName(sourceFilePath)));
        CopyFile(sourceFilePath, dest, true, false);
        LogMessage(-1, sourceFilePath + " -> " + dest);
    }
}

std::vector<std::string> GetSourceFilePaths(const std::string& sourceDir, const std::string& sourceFileMask, soulng::rex::Context& context)
{
    std::vector<std::string> sourceFilePaths;
    soulng::rex::Nfa nfa = soulng::rex::CompileFilePattern(context, ToUtf32(sourceFileMask));
    boost::system::error_code ec;
    boost::filesystem::directory_iterator it(boost::filesystem::path(sourceDir), ec);
    if (ec)
    {
        throw std::runtime_error("could not iterate directory '" + sourceDir + "': " + PlatformStringToUtf8(ec.message()));
    }
    while (it != boost::filesystem::directory_iterator())
    {
        if (boost::filesystem::is_regular_file(it->path()))
        {
            std::string fileName = it->path().filename().generic_string();
            if (soulng::rex::PatternMatch(ToUtf32(fileName), nfa))
            {
                sourceFilePaths.push_back(Path::Combine(sourceDir, fileName));
            }
        }
        ++it;
    }
    return sourceFilePaths;
}

void RunBuildActions(sngxml::dom::Document* actionDoc, const std::string& rootDir, soulng::rex::Context& context, const Variables& variables)
{
    std::unique_ptr<sngxml::xpath::XPathObject> copyActionObject = sngxml::xpath::Evaluate(U"/actions/copy", actionDoc);
    if (copyActionObject)
    {
        if (copyActionObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(copyActionObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string sourceFileAttribute = element->GetAttribute(U"sourceFile");
                    if (sourceFileAttribute.empty())
                    {
                        throw std::runtime_error("'actions/copy' element has no 'sourceFile' attribute");
                    }
                    std::u32string targetDirAttribute = element->GetAttribute(U"targetDir");
                    if (targetDirAttribute.empty())
                    {
                        throw std::runtime_error("'actions/copy' element has no 'targetDir' attribute");
                    }
                    std::string sourceFilePath = Expand(ToUtf8(sourceFileAttribute), variables);
                    std::string targetDirPath = Expand(ToUtf8(targetDirAttribute), variables);
                    sourceFilePath = GetFullPath(Path::Combine(rootDir, sourceFilePath));
                    std::string sourceDir;
                    std::string sourceFileMask;
                    sourceDir = Path::GetDirectoryName(sourceFilePath);
                    sourceFileMask = Path::GetFileName(sourceFilePath);
                    std::vector<std::string> sourceFilePaths = GetSourceFilePaths(sourceDir, sourceFileMask, context);
                    Copy(sourceFilePaths, targetDirPath);
                }
            }
        }
    }
}

void RunBuildActions(const sngcm::ast::Project& project, const Variables& variables)
{
    soulng::rex::Context context;
    for (const auto& actionFilePath : project.ActionFilePaths())
    {
        std::string rootDir = Path::GetDirectoryName(actionFilePath);
        std::unique_ptr<sngxml::dom::Document> actionDoc = sngxml::dom::ReadDocument(actionFilePath);
        RunBuildActions(actionDoc.get(), rootDir, context, variables);
    }
}

} } // namespace cmajor::build
