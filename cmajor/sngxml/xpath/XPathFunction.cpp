// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/xpath/XPathFunction.hpp>
#include <sngxml/xpath/XPathContext.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/lexical_cast.hpp>
#include <unordered_map>

namespace sngxml { namespace xpath {

using namespace soulng::unicode;

struct ElementAndDocumentStringValueComputer : public sngxml::dom::Visitor
{
    void Visit(sngxml::dom::Text* text) override
    {
        stringValue.append(text->Data());
    }
    std::u32string stringValue;
};

std::u32string StringValue(sngxml::dom::Node* node)
{
    switch (node->GetNodeType())
    {
    case sngxml::dom::NodeType::documentNode:
    case sngxml::dom::NodeType::elementNode:
    {
        ElementAndDocumentStringValueComputer stringValueComputer;
        node->Accept(stringValueComputer);
        return stringValueComputer.stringValue;
    }
    case sngxml::dom::NodeType::attributeNode:
    {
        sngxml::dom::Attr* attr = static_cast<sngxml::dom::Attr*>(node);
        return attr->Value();
    }
    case sngxml::dom::NodeType::processingInstructionNode:
    {
        sngxml::dom::ProcessingInstruction* pi = static_cast<sngxml::dom::ProcessingInstruction*>(node);
        return pi->Data();
    }
    case sngxml::dom::NodeType::commentNode:
    {
        sngxml::dom::Comment* comment = static_cast<sngxml::dom::Comment*>(node);
        return comment->Data();
    }
    case sngxml::dom::NodeType::textNode:
    {
        sngxml::dom::Text* text = static_cast<sngxml::dom::Text*>(node);
        return text->Data();
    }
    case sngxml::dom::NodeType::cdataSectionNode:
    {
        sngxml::dom::CDataSection* cdataSection = static_cast<sngxml::dom::CDataSection*>(node);
        return cdataSection->Data();
    }
    }
    return std::u32string();
}

XPathFunction::XPathFunction(const std::u32string& name_, int minArity_, int maxArity_) : name(name_), minArity(minArity_), maxArity(maxArity_)
{
}

XPathFunction::~XPathFunction()
{
}

class XPathBooleanFunction : public XPathFunction
{
public:
    XPathBooleanFunction();
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments);
};

XPathBooleanFunction::XPathBooleanFunction() : XPathFunction(U"boolean", 1, 1)
{
}

std::unique_ptr<XPathObject> XPathBooleanFunction::Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments)
{
    if (arguments.size() != 1)
    {
        throw std::runtime_error("sngxml::xpath::boolean() function requires one argument");
    }
    XPathObject* argument = arguments[0];
    switch (argument->Type())
    {
    case XPathObjectType::boolean:
    {
        XPathBoolean* arg = static_cast<XPathBoolean*>(argument);
        return std::unique_ptr<XPathObject>(new XPathBoolean(arg->Value()));
    }
    case XPathObjectType::number:
    {
        XPathNumber* arg = static_cast<XPathNumber*>(argument);
        return std::unique_ptr<XPathObject>(new XPathBoolean(arg->Value() != 0));
    }
    case XPathObjectType::nodeSet:
    {
        XPathNodeSet* arg = static_cast<XPathNodeSet*>(argument);
        return std::unique_ptr<XPathObject>(new XPathBoolean(arg->Length() != 0));
    }
    case XPathObjectType::string:
    {
        XPathString* arg = static_cast<XPathString*>(argument);
        return std::unique_ptr<XPathObject>(new XPathBoolean(!arg->Value().empty()));
    }
    }
    return std::unique_ptr<XPathObject>(new XPathBoolean(false));
}

class XPathNumberFunction : public XPathFunction
{
public:
    XPathNumberFunction();
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments);
};

XPathNumberFunction::XPathNumberFunction() : XPathFunction(U"number", 0, 1)
{
}

std::unique_ptr<XPathObject> XPathNumberFunction::Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments)
{
    XPathObject* argument = nullptr;
    XPathNodeSet nodeSet;
    if (arguments.empty())
    {
        nodeSet.Add(context.Node());
        argument = &nodeSet;
    }
    else
    {
        if (arguments.size() != 1)
        {
            throw std::runtime_error("sngxml::xpath::number() function requires zero or one arguments");
        }
        argument = arguments[0];
    }
    switch (argument->Type())
    {
    case XPathObjectType::number:
    {
        XPathNumber* number = static_cast<XPathNumber*>(argument);
        return std::unique_ptr<XPathObject>(new XPathNumber(number->Value()));
    }
    case XPathObjectType::string:
    {
        XPathString* string = static_cast<XPathString*>(argument);
        double result = boost::lexical_cast<double>(ToUtf8(Trim(string->Value())));
        return std::unique_ptr<XPathObject>(new XPathNumber(result));
    }
    case XPathObjectType::boolean:
    {
        XPathBoolean* boolean = static_cast<XPathBoolean*>(argument);
        double result = boolean->Value() ? 1 : 0;
        return std::unique_ptr<XPathObject>(new XPathNumber(result));
    }
    case XPathObjectType::nodeSet:
    {
        XPathFunction* stringFunction = GetXPathLibraryFunction(U"string");
        std::vector<XPathObject*> stringArgs;
        stringArgs.push_back(argument);
        std::unique_ptr<XPathObject> asString = stringFunction->Evaluate(context, stringArgs);
        if (asString->Type() != XPathObjectType::string)
        {
            throw std::runtime_error("string result expected");
        }
        XPathString* string = static_cast<XPathString*>(asString.get());
        double result = boost::lexical_cast<double>(ToUtf8(Trim(string->Value())));
        return std::unique_ptr<XPathObject>(new XPathNumber(result));
    }
    }
    throw std::runtime_error("invalid argument to sngxml::xpath::number() function");
    return std::unique_ptr<XPathObject>();
}

class XPathStringFunction : public XPathFunction
{
public:
    XPathStringFunction();
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments);
};

XPathStringFunction::XPathStringFunction() : XPathFunction(U"string", 0, 1)
{
}

std::unique_ptr<XPathObject> XPathStringFunction::Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments)
{
    XPathObject* argument = nullptr;
    XPathNodeSet nodeSet;
    if (arguments.empty())
    {
        nodeSet.Add(context.Node());
        argument = &nodeSet;
    }
    else
    {
        if (arguments.size() != 1)
        {
            throw std::runtime_error("sngxml::xpath::string() function requires zero or one arguments");
        }
        argument = arguments[0];
    }
    switch (argument->Type())
    {
    case XPathObjectType::nodeSet:
    {
        XPathNodeSet* nodeSet = static_cast<XPathNodeSet*>(argument);
        if (nodeSet->Length() == 0)
        {
            return std::unique_ptr<XPathObject>(new XPathString(std::u32string()));
        }
        else
        {
            sngxml::dom::Node* node = (*nodeSet)[0];
            return std::unique_ptr<XPathObject>(new XPathString(StringValue(node)));
        }
    }
    case XPathObjectType::number:
    {
        XPathNumber* number = static_cast<XPathNumber*>(argument);
        return std::unique_ptr<XPathObject>(new XPathString(ToUtf32(std::to_string(number->Value()))));
    }
    case XPathObjectType::boolean:
    {
        XPathBoolean* boolean = static_cast<XPathBoolean*>(argument);
        return std::unique_ptr<XPathObject>(new XPathString(boolean->Value() ? U"true" : U"false"));
    }
    case XPathObjectType::string:
    {
        XPathString* string = static_cast<XPathString*>(argument);
        return std::unique_ptr<XPathObject>(new XPathString(string->Value()));
    }
    }
    throw std::runtime_error("invalid argument to sngxml::xpath::string() function");
    return std::unique_ptr<XPathObject>();
}

class XPathLastFunction : public XPathFunction
{
public:
    XPathLastFunction();
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments);
};

XPathLastFunction::XPathLastFunction() : XPathFunction(U"last", 0, 0)
{
}

std::unique_ptr<XPathObject> XPathLastFunction::Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments)
{
    if (!arguments.empty())
    {
        throw std::runtime_error("sngxml::xpath::last() function requires no arguments");
    }
    return std::unique_ptr<XPathObject>(new XPathNumber(context.Size()));
}

class XPathPositionFunction : public XPathFunction
{
public:
    XPathPositionFunction();
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments);
};

XPathPositionFunction::XPathPositionFunction() : XPathFunction(U"position", 0, 0)
{
}

std::unique_ptr<XPathObject> XPathPositionFunction::Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments)
{
    if (!arguments.empty())
    {
        throw std::runtime_error("sngxml::xpath::position() function requires no arguments");
    }
    return std::unique_ptr<XPathObject>(new XPathNumber(context.Position()));
}

class XPathCountFunction : public XPathFunction
{
public:
    XPathCountFunction();
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments);
};

XPathCountFunction::XPathCountFunction() : XPathFunction(U"count", 1, 1)
{
}

std::unique_ptr<XPathObject> XPathCountFunction::Evaluate(XPathContext& context, std::vector<XPathObject*>& arguments)
{
    if (arguments.size() != 1 || arguments[0]->Type() != XPathObjectType::nodeSet)
    {
        throw std::runtime_error("sngxml::xpath::count() function requires one node-set argument");
    }
    XPathNodeSet* nodeSet = static_cast<XPathNodeSet*>(arguments[0]);
    return std::unique_ptr<XPathObject>(new XPathNumber(nodeSet->Length()));
}

class XPathFunctionLibrary
{
public:
    static void Init();
    static void Done();
    static XPathFunctionLibrary& Instance() { return *instance; }
    XPathFunction* GetFunction(const std::u32string& functionName);
private:
    static std::unique_ptr<XPathFunctionLibrary> instance;
    std::unordered_map<std::u32string, XPathFunction*> functionMap;
    std::vector<std::unique_ptr<XPathFunction>> functions;
    XPathFunctionLibrary();
};

std::unique_ptr<XPathFunctionLibrary> XPathFunctionLibrary::instance;

void XPathFunctionLibrary::Init()
{
    instance.reset(new XPathFunctionLibrary());
}

void XPathFunctionLibrary::Done()
{
    instance.reset();
}

XPathFunctionLibrary::XPathFunctionLibrary()
{
    functions.push_back(std::unique_ptr<XPathFunction>(new XPathBooleanFunction()));
    functions.push_back(std::unique_ptr<XPathFunction>(new XPathNumberFunction()));
    functions.push_back(std::unique_ptr<XPathFunction>(new XPathStringFunction()));
    functions.push_back(std::unique_ptr<XPathFunction>(new XPathLastFunction()));
    functions.push_back(std::unique_ptr<XPathFunction>(new XPathPositionFunction()));
    functions.push_back(std::unique_ptr<XPathFunction>(new XPathCountFunction()));
    for (const std::unique_ptr<XPathFunction>& function : functions)
    {
        functionMap[function->Name()] = function.get();
    }
}

XPathFunction* XPathFunctionLibrary::GetFunction(const std::u32string& functionName)
{
    auto it = functionMap.find(functionName);
    if (it != functionMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("sngxml::xpath function '" + ToUtf8(functionName) + "' not found");
    }
}

XPathFunction* GetXPathLibraryFunction(const std::u32string& functionName)
{
    return XPathFunctionLibrary::Instance().GetFunction(functionName);
}

void InitFunction()
{
    XPathFunctionLibrary::Init();
}

void DoneFunction()
{
    XPathFunctionLibrary::Done();
}

} } // namespace sngxml::xpath
